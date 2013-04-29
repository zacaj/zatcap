#pragma once
#include "process.h"

class Button;
typedef void (*buttonDrawFunc)(Button*,void*);
typedef bool (*buttonClickFunc)(Button*,int,void*);

struct DualSurface
{
	string imgs[2];
};

class Button :
	public Process
{
public:
	void *drawData,*clickData;
	int x,y,w,h;
	int rx,ry;
	bool highlighted;
	void* otherData;
	buttonDrawFunc drawFunc;
	buttonClickFunc click;
	Button(void);
	~Button(void);

	virtual void update();
	virtual void draw();

	virtual bool mouseButtonEvent( int x,int y,int button,int pressed );

};

void buttonGenericDraw(Button *button,void *data);
bool buttonGenericToggle(Button *button,int n,void *data);
