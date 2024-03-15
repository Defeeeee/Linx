import cv2 as cv
import numpy as np
import glob
import codecs, json


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


load_cam_calib_data('camCalibrationData.json')
img = cv.imread('calibration_images/test_image_juan.jpg')
cv.imwrite('undistorted1.png', undistort_img(img, cameraMatrix, dist))
cv.imshow('undistorted', cv.imread('undistorted1.png'))
cv.imshow('normal', img)
cv.waitKey(0)