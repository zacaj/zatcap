#include "twitter.h"
#include "zatcap.h"

#include "twitcurl.h"
#include "stream.h"
#include <assert.h>
#include "json\writer.h"
#include "TimedEventProcess.h"
#include "Tweet.h"
#include <SDL_rotozoom.h>
#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <iosfwd>
#include "Awesomium.h"
AvitarDownloader *aviDownloader;
map<string,Item*> tweets;
map<string,User*> users;
twitCurl *twit=NULL;
bool loggedIn=0;

SDL_Surface *defaultUserPic;
SDL_Surface *defaultSmallUserPic;
SDL_Surface *defaultMediumUserPic;
string replyId="";
void twitterInit( void  *_twit )
{
	twit=new twitCurl();
	*((void**)_twit)=twit;
	twit->setTwitterApiType(twitCurlTypes::eTwitCurlApiFormatJson);
	if(settings::proxyServer!="none")
	{
		twit->setProxyServerIp(settings::proxyServer);
		twit->setProxyServerPort(settings::proxyPort);
	}
	loadOlderTweets(0);
	print("Logging in...\n");
	doing(1);
	loadUser(twit);
	doing(-1);
	loggedIn=1;
	startThread(refreshTweets,0);debugHere();

		openUserStream(twit);
		debugHere();
}

