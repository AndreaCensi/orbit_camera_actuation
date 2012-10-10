#include "CreeperCam.h"

CreeperCam *camera;

int main(int argc, char **argv)
{
	camera = new CreeperCam("video0");
	camera->Reset();
}





