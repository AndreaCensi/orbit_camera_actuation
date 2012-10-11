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
        plan = req.plan
        for cmd_index in plan:
            self.executeCommand(cmd_index)
        return 1
                    
    def getPlan(self):
        return self.plan

    def executeCommand(self, index):
        this_cmd = self.command_list[index]
            
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
        
        s = rospy.Service('/logitech_cam/executePlan', planCommand, self.handle_execute_plan)

        while not rospy.is_shutdown():
            line = sys.stdin.readline()
            if line[:16] == "set command_list":
                self.command_list = eval(line[17:])
            elif line[:16] == "get command_list":
                print('command_list = ', self.command_list)
            elif line[:6] == "home()":
                home_srv = rospy.ServiceProxy('/logitech_cam/home', voidService)
                home_srv(0)
            else:
                try:
                    if line[0] == '[':
                        plan = eval(line)
                        for i in plan:
                            if not i < len(self.command_list):
                                print('Invalid plan, %g not available' %i)
                                plan = []
                    else:
                        print('Reusing plan ', plan)
                except:
                    print('Error: could not interpret the input')
                    
                planExecute = rospy.ServiceProxy('/logitech_cam/executePlan', planCommand)
#                command = planCommand()
#                command.data = plan
                resp1 = planExecute(plan)
            
            

            
def get_msg(intarray):
    msg = IntArray()
    msg.data = intarray
    return msg

if __name__ == '__main__':
    print sys.argv
    prog = PlanExecuter()
    prog.main(sys.argv)