# NOTE: This script requires the following Python modules:  
#  pyserial - http://pyserial.sourceforge.net/  
#  pygame   - http://www.pygame.org/  
# Win32 users may also need:  
#  pywin32  - http://sourceforge.net/projects/pywin32/  
#  
import signal
import sys
import threading
import signal
import serial  
import pygame  
  
# allow multiple joysticks  
joy = []  
  
# Arduino USB port address (try "COM5" on Win32)  
usbport = "/dev/ttyACM2"  
  
# define usb serial connection to Arduino  
ser = serial.Serial(usbport, 115200)  
  
# handle joystick event  
def handleJoyEvent(e):  
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
  
            # X to Servo 1
            if (axis == "X"):  
                pos = e.dict['value']  
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    servo = int(127 + abs(move))  
                else:  
                    servo = int(127 - move)
                # convert position to ASCII character  
                servoPosition = chr(servo)
                #Send staring byte
                ser.write(chr(255))
                # send to servo 1
                ser.write(chr(1))
                # and send to Arduino over serial connection  
                ser.write(servoPosition)  
                # uncomment to debug  
                print "X Servo 1", servo
           
            #Throttle to Servo 2
            if (axis == "Y"):
                pos = e.dict['value']
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    servo = int(127 + abs(move))  
                else:  
                    servo = int(127 - move)  
                # convert position to ASCII character  
                servoPosition = chr(servo)
                print servo  
                #Send staring byte
                ser.write(chr(255))
                # sends direction
                #if (pos>0) :
                ser.write(chr(2))
                #else :
                #    ser.write(chr(2))
                # and send to Arduino over serial connection  
                ser.write(servoPosition)  
                # uncomment to debug  
                print "Y Servo 2", servo
                    
            #Twsit to Servo 3
            if (axis == "Twist"):
                pos = e.dict['value']
                # convert joystick position to servo increment, 0-180  
                move = round(pos * 127, 0)  
                if (pos < 0):  
                    servo = int(127 + abs(move))  
                else:  
                    servo = int(127 - move)
                # convert position to ASCII character  
                servoPosition = chr(servo)
                #Send staring byte
                #ser.write(chr(255))
                # send to servo 1
                #ser.write(chr(3))
                # and send to Arduino over serial connection  
                #ser.write(servoPosition)  
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
                # convert position to ASCII character  
                servoPosition = chr(servo)
                #Send staring byte
                #ser.write(chr(255))
                # send to servo 1
                #ser.write(chr(4))
                # and send to Arduino over serial connection  
               # ser.write(servoPosition)  
                # uncomment to debug  
                print "Throttle Servo 4", servo
    
                
  
    elif e.type == pygame.JOYBUTTONDOWN:  
        str = "Button: %d" % (e.dict['button'])
        if (e.dict['button'] == 12):
            ser.write(chr(255))
            ser.write(chr(12))
            ser.write(chr(1))
        if (e.dict['button'] == 13):
            ser.write(chr(255))
            ser.write(chr(13))
            ser.write(chr(1))
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
    print "Joystick: %d; %s" % (stick, line)  
  
# wait for joystick input  
def joystickControl():  
    while True:  
        e = pygame.event.wait()  
        if (e.type == pygame.JOYAXISMOTION or e.type == pygame.JOYBUTTONDOWN):  
            handleJoyEvent(e)  
  
# main method  
def main():  
    # initialize pygame  
    pygame.joystick.init()  
    pygame.display.init()  
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
