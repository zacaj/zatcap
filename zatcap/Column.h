#pragma once
#include "process.h"
#include "zatcap.h"
#include <time.h>
#include "Button.h"
#include "Tweet.h"
#include <SDL_mutex.h>

#define COLUMNHEADERHEIGHT 35

class Column :
	public Process
{
public:
	int x,y;//temp
	int w,rw;
	bool minimized;
	int scroll;
	string columnName;
	string emptyColumnText;
	int scrollBarHeight;
	bool onOff;
	bool highlightScrollbar;
	int scrollBarY;
	int tweetHeight;
	map<string,TweetInstance*> m_tweets;
	Column(float w,string name);
	virtual ~Column(void);
	virtual void update();
	virtual void draw();
	SDL_mutex *drawingMutex;
	virtual void newTweet(Tweet *tweet)=0;

	virtual bool mouseButtonEvent( int x,int y,int button,int pressed );

	virtual void deleteTweet(string id);
	bool drawRefreshButton();
	bool redrawAllTweets;
};

extern int columnHorizontalScroll;
extern int columnHorizontalRenderAt;//lower priority columns on right, because reasons