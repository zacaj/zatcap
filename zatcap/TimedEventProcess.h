#pragma once
#include "process.h"
class User;
class TimedEventProcess :
	public Process
{
public:
	float lastTime;
	void *data;
	float interval;
	 void (*fn)(void*);
	TimedEventProcess();
	~TimedEventProcess(void);

	virtual void update();
	virtual void draw();

	virtual void newTweet( Tweet *tweet );

};

