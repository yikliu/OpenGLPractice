////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Title: CSCI550 Project 1: Interactive 2D Polygon
// Date: Sep.24th, 2011
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// Description: A simple interactive 2D polygon drawing application, see README for more 
//              information.
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#define GLUT_DISABLE_ATEXIT_HACK 
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

//Max number of vertices allowed.
#define MAX_NUM_VERTICES 500 

/*define a neighborhood range within which a left-mouse click 
  can be considered as an intention of picking up the vertex 
  which resides in the neighborhood range.
*/
#define NEIGHBORHOOD 0.5

//an indication that none vertex is picked up. 
#define PICKED_NONE 501

//Menu entry keys for drawing color
#define BLACK 0
#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define ORANGE 5
#define GREY 6

//Menu entry keys for polygon mode.
#define LINE 1
#define FILL 2 

//Menu entry keys for rubberbanding 
#define ON 1
#define OFF 2

//Key ASCII code substitution.
#define ESC 27
#define DELETE 100 /* lowercase d */

//set the window width/height constant, so is the aspect ratio
//Thus view plane is pre-defined as [0, g_Win_Width, 0, g_Win_Height] and stay constant.
static const GLint g_Win_Width = 10;
static const GLint g_Win_Height = 10;
static const GLfloat g_Aspect_Ratio = g_Win_Width/g_Win_Height;

//set the margin when drawing the boundary, so that the boundary is visible.
static const GLfloat g_margin = 0.05;

//screen width/height, initial value is 640x480
static GLint g_ScreenWidth = 640;
static GLint g_ScreenHeight = 480;

//drawing color, initial color is white
static GLfloat g_red = 1.0f;
static GLfloat g_green = 1.0f;
static GLfloat g_blue = 1.0f;

//drawing color, initial color is black
static GLfloat g_ClearRed = 0.0f;
static GLfloat g_ClearGreen = 0.0f;
static GLfloat g_ClearBlue = 0.0f;

//polygon mode, initially set to GL_LINE.
static GLint g_mode = GL_LINE;

//rubberbanding switch
static GLboolean g_rubberbanding = GL_TRUE;

//boundary coordinates for viewport,changes as window resizes.
static GLfloat v_left,v_right,v_bottom,v_top;

//current number of vertices in list.
static int numVertices = 0;

//Struct vertex to represent one vertex.
static struct vertex {GLfloat x, y;} vertices_list[MAX_NUM_VERTICES];

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

//define the view port coordinate system
void setViewport(GLfloat left,GLfloat right, GLfloat bottom, GLfloat top){
	glViewport(left,bottom,right-left,top-bottom);
}

//define the window coordinate system
void setWindow(GLfloat left,GLfloat right, GLfloat bottom, GLfloat top){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left,right,bottom,top);
}


