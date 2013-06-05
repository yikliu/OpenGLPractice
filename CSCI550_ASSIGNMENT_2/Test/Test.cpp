/* bezier.cpp by detour@metalshell.com
 *
 * Create a bezier curve by defining the three points
 * with your mouse.
 *
 * http://www.metalshell.com/
 *
 */
 
#include "stdafx.h"
#define GLUT_DISABLE_ATEXIT_HACK 
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
 
int SCREEN_HEIGHT = 480;
// Keep track of times clicked, on 3 clicks draw.
int NUMPOINTS = 0;
 
// Point class to keep it a little cleaner.
class point {
public:
    float x, y;
    void setxy(float x2, float y2) { x = x2; y = y2; }
    const point & operator=(const point &rPoint) {
         x = rPoint.x;
         y = rPoint.y;
 
         return *this;
      }
 
};
 
point abc[3];
 
void myInit() {
    glClearColor(0.0,0.0,0.0,0.0);
    glColor3f(1.0,0.0,0.0);
    glPointSize(4.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,640.0,0.0,480.0);
    
}
 
void drawDot(int x, int y) {
    glBegin(GL_POINTS);
     glVertex2i(x,y);
    glEnd();
    glFlush();
}
 
void drawLine(point p1, point p2) {
    glBegin(GL_LINES);
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
    glEnd();
    glFlush();
}
 
// Calculate the next bezier point.
point drawBezier(point A, point B, point C, double t) {
    point P;
 
    P.x = pow((1 - t), 2) * A.x + 2 * t * (1 -t) * B.x + pow(t, 2) * C.x;
    P.y = pow((1 - t), 2) * A.y + 2 * t * (1 -t) * B.y + pow(t, 2) * C.y;
 
    return P;
}
 
void myMouse(int button, int state, int x, int y) {
  // If left button was clicked
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      // Store where the user clicked, note Y is backwards.
    abc[NUMPOINTS].setxy((float)x,(float)(SCREEN_HEIGHT - y));
    NUMPOINTS++;
        
    // Draw the red  dot.
    drawDot(x, SCREEN_HEIGHT - y);
 
    // If 3 points are drawn do the curve.
    if(NUMPOINTS == 3) {
        glColor3f(1.0,1.0,1.0);
        // Draw two legs of the triangle
        drawLine(abc[0], abc[1]);
        drawLine(abc[1], abc[2]);
        point POld = abc[0];
        /* Draw each segment of the curve.  Make t increment in
                   smaller amounts for a more detailed curve. */
        for(double t = 0.0;t <= 1.0; t += 0.1) {
            point P = drawBezier(abc[0], abc[1], abc[2], t);
            drawLine(POld, P);
            POld = P;
        }
        glColor3f(1.0,0.0,0.0);
        NUMPOINTS = 0;
    }
  }
}
 
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}
 
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,150);
    glutCreateWindow("Bezier Curve");
 
    glutMouseFunc(myMouse);
    glutDisplayFunc(myDisplay);
 
    myInit();
    glutMainLoop();
 
    return 0;
}


