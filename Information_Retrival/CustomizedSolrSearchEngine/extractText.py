import re
import os
import urllib
import bs4
from bs4 import BeautifulSoup
import glob   

path = '/home/vrushali/Desktop/InformationRetrieval/HtmlFiles/Downloads/*.html' 
fNew = open('big.txt', 'r+')
files=glob.glob(path)


def visible(element):
    if element.parent.name in ['style', 'script', '[document]', 'head', 'title']:
        return False
    elif isinstance(element,bs4.element.Comment): #re.match('<!--.*-->', str(element)):
        return False
    return True
import urllib
from bs4 import BeautifulSoup
import glob   
path = '/home/mayank/pythonScript/test/*.html' 

bigTxt = ''

files=glob.glob(path)   
for fil in files: 
	f=open(fil, 'r') 
	print fil 
	soup = BeautifulSoup(f)

	# kill all script and style elements
	for script in soup(["script", "style"]):
		script.extract()    # rip it out

	# get text
	text = soup.get_text()

	# break into lines and remove leading and trailing space on each
	lines = (line.strip() for line in text.splitlines())
	# break multi-headlines into a line each
	chunks = (phrase.strip() for line in lines for phrase in line.split("  "))

	# drop blank lines
	text = ' '.join(chunk for chunk in chunks if chunk)
	f.close()	
	bigTxt += text

	#print(text)
	

fNew = open('big.txt', 'r+')
fNew.write(bigTxt.encode('utf-8'))
fNew.close()
print "comepletely done"

for fil in files: 
    f=open(fil, 'r') 
    print fil 
    soup = BeautifulSoup(f, 'html.parser')
    texts = soup.findAll(text=True)


    visible_texts = filter(visible, texts)


# break into lines and remove leading and trailing space on each
#lines = (line.strip() for line in text.splitlines())
# break multi-headlines into a line each
#chunks = (phrase.strip() for line in lines for phrase in line.split("  "))


   





    fNew.write(text.encode('utf-8'))
fNew.close()









