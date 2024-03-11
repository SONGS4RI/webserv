#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include "RequestHandler.hpp"
#include "../server/EventManager.hpp"
#include "../utils/HTTPInfo.hpp"

RequestHandler::RequestHandler(const Request* request, Client* client) {
	this->requestBody = NULL;
	this->client = client;
	this->responseBody = new ResponseBody(request->getStatusCode());
	this->bodyMaxSize = 0;
	this->buf = NULL;
	this->config = &client->getServer().getServerConfig();
	this->isUrlIndex = request->getIsUrlIndex();
	if (request->getStatus() != ERROR) {
		this->bodyMaxSize =  config->getClientMaxBodySize();
		this->buf = new char[bodyMaxSize];
		this->method = request->getProperties().find(METHOD)->second;
		this->requestUrl = request->getProperties().find(REQUEST_URL)->second;//서버 루트 url + requestUrl 해주어야함
		this->requestBody = request->getBody();
	}
}

RequestHandler::~RequestHandler() {
	if (buf != NULL) {
		delete buf;
	}
}

ResponseBody* RequestHandler::handleRequest() {
	try {
		if (responseBody->getStatusCode().getStatusCode() >= 400) {
			throw responseBody->getStatusCode();
		}
		int status;
		if (client->getPid() > 0 && waitpid(client->getPid(), &status, WNOHANG) > 0) {
			handleCgiRead();
			return responseBody;
		}
		checkResource();
		if (method == GET) {
			handleGet();
		} else if (method == DELETE) {
			handleDelete();
		} else if (requestBody->getContentType() == MULTIPART_FORM_DATA){// POST
			handleCgiExecve();
			return NULL;
		} else {// POST
			handlePost();
		}
	} catch(const StatusCode& statusCode) {
		// Handle error
		handleError(statusCode);
	}
	return responseBody;
}

void RequestHandler::handleGet() {
	bool autoIndex = config->getAutoindexOn();
	if (autoIndex && isUrlDir && !isUrlIndex) {
		string resource = HTTPInfo::defaultRoot + requestUrl;
		dirListing(resource, requestUrl);
	} else if (!autoIndex && isUrlDir && !isUrlIndex) {
		throw StatusCode(400, BAD_REQUEST);
	} else {
		size_t idx = requestUrl.rfind('.');
		string contentType = idx != SIZE_T_MAX ? string(requestUrl.begin() + idx + 1, requestUrl.end()) : "";
		cout << HTTPInfo::defaultRoot + requestUrl << "\n";////////////////////////////////
		int fd = open((HTTPInfo::defaultRoot + requestUrl).c_str(), O_RDONLY);
		int n = read(fd, buf, sizeof(buf));
		if (n < 0 ) {// max size 보다 클때도 추가
			throw StatusCode(500, INTERVER_SERVER_ERROR);
		}
		if (read(fd, buf, sizeof(buf))) {
			throw StatusCode(400, BAD_REQUEST);
		}
		responseBody->setStatusCode(StatusCode(200, OK));
		responseBody->setContentType(HTTPInfo::convertToMIME(contentType));
		responseBody->setContentLength(n);
		responseBody->setBody(buf, n);
		close(fd);
	}
}

void RequestHandler::dirListing(const string& resource, string& uri) {
	string autoIndexing = "<html><head><meta charset=\"UTF-8\"></head>" \
    "<style>body { background-color: white; font-family: Trebuchet MS;}</style>" \
	"<body><h1>" + uri + " List</h1><ul>";
	if (uri.back() != '/') uri.push_back('/');
	autoIndexing += "<table>";
	struct dirent* entry;
	string absoluteUri = "http://localhost:" + Utils::intToString(config->getPort()) + resource.substr(HTTPInfo::defaultRoot.size() + config->getRoot().size()) + "/";
	DIR* dp = opendir(resource.c_str());
	if (dp != NULL) {
		while ((entry = readdir(dp))) {
			string entryName = entry->d_name;
			if (entryName.front() == '.') continue;
			cout << absoluteUri + entryName << "\n";//////////////////////////////////////////
			autoIndexing += "<tr><td><a href='" + absoluteUri + entryName + "'>" + entryName + "</a></td></tr>";
		}
	}
	closedir(dp);
	autoIndexing += "</table></ul></body></html>";
	responseBody->setBody(autoIndexing.c_str(), autoIndexing.size());
	responseBody->setContentLength(autoIndexing.size());
	responseBody->setContentType(TEXT_HTML);
	responseBody->setStatusCode(StatusCode(200, OK));
}

