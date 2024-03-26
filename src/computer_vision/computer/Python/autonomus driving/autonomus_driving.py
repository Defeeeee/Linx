import cv2 as cv
import time
import numpy as np
from matplotlib import pyplot as plt
import json
import math


def load_config_data(filepath='config_data.json'):
    with open(filepath, 'r') as f:
        configData = json.loads(f.read())

    global lineReductions
    global camIndex
    global thresholds
    global frameSize
    global adaptiveSettings

    lineReductions = configData['line_reductions']
    camIndex = configData['cam_index']
    thresholds = np.array(configData['thresholds'])
    frameSize = np.array(configData['frame_size'])
    adaptiveSettings = np.array(configData['adaptive_settings'])


def load_cam_calib_data(filepath='camCalibrationData.json'):
    with open(filepath, 'r') as f:
        camCalibData = json.loads(f.read())

    global cameraMatrix
    global dist
    cameraMatrix = np.array(camCalibData['cameraMatrix'])
    dist = np.array(camCalibData['dist'])


def load_unwrap_data(filepath='unwrap_data.json'):
    with open(filepath, 'r') as f:
        unwrapData = json.loads(f.read())

    global unwrap_cent
    unwrap_cent = np.array(unwrapData['centers'])


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


# program Start
load_config_data()
load_cam_calib_data('../calibration/camCalibrationData.json')
load_unwrap_data()

cap = cv.VideoCapture(camIndex)

cap.set(cv.CAP_PROP_FRAME_WIDTH, frameSize[0])
cap.set(cv.CAP_PROP_FRAME_HEIGHT, frameSize[1])
pts1 = [(unwrap_cent[3][0], unwrap_cent[3][1]), (unwrap_cent[2][0], unwrap_cent[2][1]), (unwrap_cent[0][0], unwrap_cent[0][1]), (unwrap_cent[1][0], unwrap_cent[1][1])]
pts2 = [[0, 0], [0, frameSize[1]], [frameSize[0], 0], [frameSize[0], frameSize[1]]]
matrix = cv.getPerspectiveTransform(np.float32(pts1), np.float32(pts2))

