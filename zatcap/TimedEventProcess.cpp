#include "TimedEventProcess.h"
#include "twitter.h"


TimedEventProcess::TimedEventProcess()
{
	lastTime=clock();
	interval=60;
}


TimedEventProcess::~TimedEventProcess(void)
{
}

extern twitCurl *twit;


void TimedEventProcess::update()
{
	if((float)clock()/CLOCKS_PER_SEC-lastTime/CLOCKS_PER_SEC>interval)
	{
		lastTime=clock();
		startThread(fn,this);
	}
}

void TimedEventProcess::newTweet( Tweet *tweet )
{
	
}

void TimedEventProcess::draw()
{

}