void RequestHandler::handleDelete() {
	if (remove(requestUrl.c_str()) != 0) {
		throw StatusCode(405, FORBIDDEN);
	}
	responseBody->setStatusCode(StatusCode(204, NO_CONTENT));
}

void RequestHandler::handlePost() {
	string extension;
	if (requestBody->getContentType() == APPLICATION_OCTET_STREAM) {
		extension = ".bin";
	} else if (requestBody->getContentType() == IMAGE_PNG) {
		extension = ".png";
	} else if (requestBody->getContentType() == TEXT_HTML) {
		extension = ".html";
	} else if (requestBody->getContentType() == TEXT_PLAIN) {
		extension = ".txt";
	}
	time_t now = time(0);
	srand(static_cast<unsigned int>(now));
	tm* now_tm = localtime(&now);
    int randomValue = rand() % 100000 + 1;
	// 여기 수정해야함!!!!!!!!!!!!!!!!!!!!!
	char time[26];
	sprintf(time, "%04d%02d%02d_%02d%02d%02d",
                 1900 + now_tm->tm_year, now_tm->tm_mon + 1, now_tm->tm_mday,
                 now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	string fileName = string(time) + "_" + Utils::intToString(randomValue) + extension;

    ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
	if (!outFile) {
		throw StatusCode(500, INTERVER_SERVER_ERROR);
	}
	outFile.write(requestBody->getBody().c_str(), requestBody->getBody().size());
	outFile.close();
	responseBody->setStatusCode(StatusCode(200, OK));
	responseBody->setLocation(requestUrl + fileName);// 경로 설정
}

void RequestHandler::handleCgiExecve() {
	int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1 || (pid = fork()) == -1) {
        throw StatusCode(500, INTERVER_SERVER_ERROR);
    }

    if (pid == 0) { // 자식 프로세스
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);

        char **argv = NULL;// = {"/usr/bin/python", "script.py", NULL};
        char **envp = NULL;// = {NULL};
        execve(argv[0], argv, envp);
		exit(EXIT_FAILURE);
    } else { // 부모 프로세스
        close(pipefd[1]);
		client->setPipeFd(pipefd[0]);
		client->setPid(pid);
    }
}

void RequestHandler::handleCgiRead() {
	read(client->getPipeFd(), buf, bodyMaxSize);
	close(client->getPipeFd());
	string location(buf);
	if (location == "ERROR") {
		throw StatusCode(500, INTERVER_SERVER_ERROR);
	} else {
		responseBody->setStatusCode(StatusCode(201, CREATED));
		responseBody->setLocation(location);
	}
}

void RequestHandler::handleError(const StatusCode& statusCode) {
	responseBody->setStatusCode(statusCode);
	responseBody->setContentType(TEXT_HTML);
	string fileName = HTTPInfo::defaultRoot + "/root/html/" + config->getDefaultErrorPage();// config 에서 받아 써야함
	int fd = open(fileName.c_str(), O_RDONLY);
	char errorBuf[2048];
	int n = read(fd, errorBuf, sizeof(errorBuf));
	if (n < 0) {
		Utils::exitWithErrmsg(INTERVER_SERVER_ERROR);
	}
	responseBody->setContentLength(n);
	responseBody->setBody(errorBuf, n);
}

void RequestHandler::checkResource() {
	struct stat buffer;
	if (stat((HTTPInfo::defaultRoot + requestUrl).c_str(), &buffer) != 0) {
		throw StatusCode(404, string(NOT_FOUND) + ": " + requestUrl);
	}
	// 디렉토리 리스팅 해야함.....
	isUrlDir = S_ISDIR(buffer.st_mode);

	if (method == POST && !isUrlDir) {
		throw StatusCode(400, BAD_REQUEST);
	}
	if (method == DELETE && isUrlDir) {
		throw StatusCode(405, FORBIDDEN);
	}
}