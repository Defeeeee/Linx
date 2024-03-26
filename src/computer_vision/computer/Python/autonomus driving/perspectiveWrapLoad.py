import cv2 as cv
import time
import numpy as np
from cv2 import aruco
import json

marker_dict = aruco.getPredefinedDictionary(aruco.DICT_5X5_250)

param_markers = aruco.DetectorParameters()

cap = cv.VideoCapture(0)

Des_marker_IDs = [[51], [52], [53], [54]]
while True:
    start = time.time()
    ret, frame = cap.read()
    if not ret:
        break
    gray_frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    marker_corners, marker_IDs, reject = aruco.detectMarkers(
        gray_frame, marker_dict, parameters=param_markers
    )
    centers = []
    marker_IDs_ref = []
    if marker_corners:
        for ids, corners in zip(marker_IDs, marker_corners):
            cv.polylines(
                frame, [corners.astype(np.int32)], True, (0, 255, 255), 4, cv.LINE_AA
            )
            corners = corners.reshape(4, 2)
            corners = corners.astype(int)
            top_right = corners[0].ravel()
            top_left = corners[1].ravel()
            bottom_right = corners[2].ravel()
            bottom_left = corners[3].ravel()
            center = [int((top_left[0] + bottom_left[0])/2), int((top_left[1] + bottom_left[1])/2)]
            centers.append(center)
            cv.circle(frame, center, 3, (0, 0, 255), -1)
            cv.putText(
                frame,
                f"id: {ids[0]}",
                top_right,
                cv.FONT_HERSHEY_PLAIN,
                1.3,
                (200, 100, 0),
                2,
                cv.LINE_AA,
            )
            marker_IDs_ref.append(ids[0])
        print("Marker IDs: ", marker_IDs)
        if marker_IDs.size == 4:
            img = frame.copy()
            marker_indexs = []
            true_centers = []
            print(centers)
            try:
                for did in Des_marker_IDs:
                    index = marker_IDs_ref.index(did)
                    print(index)
                    marker_indexs.append(index)
                    print(centers[index])
                    true_centers.append(centers[index])

                print(true_centers)
                corners = true_centers
                pts1 = [(corners[3][0], corners[3][1]), (corners[2][0], corners[2][1]), (corners[0][0], corners[0][1]), (corners[1][0], corners[1][1])]
                pts2 = [[0, 0], [0, 480], [640, 0], [640, 480]]
                matrix = cv.getPerspectiveTransform(np.float32(pts1), np.float32(pts2))
                result = cv.warpPerspective(frame, matrix, (640, 480))
                cv.polylines(
                    frame, [np.array(corners).astype(np.int32)], True, (0, 0, 255), 4, cv.LINE_AA
                )
                cv.imshow('result', result)
            except:
                pass
    end = time.time()
    totalTime = end - start
    fps = 1 / totalTime
    #print("FPS: ", fps)
    cv.putText(frame, f'FPS: {int(fps)}', (20, 70), cv.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 0), 2)
    cv.imshow("frame", frame)
    key = cv.waitKey(1)
    if key == ord("q"):
        break
    if key == ord("r") and marker_IDs.size == 4:
        unwrap_data = '{"centers": []}'
        unwrapData = json.loads(unwrap_data)
        unwrapData["centers"] = true_centers
        with open("unwrap_data.json", "w") as cal:
            json.dump(unwrapData, cal)

cap.release()
cv.destroyAllWindows()

cv.waitKey(0)