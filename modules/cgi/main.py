import os
import cgi
from PIL import Image
from io import BytesIO

def main() :
	save_path = os.environ['SAVE_PATH']
	form = cgi.FieldStorage()
	fileitem = form["file"]
	try :
		if fileitem.filename :# success
			im = Image.open(BytesIO(fileitem.file.read()))
			im.save(os.path.join(save_path, fileitem.filename))
			print(save_path + fileitem.filename)
		else :# failure
			raise Exception

	except Exception :
		print("ERROR")

if __name__ == "__main__":
    main()