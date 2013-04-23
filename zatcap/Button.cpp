#include "Button.h"
#include "zatcap.h"


void buttonGenericDraw(Button *button,void *data)
{
	DualSurface *dual=(DualSurface*)(data);
///	drawSprite(dual->imgs[button->highlighted],screen,0,0,button->x,button->y,dual->imgs[button->highlighted]->w,dual->imgs[button->highlighted]->h);
}

bool buttonGenericToggle( Button *button,int n,void *data )
{
	bool *t=(bool*)data;
	*t=!*t;
	return 1;
}

Button::Button(void)
{
	drawFunc=0;
	click=0;
	highlighted=0;
	drawData=clickData=otherData=NULL;
}


Button::~Button(void)
{
}

void Button::update()
{
}

bool Button::mouseButtonEvent( int _x,int _y,int button,int pressed )
{
	if(_x>x && _x < x+w && _y>y && _y<y+h)
	{
		highlighted=1;
		if(pressed)
			if(click)
				return click(this,button,clickData);
	}
	else
		highlighted=0;
	return 0;
}

void Button::draw()
{
	if(drawFunc)
		drawFunc(this,drawData);
}
