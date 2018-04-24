#ifndef AR_SYSTEM_H
#define AR_SYSTEM_H

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/arMulti.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>

extern char *vconf_name;
extern char *cparam_name;
extern char *config_name;
extern ARMultiMarkerInfoT *config;
extern int xsize, ysize;


class arSystem{
	ARParam cparam;
	ARParam wparam;

public:
	int arInit();
	void Cleanup();
	void InitGL();

};

#endif