# NOTE: This script requires the following Python modules:  
#  pyserial - http://pyserial.sourceforge.net/  
#  pygame   - http://www.pygame.org/  
# Win32 users may also need:  
#  pywin32  - http://sourceforge.net/projects/pywin32/  
#  
import signal
from time import sleep
import sys
import threading
#import timer
import signal
import serial  
import pygame

global throttle
throttle = 0
global steering
steering = 127
global gear
gear = 1
NewCommand = 0
startMarker = 60
endMarker = 62

  
# allow multiple joysticks  
joy = []  
  
# Arduino USB port address (try "COM5" on Win32)  
usbport = "/dev/ttyACM1"  
  
# define usb serial connection to Arduino  
ser = serial.Serial(usbport, 57600)

# handle joystick event  
def handleJoyEvent(e):
    global throttle, steering, gear, NewCommand
    if e.type == pygame.JOYAXISMOTION:  
        axis = "unknown"  
        if (e.dict['axis'] == 0):  
            axis = "X"  
  
        if (e.dict['axis'] == 1):  
            axis = "Y"  
  
        if (e.dict['axis'] == 2):  
            axis = "Twist"  
  
        if (e.dict['axis'] == 3):  
            axis = "Throttle"
                
        if (e.dict['axis'] == 4):
            axis = "Camera 2"
                        
        if (e.dict['axis'] == 5):
            axis = "Camera 1"
                        
        if (e.dict['axis'] == 6):
            axis = "Slide"
  
        if (axis != "unknown"):  
            str = "Axis: %s; Value: %f" % (axis, e.dict['value'])  
            # uncomment to debug  
            output(str, e.dict['joy'])  
  
            # Steering to Servo 1
            if (axis == "X"):  
                pos = e.dict['value']  
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    steering = int(127 + abs(move))  
                else:  
                    steering = int(127 - move)
                NewCommand = 1
           
            #Throttle to Servo 2
            if (axis == "Y"):
                pos = e.dict['value']
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    throttle = int(127 + abs(move))  
                else:  
                    throttle = int(127 - move)
                NewCommand = 1
                    
            #Break to Servo 3
            if (axis == "Twist"):
                pos = e.dict['value']
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    servo = int(127 + abs(move))  
                else:  
                    servo = int(127 - move)
                # uncomment to debug  
                print "Twist Servo 3", servo
            
            #Throttle to servo 4
            if (axis == "Throttle"):
                pos = e.dict['value']
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    servo = int(127 + abs(move))  
                else:  
                    servo = int(127 - move) 
                print "Throttle Servo 4", servo
    
                
  
    elif e.type == pygame.JOYBUTTONDOWN:  
        str = "Button: %d" % (e.dict['button'])
        if (e.dict['button'] == 12):
            if (gear < 6):
                gear = gear + 1
                NewCommand = 1
        if (e.dict['button'] == 13):
            if (gear > 0):
                gear = gear -1
                NewCommand = 1
        if (e.dict['button'] == 3):
            ser.write(chr(255))
            ser.write(chr(4))
            ser.write(chr(0))
        # uncomment to debug  
        output(str, e.dict['joy'])  
        # Button 0 (trigger) to quit  
        if (e.dict['button'] == 0):  
            print "Bye!\n"  
            ser.close()  
            quit()  
    else:  
        pass
  
# print the joystick position  
def output(line, stick):  
    svada=0
    # print "Joystick: %d; %s" % (stick, line)  
  
# wait for joystick input  
def joystickControl():
    global throttle, steering, gear  
    while True:  
        e = pygame.event.wait()  
        if (e.type == pygame.JOYAXISMOTION or e.type == pygame.JOYBUTTONDOWN):  
            handleJoyEvent(e)
            sendControllerCommand()
            #confirmation = readControllerCommand()
            #print confirmation


def sendControllerCommand():
    global throttle, steering, gear, NewCommand
    command = "<" + str(throttle) + "," + str(steering) + "," + str(gear) + ">"
    print command
    ser.write(command)
    NewCommand = 0

def readControllerCommand():
  global startMarker, endMarker, messageFromArduino
  
  ck = ""
  x = "z" # any value that is not an end- or startMarker
  byteCount = -1 # to allow for the fact that the last increment will be one too many
  
  # wait for the start character
  while  ord(x) != startMarker: 
    x = ser.read()
  
  # save data until the end marker is found
  while ord(x) != endMarker:
    if ord(x) != startMarker:
      ck = ck + x 
      byteCount += 1
    x = ser.read()
  return("<" + ck + ">")

  
# main method  
def main():  
    # initialize pygame  
    pygame.joystick.init()  
    #pygame.display.init()
    #timer = threading.Timer(0.3, serialWrite, ())
    #timer.start()  # after 60 seconds, 'callback' will be called

    if not pygame.joystick.get_count():  
        print "\nPlease connect a joystick and run again.\n"  
        quit()  
    print "\n%d joystick(s) detected." % pygame.joystick.get_count()  
    for i in range(pygame.joystick.get_count()):  
        myjoy = pygame.joystick.Joystick(i)  
        myjoy.init()  
        joy.append(myjoy)  
        print "Joystick %d: " % (i) + joy[i].get_name()  
    print "Depress trigger (button 0) to quit.\n"  
  
    # run joystick listener loop  
    joystickControl()

    


  
# allow use as a module or standalone script  
if __name__ == "__main__":  
    main()  
