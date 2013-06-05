#include "StdAfx.h"
#include "Planet.h"

void Planet::updateAngle()
{
	if(_order != 0){
		_curOrbitAngle = _curOrbitAngle + _orbitIncrement;
			while(_curOrbitAngle > 360){
				_curOrbitAngle -= 360.0;
		}
	}

	_curRotationAngle = _curRotationAngle + _rotationIncrement;
	while (_curRotationAngle > 360)
	{
		_curRotationAngle -= 360.0;
	}	
}

Vector3 Planet::getCurrentPos(){	
	Vector3 v;
	if(_order == 0){ //Sun position is at origin
		v = Vector3(0,0,0);
	}else if (_order >= 1 && _order <= 4) //Mercury,Venus,Earth,Mars
	{
		//All orbit tilt angle is about x-axis, if the tilt is about y-axis, the following formula no 
		//longer holds.
		v.x = _orbitRadius * cos(_curOrbitAngle*PI/180);		
		v.y = _orbitRadius * cos(_orbitTilt *PI/180)* sin(_curOrbitAngle*PI/180);
		v.z = _orbitRadius * sin(_orbitTilt*PI/180)* sin(_curOrbitAngle*PI/180);
	}else // Moons' position are not supported yet.
	{
		printf("/n Moon's positions are not available.");
		exit(0);
	}
	return v;
}

Planet::~Planet(void)
{
	delete pColor;
	pColor = NULL;
}

void Planet::loadTexture(){
	// Load Textures
	textures = auxDIBImageLoadA(_fName);
	if (!textures)
	{
		printf("\nError(LoadGLTextures-1) Texture %s Not Found : ",
			_fName);
		exit(1);
	}
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 
		textures->sizeX, textures->sizeY, 
		0, GL_RGB, GL_UNSIGNED_BYTE, textures->data);
}

void Planet::draw(){	
	glBindTexture(GL_TEXTURE_2D, texture_id);
	GLUquadricObj *sphere;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);	
	gluQuadricTexture(sphere, GL_TRUE);	
	gluSphere(sphere, _radius, 50, 50);
	gluDeleteQuadric(sphere);
}

void Planet::drawOrbit(){
	GLdouble rad_angle;
	GLint i = 0;
	if( _order != 0 ) {   
		glColor3f(1.0,1.0,1.0);
		rad_angle = 2.0*PI/(80.0+_order*10.0);
		glBegin(GL_LINE_LOOP);
		  glVertex2f(_orbitRadius,0.0);  
		  for( i=2 ; i<=(80+_order*10) ; i++ )
			glVertex2f(_orbitRadius*cos(i*rad_angle),
					   _orbitRadius*sin(i*rad_angle));
		glEnd();	
	}
}
