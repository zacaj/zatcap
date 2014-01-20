#pragma once
class Tweet;
#include "zatcap.h"
class Item;
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
	virtual void newTweet(Item *tweet){}
	virtual void deleteTweet( string id ){};

	virtual bool isColumn(){return 0;}
	bool shouldRemove;
};