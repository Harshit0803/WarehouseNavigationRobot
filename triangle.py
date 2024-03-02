# import required libraries
import math
import cv2
import os
import argparse
import apriltag
from picamera import PiCamera
from time import sleep
import io
from PIL import Image
import numpy as np
from collections import Counter
global checkTriangle
global x_coords
global p
global min_val, max_val

def triangleDetect():
       
  while(1):
#         
    global checkTriangle
    global x_coords
    global p
    global min_val, max_val


    camera = PiCamera()
    my_stream = io.BytesIO()

    camera.capture(my_stream, format='jpeg')  
    my_stream.seek(0)
    image = Image.open(my_stream)
    image.save('testcamera.jpg')
    
    # read the input image
    img = cv2.imread('testcamera.jpg')

    # resize image
    resized = img[100:1000, 300:1500]

    hsv_img = cv2.cvtColor(resized, cv2.COLOR_BGR2HSV)

    # Define the range of blue color in HSV
    lower_blue = np.array([70, 80, 25])
    upper_blue = np.array([250, 255, 255])
    
    # Create a mask of the blue regions
    mask = cv2.inRange(hsv_img, lower_blue, upper_blue)
    camera.close()
    
    # Apply the mask to the original image
    thresh1 = cv2.bitwise_and(resized, resized, mask=mask)
    gray = cv2.cvtColor(thresh1, cv2.COLOR_BGR2GRAY)

    os.remove('testcamera.jpg')
    
    print(26)
    # apply thresholding to convert the grayscale image to a binary image
    ret,thresh = cv2.threshold(gray,0,255,cv2.THRESH_BINARY)

    # find the contours
    _,contours,hierarchy = cv2.findContours(thresh, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    #     print("Number of contours detected:",len(contours))


    largest_area=0
    largest_contour = None

    #finding the triangle in the captured contours
    for cnt in contours:
       approx = cv2.approxPolyDP(cnt, 0.01*cv2.arcLength(cnt, True), True)
       area=cv2.contourArea(approx)
       if len(approx) == 3 and area > largest_area:
          resized = cv2.drawContours(resized, [cnt], -1, (0,125,255), 1)
          largest_area = area
          largest_contour = approx
          # compute the center of mass of the triangle
          M = cv2.moments(cnt)
          if M['m00'] != 0.0:
             x = int(M['m10']/M['m00'])
             y = int(M['m01']/M['m00'])
          cv2.putText(resized, 'Triangle', (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)

    #if the contour detected is not perfect, again start the whole process
    if(largest_contour is None):
        continue
    x_coords = largest_contour[:, 0, 0]
    
    # filter algo to remove any noise from small triangles that may have captured
    min_val = min(x_coords)
    max_val = max(x_coords)
    checkTriangle = abs(min_val - max_val)
    if checkTriangle>15:
        break

  num_pos = 0
  num_neg = 0
  
  # incrementing the count of turn and passing it along
  for elem in x_coords:
    diff = elem - x
    if diff > 0:
        num_pos += 1
    elif diff < 0:
        num_neg += 1
  


  if num_pos == 2:
    print("left")
    turn = 5
  else:
    print("right")
    turn = 4

  return turn
