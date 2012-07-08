#include "twitter.h"
#include "zatcap.h"

#include "twitcurl.h"
#include "stream.h"
#include "WaitIndicator.h"
#include <assert.h>
#include <json\writer.h>
#include "TimedEventProcess.h"
#include "MentionColumn.h"
#include <SDL_rotozoom.h>
#include "Textbox.h"
#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <iosfwd>
using namespace colors;
map<string,Tweet*> tweets;
map<string,User*> users;
twitCurl *twit=NULL;
bool loggedIn=0;

SDL_Surface *defaultUserPic;
SDL_Surface *defaultSmallUserPic;
SDL_Surface *defaultMediumUserPic;
string replyId="";
int twitterInit( void  *_twit )
{
	twit=new twitCurl();
	*((void**)_twit)=twit;
	twit->setTwitterApiType(twitCurlTypes::eTwitCurlApiFormatJson);
	WaitIndicator *wait=new WaitIndicator("Signing in to Twitter...");
	{
		processes[102.4]=wait;
	}
	printf("Logging in...\n");
	loadUser(twit);
	loggedIn=1;
	((MentionColumn*)processes[2.5])->term=username;
	wait->text="Loading older tweets";
	updateScreen=1;debugHere();
#if 1
	if(settings::tweetsToLoadOnStartup)
	{debugHere();
		string tmpString;debugHere();
		//assert((tmpString=twit->timelineHomeGet(false,true,settings::tweetsToLoadOnStartup,"",""))!="");debugHere();//settings::tweetsToLoadOnStartup+50
		while((tmpString=twit->timelineUserGet(false,true,settings::tweetsToLoadOnStartup,""))=="");debugHere();//settings::tweetsToLoadOnStartup+50
		parseRestTweets(tmpString);debugHere();
	}
#endif
	wait->shouldRemove=1;debugHere();
	if(settings::enableStreaming)
		openUserStream(twit);
		debugHere();
	return 1;
}

int get_utc_offset() {

  time_t zero = 24*60*60L;
  struct tm * timeptr;
  int gmtime_hours;

  /* get the local time for Jan 2, 1900 00:00 UTC */
  timeptr = localtime( &zero );
  gmtime_hours = timeptr->tm_hour;

  /* if the local time is the "day before" the UTC, subtract 24 hours
    from the hours to get the UTC offset */
  if( timeptr->tm_mday < 2 )
    gmtime_hours -= 24;

  return gmtime_hours;

}

/*
  the utc analogue of mktime,
  (much like timegm on some systems)
*/
time_t mtimegm( struct tm * timeptr ) {

  /* gets the epoch time relative to the local time zone,
  and then adds the appropriate number of seconds to make it UTC */
  return mktime( timeptr ) + get_utc_offset() * 3600;

}

Entity* readEntity(FILE *fp)
{
	uchar type=ruchar(fp);
	switch(type)
	{
	default:
		return NULL;
	case 0:
		{
			URLEntity *entity=new URLEntity;
			entity->start=ruint(fp);
			entity->end=ruint(fp);
			entity->realUrl=rstr(fp);
			entity->displayUrl=rstr(fp);
			entity->text=entity->displayUrl;
			return entity;
		}
	case 1:
		{
			UserEntity *entity=new UserEntity;
			entity->start=ruint(fp);
			entity->end=ruint(fp);
			entity->id=rstr(fp);
			entity->name=rstr(fp);
			entity->username=rstr(fp);
			entity->text=entity->username;
			return entity;
		}
	case 2:
		{
			HashtagEntity *entity=new HashtagEntity;
			entity->start=ruint(fp);
			entity->end=ruint(fp);
			entity->name=rstr(fp);
			entity->text=entity->name;
			return entity;
		}
	}
}

void readTweetFile(string path)
{
	printf("Reading %s\n",path.c_str());debugHere();
	FILE *fp=fopen(path.c_str(),"rb");
	assert(fp);
	uchar version=ruchar(fp);
	vector<Retweet*> tweetsMissing;
	switch(version)
	{
	case 2:
	case 3:
		{
			int n=ruint(fp);
			for(int i=0;i<n;i++)
			{
                debug("Reading tweet %i/%i\n",i,n);
				uchar type=ruchar(fp);
				switch(type)
				{
				default:
					printf("ERROR: unknown tweet type %i\nFailed reading tweet %i/%i, skipping rest of file\n",type,i,n);
					fclose(fp);
					return;
					break;
				case 0://normal
					{
						Tweet *tweet=new Tweet;
						tweet->id=rstr(fp);
						fread(&tweet->timeTweeted,sizeof(int),9,fp);
						tweet->timeTweetedInSeconds=mktime(&tweet->timeTweeted);
						tweet->text=rstr(fp);
						tweet->userid=rstr(fp);
						tweet->_user=getUser(tweet->userid);
						tweet->favorited=ruchar(fp);
						tweet->retweeted=ruchar(fp);
						tweet->read=ruchar(fp);debugHere();
						if(version==3)
						{
							int nEntity=ruchar(fp);
							for(int iEntity=0;iEntity<nEntity;iEntity++)
							{
								Entity *entity=readEntity(fp);
								if(entity==NULL)
								{
									printf("Failure reading tweet %i/%i, skipping rest of file\n",i,n);
									fclose(fp);
									return;
								}
								tweet->entities.push_back(entity);
							}
						}
						addTweet(tweet);debugHere();
					}
					break;
					tweets;
				case 1://retweet
					{
						Retweet *retweet=new Retweet;
						retweet->id=rstr(fp);
						fread(&retweet->timeTweeted,sizeof(int),9,fp);
						retweet->timeTweetedInSeconds=mktime(&retweet->timeTweeted);
						retweet->text=rstr(fp);
						retweet->userid=rstr(fp);
						retweet->_user=getUser(retweet->userid);
						retweet->favorited=ruchar(fp);
						retweet->retweeted=ruchar(fp);
						retweet->read=ruchar(fp);
						if(version==3)
						{
							int nEntity=ruchar(fp);
							for(int iEntity=0;iEntity<nEntity;iEntity++)
							{
								Entity *entity=readEntity(fp);
								if(entity==NULL)
								{
									printf("Failure reading tweet %i/%i, skipping rest of file\n",i,n);
									fclose(fp);
									return;
								}
								retweet->entities.push_back(entity);
							}
						}
						retweet->retweetedBy=getUser(rstr(fp));
						retweet->originalID=rstr(fp);
						retweet->nRetweet=ruint(fp);
						retweet->_original=getTweet(retweet->originalID);
						//tweetsMissing.push_back(retweet);
						fread(&retweet->timeRetweeted,sizeof(struct tm),1,fp);
						retweet->timeRetweetedInSeconds=mktime(&retweet->timeRetweeted);debugHere();
						addTweet(retweet);debugHere();
					}
					break;
				}
			}
			printf("Read %i tweets from archive\n",n);
		}
		break;
	default:
		printf("ERROR: could not read %s, unknown version %i!\n",path.c_str(),version);
		break;
	}
	//for(int i=0;i<tweetsMissing.size();i++)
	//	tweetsMissing[i]->_original=getTweet(tweetsMissing[i]->originalID);
	fclose(fp);debugHere();
}
void parseRestTweets( string json )
{
	Json::Reader reader;
	Json::Value root;
	FILE *fp=fopen("test.json","wb");
	fwrite(json.c_str(),json.size(),1,fp);
	fclose(fp);
	assert(reader.parse(json,root));debugHere();
	if(root.isArray())
	{
		int s=root.size();
		for(int i=0;i<root.size();i++)
		{
			debug("processing tweet %i\n",i);
			Json::Value tweet=root[i];debugHere();
			if(tweet.isNull())
				continue;
			processTweet(root[i]);debugHere();
		}
	}
	else
	{
		tweets;
		//confusion
	}
}

