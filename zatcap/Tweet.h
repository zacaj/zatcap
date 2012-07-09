#pragma once

#include "zatcap.h"
#include "twitter.h"

class Tweet
{
public:
	char _type;
	string text;
	string originalText;
	string id;
	string userid;
	Tweet()
	{
		_user=NULL;
		_type=0;
		read=0;
	}
	struct tm timeTweeted;
	time_t timeTweetedInSeconds;
	bool favorited;
	bool retweeted;
	int read;
	vector<Entity *> entities;
	User* user()
	{
		if(_user==NULL)
			_user=getUser(userid);
		return _user;
	}
	User *_user;
	virtual int draw(int x,int y,int w,int background);
	virtual int drawButtons(int x,int y,int w,int h,bool highlighted,SDL_Surface* _screen=NULL);
	virtual void write(FILE *fp);
};

class Retweet:public Tweet
{
public:	
	User *retweetedBy;
	string originalID;
	int nRetweet;
	struct tm timeRetweeted;
	time_t timeRetweetedInSeconds;
	Tweet *_original;
	Retweet()
	{
		retweetedBy=NULL;
		_type=1;
		_original=NULL;
		read=0;
	}
	Tweet* original()
	{debugHere();
	if(_original!=NULL)
		return _original;
	return _original=getTweet(originalID);
	}

	virtual int draw( int x,int y,int w,int background );
	virtual void write(FILE *fp);
};

class TweetInstance
{
public:
	Tweet *tweet;
	SDL_Surface *pic;
	textPos *p;
	vector<int> widths;
	int background;
	SDL_Surface *surface;
	int buttonX,buttonY,buttonHeight;
	TweetInstance(Tweet *_tweet,int w,int _background );
	~TweetInstance();
	void draw(int x,int y);
	bool needsRefresh;
};