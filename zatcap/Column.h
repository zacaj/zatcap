#pragma once
#include "process.h"
#include "zatcap.h"
#include <time.h>
#include "Tweet.h"
#include <SDL_mutex.h>

#define COLUMNHEADERHEIGHT 35

class Column :
	public Process
{
public:
	int x,y;//temp
	int w;
	bool minimized;
	string columnName;
	map<string,Item*> m_tweets;
	void init(float w,string name);
	virtual ~Column(void);
	virtual void update();
	virtual void draw();
	virtual void newTweet(Item *tweet)=0;
	virtual void deleteTweet(string id);
	void add(Item* tweet);
	bool isColumn(){return 1;}
};