struct profilepic
{
	SDL_Surface **img;
	string url;
	string name;
	User *user;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	debugHere();
	debug("stream=%i\n",stream);
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
string getExt(string);
void processUserPics(User *user);
string getFile(string path)
{
	int i;
	for(i=path.size()-1;i>=0;i--)
	{
		if(path[i]=='/' || path[i]=='\\')
			break;
	}
	if(i!=0)
	{
		i++;
		return(path.substr(i,path.length()-i));
	}
	else
		return path;
}

void deleteTweet( string id )
{debugHere();
	while(tweetsInUse);debugHere();
	tweetsInUse=1;
	map<string,Tweet*>::iterator tweet=tweets.find(id);
	if(tweet==tweets.end())
		return;
	for (map<float,Process*>::iterator it=processes.begin();it!=processes.end();it++)
		it->second->deleteTweet(tweet->first);
	delete tweet->second;debugHere();
	tweets.erase(tweet);debugHere();
	tweetsInUse=0;debugHere();
}
bool tweetsInUse=0;
Tweet* getTweet( string id )
{
	if(tweets.find(id)!=tweets.end())
		return tweets[id];
	int tries=0;
	string tmpString;
	while((tmpString=twit->statusShowById(id))=="" && tries++<10);
	Json::Reader reader;
	Json::Value root;
	reader.parse(tmpString,root);
	return processTweet(root);
}

string removeLast(string id)
{
	id.replace(id.end()-15,id.end(),14,'0');
	while(id.size()<25)
		id.insert(0,"0");
	return id;
}

void saveTweets()
{
	if(tweets.empty())
		return;
	map<string,Tweet*>::iterator top=--tweets.end();
	string prefix=removeLast(top->first);
	map<string,Tweet*>::iterator bottom=tweets.upper_bound(prefix);
	top++;
	while(1)
	{
		int n=std::distance(bottom,top);
		FILE *fp=fopenf((string("tweets/")+prefix),"rb");
		if(!fp || ruint(fp)!=n)
		{
			if(fp)
			{
				fclose(fp);
				rename((string("tweets/")+prefix).c_str(),(string("tweets/")+prefix+".backup").c_str());
			}
			fp=fopen((string("tweets/")+prefix).c_str(),"wb");
			wuchar(3,fp);//version
			wuint(n,fp);
			for(map<string,Tweet*>::iterator it=bottom;it!=top;it++)
			{
				it->second->write(fp);
			}
		}
		fclose(fp);
		top=bottom;
		if(bottom==tweets.begin())
			break;
		prefix=removeLast((--bottom)->first);
		bottom=tweets.upper_bound(prefix);
	}
}

bool loadingTweets=0;

int loadOlderTweets(void *data)
{
	if(!loggedIn)
		return -1;
	if(loadingTweets)
		return 0;
	loadingTweets=1;
	string oldestLoadedId;
	if(!tweets.empty())
		oldestLoadedId=removeLast((tweets.begin())->first);
	else
		oldestLoadedId="9999999999999999999999999";
	DIR *dir=opendir("tweets");
	if(dir)
	{
		dirent *ent;
		string last="";
		while((ent=readdir(dir))!=NULL)
		{
			if(ent->d_name[0]!='.' && !isalpha(ent->d_name[strlen(ent->d_name)-2]) && string(ent->d_name)<oldestLoadedId)
			{
				last=ent->d_name;
			}
		}
		if(!last.empty())
		{
			readTweetFile((string("tweets/")+last));
		}
		closedir(dir);
	}
	loadingTweets=0;
	return 0;
}

void addTweet( Tweet *tweet )
{debugHere();
	while(tweetsInUse);
	tweetsInUse=1;debugHere();
	map<string,Tweet*>::iterator tw=tweets.find(tweet->id);
	if(tw==tweets.end())
	{
		tweets[tweet->id]=tweet;

		for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
			it->second->newTweet(tweet);
	}
	else
	{debugHere();
		for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
			it->second->deleteTweet(tw->first);
		delete tw->second;
		tw->second=NULL;
		tweets.erase(tw);
		tweetsInUse=0;debugHere();
		addTweet(tweet);debugHere();
	}
	tweetsInUse=0;debugHere();
}

int loadProfilePic(void *ptr)
{
	profilepic *pic=(profilepic*)ptr;
	SDL_Surface** img=(SDL_Surface**)pic->img;
retry:
	debugHere();
	CURL *curl= curl_easy_init();
	string r=pic->name;
	string path="profilepics/temp"+r+getFile(pic->url);
	assert(curl);
	{
		FILE *fp = fopen(path.c_str(),"wb");
		debug("%i %s\n",fp,path.c_str());
		debugHere();
		curl_easy_setopt(curl, CURLOPT_URL, pic->url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		debugHere();
		if(ftell(fp)<5)
		{
			debugHere();
			fclose(fp);
			goto retry;
		}
		debugHere();
		fclose(fp);
	}
	SDL_Surface* temp=loadImage(path);debugHere();
	if(temp!=NULL)
	{debugHere();
		string path2="profilepics/"+pic->name+"."+getExt(getFile(pic->url));debugHere();
		rename(path.c_str(),path2.c_str());debugHere();
		*img=temp;//loadImage(path2);
		if((*img)->w!=48 || (*img)->h!=48)
		{
			//SDL_FreeSurface(temp);
			temp=(*img);debugHere();
			(*img)=zoomSurface((*img),48.f/ (*img)->w,48.f/ (*img)->h,1);debugHere();//haha @ needing space after /
			SDL_FreeSurface(temp);debugHere();
		}debugHere();
		if(!pic->user->_pic)
			goto retry;debugHere();
		processUserPics(pic->user);debugHere();
	}
	else
	{
		if(temp==NULL)
			printf("loading failed of %s?\n",path.c_str());
		else
			printf("ERROR: hey look zacaj was lazy and didnt implement support for other size profile pictures hoping it would never come up, go punch him in the face!\n");
		*img=NULL;
		goto retry;
	}
	delete ptr;debugHere();
	return 0;
}

void processUserPics(User *user)
{
	user->_smallPic=zoomSurface(user->_pic,(float)settings::retweeterPicSize/user->_pic->w,(float)settings::retweeterPicSize/user->_pic->h,1);
	user->_mediumPic=zoomSurface(user->_pic,(float)settings::retweeteePicSize/user->_pic->w,(float)settings::retweeteePicSize/user->_pic->h,1);
}

User * getUser(Json::Value root)
{debugHere();
	string id=root["id_str"].asString();
	if(users.find(id)!=users.end() && users[id]->name!=users[id]->id)
		return users[id];
	if(root["name"].isNull())//stripped tweet
		return getUser(id);
	User *user=new User;
	user->id=id;
	user->name=root["name"].asString();
	user->username=root["screen_name"].asString();
	//user->pic=defaultUserPic;
	user->picURL=root["profile_image_url"].asString();
	FILE *fp;
	if((fp=fopen(user->getPicPath().c_str(),"r")))
	{debugHere();
		user->_pic=loadImage(user->getPicPath());debugHere();
		processUserPics(user);debugHere();
		fclose(fp);
	}
	else//todo should check for updates
	{
		profilepic *pic=new profilepic;
		pic->img=&user->_pic;
		pic->url=root["profile_image_url"].asString();
		pic->name=user->username;
		pic->user=user;debugHere();
		SDL_CreateThread(loadProfilePic,pic);
	}
	user->save();
	users[id]=user;debugHere();
	return user;
}
int fixUser(void *ptr)
{
	TimedEventProcess *u=(TimedEventProcess*)ptr;
	User *user=(User*)u->data;
	string tmpString;
	while((tmpString=twit->userGet(user->id,true))=="");

	Json::Reader reader;
	Json::Value root;
	if(reader.parse(tmpString,root))
	{
		getUser(root);
		u->shouldRemove=1;
	}
	return 0;
}

User * getUser( string id )
{debugHere();
	if(users.find(id)!=users.end())
		return users[id];
	if(fileExists(string("users/")+id))
	{debugHere();
		User *user=new User;
		FILE *fp=fopen((string("users/")+id).c_str(),"rb");
		assert(fp);
		user->id=id;
		user->username=rstr(fp);
		user->name=rstr(fp);
		user->picURL=rstr(fp);
		FILE *fp2;debugHere();
		if((fp2=fopen(user->getPicPath().c_str(),"r")))
		{debugHere();
			user->_pic=loadImage(user->getPicPath());debugHere();
			processUserPics(user);debugHere();
			fclose(fp2);
		}
		else//todo should check for updates
		{
			profilepic *pic=new profilepic;
			pic->img=&user->_pic;
			pic->url=user->picURL;
			pic->name=user->username;
			pic->user=user;debugHere();
			SDL_CreateThread(loadProfilePic,pic);
		}debugHere();
		fclose(fp);debugHere();
		return user;
	}debugHere();
	User *user=new User;
	string tmpString;debugHere();
	printf("Loading information for user %s\n",id.c_str());
	while((tmpString=twit->userGet(id,true))=="");
	debugHere();
	Json::Reader reader;
	Json::Value root;
	if(!reader.parse(tmpString,root))//user failed to load
	{debugHere();
		User *user=new User();
		user->id=id;
		user->name=id;
		user->username=id;
		users[id]=user;
		TimedEventProcess *timer=new TimedEventProcess;
		timer->data=user;
		timer->fn=fixUser;
		processes[-10000.f]=timer;
		return user;
	}debugHere();
	return getUser(root);

	//return user;//todo
}

bool tweetsInuse=0;

std::string User::getPicPath()
{
	return string("profilepics/")+username+"."+getExt(picURL);
}

Uint32 getBackgroundColor(int background,int read)
{
	if(background==0 && read)
		return colors::readTweetColor;
	else if(background==0 && !read)
		return colors::unreadTweetColor;
	else if(background==1 && read)
		return colors::readTweetColor2;
	else if(background==1 && !read)
		return colors::unreadTweetColor2;
	else if(background==2)
		return colors::hoverTweetColor;
}

#define drawEntities { \
{ \
	textPos *p=new textPos[entities.size()*2]; \
	for(int i=0;i<entities.size();i++) \
{ \
	p[i*2].pos=entities[i]->start; \
	p[i*2].x=-10000; \
	p[i*2+1].pos=entities[i]->end; \
	p[i*2+1].x=-10000; \
} \
	vector<int> widths; \
	height=drawTextWrappedp(text.c_str(),x+7+pic->w+8,height+1,textWidth,13,NULL,0,p,entities.size()*2,widths); \
	for(int i=0;i<entities.size();i++) \
{ \
	if(p[i*2].y==p[i*2+1].y) \
{ \
	bool hover=hoverButton(p[i*2].x,p[i*2].y,p[i*2+1].x-p[i*2].x,p[i*2].h); \
	if(colors::entityColorR[entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[entities[i]->type+hover*3]!=colors::entityColorR[entities[i]->type+hover*3] || colors::entityColorB[entities[i]->type+hover*3]!=colors::textColorB) \
	{ \
	boxColor(screen,p[i*2].x,p[i*2].y,p[i*2+1].x,p[i*2].y+p[i*2].h,getBackgroundColor(background,read)); \
	drawText(entities[i]->text.c_str(),p[i*2].x,p[i*2].y,13,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3]); \
	} \
	if(hover || !settings::underlineLinksWhenHovered) \
	{ \
	boxColor(screen,p[i*2].x,p[i*2].y+13,p[i*2+1].x,p[i*2].y+13+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
	} \
	if(doButton(p[i*2].x,p[i*2].y,p[i*2+1].x-p[i*2].x,p[i*2].h)) \
	{ \
	entities[i]->click(); \
	} \
} \
	else \
	{ \
	/*boxRGBA(screen,p[i*2].x,p[i*2].y,x+7+pic->w+8+widths[p[i*2].line],p[i*2].y+p[i*2].h,255,0,255,64);
	//boxRGBA(screen,x+7+pic->w+8,p[i*2+1].y,p[i*2+1].x,p[i*2+1].y+p[i*2+1].h,255,0,255,64);*/ \
	bool hover=hoverButton(p[i*2].x,	p[i*2].y,	x+7+pic->w+8+widths[p[i*2].line]-p[i*2].x,	p[i*2].h) \
	||	hoverButton(x+7+pic->w+8,	p[i*2+1].y,	p[i*2+1].x-(x+7+pic->w+8),	p[i*2+1].h); \
	if(colors::entityColorR[entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[entities[i]->type+hover*3]!=colors::entityColorR[entities[i]->type+hover*3] || colors::entityColorB[entities[i]->type+hover*3]!=colors::textColorB) \
	{ \
	if(p[i*2].w) \
	{ \
	boxColor(screen,p[i*2].x,	p[i*2].y,	x+7+pic->w+8+widths[p[i*2].line],	p[i*2].y+p[i*2].h,	getBackgroundColor(background,read)); \
	drawText(entities[i]->text.substr(0,p[i*2].w).c_str(),p[i*2].x,p[i*2].y,13,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3]); \
	} \
	if(p[i*2+1].w) \
	{ \
	boxColor(screen,x+7+pic->w+8,	p[i*2+1].y,	p[i*2+1].x,	p[i*2+1].y+p[i*2+1].h,getBackgroundColor(background,read)); \
	drawText(entities[i]->text.substr(p[i*2].w,entities[i]->text.size()-p[i*2].w).c_str(),x+7+pic->w+8,p[i*2+1].y,13,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3]); \
	} \
	} \
	if(hover || !settings::underlineLinksWhenHovered) \
	{ \
	boxColor(screen,p[i*2].x,p[i*2].y+13,x+7+pic->w+8+widths[p[i*2].line],p[i*2].y+13+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
	boxColor(screen,x+7+pic->w+8,p[i*2+1].y+13,p[i*2+1].x,p[i*2+1].y+13+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
	} \
	if(	doButton(p[i*2].x,	p[i*2].y,	x+7+pic->w+8+widths[p[i*2].line]-p[i*2].x,	p[i*2].h) \
		||	doButton(x+7+pic->w+8,	p[i*2+1].y,	p[i*2+1].x-(x+7+pic->w+8),	p[i*2+1].h) \
		) \
	{ \
	entities[i]->click(); \
	} \
	} \
	/*boxRGB(screen,p[i*2].x,p[i*2].y,p[i*2].x+1,p[i*2].y+p[i*2].h,255,0,255); boxRGB(screen,p[i*2+1].x,p[i*2+1].y,p[i*2+1].x+1,p[i*2+1].y+p[i*2+1].h,0,255,255); */ \
} \
delete p; }} \

int Tweet::draw( int x,int y,int w,int background)
{
	SDL_Surface *pic=user()->pic();
	int height=y;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
draw:
	int nameWidth=200;
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,15,first,usernameTextColorR,usernameTextColorG,usernameTextColorB);
	char date[1000];
	if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
	{
		tm *t=localtime(&timeTweetedInSeconds);
		strftime(date,1000,"%I:%M",t);
		if(date[0]=='0')
			strcpy(date,date+1);
	}
	else
		strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
	int timeWidth=drawTextr(date,x+w-15,y+5,13,timeTextColorR,timeTextColorG,timeTextColorB);//time
	int heightBeforeText=height;
	{{drawEntities}}
	if(height<pic->h+10+y)
		height=pic->h+10+y;
	if(height<y+48+18+3)
		height=y+48+18+3;
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	int newx;
	if(mx>x && mx<x+w && my>y && my<height)
	{
		//background=2;
		newx=drawButtons(x,y,w,height,1);
	}
	else
		newx=drawButtons(x,y,w,height,0);
	if(newx<textWidth+(x+7+pic->w+8))
	{
		textWidth=newx-(x+7+pic->w+8)-2;
	}
	if(first)
	{
		boxColor(screen,x,y,x+w-4,height,getBackgroundColor(background,read));
		first=0;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	drawSprite(pic,screen,0,0,x+5,y+5,pic->w,pic->h);
	if(hoverButton(x+5,y+5,48,48))
		drawSprite(reply[2],screen,0,0,x+5,y+5,reply[2]->w,reply[2]->h);
	return height;
}


int Retweet::draw( int x,int y,int w,int background )
{
	background=!read;
	SDL_Surface *pic=user()->pic();
	int height=y;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
draw:
	int nameWidth=200;
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,15,first,usernameTextColorR,usernameTextColorG,usernameTextColorB);
	char date[1000];
	if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
	{
		tm *t=localtime(&timeTweetedInSeconds);
		strftime(date,1000,"%I:%M",t);
		if(date[0]=='0')
			strcpy(date,date+1);
	}
	else
		strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
	int timeWidth=drawTextr(date,x+w-15,y+4,13,timeTextColorR,timeTextColorG,timeTextColorB);//time

	if(timeRetweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
	{
		tm *t=localtime(&timeRetweetedInSeconds);
		strftime(date,1000,"%I:%M",t);
		if(date[0]=='0')
			strcpy(date,date+1);
	}
	else
		strftime(date,1000,settings::dateFormat.c_str(),&timeRetweeted);
	char str[200];
	if(nRetweet<=1)
		sprintf(str,"retweeted by %s at %s",retweetedBy->username.c_str(),date);
	else
<<<<<<< HEAD
		sprintf(str,"retweeted by %s at %s (x%i)",retweetedBy->username.c_str(),date,nRetweet-1);
=======
		sprintf(str,"retweeted by %s at %s (x%i)",retweetedBy->username.c_str(),date,nRetweet);
>>>>>>> parent of 63e80ae... First stage of prerendering tweets.
	int retweetWidth=w-7-pic->w-5-7-13-3-nameWidth+5;
	drawTextWrappedw(str,((x+7+user()->pic()->w+5)+nameWidth+5),y+5,retweetWidth,12,1,retweetTextColorR,retweetTextColorG,retweetTextColorB,tempSurface);//get width
	if(retweetWidth+x+7+user()->pic()->w+5+nameWidth+5>x+w-15-timeWidth)//on top of time
		height=drawTextWrapped(str,x+7+user()->pic()->w+5,height,w-7-pic->w-5-7-13-3,12,first,retweetTextColorR,retweetTextColorG,retweetTextColorB);
	else
		height=drawTextWrapped(str,x+7+user()->pic()->w+5+nameWidth+5,y+5,w-7-pic->w-5-7-13-3-nameWidth-5,12,first,retweetTextColorR,retweetTextColorG,retweetTextColorB);
	drawEntities

	if(height<pic->h+10+y)
		height=pic->h+10+y;
	if(height<y+48+18+3)
		height=y+48+18+3;
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	int newx;
	if(mx>x && mx<x+w && my>y && my<height)
	{
	//	background=2;
		newx=drawButtons(x,y,w,height,1);
	}
	else
		newx=drawButtons(x,y,w,height,0);
	if(newx<textWidth+(x+7+pic->w+8))
	{
		textWidth=newx-(x+7+pic->w+8)-2;
	}
	if(first)
	{
		if(!background)
			boxColor(screen,x,y,x+w-4,height,colors::readTweetColor);
		else if(background==1)
			boxColor(screen,x,y,x+w-4,height,colors::unreadTweetColor);
		else if(background==2)
			boxColor(screen,x,y,x+w-4,height,colors::hoverTweetColor);
		first=0;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	pic=user()->mediumPic();
	drawSprite(pic,screen,0,0,x+5,y+5,pic->w,pic->h);
	pic=retweetedBy->smallPic();
	drawSprite(pic,screen,0,0,x+5+48-pic->w,y+5+48-pic->h,pic->w,pic->h);
	return height;
}

int favoriteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->favoriteCreate(tweet->id))=="");
	debug("fav: %s\n",tmpString.c_str());
	tweet->favorited=1;
	return 0;
}

int unfavoriteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->favoriteDestroy(tweet->id))=="");
	debug("unfav: %s\n",tmpString.c_str());
	tweet->favorited=0;
	return 0;
}
int retweetTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->retweetCreate(tweet->id))=="");
	debug("retweet: %s\n",tmpString.c_str());
	if(tweet->_type==1)
	{
		Retweet *retweet=(Retweet*)tweet;
		retweet->original()->retweeted=1;
	}
	tweet->retweeted=1;
	return 0;
}

int deleteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->statusDestroyById(tweet->id))=="");
	debug("delete: %s\n",tmpString.c_str());
	return 0;
}

int Tweet::drawButtons( int _x,int _y,int w,int h,bool highlighted,SDL_Surface* _screen )
{
	if(_screen==NULL)
		_screen=screen;
	int x=_x+w-15-16;
	int y=_y+18;
	{
		if(favorited)
		{
			drawSprite(favorite[2],_screen,0,0,x,y,16,16);
			if(doButton(x,y,16,16))
			{
				SDL_CreateThread(unfavoriteTweet,this);
			}
		}
		else if(highlighted)
		{
			drawSprite(favorite[hoverButton(x,y,16,16)],_screen,0,0,x,y,16,16);
			if(doButton(x,y,16,16))
			{
				SDL_CreateThread(favoriteTweet,this);
			}
		}
		y+=16;
	}
	if(y+19>h)	{y=_y+18;x-=16;	}
	{
		bool tweetByMe=0;
		if(_type==1)
		{
			Retweet *retweet=(Retweet*)this;
			if(retweet->retweetedBy->username==username)
				tweetByMe=1;
		}
		if(username==user()->username || tweetByMe)
		{
			drawSprite(deleteButton[hoverButton(x,y,16,16)],_screen,0,0,x,y,16,16);
			if(doButton(x,y,16,16))
			{
				SDL_CreateThread(deleteTweet,this);
			}
		}
		else
		{
			if(retweeted)
			{
				drawSprite(retweet[2],_screen,0,0,x,y,16,16);
			}
			else if(highlighted)
			{
				drawSprite(retweet[hoverButton(x,y,16,16)],_screen,0,0,x,y,16,16);
				if(doButton(x,y,16,16))
				{
					SDL_CreateThread(retweetTweet,this);
				}
			}
		}
	}
	y+=16;
	if(y+19>h)	{y=_y+18;x-=16;	}
	if(highlighted)
	{
		drawSprite(reply[hoverButton(x,y,16,16)],_screen,0,0,x,y,16,16);
		if(doButton(x,y,16,16) || doButton(_x+5,_y+5,48,48))
		{
			replyId=id;
			Textbox *textbox=tweetbox;
			textbox->str=string("@")+user()->username+" ";
			keyboardInputReceiver=textbox;
			textbox->cursorPos=textbox->str.size();
		}
	}
	y+=16;
	y+=16;
	return x;
}