void refreshTweets( void *data )
{
	doing(1);
	{
		string tmpString;debugHere();
		while((tmpString=twit->timelineHomeGet(false,true,25,"",""))=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->mentionsGet("",""))=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->timelineFriendsGet())=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->favoriteGet())=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->retweetsGet())=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->directMessageGet())=="");
		parseRestTweets(tmpString);debugHere();
	}
	{
		string tmpString;debugHere();
		while((tmpString=twit->followersIdsGet(username))=="");
		Json::Reader reader;
		Json::Value root;
		reader.parse(tmpString,root);
		root=root["ids"];
		assert(root.isArray());
		for(int i=0;i<root.size();i++)
		{
			addFollower(root[i].asString());
		}
	}
	doing(-1);
}
void loadBackTill(void *data)
{
	doing(1);
	int hours=*(int*)data;
	int secs=hours*60*60;
	int now=(--tweets.end())->second->timeTweetedInSeconds;
	string oldestLoadedId="";
	while(1)
	{
		string tmpString;debugHere();
		while((tmpString=twit->timelineHomeGet(false,true,800,"",oldestLoadedId))=="");
		oldestLoadedId=parseRestTweets(tmpString);debugHere();
		if(oldestLoadedId.empty())
			break;
		Item *item=getTweet(oldestLoadedId);
		if(now-secs>item->timeTweetedInSeconds)
			break;
	}
	doing(-1);
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
	print("Reading %s\n",path.c_str());debugHere();
	FILE *fp=fopen(path.c_str(),"rb");
	assert_(fp);
	uchar version=ruchar(fp);
	vector<Retweet*> tweetsMissing;
	switch(version)
	{
	case 2:
	case 3:
	case 4:
	case 5:
		{
			int n=ruint(fp);
			for(int i=0;i<n;i++)
			{
                debug("Reading tweet %i/%i\n",i,n);
				uchar type=ruchar(fp);
				switch(type)
				{
				default:
					print("ERROR: unknown tweet type %i\nFailed reading tweet %i/%i, skipping rest of file\n",type,i,n);
					fclose(fp);
					return;
					break;
				case 0://normal
					{
						Tweet *tweet=new Tweet;
						tweet->id=rstr(fp);
						if(version>=5)
						{
							tweet->timeTweetedInSeconds=ruint(fp);
							tweet->timeTweeted=*localtime(&tweet->timeTweetedInSeconds);
						}
						else
						{
							fread(&tweet->timeTweeted,sizeof(int),9,fp);
							if(version==4) fgetc(fp);
							tweet->timeTweetedInSeconds=mktime(&tweet->timeTweeted);
						}
						tweet->text=rstr(fp);
						tweet->userid=rstr(fp);
						tweet->_user=getUser(tweet->userid);
						tweet->favorited=ruchar(fp);
						tweet->retweeted=ruchar(fp);
						tweet->read=ruchar(fp);debugHere();
						//if(!tweet->read) nUnread++;
						if(version>=3)
						{
							int nEntity=ruchar(fp);
							for(int iEntity=0;iEntity<nEntity;iEntity++)
							{
								Entity *entity=readEntity(fp);
								if(entity==NULL)
								{
									print("Failure reading tweet %i/%i, skipping rest of file\n",i,n);
									fclose(fp);
									return;
								}
								tweet->entities.push_back(entity);
							}
						}
						addTweet((Item**)&tweet);debugHere();
					}
					break;
					tweets;
				case 1://retweet
					{
						Retweet *retweet=new Retweet;
						retweet->id=rstr(fp);
						if(version>=5)
						{
							retweet->timeTweetedInSeconds=ruint(fp);
							retweet->timeTweeted=*localtime(&retweet->timeTweetedInSeconds);
						}
						else
						{
							fread(&retweet->timeTweeted,sizeof(int),9,fp);
							if(version==4) fgetc(fp);
							retweet->timeTweetedInSeconds=mktime(&retweet->timeTweeted);
						}
						retweet->text=rstr(fp);
						retweet->userid=rstr(fp);
						retweet->_user=getUser(retweet->userid);
						retweet->favorited=ruchar(fp);
						retweet->retweeted=ruchar(fp);
						retweet->read=ruchar(fp);
						//if(!retweet->read)
						//	nUnread++;
						if(version>=3)
						{
							int nEntity=ruchar(fp);
							for(int iEntity=0;iEntity<nEntity;iEntity++)
							{
								Entity *entity=readEntity(fp);
								if(entity==NULL)
								{
									print("Failure reading tweet %i/%i, skipping rest of file\n",i,n);
									fclose(fp);
									return;
								}
								retweet->entities.push_back(entity);
							}
						}
						retweet->retweetedBy=getUser(rstr(fp));
						retweet->originalID=rstr(fp);
						retweet->nRetweet=ruint(fp);
						retweet->_original=(Retweet*)getTweet(retweet->originalID);
						if(version>=5)
						{
							retweet->timeRetweetedInSeconds=ruint(fp);
							retweet->timeRetweeted=*localtime(&retweet->timeTweetedInSeconds);
						}
						else
						{
							fread(&retweet->timeRetweeted,sizeof(int),9,fp);
							if(version==4) fgetc(fp);
							retweet->timeRetweetedInSeconds=mktime(&retweet->timeTweeted);
						}
						//retweet->timeRetweetedInSeconds=mktime(&retweet->timeRetweeted);debugHere();
						addTweet((Item**)(Tweet**)(&retweet));debugHere();
					}
					break;
				}
			}
			print("Read %i tweets from archive\n",n);
		}
		break;
	default:
		print("ERROR: could not read %s, unknown version %i!\n",path.c_str(),version);
		break;
	}
	//for(int i=0;i<tweetsMissing.size();i++)
	//	tweetsMissing[i]->_original=getTweet(tweetsMissing[i]->originalID);
	fclose(fp);debugHere();
}
string parseRestTweets( string json )
{debugHere();
	Json::Reader reader;
	Json::Value root;
	string ret="";
	FILE *fp=fopen("test.json","wb");
	fwrite(json.c_str(),json.size(),1,fp);
	fclose(fp);
	assert_(reader.parse(json,root));debugHere();
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
			ret=root[i]["id_str"].asString();
		}
	}
	else
	{
		tweets;
		//confusion
	}
	return ret;
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	debugHere();
	debug("stream=%i\n",stream);
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
string getExt(string path)
{
	size_t pos=path.rfind('.');
	if(pos!=path.npos)
		return(path.substr(pos+1,path.size()-pos-1));
	else
		return "";
}

void processUserPics(User *user);

