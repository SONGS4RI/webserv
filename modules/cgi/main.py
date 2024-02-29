import cgi
import os
from PIL import Image
from io import BytesIO
def main():
	save_path = os.environ['SAVE_PATH']
	form = cgi.FieldStorage()
	fileitem = form["file"]
	try:
		if fileitem.filename:
			im = Image.open(BytesIO(fileitem.file.read()))
			im.save(os.path.join(save_path, fileitem.filename))

			# success
			print(save_path + fileitem.filename)
		else:
			# failure
			raise Exception

	except Exception as e:
		print("HTTP/1.1 500 Internal Server Error", end = "\r\n")
		print("Location: /errors/error.html", end = "\r\n")
		print(end = "\r\n")
		print("ERROR")

if __name__ == "__main__":
    main()