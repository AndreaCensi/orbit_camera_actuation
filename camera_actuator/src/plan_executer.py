#!/usr/bin/env python
from camera_actuator.msg import IntArray
from camera_actuator.srv import voidService
from camera_actuator.srv import planCommand
import roslib 
import rospy
import sys
roslib.load_manifest('camera_actuator')

class PlanExecuter:
    command_list = [[256, 0, 0], [-256, 0, 0], [0, 256, 0], [0, -256, 0]]
# Generates a IntArray message with the command data given in intarray
        
    def sendCommand(self, data):
        self.cmd_publisher.publish(get_msg(data))
    
    def planCallback(self, msg):
        self.plan = msg.data
        
    def handle_execute_plan(self, req):
        
        self.executePlan(req.plan)        
        
    def getPlan(self):
        return self.plan

    def executeCommand(self, index):
        this_cmd = self.command_list[cmd_index]
            
        print 'Sending command:     ',this_cmd
        self.cmd_publisher.publish(get_msg(this_cmd))
        try:
            cameraBusy = rospy.ServiceProxy('/logitech_cam/cameraBusy', voidService)
            cameraBusy(1)
            print('Device is ready for next command')
        except rospy.ServiceException, e:
            print('Unexpected Error: rospy.ServiceException')
            
    def executePlan(self):
        print('Executing plan: ', self.plan )
        for cmd_index in self.getPlan():
            executeCommand(cmd_index)
        
    def main(self, args):
        print('main()')
        if len(args)>1:
            self.plan = eval(args[-1])
        rospy.init_node('plan_executer', anonymous=True)
        self.cmd_publisher = rospy.Publisher('/logitech_cam/camera_instr', IntArray)
        
        s = rospy.Service('/logitech_camera/executePlan', planCommand, self.handle_execute_plan)
        
        sys.stdin.readline()

        self.executePlan()

        while not rospy.is_shutdown():        
            rospy.spin()
            

            
def get_msg(intarray):
    msg = IntArray()
    msg.data = intarray
    return msg

if __name__ == '__main__':
    print sys.argv
    prog = PlanExecuter()
    prog.main(sys.argv)