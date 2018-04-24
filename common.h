#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include "utils.h"

#include<WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <vector>

#define OBJECT_SIZE 80.0
#define FINGERR_N 10
#define ENLARGE 50
#define POINTSIZE 50

//! トラックボール(マウスによる視点回転用)
extern rxTrackball g_tbView;

extern btAlignedObjectArray<btCollisionShape*>	g_vCollisionShapes;		//!< 剛体オブジェクトの形状を格納する動的配列
extern btSoftRigidDynamicsWorld* g_pDynamicsWorld;	//!< Bulletワールド
extern btSoftBodyWorldInfo g_softBodyWorldInfo;

extern SOCKET sock;
extern double finger_x[FINGERR_N];
extern double finger_y[FINGERR_N];
extern double finger_z[FINGERR_N];
extern double distal_x[FINGERR_N];
extern double distal_y[FINGERR_N];
extern double distal_z[FINGERR_N];
extern double intermediate_x[FINGERR_N];
extern double intermediate_y[FINGERR_N];
extern double intermediate_z[FINGERR_N];
extern double proximal_x[FINGERR_N];
extern double proximal_y[FINGERR_N];
extern double proximal_z[FINGERR_N];
extern double metacarpal_x[FINGERR_N];
extern double metacarpal_y[FINGERR_N];
extern double metacarpal_z[FINGERR_N];
extern int finger_no[FINGERR_N];

extern double finger2_x[FINGERR_N];
extern double finger2_y[FINGERR_N];
extern double finger2_z[FINGERR_N];

extern int sendNum;

extern const GLfloat RX_LIGHT0_POS[4];
extern const GLfloat RX_LIGHT1_POS[4];
extern const GLfloat RX_LIGHT_DIFF[4];
extern const GLfloat RX_LIGHT_SPEC[4];
extern const GLfloat RX_LIGHT_AMBI[4];


#endif