void deleteTweet( string id )
{debugHere();
	enterMutex(tweetsMutex);
	map<string,Item*>::iterator tweet=tweets.find(id);
	if(tweet==tweets.end())
		return;
	if(!tweet->second->read)
		nUnread--;
	for (map<float,Process*>::iterator it=processes.begin();it!=processes.end();it++)
	{
		leaveMutex(tweetsMutex);
		it->second->deleteTweet(tweet->first);
		enterMutex(tweetsMutex);
	}
	//debug delete tweet->second;debugHere();
	tweets.erase(tweet);debugHere();
	leaveMutex(tweetsMutex);
}
Item* getTweet( string id )
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
	print("Saving tweets\n");
	if(tweets.empty())
		return;
	map<string,Item*>::iterator top=--tweets.end();
	string prefix=removeLast(top->first);
	map<string,Item*>::iterator bottom=tweets.upper_bound(prefix);
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
			wuchar(5,fp);//version
			wuint(n,fp);
			for(map<string,Item*>::iterator it=bottom;it!=top;it++)
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
Mutex tweetsMutex;
void saveMute() ;
void addTweet( Item** tweet,bool newTweet )
{debugHere();
enterMutex(tweetsMutex);
debugHere();
	map<string,Item*>::iterator tw=tweets.find((*tweet)->id);debugHere();
	debug("New tweet: %s\n",escape((*tweet)->text,true).c_str());
	if(tw==tweets.end())
	{
		time_t t=time(NULL);
		for(auto it=mute.begin();it!=mute.end();)
		{
			if(it->second<t)
			{
				string str=it->first;
				it++;
				mute.erase(str);
				saveMute();
			}
			else
			{
				Tweet *t=NULL;
				if((*tweet)->_type<2)
					t=(Tweet*)*tweet;
				if((*tweet)->text.find(it->first)!=string::npos || (t!=NULL && it->first.find(t->user()->username)!=string::npos))
				{
					leaveMutex(tweetsMutex);
					return;
				}
				it++;
			}
		}
		if(newTweet && (*tweet)->text.find(username)!=string::npos)
		{
			notifyIcon(1);
		}
		tweets[(*tweet)->id]=*tweet;
		if(!((*tweet)->read))
			nUnread++;
		for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
			it->second->newTweet(*tweet);
	}
	else
	{debugHere();
		//nUnread-=(*tweet)->read-tw->second->read;
		if(**tweet!=*tw->second)
		{
			bool oldRead=tw->second->read;
			leaveMutex(tweetsMutex);
			deleteTweet(((Tweet*)(*tweet))->id);
			//enterMutex(tweetsMutex);

			(*tweet)->read=oldRead;
			/*if(!(*tweet)->read)
				nUnread++;
			tweets[(*tweet)->id]=*tweet;
			for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
				it->second->newTweet(*tweet);*/
			addTweet(tweet,0);
		}
	}
	leaveMutex(tweetsMutex);
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
	user->save();
	users[id]=user;debugHere();
	return user;
}
void fixUser(void *ptr)
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
}

