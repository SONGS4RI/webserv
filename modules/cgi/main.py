import os
import cgi
# from PIL import Image
from io import BytesIO
import time

def main() :
	print("Start!!")
	# savePath = os.environ['SAVE_PATH']
	# boundary = os.environ['BOUNDARY']
	# contentLength = os.environ['CONTENT_LENGTH']
	# body = os.environ['BODY']

	# print("savePath: " + savePath)
	# print("boundary: " + boundary)
	# print("contentLength: " + contentLength)
	# print(body)

	# form = cgi.FieldStorage()
	# fileitem = form["file"]
	# try :
	# 	if fileitem.filename :# success
	# 		im = Image.open(BytesIO(fileitem.file.read()))
	# 		im.save(os.path.join(savePath, fileitem.filename))
	# 		print(save_path + "/" + fileitem.filename)
	# 	else :# failure
	# 		raise Exception

	# except Exception :
	# 	print("ERROR")

if __name__ == '__main__':
    main()