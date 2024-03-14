import os
import cgi
import sys
# from PIL import Image
from io import BytesIO
import time

def main() :
	try :
		curPath = os.environ['CUR_PATH']
		savePath = os.environ['SAVE_PATH']
		boundary = os.environ['BOUNDARY']
		contentLength = int(os.environ['CONTENT_LENGTH'])
		body = sys.stdin.buffer.read()

		stdin_io = BytesIO(body)
		fs = cgi.FieldStorage(
			fp = stdin_io,
			environ={
				'REQUEST_METHOD' : 'POST',
				'CONTENT_TYPE' : f'multipart/form-data; boundary={boundary}',
				'CONTENT_LENGTH' : contentLength
			},
			keep_blank_values=True
		)

		fileItem = fs['file']

		if fileItem.filename :
			fileName = os.path.basename(fileItem.filename)
			filePath = savePath + "/" + fileName
			

			with open(filePath, 'wb') as f :
				f.write(fileItem.file.read())
		else :
			raise Exception
		print(filePath.replace(curPath, ""))
	except Exception as e :
		print("ERROR")

if __name__ == '__main__':
    main()