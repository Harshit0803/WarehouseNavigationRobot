#import header files

import os
import cv2
import argparse
import apriltag
from picamera import PiCamera
from time import sleep
import io
from PIL import Image
import numpy as np



def aprilTag():

# create an object of the camera and take an image and save it
    camera = PiCamera()

    my_stream = io.BytesIO()
    camera.capture(my_stream, format='jpeg')
    my_stream.seek(0)
    image = Image.open(my_stream)
    image.save('testcamera.jpg')

# read the image, and extract the april tag params from the image
    print("[INFO] loading image...")
    image1 = cv2.imread('testcamera.jpg')
    image = cv2.resize(image1, (800, 800))
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    os.remove('testcamera.jpg')
    print("[INFO] detecting AprilTags...")
    options = apriltag.DetectorOptions(families= "tag36h11, tag25h7")
    detector = apriltag.Detector(options)
    results = detector.detect(gray)
    print("[INFO] {} total AprilTags detected".format(len(results)))
    tag_list = []

    # loop over the AprilTag detection results
    for r in results:
        # extract the bounding box (x, y)-coordinates for the AprilTag
        # and convert each of the (x, y)-coordinate pairs to integers
        (ptA, ptB, ptC, ptD) = r.corners
        ptB = (int(ptB[0]), int(ptB[1]))
        ptC = (int(ptC[0]), int(ptC[1]))
        ptD = (int(ptD[0]), int(ptD[1]))
        ptA = (int(ptA[0]), int(ptA[1]))
        # draw the bounding box of the AprilTag detection
        cv2.line(image, ptA, ptB, (0, 255, 0), 2)
        cv2.line(image, ptB, ptC, (0, 255, 0), 2)
        cv2.line(image, ptC, ptD, (0, 255, 0), 2)
        cv2.line(image, ptD, ptA, (0, 255, 0), 2)
        # draw the center (x, y)-coordinates of the AprilTag
        (cX, cY) = (int(r.center[0]), int(r.center[1]))
        cv2.circle(image, (cX, cY), 5, (0, 0, 255), -1)
        # draw the tag family on the image
        tagFamily = r.tag_family.decode("utf-8")
        tag_list.append(tagFamily)
        cv2.putText(image, tagFamily, (ptA[0], ptA[1] - 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
        print("[INFO] tag family: {}".format(tagFamily))
        
    defective = 0
    non_defective= 0
    values, counts = np.unique(tag_list, return_counts = True)
    if (len(counts != 0)):
        for iCount in range(len(values)):
            print("Tag: ", values[iCount], "Length: ", counts[iCount])
            
            if(values[iCount]=="tag25h7"):
                defective= counts[iCount]
                
            else:
                non_defective= counts[iCount]
                #LED
    else:
        print("No tags")
        
    print(defective)
    print(non_defective)
    
    # return the defective and non-defective count of the april tag
    arrayType = [defective, non_defective]
       
    
    camera.close()
    return arrayType
