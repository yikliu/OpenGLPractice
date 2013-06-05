////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Title: CSCI550 Project 3 : Solar System
// Date: Nov.10th, 2011
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// 
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <vector>
using namespace std;
#include "mycolor.h"
#include "Vector3.h"
#include "Camera.h"
#include "Planet.h"

//Menu entry keys for colors
#define BLACK 0
#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define ORANGE 5
#define GREY 6

//Menu entry keys for polygon mode.
#define LINE 1
#define FILL  2 

//Menu entry keys for rubberbanding 
#define ON 1
#define OFF 2

#define SCALE_FACTOR 20

static void SpecialKeyFunc( int Key, int x, int y );
void rotating(int i);

Camera * aCamera;

static bool g_bIsAnimating = true;

static float g_SlideFactor = -10.0;
static float g_PitchFactor = 0.0;
static float g_YawFactor = 0.0;
static float g_RollFactor = 0.0;
static float g_zoomFactor = 1.0;


//set the window width/height constant, so is the aspect ratio
//Thus view plane is pre-defined as [0, g_Win_Width, 0, g_Win_Height] and stay constant.
static const GLint g_Win_Width = 100;
static const GLint g_Win_Height = 100;


//screen width/height, initial value is 1024x768
static GLint g_ScreenWidth = 1024;
static GLint g_ScreenHeight = 600;
static GLdouble g_Aspect_Ratio = g_ScreenWidth/g_ScreenHeight;
//drawing background color, initial color is black
static myColor g_ClearColor(0,0,0);

//Pointers to planets.
static Planet* pSun; 
static Planet* pMercury;
static Planet* pVenus;
static Planet* pEarth;
static Planet* pMars;
static Planet* pEarthMoon, *pMarsMoon1, *pMarsMoon2;


// Sun is yellow
static myColor g_SunColor(1,1,0);
static float   g_SunRadius = 2.0;
static float   g_SunRotateIncrement = 10.0;

// Earth is blue 
static myColor g_EarthColor(0,0,1);
static float   g_EarthRadius = 1;
static float   g_EarthOrbitRadius = 8;
static float   g_EarthOrbitTilt = 0.0;
static float   g_EarthOrbitIncrement = 1.0; 
static float   g_EarthRotateIncrement = 1.0;

// Mercury is silver
static myColor g_MercuryColor(0.909803922,0.909803922,0.980392157);
static float   g_MercuryRadius = 0.6;
static float   g_MercuryOrbitRadius = 3;
static float   g_MercuryOrbitTilt = 15.0;
static float   g_MercuryOrbitIncrement = 4; 
static float   g_MercuryRotateIncrement = 10.0;


// Venus is gold 
static myColor g_VenusColor(0.803921569,0.498039216,0.196078431);
static float   g_VenusRadius = 0.9;
static float   g_VenusOrbitRadius = 5.76;
static float   g_VenusOrbitTilt = -15.0;
static float   g_VenusOrbitIncrement = 1.6; 
static float   g_VenusRotateIncrement = 20.0; 

// Mars is red
static myColor g_MarsColor(0.803921569,0.309803922,0.223529412);
static float   g_MarsRadius = 0.53;
static float   g_MarsOrbitRadius = 12;
static float   g_MarsOrbitTilt = 10.0;
static float   g_MarsOrbitIncrement = 0.53; 
static float   g_MarsRotateIncrement = 1.0;

// Moons are all orange
static myColor g_MoonColor(1,0.6,0.2);
static float   g_MoonRadius = 0.2;
static float   g_MoonOrbitRadius = 1.5;
static float   g_MoonOribtTilt = 30.0;
static float   g_MoonOrbitIncrement = 5.0; 
static float   g_MoonRotateIncrement = 10.0;

float light0_amb[] = { 0.5, 0.5, 0.5, 1.0 };
float light0_dif[] = { 0.25, 0.25, 0.25, 1.0 };

