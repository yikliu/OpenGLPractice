#ifndef _COLOR_H
#define _COLOR_H
class myColor
{
public:
	float red;
	float green;
	float blue;
public:
	myColor():red(0.0),green(0.0),blue(0.0){};
	myColor(float r, float g, float b):red(r),green(g),blue(b){};
	void setColor(float r, float g, float b){
		red = r;
		green = g;
		blue = b;
	}
	virtual ~myColor(void){};
};
#endif