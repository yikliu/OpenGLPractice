// BezierCurve.cpp : 
//
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Title: CSCI550 Project 2 : Bezier Curve Drawing
// Date: Oct.10th, 2011
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// Description: 
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <vector>
using namespace std;
#include "mycolor.h"
#include "my3DVector.h"

//Max number of vertices allowed.
#define MAX_NUM_VERTICES 500 

/*define a neighborhood range within which a left-mouse click 
  can be considered as an intention of picking up the vertex 
  which resides in the neighborhood range.
*/
#define NEIGHBORHOOD 0.5

//an indication that none vertex is picked up. 
#define PICKED_NONE 501

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

//Key ASCII code substitution.
#define ESC 27
#define DEL 100 /* lowercase d */
#define PLUS 43
#define MINUS 45
#define SWAP 115

//Rotating Axis
#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3
#define MIX      4

//set the window width/height constant, so is the aspect ratio
//Thus view plane is pre-defined as [0, g_Win_Width, 0, g_Win_Height] and stay constant.
static const GLint g_Win_Width = 10;
static const GLint g_Win_Height = 10;
static const GLdouble g_Aspect_Ratio = g_Win_Width/g_Win_Height;

//set the margin when drawing the boundary, so that the boundary is visible.
static const GLdouble g_margin = 0.05;

//screen width/height, initial value is 1024x768
static GLint g_ScreenWidth = 640;
static GLint g_ScreenHeight = 480;

//drawing line color, initial color is white
static myColor g_LineColor(1,1,1);

//drawing background color, initial color is black
static myColor g_ClearColor(0,0,0);

//rubber-banding switch
static GLboolean g_rubberbanding = GL_TRUE;

//boundary coordinates for viewport,changes as window resizes.
static GLdouble v_left,v_right,v_bottom,v_top;

//current number of vertices in list.
static int numVertices = 0;

//current level of approximation, initially set to 4;
static int bazierCurveLevel = 4;

//boolean value to indicate if its 2D rendering or 3D rendering default value is 2D
static bool is2DRendering = true;

static my3DVector initControlPtrs[MAX_NUM_VERTICES];

//a flag variable to show whether a point is being dragged.
static GLboolean dragging = GL_FALSE;  

//mouse position during drag
static int activeMouseX, activeMouseY; 

/*index of the vertex in the list which is being dragged.
  activeVertexIndex serves another important use:
  1. if activeVertex == [0...numberVertice-1] then the dragging vertex is picked up from
	 existing list of vertices.
  2. if activeVertex == numberVertice, then either there is no dragging vertex, or 
	 the dragging vertex is newly created from mouse clicking in empty spaces.
*/
static int activeVertexIndex;

vector<my3DVector> ctrlPointsList;
static GLdouble g_ZHeight = 5;
static const int timerInterval = 50;
static const float rotateAngleInterval = 1.0f;

static my3DVector*  pRotatingAxis = new my3DVector(1,1,1);

//define the view port coordinate system
void setViewport(GLdouble left,GLdouble right, GLdouble bottom, GLdouble top){
	glViewport(left,bottom,right-left,top-bottom);
}

