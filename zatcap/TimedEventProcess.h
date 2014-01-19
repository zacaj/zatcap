#pragma once
#include "process.h"
class User;
class TimedEventProcess :
	public Process
{
public:
	int ticks;
	void *data;
	int maxTicks;
	 int (*fn)(void*);
	TimedEventProcess();
	~TimedEventProcess(void);

	virtual void update();
	virtual void draw();

	virtual void newTweet( Tweet *tweet );

};