while cap.isOpened():
    try:
        ret, img = cap.read()
    except:
        print("could not grab frame")
        continue
    start = time.time()

    img = cv.cvtColor(img, cv.COLOR_BGR2RGB)
    # img = undistort_img(img, cameraMatrix, dist)
    img = cv.warpPerspective(img, matrix, (frameSize[0], frameSize[1]))

    b = img.copy()
    b[:, :, 2] = 0
    b[:, :, 1] = 0

    gray = cv.cvtColor(b, cv.COLOR_RGB2GRAY)

    mask1 = cv.bitwise_not(cv.threshold(gray, thresholds[0], thresholds[1], cv.THRESH_OTSU)[1])
    mask = cv.bitwise_not(cv.adaptiveThreshold(gray, thresholds[0], cv.ADAPTIVE_THRESH_MEAN_C, cv.THRESH_BINARY, adaptiveSettings[0], adaptiveSettings[1]))

    mask = cv.bitwise_and(mask, mask1)
    mask = cv.inRange(mask, 200, 255)

    can = cv.Canny(mask, 100, 125, apertureSize=7, L2gradient=True)

    lines = cv.HoughLinesP(
        can,
        1,
        np.pi/90,
        100,
        minLineLength=50,
        maxLineGap=100
    )

    end = time.time()
    transformTime = end - start
    print("Time: ", transformTime)

    lines_ = []
    img_lines = img.copy()

    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            lines_.append([x1, y1, x2, y2, (math.atan2(y2 - y1, x2 - x1) * 180 / np.pi)])

        for i in range(lineReductions):
            for line, enum in zip(lines_, enumerate(lines_)):
                x1, y1, x2, y2, angle = line
                print("evaluating", line)
                print()
                if angle > 90:
                    angle = -180 + angle
                    lines_[enum][4] = angle
                elif angle < -90:
                    angle = 180 + angle
                    lines_[enum][4] = angle
                for line_ in lines_:
                    x1_, y1_, x2_, y2_, angle_ = line_
                    if line == line_:   # Skip the same line
                        continue
                    if 10 > abs(angle - angle_) > 0:
                        print("Removed: ", line_)
                        print("diff: ", abs(angle - angle_))
                        lines_.remove(line_)
                    else:
                        print("Not Removed: ", line_)
                        print("diff: ", abs(angle - angle_))
                print()

        vert_lines = []
        hori_lines = []

        for line in lines_:
            x1, y1, x2, y2, angle = line
            if -45 < angle < 45:
                vert_lines = np.append(vert_lines, [line])
            else:
                hori_lines = np.append(hori_lines, [line])

        lines_ = np.append(vert_lines, hori_lines)
        if 2 <= vert_lines.__len__() < 3:
            print("Lines: ", lines_)
            x1, y1, x2, y2, angle = lines_[0]
            x = [x1, x2]
            y = [y1, y2]
            x.sort()
            y.sort()
            x1, x2 = x
            y1, y2 = y
            if x1 == x2:
                x2 = x2+0.5
            slope = (y2 - y1) / (x2 - x1)
            if slope == 0:
                slope = 0.05
            b = y1 - slope * x1
            print(lines_[0])
            print(img.shape)
            print(slope)
            lines_[0][0] = int((img.shape[0] - b) / slope)
            lines_[0][1] = img.shape[0]
            lines_[0][2] = int((lines_[1][3] - b) / slope)
            lines_[0][3] = lines_[1][3]
            print("Line 0: ", lines_[0])
            x1, y1, x2, y2, angle = lines_[1]
            x = [x1, x2]
            x_ = [x1, x2]
            x.sort()
            if x != x_:
                x1, x2 = x_
                y = [y1, y2]
                y2, y1 = y
            if x1 == x2:
                x2 = x2+0.5
            slope = (y2 - y1) / (x2 - x1)
            if slope == 0:
                slope = 0.05
            b = y1 - slope * x1
            lines_[1][0] = int((img.shape[0] - b) / slope)
            lines_[1][1] = img.shape[0]
            print("Line 1: ", lines_[1])

            error_line = [int((lines_[0][0] + lines_[1][0]) / 2), img.shape[0], int((lines_[0][2] + lines_[1][2]) / 2), lines_[1][3], 0]
            x1, y1, x2, y2, null = error_line
            error_line[4] = (math.atan2(y2 - y1, x2 - x1) * 180 / np.pi)
            error = abs(error_line[4])-90
            print("Error Line: ", error_line)
            cv.line(img_lines, (x1, y1), (x2, y2), (100, 100, 255), 5)
            cv.putText(img_lines, " Error: "+str("{:.2f}".format(error)), (int((x2 + x1) / 2), int((y2 + y1) / 2)),
                       cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv.LINE_AA)
        elif vert_lines.__len__() == 1:
            print("Lines: ", lines_)
            error_line = lines_[0]
            x1, y1, x2, y2, null = error_line
            error_line[4] = (math.atan2(y2 - y1, x2 - x1) * 180 / np.pi)
            error = abs(error_line[4])
            print("Error Line: ", error_line)
            cv.line(img_lines, (x1, y1), (x2, y2), (100, 100, 255), 5)
            cv.putText(img_lines, " Error: " + str("{:.2f}".format(error)), (int((x2 + x1) / 2), int((y2 + y1) / 2)),
                       cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv.LINE_AA)

        for line in lines_:
            x1, y1, x2, y2, angle = line
            cv.line(img_lines, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv.putText(img_lines, " Line "+str(lines_.index(line)), (int((x2+x1)/2), int((y2+y1)/2)), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv.LINE_AA)
            cv.circle(img_lines, (x1, y1), 5, (255, 0, 0), -1)
            cv.putText(img_lines, str(x1)+", "+str(y1), (x1, y1), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv.LINE_AA)
            cv.circle(img_lines, (x2, y2), 5, (255, 255, 0), -1)
            cv.putText(img_lines, str(x2) + ", " + str(y2), (x2, y2), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2, cv.LINE_AA)
            print(angle)

        print(lines.shape)
        print(lines_.__len__())

    end = time.time()
    totalTime = end - start
    fps = 1 / totalTime
    totalTime_Ms = (totalTime * 1000)
    print("Time: ", str("{:.2f}".format(totalTime_Ms)) + "ms")
    print("FPS: ", fps)

    cv.putText(img, f'FPS: {int(fps)}', (20, 70), cv.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 0), 2)
    cv.imshow("image", img_lines)
    key = cv.waitKey(1) & 0xFF
    if key == ord("q"):
        break


plt.show()

cv.waitKey(0)

