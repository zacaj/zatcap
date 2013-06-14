#pragma once

#include "zatcap.h"
#include "twitter.h"
#include <set>
class TweetInstance;
#define FAVORITE 2
#define FOLLOW 3
#define DIRECTMESSAGE 4
#define ACTIVITY 5
class Column;
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
	bool operator !=(const Item &i) const;
	virtual string getHtml(string columnName) =0;
	virtual void write(FILE *fp)=0;
	set<Column*> instances;
	Item()
	{
		read=0;
	}
};

class Favorite:public Item
{
public:
	User *favoriter;
	string action;
	Favorite(string _text,string _id,User *_favoriter,string _action,struct tm _time)
	{
		_type=FAVORITE;
		action=_action;
		text=_text;
		id=_id+"fav";
		favoriter=_favoriter;
		timeTweeted=_time;
		timeTweetedInSeconds=mktime(&timeTweeted);
	}
	virtual string getHtml( string columnName );

	virtual void write( FILE *fp );

};
class Follow:public Item
{
public:
	User *follower;
	string action;
	Follow(User *_follower,struct tm _time)
	{
		_type=FOLLOW;
		id="follow"+_follower->id;
		follower=_follower;
		timeTweeted=_time;
		timeTweetedInSeconds=mktime(&timeTweeted);
	}
	virtual string getHtml( string columnName );

	virtual void write( FILE *fp );

};
class Activity:public Item
{
public:
	string username;
	string stoppable;
	Activity(string _action,string _stoppable="",string name="")
	{
		_type=ACTIVITY;
		id="activity"+_action;
		for(int i=0;i<id.size();i++)
		{
			if(id[i]=='\\' || id[i]=='\'' || id[i]=='\"')
				id[i]='_';
		}
		text=_action;
		username=name;
		stoppable=_stoppable;
		timeTweeted=getTime();
		timeTweetedInSeconds=mktime(&timeTweeted);
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
		isDM=0;
	}
	bool favorited;
	bool retweeted;
	bool isDM;
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
	virtual int cachedDraw(TweetInstance *instance);
	virtual void write(FILE *fp);
	virtual string getHtml(string columnName) ;
};
class DirectMessage:public Tweet
{
public:
	DirectMessage()
	{
		_user=NULL;
		_type=DIRECTMESSAGE;
		read=0;
		isDM=1;
		favorited=0;
		retweeted=0;
		replyTo="";
	}
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
	virtual int cachedDraw(TweetInstance *instance);
	virtual void write(FILE *fp);

	virtual string getHtml( string columnName );

};

void favoriteTweet(void *data);
void unfavoriteTweet(void *data);
void retweetTweet(void *data);
void deleteTweet(void *data);