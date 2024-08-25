import cv2 as cv
import numpy as np
import sys
import matplotlib.pyplot as plt
import pytesseract

img = cv.imread(sys.argv[1], cv.IMREAD_GRAYSCALE)

assert img is not None, "file could not be read, check with os.path.exists()"
'''img = cv.medianBlur(img,3)
img = (255-img)
ret,th1 = cv.threshold(img,127,255,cv.THRESH_BINARY)
th2 = cv.adaptiveThreshold(img,255,cv.ADAPTIVE_THRESH_MEAN_C,\
            cv.THRESH_BINARY,11,2)
th3 = cv.adaptiveThreshold(img,255,cv.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv.THRESH_BINARY,11,2)
titles = ['Original Image', 'Global Thresholding (v = 127)',
            'Adaptive Mean Thresholding', 'Adaptive Gaussian Thresholding']
images = [img, th1, th2, th3]
for i in range(4):
    plt.subplot(2,2,i+1),plt.imshow(images[i],'gray')
    plt.title(titles[i])
    plt.xticks([]),plt.yticks([])
plt.show()'''

assert img is not None, "file could not be read, check with os.path.exists()"

# calculate the average color of each row of our image
avg_color_per_row = np.average(img, axis=0)

# calculate the averages of our rows
avg_colors = np.average(avg_color_per_row, axis=0)

# avg_color is a tuple in BGR order of the average colors
# but as float values
print(f'avg_colors: {avg_colors}')
if avg_colors < 90:
    img = (255-img)
img = cv.medianBlur(img,3)
th3 = cv.adaptiveThreshold(img,255,cv.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv.THRESH_BINARY,11,2)
new_file = sys.argv[1]+".otsubin.png"
cv.imwrite(new_file, th3)
#print(pytesseract.image_to_string(th3, lang='eng+rus'))