//define the window coordinate system
void set2DCamera(GLdouble left,GLdouble right, GLdouble bottom, GLdouble top){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left,right,bottom,top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//return the scale from current window to the view plane
GLdouble getScale(){
	GLdouble scale;
	if(g_Aspect_Ratio > g_ScreenWidth/g_ScreenHeight){
		scale = g_ScreenWidth/g_Win_Width;
	}else{
		scale = g_ScreenHeight/g_Win_Height;
	}
	return scale;	
}

//update view port coordinate system when current window resizes.
//always place the view port in middle of the screen. 
void updateViewportBoundary(int w, int h){
	if(g_Aspect_Ratio > w/h){
		v_left = 0;
		v_right = w;
		v_bottom = (h - w/g_Aspect_Ratio)/2;
		v_top = h - (h-w/g_Aspect_Ratio)/2;		
	}else{
		v_left = (w - h*g_Aspect_Ratio)/2;
		v_right = w - (w - h * g_Aspect_Ratio)/2;
		v_bottom = 0;
		v_top = h;
	}
}

//convert a coordination from view port to view plane. 
my3DVector convertToViewCS(GLdouble x, GLdouble y){
	my3DVector v;
	v.x = (x - v_left)/getScale();
	v.y = (y - v_bottom)/getScale();
	v.z = 0;
	return v;		
}


//check if a point from screen can be considered as within the view plane of world window.
GLboolean isInsideViewport(int x, int y){
	my3DVector v = convertToViewCS(x, y);
	if (v.x > g_Win_Width || v.x < 0 || v.y > g_Win_Height || v.y < 0 ){
		return GL_FALSE;
	}
	return GL_TRUE;
}


//draw a visible boundary of view port.
void drawBoundary(){
	glColor3f(0.0,1.0,0.0);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glBegin(GL_POLYGON);
		glVertex3f(g_margin,g_margin,0);
		glVertex3f(g_margin,g_Win_Height-g_margin,0);
		glVertex3f(g_Win_Width-g_margin,g_Win_Height-g_margin,0);
		glVertex3f(g_Win_Width-g_margin,g_margin,0);
	glEnd();
}

//reshape callback
void reshape(int w, int h) {
	g_ScreenHeight = h;
	g_ScreenWidth = w;	
	updateViewportBoundary(g_ScreenWidth,g_ScreenHeight);
	setViewport(v_left,v_right,v_bottom,v_top);
}

//divide list of control points to two separate parts based on the middle point method.
void subDivideCurve(vector<my3DVector> &p, vector<my3DVector> &l, vector<my3DVector> &r){	
	vector<my3DVector> s(p);
	vector<my3DVector> midList;	
	while (s.size()>2){
		l.push_back(s[0]);
		r.push_back(s[s.size()-1]);			
		for(unsigned int i = 0; i < s.size() - 1; i++){			
			my3DVector mid = (s[i]+s[i+1]) /2;
			midList.push_back(mid);		
		}
		s = midList;
		midList.clear();
	}
	l.push_back(s[0]);
	r.push_back(s[1]);	
	my3DVector mid = (s[0]+s[1]) /2;
	l.push_back(mid);	
	r.push_back(mid);
}

//Besier curve generation
void drawBesierCurve(vector<my3DVector> &p,int level){	
	if(level >= bazierCurveLevel){ //no more division, start drawing
		glColor3f(g_LineColor.red,g_LineColor.green,g_LineColor.blue); 			
		if (is2DRendering){ //2D 
			glBegin(GL_LINE_STRIP);				
			for (unsigned int i = 0 ;i < p.size(); i++ ){					
				glVertex3f(p[i].x,p[i].y,0);
			}
			glEnd();
		} 
		else	{ //3D using polygon mesh				
			for (unsigned int j = 0; j < p.size() - 1; j++ ){
				my3DVector a,b,c,d;
				my3DVector ba, ca, normal;
				a = p[j];
				b = p[j+1];
				ba = b - a;
				c = a; 
				c.z = a.z + g_ZHeight;
				ca = c - a;
				d = b + ca;
				normal = ba.cross(ca);	
				glBegin(GL_POLYGON);
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(a.x,a.y,a.z);
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(c.x,c.y,c.z);		
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(d.x,d.y,d.z);
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(b.x,b.y,b.z);	
				glEnd();		
			}						
		}				
	}
	else{ //further approximation
		vector<my3DVector> left;
		vector<my3DVector> right;		
		subDivideCurve(p,left,right);
		drawBesierCurve(left,level+1);
		drawBesierCurve(right,level+1);		
	}
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


//display callback
void display(void) {
	glClearColor(g_ClearColor.red,g_ClearColor.green,g_ClearColor.blue, 1.0);/*choose clear color*/
	glClear(GL_COLOR_BUFFER_BIT);  /* clear off screen buffer */		
	if(!is2DRendering){	//3D rendering
		glColor3f(1,0,0);//Z-axis in red		
		axis(abs(v_left-v_right) * 2);
		glPushMatrix();
		glRotated(90,0,1.0,0);
		glColor3f(1,1,0);	//X-axis in yellow
		axis(abs(v_left-v_right) * 2);
		glRotated(-90.0,1,0,0);
		glColor3f(0,1,1);	//Y-axis in Cyan
		axis(abs(v_left-v_right) * 2);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-5,-5,0);
		drawBoundary();	
		if(ctrlPointsList.size() >=3){
			drawBesierCurve(ctrlPointsList,0);
		}		
		glPopMatrix();
		glutSwapBuffers();
		return;	
	}
	drawBoundary();		
	glColor3f(g_LineColor.red,g_LineColor.green,g_LineColor.blue); /*choose color*/	
	ctrlPointsList.clear();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= numVertices; i++){ /* this loop goes from 0 to numVertices*/
		if (i == activeVertexIndex && g_rubberbanding){ // if the point to draw is dragging, including new point.
			if (dragging){ 
				my3DVector v = convertToViewCS(activeMouseX, g_ScreenHeight - activeMouseY);
				ctrlPointsList.push_back(v);
				glVertex3f(v.x,v.y,0);
			}				
			continue;/*if the point is dragging, skip its value in the stored list*/
		}
		if(i!=numVertices){ /*don't draw beyond numVertices-1 */
			ctrlPointsList.push_back(initControlPtrs[i]);
			glVertex3f(initControlPtrs[i].x , initControlPtrs[i].y,0);
		}		
	}
	glEnd();
	
	if(numVertices >= 3){
		drawBesierCurve(ctrlPointsList,0);		
	}
	glutSwapBuffers();  /* swap offscreen/display buffer (flushes GL commands) */
}

