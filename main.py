import RPi.GPIO as GPIO
import time
from triangle import triangleDetect
from apriltagtry2 import aprilTag
from nomansland_try import nomansland

import math
import cv2
import os
import argparse
import apriltag
from picamera import PiCamera

import io
from PIL import Image
import numpy as np


#prop to pi
GPIO.setmode(GPIO.BCM)
GPIO.setup(26, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)


# Define the GPIO pins for each segment of the display
a = 9 #d
b = 10
c = 6 #d
d = 13 #d
e = 19 #d
f = 11 #d
g = 0 #d
dp = 5 #d

# Define the numbers to display and their corresponding segments
numbers = {
    0: [a, b, c, d, e, f],
    1: [b, c],
    2: [a, b, g, e, d],
    3: [a, b, c, d, g],
    4: [b, c, f, g],
    5: [a, c, d, f, g],
    6: [a, c, d, e, f, g],
    7: [a, b, c],
    8: [a, b, c, d, e, f, g],
    9: [a, b, c, d, f, g],
    '.':[dp]
}

GPIO.setup([a, b, c, d, e, f, g, dp], GPIO.OUT)
GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)


GPIO.setup([14, 15], GPIO.OUT)
GPIO.output([14, 15], GPIO.LOW)

#pi to prop
GPIO.setup(2, GPIO.OUT)
GPIO.output(2, GPIO.LOW)
GPIO.setup(3, GPIO.OUT)
GPIO.output(3, GPIO.LOW)

defective_count = 0
nonDefective_count = 0
   
#iterating over 3 turns
for count in range(3):

    while(GPIO.input(26) == GPIO.LOW):
        print("Pause")
    
    #reading the triangle orientation
    if(GPIO.input(26) == GPIO.HIGH):
        sig1=triangleDetect()
        
    if(sig1==5): #Left
        GPIO.output(2, GPIO.HIGH)
        time.sleep(3)
        GPIO.output(2, GPIO.LOW)
    elif(sig1==4): #Right
        GPIO.output(3, GPIO.HIGH)
        time.sleep(3)
        GPIO.output(3, GPIO.LOW)

    # reading the april tag
    while(GPIO.input(26) == GPIO.LOW):
        print("Pause_April1")
        
    if(GPIO.input(26) == GPIO.HIGH):
        sig2=aprilTag()
        for iDef in range(sig2[0]):
            GPIO.output(14, GPIO.HIGH)
            time.sleep(1)
            GPIO.output(14, GPIO.LOW)
            
        for iNDef in range(sig2[1]):
            GPIO.output(15, GPIO.HIGH)
            time.sleep(1)
            GPIO.output(15, GPIO.LOW)   
        
        GPIO.output(2, GPIO.HIGH)
        time.sleep(3)
        GPIO.output(2, GPIO.LOW)
        print(sig2)
        defective_count = defective_count + sig2[0]
        nonDefective_count = nonDefective_count + sig2[1]

    # reading the april tag for the opposite side
    while(GPIO.input(26) == GPIO.LOW):
        print("Pause_April2")
        
    if(GPIO.input(26) == GPIO.HIGH):
        sig2=aprilTag()
        for jDef in range(sig2[0]):
            GPIO.output(14, GPIO.HIGH)
            time.sleep(1)
            GPIO.output(14, GPIO.LOW)
            
        for jNDef in range(sig2[1]):
            GPIO.output(15, GPIO.HIGH)
            time.sleep(1)
            GPIO.output(15, GPIO.LOW)   
        
        GPIO.output(2, GPIO.HIGH)
        time.sleep(3)
        GPIO.output(2, GPIO.LOW)
        print(sig2)
        defective_count = defective_count + sig2[0]
        nonDefective_count = nonDefective_count + sig2[1]
        
        

        

#no man's land
while(GPIO.input(26) == GPIO.LOW):
    print("Pause")

if(GPIO.input(26) == GPIO.HIGH):
    sig1=triangleDetect()
    
if(sig1==5): #Left
    GPIO.output(2, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(2, GPIO.LOW)
elif(sig1==4): #Right
    GPIO.output(3, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(3, GPIO.LOW)
time.sleep(3)

nomansland()
GPIO.output(2, GPIO.HIGH)
time.sleep(3)
GPIO.output(2, GPIO.LOW)

time.sleep(6)

print("Defective: ",defective_count)
print("Non-Defective: ",nonDefective_count)

# GPIO pin for red
for i in range(3):

    GPIO.output(14, GPIO.HIGH)


    if defective_count > 9:
        temp_def1 = defective_count/10
        segments = numbers[temp_def1]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(1.5)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)
        temp_def2 = defective_count%10
        segments = numbers[temp_def2]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(1.5)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)
        
    else:
        segments = numbers[defective_count]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(3)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)

    # time.sleep(1)
    GPIO.output(14, GPIO.LOW)

    time.sleep(5)

    # GPIO pin for green

    # time.sleep(1)
    GPIO.output(15, GPIO.HIGH)

    if nonDefective_count > 9:
        temp_nonDef1 = nonDefective_count/10
        segments = numbers[temp_nonDef1]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(1.5)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)
        temp_def2 = defective_count%10
        segments = numbers[temp_def2]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(1.5)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)
        
    else:
        segments = numbers[nonDefective_count]
        for segment in segments:
            GPIO.output(segment, GPIO.HIGH)
        time.sleep(3)
        GPIO.output([a, b, c, d, e, f, g, dp], GPIO.LOW)

    GPIO.output(15, GPIO.LOW)




GPIO.cleanup()

