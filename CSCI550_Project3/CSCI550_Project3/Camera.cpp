// Sandy Hill,  for ECE661
// a Camera class for flying through 3D scenes.
#include "stdafx.h"
#include <windows.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>
#include <math.h>
#include "Camera.h" 

//<<<<<<<<<<<<<<<<<<<<<<< default constructor >>>>>>>>>>>>>>>>>
void Camera :: setDefaultCamera(void)
{ // make default camera
	//setShape(75.0f, 640/(float)480, 0.1f, 200.0f); // good default values here
	//set(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0, 1, 0);
}
//<<<<<<<<<<<<<<<<<<< setDisplay >>>>>>>>>>>>
void Camera :: setDisplay(int numRows, int numCols)
{ // tell camera screen size
	nRows = numRows; 	nCols = numCols;
}
//<<<<<<<<<<<<<<<<<<<< set >>>>>>>>>>>>>>>>>>>
void Camera :: set(Point3 Eye, Point3 look, Vector3 up)
{ // make u, v, n vectors
	eye = Eye;
	n = Vector3(eye.x - look.x, eye.y - look.y,eye.z - look.z);
	u = up.cross(n);
	v = n.cross(u);
	u.normalize(); v.normalize(); n.normalize();
	setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<<< set - takes 9 params >>>>>>>>>>>>>>>>>
void Camera :: set(float ex, float ey, float ez, float lx, float ly, float lz, float upx, float upy, float upz)
{
	Point3 e(ex, ey, ez);
	Point3 look(lx, ly, lz);
	Vector3 Up(upx, upy, upz);
	set(e,look, Up);
}
//<<<<<<<<<<<<<<<< setModelViewMatrix >>>>>>>>>>>>>>>>
void Camera :: setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	float m[16];
	Vector3 eVec(eye.x, eye.y, eye.z);

	m[0] =  u.x; m[4] =  u.y; m[8]  =  u.z;  m[12] = -u.dot(eye);
	m[1] =  v.x; m[5] =  v.y; m[9]  =  v.z;  m[13] = -v.dot(eye);
	m[2] =  n.x; m[6] =  n.y; m[10] =  n.z;  m[14] = -n.dot(eye);
	m[3] =  0;   m[7] =  0;   m[11] =  0;    m[15] = 1.0;	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	doLighting();
	glMultMatrixf(m);	
}

void Camera::doLighting(){
	glEnable( GL_LIGHTING );	
	//Head light
	glLightfv( GL_LIGHT1, GL_AMBIENT, light0_amb);
	glLightfv( GL_LIGHT1, GL_DIFFUSE, light0_dif);
	glLightfv( GL_LIGHT1, GL_SPECULAR, light0_spec);
	glLightfv( GL_LIGHT1, GL_POSITION, light0_pos);	
	glEnable(GL_COLOR_MATERIAL);
	if (b_HeadLight_On){
		glEnable(GL_LIGHT1);
	} 
	else{
		glDisable(GL_LIGHT1);
	}
	
	// Sun light
	glPushMatrix();				
		glLightfv( GL_LIGHT0, GL_AMBIENT, light0_amb);
		glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_dif);

		glLightfv( GL_LIGHT0, GL_SPECULAR, light0_spec);
		glLightfv( GL_LIGHT0, GL_POSITION, light0_pos);
		if (b_SunLight_On){
			glEnable(GL_LIGHT0);
		} 
		else{
			glDisable(GL_LIGHT0);
		}		
		glEnable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

//<<<<<<<<<<<<<<<<<<<<<< setProjectionMatrix >>>>>>>>>>>>>
void Camera:: setProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	gluPerspective(viewAngle, aspect, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW); // set its mode back again
}
//<<<<<<<<<<<<<<<<<<<<<<< setShape >>>>>>>>>>>>>>>>
void Camera :: setShape(float vAngle, float asp, float nr, float fr)
{ // load projection matrix and camera values
	viewAngle = vAngle; // viewangle in degrees - must be < 180
	aspect = asp; nearDist = nr; farDist = fr;
	H = nearDist * tan(3.14159265 * viewAngle/(2.0 * 180));
	W = aspect * H;
	setProjectionMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< setAspect >>>>>>>>>>>>>>>>>>>>>>
void Camera:: setAspect(float asp)
{
	aspect = asp; W = aspect * H;
  	setShape(viewAngle,aspect,nearDist,farDist);
}
//<<<<<<<<<<<<<<<<<<<<<<< getShape >>>>>>>>>>>>>>>>
void Camera :: getShape(float& vAngle, float& asp, float& nr, float& fr)
{
	vAngle = viewAngle;	asp = aspect;
	nr = nearDist; fr = farDist;
}
//<<<<<<<<<<<<<<<<<<<<<< slide >>>>>>>>>>>>>>>>>>>>>>..
void Camera :: slide(double du, double dv, double dn)
{ // slide both eye and lookAt by amount du * u + dv * v + dn * n;
	eye.x += du * u.x + dv * v.x + dn * n.x;
	eye.y += du * u.y + dv * v.y + dn * n.y;
	eye.z += du * u.z + dv * v.z + dn * n.z;
	setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<< rotAxes >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void Camera:: rotAxes(Vector3& a, Vector3& b, float angle)
{ // rotate orthogonal vectors a (like x axis) and b(like y axia) through angle degrees
	float ang = 3.14159265/180*angle;
	float C = cos(ang), S = sin(ang);
	Vector3 t(C * a.x + S * b.x, C * a.y + S * b.y, C * a.z + S * b.z); // tmp for a vector
	b = Vector3(-S * a.x + C * b.x, -S * a.y + C * b.y,-S * a.z + C * b.z);
	a = t; // put tmp into a'
}
//<<<<<<<<<<<<<<<<<<<<<<<< roll >>>>>>>>>>>>>>>>>>>>>>>>>>>
void Camera :: roll(float angle)
{
	rotAxes(u, v, -angle); setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<< pitch >>>>>>>>>>>>>>>>>>>>>>>>>
void Camera :: yaw(float angle)
{
	rotAxes(n, u, -angle); setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<<< yaw >>>>>>>>>>>>>>>>>>>>>>>>
void Camera :: pitch(float angle)
{
	rotAxes(n, v, -angle); setModelViewMatrix();
}