//return the picked point index or PICKED_NONE
int pickPoint(int x,int y){	
	if(!g_rubberbanding){
		return PICKED_NONE;
	}
	my3DVector v = convertToViewCS(x, g_ScreenHeight - y);
	for(int i = 0; i < numVertices; i++){
		if((abs(v.x-initControlPtrs[i].x) <= NEIGHBORHOOD)&&(abs(v.y-initControlPtrs[i].y) <= NEIGHBORHOOD)){
			return i;
		}
	}
	return PICKED_NONE;
}

//debug utility function
void debugVerticeList(){
	printf("*********\n");
	for(int i=0; i<numVertices; i++){
		printf("[%d].x = %f",i,initControlPtrs[i].x);
		printf("[%d].y = %f",i,initControlPtrs[i].y);
		printf("\n");
	}
}

//delete a vertex in the list by index.
void deletePoint(int i){
	if (i < 0 || i>= numVertices){
		return;
	}
	for (int j = i; j<numVertices; j++ ){ 
		initControlPtrs[j].x = initControlPtrs[j+1].x;
		initControlPtrs[j].y = initControlPtrs[j+1].y;
	}	
	numVertices--;
	//debugVerticeList();
}


//mouse clicking event callback
void mouse(int button, int state, int x, int y) {
	if (!is2DRendering){
		return;
	}
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN ) {
			int index = pickPoint(x,y);
			if (index != PICKED_NONE){ /*picked a existing vertex*/
				activeVertexIndex = index;
			} 			
			dragging = GL_TRUE;
			activeMouseX = x;
			activeMouseY = y;			
		} else if (state == GLUT_UP && dragging) {
			if (numVertices < MAX_NUM_VERTICES) {				
				if(isInsideViewport(activeMouseX,g_ScreenHeight - activeMouseY)){/*only add vertex when within view plane*/
					my3DVector v = convertToViewCS(activeMouseX, g_ScreenHeight - activeMouseY);
					initControlPtrs[activeVertexIndex] = v;					
					if (activeVertexIndex == numVertices){ /*only increment numVertices when this is a new vertex*/
						numVertices++;	
						//debugVerticeList();
					} 					
				}	
				activeVertexIndex = numVertices; /*reset activeVertexIndex*/
			}      
			dragging = GL_FALSE;	
			glutPostRedisplay();   
		}
		
	}
}

void mouseMotion(int x, int y) {
	if(!is2DRendering){
		return;
	}
	if (dragging) {
		activeMouseX = x;
		activeMouseY = y;    
		glutPostRedisplay();         
	}
}

void myInit(){
	if(is2DRendering){ //switch to 2D, reset settings
		set2DCamera(0,g_Win_Width,0,g_Win_Height);	
		updateViewportBoundary(g_ScreenWidth,g_ScreenHeight);
		setViewport(v_left,v_right,v_bottom,v_top);		
	}
	else{//3D settings	
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		double a = 1.4;
		glOrtho(-g_Win_Width*a/2,g_Win_Width*a/2,-g_Win_Height*a/2,g_Win_Height*a/2,1, 100);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(40,40,40, 	0,0,0, 	0,1,0);				
		glRotated(-90,1,0,0);		

		updateViewportBoundary(g_ScreenWidth,g_ScreenHeight);
		setViewport(v_left,v_right,v_bottom,v_top);
	}
}

