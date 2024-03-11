import cv2 as cv
import numpy as np
import glob
import codecs, json

chessboardsize = (6, 9)
framesize = (1280, 720)

# termination criteria
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((chessboardsize[0]*chessboardsize[1], 3), np.float32)
objp[:, :2] = np.mgrid[0:chessboardsize[0], 0:chessboardsize[1]].T.reshape(-1, 2)

size_of_chessboard_squares_mm = 20
objp = objp * size_of_chessboard_squares_mm

# Arrays to store object points and image points from all the images.
objpoints = []  # 3d point in real world space
imgpoints = []  # 2d points in image plane.

# Get the path of the images
images = glob.glob('calibration_images/webcam-genius/*.jpg')

for image in images:
    print(image)
    img = cv.imread(image)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv.findChessboardCorners(gray, chessboardsize, None)
    print(ret)

    # If found, add object points, image points (after refining them)
    if ret == True:
        objpoints.append(objp)

        corners2 = cv.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)

        imgpoints.append(corners2)

        # Draw and display the corners
        img = cv.drawChessboardCorners(img, chessboardsize, corners2, ret)
        cv.imshow('img', img)
        cv.waitKey(1)  # Add this line

cv.destroyAllWindows()

############## CALIBRATION #######################################################

ret, cameraMatrix, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, framesize, None, None)


calib = '{"cameraMatrix": [], "dist": []}'
calibData = json.loads(calib)
calibData["cameraMatrix"] = cameraMatrix.tolist()
calibData["dist"] = dist.tolist()

with open("camCalibrationData.json", "w") as cal:
    json.dump(calibData, cal)


############## UNDISTORTION #####################################################
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

img_ = cv.imread('calibration_images/test_image.jpg')
cv.imwrite('undistorted1.png', undistort_img(img_, cameraMatrix, dist))


# Reprojection Error
mean_error = 0

for i in range(len(objpoints)):
    imgpoints2, _ = cv.projectPoints(objpoints[i], rvecs[i], tvecs[i], cameraMatrix, dist)
    error = cv.norm(imgpoints[i], imgpoints2, cv.NORM_L2)/len(imgpoints2)
    mean_error += error

print( "total error: {}".format(mean_error/len(objpoints)) )