float light0_spec[] = {1.0, 0.8, 1.0, 1.0};
float light0_pos[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat material_shininess[] = { 50.0 };
GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
float material_amb[] = { 1.0, 1.0, 1.0, 1.0 };
float material_dif[] = { 1.0, 1.0, 1.0, 1.0 };

//boundary coordinates for viewport,changes as window resizes.
static GLdouble v_left,v_right,v_bottom,v_top;

static const int timerInterval = 50;

static Planet* pSelectedPlanet = NULL;
static int selected_planet = 0;

float Rot90X[9] = {1,0,0,0,0,-1,0,1,0};
float Rot90Y[9] = {0,0,1,0,1,0,-1,0,0};

float RotMinus90X[9] = {1,0,0,0,0,1,0,-1,0};
float RotMInus90Y[9] = {0,0,-1,0,1,0,1,0,0};

bool b_HeadLight_On = true;
bool b_SunLight_On = true;

//Vectors used in glulookat()
static Vector3 look;
static Vector3 eye;
static Vector3 up = Vector3(0,1,0);	
static Vector3 vec = Vector3(15,15,10);

//reshape callback
void reshape(int w, int h) {	
	glViewport( 0, 0, w, h );	// View port uses whole window
	g_Aspect_Ratio = (float)w/(float)h;
}

//For drawing axis, from the book
void axis(double length){
	glPushMatrix();
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,0,length);
		glEnd();
	glPopMatrix();
}

void selectPlanet(int order){
	switch(order){
	case 0: pSelectedPlanet = pSun;break;
	case 1:	pSelectedPlanet = pMercury;break;
	case 2:	pSelectedPlanet = pVenus;break;
	case 3:	pSelectedPlanet = pEarth;break;
	case 4:	pSelectedPlanet = pMars;break;
	}
	
}

void draw_planet(int order){
	switch(order){
	case 0: 
		glPushMatrix();				
			glRotatef(pSun->_curRotationAngle,0.0,0,1);
			glColor3f(pSun->pColor->red,pSun->pColor->green,pSun->pColor->blue);			
			glMaterialfv(GL_FRONT,GL_EMISSION,light0_dif);
			pSun->draw();		
		glPopMatrix();
		break;
	case 1:
		glPushMatrix();
			glRotatef(pMercury->_orbitTilt,1.0,0.0,0.0);
			pMercury->drawOrbit();
			glRotatef( pMercury->_curOrbitAngle, 0.0, 0.0, 1.0 );
			glTranslatef(pMercury->_orbitRadius, 0.0, 0.0);
			glRotatef( pMercury->_curRotationAngle, 0.0, 0, 1.0 );
			glColor3f(pMercury->pColor->red,pMercury->pColor->green,pMercury->pColor->blue);
			pMercury->draw();
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
			glRotatef(pVenus->_orbitTilt,1.0,0.0,0.0);	
			pVenus->drawOrbit();
			glRotatef( pVenus->_curOrbitAngle, 0.0, 0, 1.0 );
			glTranslatef(pVenus->_orbitRadius, 0.0, 0.0 );
			glRotatef( pVenus->_curRotationAngle, 0.0, 0.0, 1.0 );
			glColor3f(pVenus->pColor->red,pVenus->pColor->green,pVenus->pColor->blue);
			pVenus->draw();
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
			pEarth->drawOrbit();
			glRotatef(pEarth->_curOrbitAngle, 0.0, 0.0, 1.0); //Rotate around the sun
			glTranslatef(pEarth->_orbitRadius, 0.0, 0.0);
			glPushMatrix();						// Save matrix state
				glRotatef(pEarth->_curRotationAngle, 0.0, 0.0, 1.0 ); //Self rotate.			
				glColor3f(pEarth->pColor->red,pEarth->pColor->green,pEarth->pColor->blue);
				pEarth->draw();
			glPopMatrix();						// Restore matrix state
			glPushMatrix();
				//Moon
				glRotatef(pEarthMoon->_orbitTilt,0.0,1.0,0.0);
				pEarthMoon->drawOrbit();
				glRotatef(pEarthMoon->_curOrbitAngle, 0.0, 0.0, 1.0);
				glTranslatef(pEarthMoon->_orbitRadius, 0.0, 0.0 );
				glRotatef( pEarthMoon->_curRotationAngle, 0.0, 0.0, 1.0 );
				glColor3f(pEarthMoon->pColor->red,pEarthMoon->pColor->green,pEarthMoon->pColor->blue);
				pEarthMoon->draw();
			glPopMatrix();
		glPopMatrix();
		break;
	case 4:
		glPushMatrix();
			glRotatef(pMars->_orbitTilt,1.0,0.0,0.0);
			pMars->drawOrbit();
			glRotatef(pMars->_curOrbitAngle, 0.0, 0.0, 1.0 );
			glTranslatef(pMars->_orbitRadius, 0.0, 0.0 );
			glPushMatrix();						// Save matrix state
				glRotatef(pMars->_curRotationAngle, 0.0, 0.0, 1.0 );
				glColor3f(pMars->pColor->red,pMars->pColor->green,pMars->pColor->blue);
				pMars->draw();
			glPopMatrix();						// Restore matrix state
			glPushMatrix();
				//Moon1
				glRotatef(pMarsMoon1->_orbitTilt,1.0,0.0,0.0);
				pMarsMoon1->drawOrbit();
				glRotatef(pMarsMoon1->_curOrbitAngle, 0.0,0.0,1.0);
				glTranslatef(pMarsMoon1->_orbitRadius, 0.0, 0.0 );
				glColor3f(pMarsMoon1->pColor->red,pMarsMoon1->pColor->green,pMarsMoon1->pColor->blue);
				pMarsMoon1->draw();
			glPopMatrix();
			glPushMatrix();
				//Moon2
				glRotatef(pMarsMoon2->_orbitTilt,0.0,1.0,0.0);
				pMarsMoon2->drawOrbit();
				glRotatef(pMarsMoon2->_curOrbitAngle, 0.0,0.0,1.0);
				glTranslatef(-pMarsMoon2->_orbitRadius, 0.0, 0.0 );
				glColor3f(pMarsMoon2->pColor->red,pMarsMoon2->pColor->green,pMarsMoon2->pColor->blue);
				pMarsMoon2->draw();
			glPopMatrix();			
		glPopMatrix();
		break;
	}
}