//return the scale from current window to the view plane
float getScale(){
	float scale;
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
vertex convertToViewCS(float x, float y){
	vertex v;
	v.x = (x - v_left)/getScale();
	v.y = (y - v_bottom)/getScale();
	return v;		
}


//check if a point from screen can be considered as within the view plane of world window.
GLboolean isInsideViewport(int x, int y){
	vertex v = convertToViewCS(x, y);
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
		glVertex2f(g_margin,g_margin);
		glVertex2f(g_margin,g_Win_Height-g_margin);
		glVertex2f(g_Win_Width-g_margin,g_Win_Height-g_margin);
		glVertex2f(g_Win_Width-g_margin,g_margin);
	glEnd();
}

//reshape callback
void reshape(int w, int h) {
	g_ScreenHeight = h;
	g_ScreenWidth = w;	
	updateViewportBoundary(g_ScreenWidth,g_ScreenHeight);
	setViewport(v_left,v_right,v_bottom,v_top);
}


//display callback
void display(void) {
	glClearColor(g_ClearRed, g_ClearGreen, g_ClearBlue, 1.0);/*choose clear color*/
	glClear(GL_COLOR_BUFFER_BIT);  /* clear off screen buffer */	
	drawBoundary();	
	glColor3f(g_red,g_green,g_blue); /*choose color*/
	glPolygonMode(GL_FRONT_AND_BACK,g_mode); /*choose polygon mode*/
	glBegin(GL_POLYGON);
	for (int i = 0; i <= numVertices; i++){ /* this loop goes from 0 to numVertices*/
		if (i == activeVertexIndex && g_rubberbanding){ // if the point to draw is dragging, including new point.
			if (dragging){ 
				vertex v = convertToViewCS(activeMouseX, g_ScreenHeight - activeMouseY);
				glVertex2f(v.x,v.y);
			}				
			continue;/*if the point is dragging, skip its value in the stored list*/
		}
		if(i!=numVertices){ /*don't draw beyond numVertices-1 */
			glVertex2f(vertices_list[i].x , vertices_list[i].y);
		}		
	}
	glEnd();
	glutSwapBuffers();  /* swap offscreen/display buffer (flushes GL commands) */
}

//return the picked point index or PICKED_NONE
int pickPoint(int x,int y){	
	if(!g_rubberbanding){
		return PICKED_NONE;
	}
	vertex v = convertToViewCS(x, g_ScreenHeight - y);
	for(int i = 0; i < numVertices; i++){
		if((abs(v.x-vertices_list[i].x) <= NEIGHBORHOOD)&&(abs(v.y-vertices_list[i].y) <= NEIGHBORHOOD)){
			return i;
		}
	}
	return PICKED_NONE;
}

//debug utility function
void displayVerticeList(){
	printf("*********\n");
	for(int i=0; i<numVertices; i++){
		printf("[%d].x = %f",i,vertices_list[i].x);
		printf("[%d].y = %f",i,vertices_list[i].y);
		printf("\n");
	}
}

//delete a vertex in the list by index.
void deletePoint(int i){
	if (i < 0 || i>= numVertices){
		return;
	}
	for (int j = i; j<numVertices; j++ ){ 
		vertices_list[j].x = vertices_list[j+1].x;
		vertices_list[j].y = vertices_list[j+1].y;
	}	
	numVertices--;
	displayVerticeList();
}


//mouse clicking event callback
void mouse(int button, int state, int x, int y) {
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
					vertex v = convertToViewCS(activeMouseX, g_ScreenHeight - activeMouseY);
					vertices_list[activeVertexIndex] = v;					
					if (activeVertexIndex == numVertices){ /*only increment numVertices when this is a new vertex*/
						numVertices++;	
						displayVerticeList();
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
	if (dragging) {
		activeMouseX = x;
		activeMouseY = y;    
		glutPostRedisplay();         
	}
}


void keyboard(unsigned char key, int x, int y) {
	if (key == ESC) exit(0);
	if (key == DELETE){
		if (dragging && activeVertexIndex!=numVertices)
		{
			deletePoint(activeVertexIndex);
			dragging = GL_FALSE; /*disable dragging*/
			activeVertexIndex = numVertices; 
			glutPostRedisplay();
		}
	}

}

void myInit(){
	setWindow(0,g_Win_Width,0,g_Win_Height);	
	updateViewportBoundary(g_ScreenWidth,g_ScreenHeight);
	setViewport(v_left,v_right,v_bottom,v_top);		
}

void colorMenuHandler(int item){
	switch (item) {
	case WHITE :
		g_red = 1.0f;
		g_green = 1.0f;
		g_blue = 1.0f; 
		break;
	case BLACK :
		g_red = 0.0f;
		g_green = 0.0f;
		g_blue = 0.0f; 
		break;
	case RED :
		g_red = 1.0f;
		g_green = 0.0f;
		g_blue = 0.0f; 
		break;
	case GREEN :
		g_red = 0.0f;
		g_green = 1.0f;
		g_blue = 0.0f; 
		break;
	case BLUE :
		g_red = 0.0f;
		g_green = 0.0f;
		g_blue = 1.0f; 
		break;
	case ORANGE :
		g_red = 1.0f;
		g_green = 0.65f;
		g_blue = 0.0f; 
		break;
	}
	glutPostRedisplay();
}

void clearMenuHandler(int item){
	switch (item) {
	case WHITE :
		g_ClearRed = 1.0f;
		g_ClearGreen = 1.0f;
		g_ClearBlue = 1.0f; 
		break;
	case BLACK :
		g_ClearRed = 0.0f;
		g_ClearGreen = 0.0f;
		g_ClearBlue = 0.0f; 
		break;
	case GREY :
		g_ClearRed = 0.5f;
		g_ClearGreen = 0.5f;
		g_ClearBlue = 0.5f; 
		break;	
	}
	glutPostRedisplay();
}

void fillMenuHandler(int item){
	switch(item){
		case LINE :
			g_mode = GL_LINE;
			break;
		case FILL :
			g_mode = GL_FILL;
			break;
	}
	glutPostRedisplay();
}


void rubberbandingMenuHandler(int item){
	if (item == ON)
	{
		g_rubberbanding = GL_TRUE;
	} 
	else
	{
		g_rubberbanding = GL_FALSE;
	}
	glutPostRedisplay();
}

void menuHandler(int item){}

void createMenus() {
	int clearMenu,rubberbandingMenu,fillMenu,colorMenu,mainMenu;
	
	rubberbandingMenu = glutCreateMenu(rubberbandingMenuHandler);
	glutAddMenuEntry("ON",ON);
	glutAddMenuEntry("OFF",OFF);
	
	fillMenu = glutCreateMenu(fillMenuHandler);
	glutAddMenuEntry("Fill",FILL);
	glutAddMenuEntry("Line",LINE);

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

	mainMenu = glutCreateMenu(menuHandler);
	glutAddSubMenu("Rubberbanding", rubberbandingMenu);
	glutAddSubMenu("Polygon Mode", fillMenu);
	glutAddSubMenu("Drawing Color", colorMenu);
	glutAddSubMenu("Clear Color", clearMenu);
		
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); /* double buffer */
	glutInitWindowSize(g_ScreenWidth,g_ScreenHeight);
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