User * getUser( string id )
{debugHere();
//print("%i\n",rand());
	if(users.find(id)!=users.end())
		return users[id];
	if(fileExists(string("users/")+id))
	{debugHere();
	User *user=new User;debugHere();
	debug("%s\n",id.c_str());
		FILE *fp=fopen((string("users/")+id).c_str(),"rb");debugHere();
		assert_(fp);debugHere();
		user->id=id;debugHere();
		user->username=rstr(fp);debugHere();
		user->name=rstr(fp);debugHere();
		user->picURL=rstr(fp);debugHere();
		fclose(fp);debugHere();//loadProfilePic
		return user;
	}debugHere();
	User *user=new User;
	string tmpString;debugHere();
	debug("%s\n",id.c_str());
	print("Loading information for user %s\n",id.c_str());debugHere();
	while((tmpString=twit->userGet(id,true))=="") debugHere();
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


void User::save()
{
	FILE *fp=fopenf(string("users/")+id,"wb");
	wstr(username,fp);
	wstr(name,fp);
	wstr(picURL,fp);
	fclose(fp);
}

std::string User::getHtml()
{
	string content=f2s("resources/user.html");
	string json;
	while((json=twit->userGet(id,true))=="");
	Json::Reader reader;
	Json::Value root;
	reader.parse(json,root);
	replace(content,string("$ID"),id);
	replace(content,string("$TEXT"),escape(root["description"].asString()));
	replace(content,string("$USERNAME"),escape(username));
	replace(content,string("$FULLNAME"),escape(name));
	replace(content,string("$AVATAR"),root["profile_image_url"].asString());
	replace(content,string("$NUMTWEETS"),i2s(root["statuses_count"].asInt()));
	replace(content,string("$NUMFOLLOWING"),i2s(root["friends_count"].asInt()));
	replace(content,string("$NUMFOLLOWERS"),i2s(root["followers_count"].asInt()));
	replace(content,string("$TIMEZONE"),root["time_zone"].asString());
	replace(content,string("$FOLLOW"),root["following"].asBool()?"Unfollow":"Follow");
	return content;
}

void UnquoteHTML
	(
	std::istream & In,
	std::ostream & Out
	);
bool urlIsPicture(string url)
{
	for(int i=0;i<url.size();i++)
		url[i]=tolower(url[i]);
	string ext=getExt(url);
	if(ext=="jpg" || ext=="jpeg" || ext=="png" || ext=="bmp")
		return 1;
	if(url.find(".jpg")!=string::npos)
		return 1;
	if(url.find(".png")!=string::npos)
		return 1;
	if(url.find(".gif")!=string::npos)
		return 1;
	if(url.find("pic.")!=string::npos)
		return 1;
	if(url.find("imgur.com")!=string::npos)
		return 1;
	//return 1;
	return 0;
}
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
	else if(!jtweet["sender"].isNull())
	{
		tweet=new DirectMessage();
	}
	else
		tweet=new Tweet();
	Json::Value juser;
	if(tweet->isDM)
		juser=jtweet["sender"];
	else
		juser=jtweet["user"];
	tweet->userid=juser["id_str"].asString();
	string tmp=jtweet["text"].asString();
	istringstream is(tmp);
	ostringstream os;
	UnquoteHTML(is,os);debugHere();
	tweet->_user=getUser(juser);debugHere();
	tweet->text=tmp;//os.str();//
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
				url->text="<a href='"+url->realUrl+"'>"+url->displayUrl+"</a>";
				if(!urlIsPicture(url->realUrl))
					url->text="<a href='javascript:;' oncontextmenu=\"cpp.handleImage('"+url->realUrl+"');\" onclick=\"cpp.openInNativeBrowser('"+url->realUrl+"');\">"+url->displayUrl+"</a>";
				else
					url->text="<a href='javascript:;' oncontextmenu=\"cpp.openInNativeBrowser('"+url->realUrl+"'); return false;\" onclick=\"cpp.handleImage('"+url->realUrl+"');\">"+url->displayUrl+"</a>";
				tweet->entities.push_back(url);
				entities[url->start]=url;
			}
		}
		if(!jtweet["entities"]["media"].isNull())
		{
			for(int i=jtweet["entities"]["media"].size()-1;i>=0;i--)//go backwards to handle multiple entities in a single tweet correctly
			{
				URLEntity *url=new URLEntity;
				Json::Value entity=jtweet["entities"]["media"][i];
				url->displayUrl=entity["display_url"].asString();
				if(!entity["media_url"].isNull())
					url->realUrl=entity["media_url"].asString();
				else if(!entity["expanded_url"].isNull())
					url->realUrl=entity["expanded_url"].asString();
				else
					url->realUrl=entity["url"].asString();
				url->start=entity["indices"][0u].asInt();
				url->end=entity["indices"][1u].asInt();
				url->text="<a href='"+url->realUrl+"'>"+url->displayUrl+"</a>";
				url->text="<a href='javascript:;' oncontextmenu=\"cpp.openInNativeBrowser('"+url->realUrl+"'); return false;\" onclick=\"cpp.handleImage('"+url->realUrl+"');\">"+url->displayUrl+"</a>";
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
				//entity->text="<a oncontextmenu='mutePop(\""+entity->username+"\"); return false;' href='#' onclick=\"cpp.openInNativeBrowser('https://twitter.com/"+entity->username+"');\" class='"+(followers.find(entity->id)!=followers.end()?"followinguser":"user")+" "+entity->id+"'>"+entity->username+"</a>";
				entity->text="<a oncontextmenu='mutePop(\""+entity->username+"\"); return false;' href='#' onclick=\"cpp.showUser('"+entity->id+"','https://twitter.com/"+entity->username+"');\" class='"+(followers.find(entity->id)!=followers.end()?"followinguser":"user")+" "+entity->id+"'>"+entity->username+"</a>";
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
				entity->text="<a oncontextmenu='mutePop(\""+entity->name+"\"); return false;' href='javascript:;' onclick=\"cpp.openInNativeBrowser('https://twitter.com/search?q=%23"+entity->name.substr(1,entity->name.size()-1)+"');\">"+entity->name+"</a>";
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
		if(jtweet["in_reply_to_status_id_str"].isNull())
			tweet->replyTo="";
		else
			tweet->replyTo=jtweet["in_reply_to_status_id_str"].asString();
	}
	else//is a rt
	{
		debugHere();
		Retweet *retweet=(Retweet*)tweet;
//		if(settings::multipleRetweet)
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
		//else
		//	retweet->id=jtweet["id_str"].asString();
		retweet->nRetweet=jtweet["retweet_count"].asInt();
		retweet->timeRetweetedInSeconds=mktime(&retweet->timeRetweeted);debugHere();
		if(original["in_reply_to_status_id_str"].isNull())
			tweet->replyTo="";
		else
			tweet->replyTo=(original["in_reply_to_status_id_str"].asString());
	}
	tweet->timeTweetedInSeconds=mktime(&tweet->timeTweeted);debugHere();
	if(tweet->user()->username==username)
		tweet->read=1;
	else
	{
		//nUnread++;
	}
	addTweet((Item**)&tweet);debugHere();
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