//display callback
void display(void) {	
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	look = pSelectedPlanet->getCurrentPos();
	//Look needs to be rotated same as the coordinate system otherwise camera won't focus.
	look = look.leftMultiplyBy3DMatrix(RotMinus90X);
	look = look.leftMultiplyBy3DMatrix(RotMInus90Y);
		
	eye = look + vec;		
	aCamera->set(eye.x,eye.y,eye.z, look.x,look.y,look.z, up.x,up.y,up.z);		
	aCamera->setShape(135*g_zoomFactor, g_Aspect_Ratio, 0.1f, 100.0f);
	
	aCamera->roll(g_RollFactor);
	aCamera->yaw(g_YawFactor);
	aCamera->pitch(g_PitchFactor);	
	aCamera->slide(0,0,g_SlideFactor);	

	glShadeModel(GL_SMOOTH);
	
	/* test to see where look is.
	glPushMatrix();
	glTranslatef(look.x,look.y,look.z);
	glutSolidSphere(0.5,10,10);
	glPopMatrix();
	*/
	
	//Rotate so that Z-axis pointing upward.
	glRotatef(-90,0,1,0);
	glRotatef(-90,1,0,0);
	
	//Draw Axis
	glColor3f(1,0,0);//Z-axis in red		
	axis(100);
	glPushMatrix();
	glRotated(90,0,1.0,0);
	glColor3f(1,1,0);	//X-axis in yellow
	axis(100);
	glRotated(-90.0,1,0,0);
	glColor3f(0,1,1);	//Y-axis in Cyan
	axis(100);
	glPopMatrix();
	
	//Draw the planets
	for (int i = 0; i < 5; i++){		
		draw_planet(i);
	}
	
	// Flush the pipeline, and swap the buffers
	glFlush();
	glutSwapBuffers();	
}

void myInit(){
	//3D settings	
	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepth( 1.0 );
	glEnable( GL_DEPTH_TEST );

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	aCamera = new Camera();
	
	pSun = new Planet(0,g_SunRadius,0.0,0.0,0,g_SunRotateIncrement,g_SunColor.red,g_SunColor.green,g_SunColor.blue,"sun.bmp");
	pMercury = new Planet(1,g_MercuryRadius,g_MercuryOrbitRadius,g_MercuryOrbitTilt,g_MercuryOrbitIncrement,g_MercuryRotateIncrement,g_MercuryColor.red,g_MercuryColor.green,g_MercuryColor.blue,"mercury.bmp");
	pVenus = new Planet(2,g_VenusRadius,g_VenusOrbitRadius,g_VenusOrbitTilt,g_VenusOrbitIncrement,g_VenusRotateIncrement,g_VenusColor.red,g_VenusColor.green,g_VenusColor.blue,"venus.bmp");
	pEarth = new Planet(3,g_EarthRadius,g_EarthOrbitRadius,g_EarthOrbitTilt,g_EarthOrbitIncrement,g_EarthRotateIncrement,g_EarthColor.red,g_EarthColor.green,g_EarthColor.blue,"earth.bmp");
	pMars = new Planet(4,g_MarsRadius,g_MarsOrbitRadius,g_EarthOrbitTilt,g_MarsOrbitIncrement,g_MarsRotateIncrement,g_MarsColor.red,g_MarsColor.green,g_MarsColor.blue,"mars.bmp");
	pEarthMoon = new Planet(5,g_MoonRadius,g_MoonOrbitRadius,g_MoonOribtTilt,g_MoonOrbitIncrement,g_MoonRotateIncrement,g_MoonColor.red,g_MoonColor.green,g_MoonColor.blue,"moon.bmp");
	pMarsMoon1 = new Planet(6,g_MoonRadius,g_MoonOrbitRadius,g_MoonOribtTilt,g_MoonOrbitIncrement,g_MoonRotateIncrement,g_MoonColor.red,g_MoonColor.green,g_MoonColor.blue,"moon.bmp");
	pMarsMoon2 = new Planet(7,g_MoonRadius,g_MoonOrbitRadius,-g_MoonOribtTilt,g_MoonOrbitIncrement,g_MoonRotateIncrement,g_MoonColor.red,g_MoonColor.green,g_MoonColor.blue,"moon.bmp");

	//Select Sun at start.
	pSelectedPlanet = pSun;
}

