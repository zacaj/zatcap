#pragma once

#include "zatcap.h"
#include "twitter.h"
class TweetInstance;
#define FAVORITE 2
#define FOLLOW 3
class Item
{
public:
	char _type;
	string text;
	string id;
	string html;
	struct tm timeTweeted;
	time_t timeTweetedInSeconds;
	int read;
	virtual string getHtml(string columnName) =0;
	virtual void write(FILE *fp)=0;
};

class Favorite:public Item
{
public:
	User *favoriter;
	string action;
	Favorite(string _text,string _id,User *_favoriter,string _action)
	{
		_type=FAVORITE;
		action=_action;
		text=_text;
		id=_id+"fav";
		favoriter=_favoriter;
	}
	virtual string getHtml( string columnName );

	virtual void write( FILE *fp );

};
class Follow:public Item
{
public:
	User *follower;
	string action;
	Follow(User *_follower)
	{
		_type=FOLLOW;
		id="follow"+_follower->id;
		follower=_follower;
	}
	virtual string getHtml( string columnName );

	virtual void write( FILE *fp );

};

class Tweet:public Item
{
public:
	string originalText;
	string userid;
	Tweet()
	{
		_user=NULL;
		_type=0;
		read=0;
	}
	bool favorited;
	bool retweeted;
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
	virtual string getHtml(string columnName) ;
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
	return _original=(Tweet*)getTweet(originalID);
	}

	virtual int draw( int x,int y,int w,int background );
	virtual int draw(TweetInstance *instance,int w);
	virtual int cachedDraw(TweetInstance *instance);
	virtual void write(FILE *fp);

	virtual string getHtml( string columnName );

};

class TweetInstance
{
public:
	Item *tweet;
	TweetInstance *replyTo;
	string pic;
	string pic2;
	bool read;
	int w;
	vector<int> widths;
	bool drawReply;
	int background;
	int buttonX,buttonY,buttonHeight;
	TweetInstance(Item *_tweet,int w,int _background );
	~TweetInstance();
	int draw(int x,int y);
	void refresh(int w);
};
void favoriteTweet(void *data);
void unfavoriteTweet(void *data);
void retweetTweet(void *data);
void deleteTweet(void *data);