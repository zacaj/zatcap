#pragma once
#include "zatcap.h"
//void=twitCurl
void twitterInit(void *twit);
#include <time.h>
#include "file.h"
class Tweet;
class Retweet;
class Item;
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
		_pic="defaultpic.png";
	}
	string _pic;
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
		msystem("\""+settings::browserCommand+"\" \""+realUrl+"\"");
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
		msystem("\""+settings::browserCommand+"\" \"https://twitter.com/"+username+"\"");
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
		msystem("\""+settings::browserCommand+"\" \"https://twitter.com/search?q=%23"+name.substr(1,name.size()-1)+"\"");
		//s/tring cmd=settings::browserCommand+" \"https://twitter.com/search?q=%23"+name.substr(1,name.size()-1)+"\"";
		//system(cmd.c_str());
	}
	void write(FILE *fp)
	{
		wuchar(2,fp);
		wuint(start,fp);
		wuint(end,fp);
		wstr(name,fp);
	}
};

Item* getTweet(string id);



void deleteTweet(string id);

//string=id
extern map<string,Item*> tweets;
void addTweet(Item** tweet);
extern Mutex tweetsMutex;


void parseRestTweets(string json);

Tweet* processTweet(Json::Value jtweet);

void saveTweets();

int loadOlderTweets(void *data);
time_t mtimegm( struct tm * timeptr );
class AvitarDownloader;
extern AvitarDownloader *aviDownloader;