void rotating(int i){
	if(g_bIsAnimating){
		pSun->updateAngle();
		pMercury->updateAngle();
		pVenus->updateAngle();
		pEarth->updateAngle();
		pMars->updateAngle();
		pMarsMoon1->updateAngle();
		pMarsMoon2->updateAngle();
		pEarthMoon->updateAngle();	
		glutPostRedisplay();
	}
	glutTimerFunc(timerInterval, rotating, 0);
}

void lightMenuHandler(int item){	
	switch (item){
	case 1:
		b_SunLight_On = true;
		b_HeadLight_On = true;
		break;
	case 2:
		b_SunLight_On = false;
		b_HeadLight_On = false;
		break;
	case 3:
		b_HeadLight_On = true;
		break;
	case 4:
		b_HeadLight_On =false;
		break;
	case 5:
		b_SunLight_On = true;
		break;
	case 6:
		b_SunLight_On = false;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void createMenus() {
	int lightMenu;	
	
	lightMenu = glutCreateMenu(lightMenuHandler);
	glutAddMenuEntry("ALL LIGHTS ON",1);
	glutAddMenuEntry("ALL LIGHTS OFF",2);
	glutAddMenuEntry("HEAD LIGHT ON",3);
	glutAddMenuEntry("HEAD LIGHT OFF",4);
	glutAddMenuEntry("SUN LIGHT ON",5);
	glutAddMenuEntry("SUN LIGHT OFF",6);	
			
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'S': 
		g_SlideFactor+=0.2;
		break;
	case 'S' + 32: 
		g_SlideFactor += -0.2;		
		break;
	case 'P': g_PitchFactor += 1.0;break;
	case 'P'+ 32: g_PitchFactor += -1.0;break;
	case 'Y': g_YawFactor += -1.0;break;
	case 'Y'+ 32: g_YawFactor += 1.0;break;
	case 'R': g_RollFactor += 1.0;break;
	case 'R'+ 32: g_RollFactor += -1.0;break;
	case 'Z': g_zoomFactor += 0.1;break;
	case 'Z'+ 32 : g_zoomFactor -= 0.1;break;
	case ' ': g_bIsAnimating = !g_bIsAnimating;break; 
	case '0': 
		selected_planet = 0;
		selectPlanet(selected_planet);
		g_SlideFactor = -20.80f;
		break;
	case '1': 
		selected_planet = 1;
		selectPlanet(selected_planet);
		g_SlideFactor = -18.20f;
		break;
	case '2': 
		selected_planet = 2;
		selectPlanet(selected_planet);
		g_SlideFactor = -20.80f;
		break;
	case '3': 
		selected_planet = 3;
		selectPlanet(selected_planet);
		g_SlideFactor = -20.20f;
		break;
	case '4': 
		selected_planet = 4;
		selectPlanet(selected_planet);
		g_SlideFactor = -20.80f;
		break;
	case 'v': //Reset to startup view.
		selectPlanet(0); //select sun
		g_SlideFactor = -10.0;
		g_PitchFactor = 0.0;
		g_YawFactor = 0.0;
		g_RollFactor = 0.0;
		g_zoomFactor = 1.0;
		break; 
	case 'q':
	case 'Q':
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main( int argc, char** argv )
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(g_ScreenWidth,g_ScreenHeight);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Arthur Liu CSCI550 Project 3: Solar System");	
	createMenus();

	//Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	
	myInit();

	glutTimerFunc(timerInterval, rotating, 0);

	glutMainLoop();

	delete aCamera,pSun,pMercury,pVenus,pEarth,pEarthMoon,pMars,pMarsMoon1,pMarsMoon2;
	pSelectedPlanet = NULL;

	return 0;	
}

