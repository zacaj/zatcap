#include "TimedEventProcess.h"
#include "twitter.h"


TimedEventProcess::TimedEventProcess()
{
	ticks=100;
	maxTicks=3600;
}


TimedEventProcess::~TimedEventProcess(void)
{
}

extern twitCurl *twit;


void TimedEventProcess::update()
{
	if(ticks++>maxTicks)
	{
		ticks=0;
		SDL_CreateThread(fn,this);
	}
}

void TimedEventProcess::newTweet( Tweet *tweet )
{
	
}

void TimedEventProcess::draw()
{

}
