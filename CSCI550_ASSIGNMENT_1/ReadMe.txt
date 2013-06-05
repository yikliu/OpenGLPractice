========================================================================
    CSCI550_ASSIGNMENT_1 Project README

	Author: Arthur (Yikun) Liu
	Date: Sep.24th, 2011.
========================================================================

This is submission to CSCI550 project 1: Interative 2D polygon drawing.

Define vertex and draw polygon
    The application is initialized to show a green line bounded rectangle as drawing area.
	clicking at least three times inside the green box will define and draw a polygon. Each
	clicked vertex will added to the polygon.

Menus: Right click will invoke the pop-up menu, it has four submenus
	1. Rubberbanding: turning rubberbanding on/off
	2. Polygon mode: select polygon drawing mode as lines(wireframe) or filled.
	3. Color: select the color of polygon, when polygon mode is set to wireframe, this is setting 
		color of lines; when polygon mode is set to filled, this is setting the filled color.
	4. Clear: select the background color.
	
Keys: Two key actions are defined:
	1. Press "ESC" key at anytime will terminate the program and exit
	2. Press "d" when dragging a vertex(using rubberbanding, see below) will delete the vertex. 

Rubberbanding:
	When rubberbanding is enabled (See Menu), clicking and dragging a point can adjust the position of 
	the point. When left mouse is up, the point is define. However, if mouse is up outside 
	the green box,  no point is defined. 

Select/Edit/Delete a vertex:
	Click close to any vertex will enable the vertex to be dragged. A reasonable neighborhood 
	is defined to enable easy pick-up. When picked up, a vertex can be dragged using mouse dragging
	action. 
	
	Edit its postion by releasing the left mouse at an appropriate position. A vertex cannot be dragged 
	out of the green box and released there, it must be released within the green box. 
	
	Press "d" key when dragging a vertex will result in deleting the picked-up vertex.If 
	there is only three vetices left, deleting any of the three vertices will make the polygon 
	disappear because there is only two vertices left which cannot make up a polygon. 


