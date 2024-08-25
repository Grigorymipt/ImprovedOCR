import cv2 as cv
import numpy as np
import sys
import matplotlib.pyplot as plt
import pytesseract

img = cv.imread(sys.argv[1], cv.IMREAD_GRAYSCALE)
assert img is not None, "file could not be read, check with os.path.exists()"
#img = cv.medianBlur(img,1)
th3 = cv.adaptiveThreshold(img,255,cv.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv.THRESH_BINARY,11,2)
new_file = sys.argv[1]+".otsubin.png"
cv.imwrite(new_file, th3)
#print(pytesseract.image_to_string(th3, lang='eng+rus'))
