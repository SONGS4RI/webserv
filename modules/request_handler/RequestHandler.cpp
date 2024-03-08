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
	if (request->getStatus() != ERROR) {
		this->bodyMaxSize =  client->getServer().getServerConfig().getClientMaxBodySize();
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
	bool autoIndex = client->getServer().getServerConfig().getAutoindexOn();
	if (autoIndex && isUrlDir) {
		string resource = HTTPInfo::defaultRoot + requestUrl;
		dirListing(resource, requestUrl);
	} else if (!autoIndex && isUrlDir) {
		throw StatusCode(400, BAD_REQUEST);
	} else {
		size_t idx = requestUrl.rfind('.');
		string contentType = idx != SIZE_T_MAX ? string(requestUrl.begin() + idx + 1, requestUrl.end()) : "";
		
		int fd = open((HTTPInfo::root + requestUrl).c_str(), O_RDONLY);
		int n = read(fd, buf, bodyMaxSize);
		if (n < 0) {// max size 보다 클때도 추가
			handleError(StatusCode(500, INTERVER_SERVER_ERROR));
			return ;
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
	string absoluteUri = "http://localhost:" + Utils::intToString(client->getServer().getServerConfig().getPort()) + "/" + resource.substr(HTTPInfo::defaultRoot.size()) + "/";
	DIR* dp = opendir(resource.c_str());
	if (dp != NULL) {
		while ((entry = readdir(dp))) {
			string entryName = entry->d_name;
			if (entryName.front() == '.') continue;
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
	string fileName = HTTPInfo::defaultRoot + "html/" + "error" + ".html";// config 에서 받아 써야함
	int fd = open(fileName.c_str(), O_RDONLY);
	if (fd < 0) {
		Utils::exitWithErrmsg(INTERVER_SERVER_ERROR);
	}
	int n = read(fd, buf, bodyMaxSize);
	if (n < 0) {
		Utils::exitWithErrmsg(INTERVER_SERVER_ERROR);
	}
	responseBody->setContentLength(n);
	responseBody->setBody(buf, n);
}

void RequestHandler::checkResource() {
	struct stat buffer;
	if (stat((HTTPInfo::root + requestUrl).c_str(), &buffer) != 0) {
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