void Tweet::write( FILE *fp )
{
	wuchar(_type,fp);
	wstr(id,fp);
	fwrite(&timeTweeted,sizeof(int),9,fp);
	wstr(text,fp);
	wstr(userid,fp);
	wuchar(favorited,fp);
	wuchar(retweeted,fp);
	wuchar(read,fp);
	wuchar(entities.size(),fp);
	for(int i=0;i<entities.size();i++)
		entities[i]->write(fp);
}

void User::save()
{
	FILE *fp=fopenf(string("users/")+id,"wb");
	wstr(username,fp);
	wstr(name,fp);
	wstr(picURL,fp);
	fclose(fp);
}

void Retweet::write( FILE *fp )
{
	Tweet::write(fp);
	wstr(retweetedBy->id,fp);
	wstr(originalID,fp);
	wuint(nRetweet,fp);
	fwrite(&timeRetweeted,sizeof(int),9,fp);
}
void UnquoteHTML
	(
	std::istream & In,
	std::ostream & Out
	);
Tweet* processTweet(Json::Value jtweet)
{
	debugHere();
	Tweet *tweet;
	Json::Value original;
	if(!jtweet["retweeted_status"].isNull())
	{
		original=jtweet;
		jtweet=jtweet["retweeted_status"];
		tweet=new Retweet();
	}
	else
		tweet=new Tweet();
	Json::Value juser=jtweet["user"];
	tweet->userid=juser["id_str"].asString();
	string tmp=jtweet["text"].asString();
	istringstream is(tmp);
	ostringstream os;
	UnquoteHTML(is,os);debugHere();
	tweet->_user=getUser(juser);debugHere();
	tweet->text=os.str();
	tweet->originalText=os.str();
	map<int,Entity*> entities;
	if(!jtweet["entities"].isNull())
	{
		if(!jtweet["entities"]["urls"].isNull())
		{
			for(int i=jtweet["entities"]["urls"].size()-1;i>=0;i--)//go backwards to handle multiple entities in a single tweet correctly
			{
				URLEntity *url=new URLEntity;
				Json::Value entity=jtweet["entities"]["urls"][i];
				url->displayUrl=entity["display_url"].asString();
				if(!entity["expanded_url"].isNull())
					url->realUrl=entity["expanded_url"].asString();
				else
					url->realUrl=entity["url"].asString();
				url->start=entity["indices"][0u].asInt();
				url->end=entity["indices"][1u].asInt();
				url->text=url->displayUrl;
				tweet->entities.push_back(url);
				entities[url->start]=url;
			}
		}
		if(!jtweet["entities"]["user_mentions"].isNull())
		{
			for(int i=jtweet["entities"]["user_mentions"].size()-1;i>=0;i--)//go backwards to handle multiple entities in a single tweet correctly
			{
				UserEntity *entity=new UserEntity;
				Json::Value jentity=jtweet["entities"]["user_mentions"][i];
				entity->name="@"+jentity["name"].asString();
				entity->username="@"+jentity["screen_name"].asString();
				entity->id=jentity["id_str"].asString();
				entity->start=jentity["indices"][0u].asInt();
				entity->end=jentity["indices"][1u].asInt();
				entity->text=entity->username;
				tweet->entities.push_back(entity);
				entities[entity->start]=entity;
			}
		}
		if(!jtweet["entities"]["hashtags"].isNull())
		{
			for(int i=jtweet["entities"]["hashtags"].size()-1;i>=0;i--)//go backwards to handle multiple entities in a single tweet correctly
			{
				HashtagEntity *entity=new HashtagEntity;
				Json::Value jentity=jtweet["entities"]["hashtags"][i];
				entity->name="#"+jentity["text"].asString();
				entity->start=jentity["indices"][0u].asInt();
				entity->end=jentity["indices"][1u].asInt();
				entity->text=entity->name;
				tweet->entities.push_back(entity);
				entities[entity->start]=entity;
			}
		}
	}
	for(map<int,Entity*>::reverse_iterator rit=entities.rbegin();rit!=entities.rend();rit++)//replacing shortened urls with longer ones invalidates the positions of all the other links after it.
	{
		Entity *entity=rit->second;
		string tmp=tweet->text;
		tweet->text.replace(entity->start,entity->end-entity->start,entity->text);
		for(int j=0;j<tweet->entities.size();j++)//fix link indices because text length changed
		{
			if(tweet->entities[j]->start>entity->end)
			{
				tweet->entities[j]->start+=tweet->text.size()-tmp.size();
				tweet->entities[j]->end+=tweet->text.size()-tmp.size();
			}
		}
		entity->end=entity->start+entity->text.size();
	}
	tweet->favorited=jtweet["favorited"].asBool();
	tweet->retweeted=jtweet.get("retweeted",0).asBool();debugHere();
	if(original.isNull())
	{
		debugHere();
		struct tm otm=convertTimeStringToTM(jtweet["created_at"].asString());
		time_t ott=mtimegm(&otm);
		tweet->timeTweeted=*localtime(&ott);
		tweet->id=jtweet["id_str"].asString();
	}
	else//is a rt
	{
		debugHere();
		Retweet *retweet=(Retweet*)tweet;
		if(settings::multipleRetweet)
		{
			retweet->originalID=jtweet["id_str"].asString();

			{
				struct tm otm=convertTimeStringToTM(original["created_at"].asString());
				time_t ott=mtimegm(&otm);
				retweet->timeTweeted=*localtime(&ott);
			}
			{
				struct tm otm=convertTimeStringToTM(jtweet["created_at"].asString());
				time_t ott=mtimegm(&otm);
				retweet->timeRetweeted=*localtime(&ott);
			}
			debugHere();
			retweet->retweetedBy=getUser(original["user"]["id_str"].asString());debugHere();
			retweet->id=original["id_str"].asString();
		}
		else
			retweet->id=jtweet["id_str"].asString();
		retweet->nRetweet=jtweet["retweet_count"].asInt();
		retweet->timeRetweetedInSeconds=mktime(&retweet->timeRetweeted);debugHere();
	}
	tweet->timeTweetedInSeconds=mktime(&tweet->timeTweeted);debugHere();
	if(tweet->user()->username==username)
		tweet->read=1;
	addTweet(tweet);debugHere();
	return tweet;
}
typedef struct
  {
    const char * Name;
    unsigned int Value;
  } EntityNameEntry;