void rotating(int i){	
	if(is2DRendering) return;	
	glRotated(1.0,pRotatingAxis->x, pRotatingAxis->y, pRotatingAxis->z);		
	glutPostRedisplay();	
	glutTimerFunc(timerInterval, rotating, 0);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == ESC) exit(0);
	if (key == DEL){
		if(!is2DRendering){
			return;
		}
		if (dragging && activeVertexIndex!=numVertices)
		{
			deletePoint(activeVertexIndex);
			dragging = GL_FALSE; /*disable dragging*/
			activeVertexIndex = numVertices; 
			glutPostRedisplay();
		}
	}
	if (key == PLUS){		
		if(bazierCurveLevel != 9){
			bazierCurveLevel ++;
			glutPostRedisplay();
		}		
	}
	if(key == MINUS){
		if(bazierCurveLevel !=1){
			bazierCurveLevel --;
			glutPostRedisplay();
		}
	}
	if(key == SWAP){
		is2DRendering = !is2DRendering;
		myInit();		
		if(is2DRendering){
			glutPostRedisplay();
		}
		else{
			glutTimerFunc(timerInterval,rotating,0);
		}		
	}	
}


void colorMenuHandler(int item){
	switch (item) {
	case WHITE :	
		g_LineColor.setColor(1.0f,1.0f,1.0f);
		break;
	case BLACK :
		g_LineColor.setColor(0.0f,0.0f,0.0f);
		break;
	case RED :
		g_LineColor.setColor(1.0f,0.0f,0.0f);
		break;
	case GREEN :
		g_LineColor.setColor(0.0f,1.0f,0.0f);
		break;
	case BLUE :
		g_LineColor.setColor(0.0f,0.0f,1.0f);
		break;
	case ORANGE :
		g_LineColor.setColor(1.0f,0.55f,0.0f);
		break;
	}
	glutPostRedisplay();
}


void clearMenuHandler(int item){
	switch (item) {
	case WHITE :
		g_ClearColor.setColor(1.0f,1.0f,1.0f);
		break;
	case BLACK :
		g_ClearColor.setColor(0.0f,0.0f,0.0f);
		break;
	case GREY :
		g_ClearColor.setColor(0.5f,0.5f,0.5f);
		break;	
	}
	glutPostRedisplay();
}

void rotatingAxisHandler(int item){
	switch (item) {
	case X_AXIS :
		pRotatingAxis->x = 1;
		pRotatingAxis->y = 0;
		pRotatingAxis->z = 0;
		break;
	case Y_AXIS :
		pRotatingAxis->x = 0;
		pRotatingAxis->y = 1;
		pRotatingAxis->z = 0;
		break;
	case Z_AXIS :
		pRotatingAxis->x = 0;
		pRotatingAxis->y = 0;
		pRotatingAxis->z = 1;
		break;	
	case MIX :
		pRotatingAxis->x = 1;
		pRotatingAxis->y = 1;
		pRotatingAxis->z = 1;
		break;	
	}
	myInit();
	glutPostRedisplay();
}

void rubberbandingMenuHandler(int item){
	if (item == ON)	{
		g_rubberbanding = GL_TRUE;
	} 
	else	{
		g_rubberbanding = GL_FALSE;
	}
	glutPostRedisplay();
}

void menuHandler(int item){}

void createMenus() {
	int clearMenu,rubberbandingMenu,colorMenu, rotateAxisMenu, mainMenu;
	
	rubberbandingMenu = glutCreateMenu(rubberbandingMenuHandler);
	glutAddMenuEntry("ON",ON);
	glutAddMenuEntry("OFF",OFF);
	
	colorMenu = glutCreateMenu(colorMenuHandler);
	glutAddMenuEntry("White",WHITE);
	glutAddMenuEntry("Black",BLACK);
	glutAddMenuEntry("Red",RED);
	glutAddMenuEntry("Blue",BLUE);
	glutAddMenuEntry("Green",GREEN);
	glutAddMenuEntry("Orange",ORANGE);
	
	clearMenu = glutCreateMenu(clearMenuHandler);
	glutAddMenuEntry("White",WHITE);
	glutAddMenuEntry("Black",BLACK);
	glutAddMenuEntry("GREY",GREY);	

	rotateAxisMenu = glutCreateMenu(rotatingAxisHandler);
	glutAddMenuEntry("X-Axis", X_AXIS);
	glutAddMenuEntry("Y-Axis", Y_AXIS);
	glutAddMenuEntry("Z-Axis", Z_AXIS);
	glutAddMenuEntry("MIX", MIX);


	mainMenu = glutCreateMenu(menuHandler);
	glutAddSubMenu("Rubberbanding", rubberbandingMenu);	
	glutAddSubMenu("Line Color", colorMenu);	
	glutAddSubMenu("Clear Color", clearMenu);
	glutAddSubMenu("Rotating Axis", rotateAxisMenu);
		
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); /* double buffer */
	glutInitWindowSize(g_ScreenWidth,g_ScreenHeight);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Arthur Liu CSCI550 Project 1");	
	createMenus();
	
	//Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	
	myInit();
		
	glutMainLoop();

	return 0;
}
