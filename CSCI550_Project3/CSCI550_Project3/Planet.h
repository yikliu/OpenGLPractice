////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Class: Planet: (including the sun) for draw planets, orbits, calculating positions. 
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// 
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <stdio.h>
#include <stdlib.h>                         // Header File For Standard Input/Output ( NEW )

#include  <gl\glut.h>                            // Header File For The GLu32 Library
#include  <gl\glaux.h>
#include "myColor.h"
#include "Vector3.h"
#include <string>


using namespace std;
#define PI 3.14159265358


extern float material_shininess[];
extern float material_amb[];
extern float material_dif[];
extern float material_specular[];
extern float light_position[];

class Planet
{
public:
	Planet(int order, float radius, float orbitRadius, float orbitTilt,
		   float obitInc, float rotInc,
		   float red, float green, float blue,TCHAR* szFileName ):
		_order(order), _radius(radius), _orbitRadius(orbitRadius), _orbitTilt(orbitTilt),
		_orbitIncrement(obitInc),_rotationIncrement(rotInc),
		_curOrbitAngle(0.0f),_curRotationAngle(0.0f),_fName(szFileName){
		pColor = new myColor(red,green,blue);
		loadTexture();
	};

	Vector3 getCurrentPos();
	void updateAngle();

	void loadTexture();

	void draw();

	void drawOrbit();

	virtual ~Planet(void);
public:
	int _order;
	
	float _radius;

	float _orbitRadius;

	float _orbitTilt;

	float _orbitIncrement;
	float _rotationIncrement;

	float _curOrbitAngle;
	float _curRotationAngle;

	TCHAR* _fName;
	AUX_RGBImageRec *textures; 
	GLuint texture_id;
	
	myColor* pColor;
};

