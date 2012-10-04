#!/usr/bin/env python
from camera_actuator.msg import IntArray
from camera_actuator.srv import voidService
import roslib 
import rospy
import sys
roslib.load_manifest('camera_actuator')

# Generates a IntArray message with the command data given in intarray
def get_msg(intarray):
    msg = IntArray()
    msg.data = intarray
    return msg
    

def main(args):
    rospy.init_node('command_console', anonymous=True)
    cmd_publisher = rospy.Publisher('/logitech_cam/camera_instr',IntArray)
    this_cmd = [0, 0, 0]
    print("Command Console Started, you can now type commands of the form: [pan, tilt, zoom]")
    
    while not rospy.is_shutdown():
        line = sys.stdin.readline()
        if(line == '\n'):
            pass
        else:
            try:
                this_cmd = eval(line)
            except:
                print('Unexpected Error, could not parse input.')
        print 'Sending command:     ',this_cmd
        cmd_publisher.publish(get_msg(this_cmd))
        try:
            cameraBusy = rospy.ServiceProxy('/logitech_cam/cameraBusy', voidService)
            cameraBusy(1)
            print('Device is ready for next command')
        except rospy.ServiceException, e:
            print('Unexpected Error: rospy.ServiceException')
            
    

if __name__ == '__main__':
    main(sys.argv)