# importing libraries
import cv2
import numpy as np
import RPi.GPIO as GPIO
import os
import argparse
import apriltag
from picamera import PiCamera
from time import sleep
import io
from PIL import Image


def nomansland():
    # Set up video capture from Raspberry Pi camera
    cap = cv2.VideoCapture(0)


    # Set up GPIO pin for LED
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(18, GPIO.OUT)
    
    #setting up april tag options
    options = apriltag.DetectorOptions(families= "tag36h11, tag25h7")
    detector = apriltag.Detector(options)

    while True:
        # Read a frame from the camera
        ret, frame = cap.read()
        
        height,width,_ = frame.shape
        
        c_x = width//2
        c_y = height//2
        
        c_p = frame[c_y-100:c_y+100, c_x-18:c_x+18]
        
        # detecting april tag only in the center pixel and iterating over that
        gray = cv2.cvtColor(c_p, cv2.COLOR_BGR2GRAY)
        
        results = detector.detect(gray)
        tag_list = []
        
        for r in results:
            # extract the bounding box (x, y)-coordinates for the AprilTag
            # and convert each of the (x, y)-coordinate pairs to integers
            (ptA, ptB, ptC, ptD) = r.corners
            ptB = (int(ptB[0]), int(ptB[1]))
            ptC = (int(ptC[0]), int(ptC[1]))
            ptD = (int(ptD[0]), int(ptD[1]))
            ptA = (int(ptA[0]), int(ptA[1]))
            # draw the bounding box of the AprilTag detection
            cv2.line(frame, ptA, ptB, (0, 255, 0), 2)
            cv2.line(frame, ptB, ptC, (0, 255, 0), 2)
            cv2.line(frame, ptC, ptD, (0, 255, 0), 2)
            cv2.line(frame, ptD, ptA, (0, 255, 0), 2)
            # draw the center (x, y)-coordinates of the AprilTag
            (cX, cY) = (int(r.center[0]), int(r.center[1]))
            cv2.circle(frame, (cX, cY), 5, (0, 0, 255), -1)
            # draw the tag family on the image
            tagFamily = r.tag_family.decode("utf-8")
            tag_list.append(tagFamily)
            if len(tag_list) == 1:
                break

        if len(tag_list) == 1:
            break

        # Display the frame
        cv2.imshow('frame', frame)

        # Exit the loop if the 'q' key is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        

    # Release the video capture and close the window
    cap.release()
    cv2.destroyAllWindows()
