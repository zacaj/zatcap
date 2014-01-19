#pragma once
class Tweet;
#include "zatcap.h"

class Process
{
public:
	Process()
	{
		shouldRemove=0;
	}
	virtual ~Process(){};
	virtual void update()=0;
	virtual void draw()=0;
	virtual void newTweet(Tweet *tweet){}
	/**
		\param button If -1, a mouse motion

		*/
	virtual bool mouseButtonEvent(int x,int y,int button,int pressed){return 0;}
	virtual bool keyboardEvent(int key,int pressed,int mod){return 0;}

	virtual void deleteTweet( string id ){};


	bool shouldRemove;
};