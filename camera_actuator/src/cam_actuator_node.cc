#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sensor_msgs/Image.h>
#include "CreeperCam.h"
#include "camera_actuator/voidService.h"
#include "camera_actuator/imageService.h"
#include "camera_actuator/CamCmd.h"
#include "camera_actuator/IntArray.h"
#include <cmath>
#include "CameraConfiguration.h"
#include <sstream>
#include <ctime>
#include <unistd.h>
#include <string>
CreeperCam *camera;
CameraConfiguration *camconf;
ros::Publisher pub;

/**
 * Callback function for recieved IntArray of cammands
 * Message is assumed to have the form [Pan, Tilt, [Zoom]] as relative motion.
 * Zoom is ignored by the camera actuator node.
 *
 * Adam 7/10/12
 */
double reserved_time = 0.0;
int image_requested = 0;
sensor_msgs::Image image;
void receive_image(const sensor_msgs::Image msg)
{
//	ROS_INFO("image_requested %d", image_requested);
//	if(image_requested > 0)
//	{
//		ROS_INFO("Taking image");
		image = msg;
//		image_requested = 0;
//	}
}
bool take_image(camera_actuator::imageService::Request &req,
		camera_actuator::imageService::Response &res)
{
	ROS_INFO("Try to take image");
//	image_requested = 1;
//	usleep(500000);
//	while(image_requested){
//		usleep(500000);
//		ROS_INFO("Wating for image");
//	}
	try{
		res.image = image;
		return true;
	}
	catch(char *str){
		return false;
	}
}
void receive_callback(const camera_actuator::IntArray msg)
{
	ROS_INFO("Received command");
	double now = ros::Time::now().toSec();
	if(now >= reserved_time)
	{
		ROS_INFO("Camera Idle, checking workspace....");

		ROS_INFO("time in ms = %f", now);
		if(camconf->validRelativeCommand(msg)){
			ROS_INFO("Executing command");
			pub.publish(msg);

			double time_tilt = camconf->timeTilt(msg.data[1]);
			double time_pan = camconf->timePan(msg.data[0]);
			double est_time = time_pan + time_tilt;
			reserved_time = now + est_time;

			camera->TiltRelative(msg.data[1]);
			camera->stall(time_tilt);
			camera->PanRelative(msg.data[0]);

			camconf->updateStateRelative(msg);
		}
		else{
			ROS_INFO("Ignoring invalid command, OutOfBound");
		}
	}
	else{
		ROS_INFO("Ignoring invalid command, Device Busy. %f < %f", now, reserved_time);
	}
}
bool waitIdel(camera_actuator::voidService::Request &req,
		camera_actuator::voidService::Response &res)
{
	//
	// <returns> time until device is idle
	//
	double now = ros::Time::now().toSec();
	if(now >= reserved_time)
	{
		return true;
	}
	else{
		while(reserved_time > now){
			now = ros::Time::now().toSec();
		}
		return true;
	}
}
void home()
{
	ROS_INFO("Going to home");
	int startup_time = 3;
	camera->Reset();
	camera->stall(startup_time);
	camconf->setZero();
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "CameraController");

	ros::NodeHandle n;
	ros::Rate loop_rate(10);

	int count = 0;

	camconf = new CameraConfiguration();
	camconf->setDefaultROSParameters();
	camconf->loadROSParameters();


	ros::NodeHandle nh;
	std::string dev;
	nh.getParam("/device", dev);
//	ROS_INFO(dev);

	camera = new CreeperCam(dev.c_str());
	home();

	pub = n.advertise<camera_actuator::IntArray>("/logitech_cam/camera_executed",1000);
	ros::Subscriber sub = n.subscribe("/logitech_cam/camera_instr",1000, receive_callback);
	ros::ServiceServer service = n.advertiseService("/logitech_cam/cameraBusy", waitIdel);

	ros::Subscriber sub_image = n.subscribe("/usb_cam/image_raw",1000, receive_image);
	ros::ServiceServer service_image = n.advertiseService("/logitech_cam/take_image", take_image);
	ROS_INFO("Camera Ready to take command");

	ros::spin();

	camera->close();
	delete[] camera;

	return 0;
}