static const EntityNameEntry StaticEntityNames[] =
  /* list of entity names defined in HTML 4.0 spec */
  {
    {"nbsp", 160},
    {"iexcl", 161},
    {"cent", 162},
    {"pound", 163},
    {"curren", 164},
    {"yen", 165},
    {"brvbar", 166},
    {"sect", 167},
    {"uml", 168},
    {"copy", 169},
    {"ordf", 170},
    {"laquo", 171},
    {"not", 172},
    {"shy", 173},
    {"reg", 174},
    {"macr", 175},
    {"deg", 176},
    {"plusmn", 177},
    {"sup2", 178},
    {"sup3", 179},
    {"acute", 180},
    {"micro", 181},
    {"para", 182},
    {"middot", 183},
    {"cedil", 184},
    {"sup1", 185},
    {"ordm", 186},
    {"raquo", 187},
    {"frac14", 188},
    {"frac12", 189},
    {"frac34", 190},
    {"iquest", 191},
    {"Agrave", 192},
    {"Aacute", 193},
    {"Acirc", 194},
    {"Atilde", 195},
    {"Auml", 196},
    {"Aring", 197},
    {"AElig", 198},
    {"Ccedil", 199},
    {"Egrave", 200},
    {"Eacute", 201},
    {"Ecirc", 202},
    {"Euml", 203},
    {"Igrave", 204},
    {"Iacute", 205},
    {"Icirc", 206},
    {"Iuml", 207},
    {"ETH", 208},
    {"Ntilde", 209},
    {"Ograve", 210},
    {"Oacute", 211},
    {"Ocirc", 212},
    {"Otilde", 213},
    {"Ouml", 214},
    {"times", 215},
    {"Oslash", 216},
    {"Ugrave", 217},
    {"Uacute", 218},
    {"Ucirc", 219},
    {"Uuml", 220},
    {"Yacute", 221},
    {"THORN", 222},
    {"szlig", 223},
    {"agrave", 224},
    {"aacute", 225},
    {"acirc", 226},
    {"atilde", 227},
    {"auml", 228},
    {"aring", 229},
    {"aelig", 230},
    {"ccedil", 231},
    {"egrave", 232},
    {"eacute", 233},
    {"ecirc", 234},
    {"euml", 235},
    {"igrave", 236},
    {"iacute", 237},
    {"icirc", 238},
    {"iuml", 239},
    {"eth", 240},
    {"ntilde", 241},
    {"ograve", 242},
    {"oacute", 243},
    {"ocirc", 244},
    {"otilde", 245},
    {"ouml", 246},
    {"divide", 247},
    {"oslash", 248},
    {"ugrave", 249},
    {"uacute", 250},
    {"ucirc", 251},
    {"uuml", 252},
    {"yacute", 253},
    {"thorn", 254},
    {"yuml", 255},
    {"fnof", 402},
  /* Greek */
    {"Alpha", 913},
    {"Beta", 914},
    {"Gamma", 915},
    {"Delta", 916},
    {"Epsilon", 917},
    {"Zeta", 918},
    {"Eta", 919},
    {"Theta", 920},
    {"Iota", 921},
    {"Kappa", 922},
    {"Lambda", 923},
    {"Mu", 924},
    {"Nu", 925},
    {"Xi", 926},
    {"Omicron", 927},
    {"Pi", 928},
    {"Rho", 929},
    {"Sigma", 931},
    {"Tau", 932},
    {"Upsilon", 933},
    {"Phi", 934},
    {"Chi", 935},
    {"Psi", 936},
    {"Omega", 937},
    {"alpha", 945},
    {"beta", 946},
    {"gamma", 947},
    {"delta", 948},
    {"epsilon", 949},
    {"zeta", 950},
    {"eta", 951},
    {"theta", 952},
    {"iota", 953},
    {"kappa", 954},
    {"lambda", 955},
    {"mu", 956},
    {"nu", 957},
    {"xi", 958},
    {"omicron", 959},
    {"pi", 960},
    {"rho", 961},
    {"sigmaf", 962},
    {"sigma", 963},
    {"tau", 964},
    {"upsilon", 965},
    {"phi", 966},
    {"chi", 967},
    {"psi", 968},
    {"omega", 969},
    {"thetasym", 977},
    {"upsih", 978},
    {"piv", 982},
  /* General Punctuation */
    {"bull", 8226},
    {"hellip", 8230},
    {"prime", 8242},
    {"Prime", 8243},
    {"oline", 8254},
    {"frasl", 8260},
  /* Letterlike Symbols */
    {"weierp", 8472},
    {"image", 8465},
    {"real", 8476},
    {"trade", 8482},
    {"alefsym", 8501},
  /* Arrows */
    {"larr", 8592},
    {"uarr", 8593},
    {"rarr", 8594},
    {"darr", 8595},
    {"harr", 8596},
    {"crarr", 8629},
    {"lArr", 8656},
    {"uArr", 8657},
    {"rArr", 8658},
    {"dArr", 8659},
    {"hArr", 8660},
  /* Mathematical Operators */
    {"forall", 8704},
    {"part", 8706},
    {"exist", 8707},
    {"empty", 8709},
    {"nabla", 8711},
    {"isin", 8712},
    {"notin", 8713},
    {"ni", 8715},
    {"prod", 8719},
    {"sum", 8721},
    {"minus", 8722},
    {"lowast", 8727},
    {"radic", 8730},
    {"prop", 8733},
    {"infin", 8734},
    {"and", 8743},
    {"or", 8744},
    {"cap", 8745},
    {"cup", 8746},
    {"int", 8747},
    {"there4", 8756},
    {"sim", 8764},
    {"cong", 8773},
    {"asymp", 8776},
    {"ne", 8800},
    {"equiv", 8801},
    {"le", 8804},
    {"ge", 8805},
    {"sub", 8834},
    {"sup", 8835},
    {"nsub", 8836},
    {"sube", 8838},
    {"supe", 8839},
    {"oplus", 8853},
    {"otimes", 8855},
    {"perp", 8869},
    {"sdot", 8901},
  /* Miscellaneous Technical */
    {"lceil", 8968},
    {"rceil", 8969},
    {"lfloor", 8970},
    {"rfloor", 8971},
    {"lang", 9001},
    {"rang", 9002},
  /* Geometric Shapes */
    {"loz", 9674},
  /* Miscellaneous Symbols */
    {"spades", 9824},
    {"clubs", 9827},
    {"hearts", 9829},
    {"diams", 9830},
    {"quot", 34},
    {"amp", 38},
    {"lt", 60},
    {"gt", 62},
  /* Latin Extended-A */
    {"OElig", 338},
    {"oelig", 339},
    {"Scaron", 352},
    {"scaron", 353},
    {"Yuml", 376},
  /* Spacing Modifier Letters */
    {"circ", 710},
    {"tilde", 732},
  /* General Punctuation */
    {"ensp", 8194},
    {"emsp", 8195},
    {"thinsp", 8201},
    {"zwnj", 8204},
    {"zwj", 8205},
    {"lrm", 8206},
    {"rlm", 8207},
    {"ndash", 8211},
    {"mdash", 8212},
    {"lsquo", 8216},
    {"rsquo", 8217},
    {"sbquo", 8218},
    {"ldquo", 8220},
    {"rdquo", 8221},
    {"bdquo", 8222},
    {"dagger", 8224},
    {"Dagger", 8225},
    {"permil", 8240},
    {"lsaquo", 8249},
    {"rsaquo", 8250},
    {"euro", 8364},
    {NULL, 0} /* marks end of list */
  } /*StaticEntityNames*/;

