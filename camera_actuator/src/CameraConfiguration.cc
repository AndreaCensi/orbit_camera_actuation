#include "ros/ros.h"
#include "CameraConfiguration.h"
#include <string>
#include "camera_actuator/IntArray.h"

CameraConfiguration::CameraConfiguration(void){
	this->device = "/logitech_camera";
	ROS_INFO("CameraConfiguration Constructed");
}

bool CameraConfiguration::validRelativeCommand(const camera_actuator::IntArray msg)
{
	int failed = 0;
	ROS_DEBUG("New state is: [%ld, %ld,  ]", this->pan_state + msg.data[0], this->pan_state + msg.data[1]);
	if(this->pan_state + msg.data[0] < this->pan_min){ROS_INFO("Failed: pan_min"); failed += 1;}
	if(this->pan_state + msg.data[0] > this->pan_max){ROS_INFO("Failed: pan_max"); failed += 1;}
	if(this->tilt_state + msg.data[1] < this->tilt_min){ROS_INFO("Failed: tilt_min"); failed += 1;}
	if(this->tilt_state + msg.data[1] > this->tilt_max){ROS_INFO("Failed: tilt_max"); failed += 1;}
	ROS_DEBUG("Failed conditions = %ld", failed);
	if(failed == 0){return true;}
	else{
		ROS_INFO("WARNING: requested pan state  = %ld", this->pan_state + msg.data[0]);
		ROS_INFO("WARNING: requested tilt state = %ld", this->tilt_state + msg.data[1]);
		return false;
	}
}
bool CameraConfiguration::updateStateRelative(const camera_actuator::IntArray msg)
{
	this->pan_state = this->pan_state + msg.data[0];
	this->tilt_state = this->tilt_state + msg.data[1];
}
double CameraConfiguration::timePan(int amount)
{
	return this->delay_pan + abs(amount)*this->inertia_pan;
}
double CameraConfiguration::timeTilt(int amount)
{
	return this->delay_tilt + abs(amount)*this->inertia_tilt;
}
void CameraConfiguration::setZero(void)
{
	this->pan_state = 0;
	this->tilt_state = 0;
}

void CameraConfiguration::loadROSParameters(void)
{
	std::string device = this->device;
	ros::NodeHandle nh;
	// Workspace parameters
	if(not nh.getParam((device + "/pan_min").c_str(), pan_min))
	{
		ROS_ERROR("Parameter pan_min does not exist.");
	}
	if(not nh.getParam((device + "/pan_max").c_str(), pan_max))
	{
		ROS_ERROR("Parameter pan_max does not exist.");
	}
	if(not nh.getParam((device + "/tilt_min").c_str(), tilt_min))
	{
		ROS_ERROR("Parameter tilt_min does not exist.");
	}
	if(not nh.getParam((device + "/tilt_max").c_str(), tilt_max))
	{
		ROS_ERROR("Parameter tilt_max does not exist.");
	}
	if(not nh.getParam((device + "/zoom_min").c_str(), zoom_min))
	{
		ROS_ERROR("Parameter zoom_min does not exist.");
	}
	if(not nh.getParam((device + "/zoom_max").c_str(), zoom_max))
	{
		ROS_ERROR("Parameter zoom_max does not exist.");
	}

	// Dynamics parameters
	if(not nh.getParam((device + "/delay_pan"), delay_pan))
	{
		ROS_ERROR("Parameter delay_pan does not exist.");
	}
	if(not nh.getParam((device + "/delay_tilt"), delay_tilt))
	{
		ROS_ERROR("Parameter delay_tilt does not exist.");
	}

	if(not nh.getParam((device + "/inertia_pan"), inertia_pan))
	{
		ROS_ERROR("Parameter inertia_pan does not exist.");
	}

	if(not nh.getParam((device + "/inertia_tilt"), inertia_tilt))
	{
		ROS_ERROR("Parameter inertia_tilt does not exist.");
	}

}

void CameraConfiguration::setDefaultROSParameters(void)
{
	std::string device = this->device;
	ROS_INFO("Set defaults starting");
	ros::NodeHandle nh;
	char* param = "/pan_max";
	std::string joined = device + "hej";
	ROS_INFO(joined.c_str());

	// Workspace parameters
	nh.setParam((device + "/pan_min").c_str(), -4000);
	nh.setParam((device + "/pan_max").c_str(), 4000);
	nh.setParam((device + "/tilt_min").c_str(), -1600);
	nh.setParam((device + "/tilt_max").c_str(), 1400);
	nh.setParam((device + "/zoom_min").c_str(), 100);
	nh.setParam((device + "/zoom_max").c_str(), 200);

	// Dynamics parameters
	ROS_DEBUG("Set dynamic params");
	nh.setParam((device + "/delay_pan").c_str(), 0.2);
	nh.setParam((device + "/delay_tilt").c_str(), 0.15);
	nh.setParam((device + "/inertia_pan").c_str(), .3/1000);
	nh.setParam((device + "/inertia_tilt").c_str(), .2/1000);
}
