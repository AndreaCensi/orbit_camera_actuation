#include "ros/ros.h"
#include "camera_actuator/IntArray.h"

class CameraConfiguration {
	public:
		std::string device;
		int pan_min;
		int pan_max;
		int tilt_min;
		int tilt_max;
		int zoom_min;
		int zoom_max;
		int pan_state;
		int tilt_state;
		int zoom_state;
		double delay_pan, delay_tilt;
		double inertia_pan, inertia_tilt;
		CameraConfiguration(void);
		bool updateStateRelative(const camera_actuator::IntArray);
		void loadROSParameters(void);
		void setDefaultROSParameters(void);
		//void setParameters();
		bool validRelativeCommand(const camera_actuator::IntArray);
		void setZero(void);
		double timePan(int amount);
		double timeTilt(int amount);
};
