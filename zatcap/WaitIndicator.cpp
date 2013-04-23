#include "WaitIndicator.h"

WaitIndicator::WaitIndicator( string _text )
{
	text=_text;
}


WaitIndicator::~WaitIndicator(void)
{
///	updateScreen=1;
}

void WaitIndicator::update()
{

}

void WaitIndicator::draw()
{
/*		boxColor(screen,screen->w/2-100,screen->h/2-80,screen->w/2+100,screen->h/2-40,colors::buttonBackgroundColor);
		rectangleColor(screen,screen->w/2-100,screen->h/2-80,screen->w/2+100,screen->h/2-40,colors::buttonBorderColor);
		drawTextcc(text.c_str(),screen->w/2,screen->h/2-60,14);*/
}
