#pragma once
#include "process.h"
class User;
class UserRetryProcess :
	public Process
{
public:
	int ticks;
	User *user;
	UserRetryProcess(User *_user);
	~UserRetryProcess(void);

	virtual void update();
	virtual void draw();

	virtual void newTweet( Tweet *tweet );

};

