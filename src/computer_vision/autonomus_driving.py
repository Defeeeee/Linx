import cv2 as cv
import time
import numpy as np
from matplotlib import pyplot as plt
import json
import math


def load_cam_calib_data(filepath='camCalibrationData.json'):
    with open(filepath, 'r') as f:
        camCalibData = json.loads(f.read())

    global cameraMatrix
    global dist
    cameraMatrix = np.array(camCalibData['cameraMatrix'])
    dist = np.array(camCalibData['dist'])


def undistort_img(_img, _cameraMatrix, _dist):
    h,  w = _img.shape[:2]
    newCameraMatrix, roi = cv.getOptimalNewCameraMatrix(_cameraMatrix, _dist, (w,h), 1, (w,h))

    # Undistort with Remapping
    mapx, mapy = cv.initUndistortRectifyMap(_cameraMatrix, _dist, None, newCameraMatrix, (w,h), 5)
    dst = cv.remap(_img, mapx, mapy, cv.INTER_LINEAR)

    # crop the image
    x, y, w, h = roi
    dst = dst[y:y+h, x:x+w]
    return dst


load_cam_calib_data('../calibration/camCalibrationData.json')

start = time.time()

img = cv.imread('images/test10.jpg')
img = cv.cvtColor(img, cv.COLOR_BGR2RGB)
#img = undistort_img(img, cameraMatrix, dist)
img = cv.resize(img, (640, 480))
b = img.copy()
b[:, :, 1] = 0
b[:, :, 2] = 0
img = b
gray = cv.cvtColor(img, cv.COLOR_RGB2GRAY)
#gray = cv.GaussianBlur(gray, (9, 9), 10)

threshhold1 = 80
threshhold2 = 120
mask1 = cv.bitwise_not(cv.threshold(gray, threshhold1, threshhold2, cv.THRESH_OTSU)[1])
mask = cv.bitwise_not(cv.adaptiveThreshold(gray, threshhold1, cv.ADAPTIVE_THRESH_MEAN_C, cv.THRESH_BINARY, 25, 10))
mask = cv.bitwise_and(mask, mask1)
mask = cv.inRange(mask, 220, 255)

print(mask.dtype)
print(gray.dtype)
can = cv.Canny(mask, 100, 125, apertureSize=7, L2gradient=True)

lines = cv.HoughLines(
    can,
    1,
    np.pi/180,
    100,
    min_theta=0,
    max_theta=np.pi
)

lines_ = []

for line in lines:
    rho,theta = line[0]
    a = np.cos(theta)
    b = np.sin(theta)
    x0 = a * rho
    y0 = b * rho
    # x1 stores the rounded off value of (r * cos(theta) - 1000 * sin(theta))
    x1 = int(x0 + 1000 * (-b))
    # y1 stores the rounded off value of (r * sin(theta)+ 1000 * cos(theta))
    y1 = int(y0 + 1000 * (a))
    # x2 stores the rounded off value of (r * cos(theta)+ 1000 * sin(theta))
    x2 = int(x0 - 1000 * (-b))
    # y2 stores the rounded off value of (r * sin(theta)- 1000 * cos(theta))
    y2 = int(y0 - 1000 * (a))
    lines_.append([x1, y1, x2, y2, (math.atan2(y2 - y1, x2 - x1) * 180 / np.pi)])

for line in lines_:
    x1, y1, x2, y2, angle = line
    print(angle)
    for line_ in lines_:
        x1_, y1_, x2_, y2_, angle_ = line_
        if 10 > abs(angle - angle_) > 0 and line != line_:
            print("Removed: ", line)
            print("diff: ", abs(angle - angle_))
            lines_.remove(line_)
            break

for line in lines_:
    x1, y1, x2, y2, angle = line
    cv.line(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
    print(angle)
print(lines.shape)
print(lines_.__len__())

end = time.time()
totalTime = end - start
fps = 1 / totalTime
print("FPS: ", fps)
cv.imshow("image", img)
cv.waitKey(0)

titles = ['Original Image', 'Canny', 'Mask', 'gray']
images = [b, can, mask, gray]
'''
for i in range(4):
    plt.subplot(2, 2, i+1), plt.imshow(images[i], 'gray')
    plt.title(titles[i])
    plt.xticks([]), plt.yticks([])

plt.show()
'''



