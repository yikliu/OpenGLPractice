////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Class: Camera: based on work by source code by book.
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// 
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#ifndef _HlCamera
#define _HlCamera

#include "Vector3.h"

typedef Vector3 Point3;

extern float light0_amb[];
extern float light0_dif[];
extern float light0_spec[];
extern float light0_pos[];
extern bool b_HeadLight_On;
extern bool b_SunLight_On;

class Camera{
//private:
public:
	Point3 eye;
	Vector3 u, v, n;
	float aspect, nearDist, farDist, viewAngle, W, H;
	void setModelViewMatrix();
public:
	int nRows, nCols;
	Camera()
	{ // make default camera
	   setDefaultCamera();
	}

	void doLighting(void);
	void printCamera(void);
	void roll(float angle);
	void pitch(float angle);
	void yaw(float angle);
	void slide(double du, double dv, double dn);
	void set(Point3 Eye,  Point3 look, Vector3 up);
	void set(float ex, float ey, float ez, float lx, float ly, float lz, float upx, float upy, float upz);
	void setDisplay(int numRows, int numCols);
	void setProjectionMatrix();
	void setShape(float viewAngle, float aspect, float Near, float Far);
	void setAspect(float asp);
	void getShape(float& viewAngle, float& aspect, float& Near, float& Far);
	void rotAxes(Vector3& a, Vector3& b, float angle);
	void setDefaultCamera();
	//void drawOpenGL(Scene& scn); //draw whole scene using OpenGL tools
	//void raytrace(Scene& scn, int blockSize); // raytrace the scene
};
#endif