#pragma once

#include "zatcap.h"
#include "twitter.h"
class TweetInstance;

class Tweet
{
public:
	char _type;
	string text;
	string originalText;
	string id;
	string userid;
	string html;
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
	string replyTo;
	User* user()
	{
		if(_user==NULL)
			_user=getUser(userid);
		return _user;
	}
	User *_user;
	virtual int draw(int x,int y,int w,int background);
	virtual int draw(TweetInstance *instance,int w);
	virtual int drawButtons(int x,int y,int w,int h,bool highlighted);
	virtual int cachedDraw(TweetInstance *instance);
	virtual void write(FILE *fp);
	virtual string getHtml() ;
};
#define TOGGLECONVODISPLAY -1
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
	virtual int draw(TweetInstance *instance,int w);
	virtual int cachedDraw(TweetInstance *instance);
	virtual void write(FILE *fp);
};

class TweetInstance
{
public:
	Tweet *tweet;
	TweetInstance *replyTo;
	string pic;
	string pic2;
	bool read;
	int w;
	vector<int> widths;
	bool drawReply;
	int background;
	int buttonX,buttonY,buttonHeight;
	TweetInstance(Tweet *_tweet,int w,int _background );
	~TweetInstance();
	int draw(int x,int y);
	void refresh(int w);
};