typedef std::map<std::string, unsigned int> EntityNameMap;
typedef std::pair<std::string, unsigned int> EntityNamePair;
static EntityNameMap
    EntityNames;

static void WriteUTF8
  (
    std::ostream & Out,
    unsigned int Ch
  )
  /* writes Ch in UTF-8 encoding to Out. Note this version only deals
    with characters up to 16 bits. */
  {
    if (Ch >= 0x800)
      {
        Out.put(0xE0 | Ch >> 12 & 0x0F);
        Out.put(0x80 | Ch >> 6 & 0x3F);
        Out.put(0x80 | Ch & 0x3F);
      }
    else if (Ch >= 0x80)
      {
        Out.put(0xC0 | Ch >> 6 & 0x1F);
        Out.put(0x80 | Ch & 0x3F);
      }
    else
      {
        Out.put(Ch);
      } /*if*/
  } /*WriteUTF8*/

void UnquoteHTML
  (
    std::istream & In,
    std::ostream & Out
  )
  /* copies In to Out, expanding any HTML entity references into literal
    UTF-8 characters. */
  {
    enum
      {
        NoMatch,
        MatchBegin,
        MatchName,
        MatchNumber,
        MatchDecimalNumber,
        MatchHexNumber,
      } MatchState;
    std::string MatchingName;
    unsigned int CharCode;
    bool ProcessedChar, GotCharCode;
    MatchState = NoMatch;
    for (;;)
      {
        const unsigned char ThisCh = In.get();
        if (In.eof())
            break;
        ProcessedChar = false; /* to begin with */
        GotCharCode = false; /* to begin with */
        switch (MatchState)
          {
        case MatchBegin:
            if (ThisCh == '#')
              {
                MatchState = MatchNumber;
                ProcessedChar = true;
              }
            else if
              (
                  (  ThisCh >= 'a' && ThisCh <= 'z')
                ||
                    (ThisCh >= 'A' && ThisCh <= 'Z')
              )
              {
                MatchingName.append(1, ThisCh);
                MatchState = MatchName;
                ProcessedChar = true;
              }
            else
              {
                Out.put('&');
                MatchState = NoMatch;
              } /*if*/
        break;
        case MatchName:
            if
              (
                    (ThisCh >= 'a' && ThisCh <= 'z')
                ||
                    (ThisCh >= 'A' && ThisCh <= 'Z')
                ||
                    (ThisCh >= '0' && ThisCh <= '9')
              )
              {
                MatchingName.append(1, ThisCh);
                ProcessedChar = true;
              }
            else if (ThisCh == ';')
              {
                if (EntityNames.empty())
                  {
                  /* first use, load EntityNames from StaticEntityNames */
                    const EntityNameEntry * ThisEntry;
                    ThisEntry = StaticEntityNames;
                    for (;;)
                      {
                        if (ThisEntry->Name == NULL)
                            break;
                        EntityNames.insert
                          (
                            EntityNamePair(std::string(ThisEntry->Name), ThisEntry->Value)
                          );
                        ++ThisEntry;
                      } /*for*/
                  } /*if*/
                if (EntityNames.find(MatchingName)!= EntityNames.end())
                  {
                    CharCode = EntityNames.find(MatchingName)->second;
                    ProcessedChar = true;
                    GotCharCode = true;
                  } /*if*/
              } /*if*/
            if (! ProcessedChar)
              {
                Out.put('&');
                for (unsigned int i = 0; i < MatchingName.size(); ++i)
                  {
                    Out.put(MatchingName[i]);
                  } /*for*/
                MatchState = NoMatch;
              } /*if*/
        break;
        case MatchNumber:
            if (ThisCh == 'x' || ThisCh == 'X')
              {
                ProcessedChar = true;
                MatchState = MatchHexNumber;
                CharCode = 0;
              }
            else if (ThisCh >= '0' && ThisCh <= '9')
              {
                CharCode = ThisCh - '0';
                MatchState = MatchDecimalNumber;
                ProcessedChar = true;
              }
            else
              {
                MatchState = NoMatch;
              } /*if*/
        break;
        case MatchDecimalNumber:
            if (ThisCh >= '0' && ThisCh <= '9')
              {
                CharCode = CharCode * 10 + ThisCh - '0';
                ProcessedChar = true;
              }
            else if (ThisCh == ';')
              {
                ProcessedChar = true;
                GotCharCode = true;
              }
            else
              {
                MatchState = NoMatch;
              } /*if*/
        break;
        case MatchHexNumber:
            if (ThisCh >= '0' && ThisCh <= '9')
              {
                CharCode = CharCode * 16 + ThisCh - '0';
                ProcessedChar = true;
              }
            else if (ThisCh >= 'a' && ThisCh <= 'f')
              {
                CharCode = CharCode * 16 + ThisCh - 'a' + 10;
                ProcessedChar = true;
              }
            else if (ThisCh >= 'A' && ThisCh <= 'F')
              {
                CharCode = CharCode * 16 + ThisCh - 'A' + 10;
                ProcessedChar = true;
              }
            else if (ThisCh == ';')
              {
                ProcessedChar = true;
                GotCharCode = true;
              }
            else
              {
                MatchState = NoMatch;
              } /*if*/
        break;
          } /*switch*/
        if (GotCharCode)
          {
            WriteUTF8(Out, CharCode);   
            MatchState = NoMatch;
          }
        else if (! ProcessedChar && MatchState == NoMatch)
          {
            if (ThisCh == '&')
              {
                MatchState = MatchBegin;
                MatchingName.erase();
              }
            else
              {
                Out.put(ThisCh);
              } /*if*/
          } /*if*/
      } /*for*/
  } /*UnquoteHTML*/