#pragma once
#include "zatcap.h"
//void=twitCurl
int twitterInit(void *twit);
#include <time.h>
#include <assert.h>
#include "file.h"
extern SDL_Surface *defaultUserPic;
extern SDL_Surface *defaultSmallUserPic;
extern SDL_Surface *defaultMediumUserPic;
extern bool loggedIn;
extern string replyId;
extern twitCurl *twit;
class User;
void processUserPics(User *user);
class User
{
public:
	string username,name;
	string id;
	User()
	{
		_pic=_smallPic=_mediumPic=NULL;
	}
	SDL_Surface *_pic;
	SDL_Surface *_smallPic;
	SDL_Surface *_mediumPic;
	SDL_Surface* pic()
	{
		if(_pic!=NULL)
			return _pic;
		return defaultUserPic;
	}
	SDL_Surface* smallPic()
	{
		if(_smallPic!=NULL)
			return _smallPic;
		return defaultSmallUserPic;
	}
	SDL_Surface* mediumPic()
	{
		if(_mediumPic!=NULL)
			return _mediumPic;
		return defaultMediumUserPic;
	}
	string picURL;
	string getPicPath();
	void save();
};
User *getUser(string id);
User * getUser(Json::Value root);

extern map<string,User*> users;

class Entity
{
public:
	uchar type;
	int start,end;
	string text;
	virtual void click()=0;
	virtual void write(FILE *fp)=0;
};
class URLEntity:public Entity
{
public:
	URLEntity()
	{
		type=0;
	}
	string realUrl;
	string displayUrl;
	void click()
	{
		printf("%s\n",realUrl.c_str());
		string cmd=settings::browserCommand+" \""+realUrl+"\"";
		system(cmd.c_str());
	}
	void write(FILE *fp)
	{
		wuchar(0,fp);
		wuint(start,fp);
		wuint(end,fp);
		wstr(realUrl,fp);
		wstr(displayUrl,fp);
	}
};
class UserEntity:public Entity
{
public:
	UserEntity()
	{
		type=1;
	}
	string id;
	string name;
	string username;
	void click()
	{
		string cmd=settings::browserCommand+" \"https://twitter.com/"+username+"\"";
		system(cmd.c_str());
	}
	void write(FILE *fp)
	{
		wuchar(1,fp);
		wuint(start,fp);
		wuint(end,fp);
		wstr(id,fp);
		wstr(name,fp);
		wstr(username,fp);
	}
};
class HashtagEntity:public Entity
{
public:
	HashtagEntity()
	{
		type=2;
	}
	string name;
	void click()
	{
		string cmd=settings::browserCommand+" \"https://twitter.com/search?q=%23"+name.substr(1,name.size()-1)+"\"";
		system(cmd.c_str());
	}
	void write(FILE *fp)
	{
		wuchar(2,fp);
		wuint(start,fp);
		wuint(end,fp);
		wstr(name,fp);
	}
};

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

Tweet* getTweet(string id);



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

void deleteTweet(string id);

//string=id
extern map<string,Tweet*> tweets;
void addTweet(Tweet *tweet);

extern bool tweetsInUse;

void parseRestTweets(string json);

Tweet* processTweet(Json::Value jtweet);

void saveTweets();

int loadOlderTweets(void *data);