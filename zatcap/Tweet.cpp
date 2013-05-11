#include "Tweet.h"
#include "zatcap.h"
#include "Awesomium.h"
using namespace colors;


string getBackgroundColor(int background,int read)
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

int Tweet::draw( int x,int y,int w,int background)
{
	return 0;
}


int Tweet::draw( TweetInstance *instance,int w )
{
	
	return 0;
}


int Retweet::draw( int x,int y,int w,int background )
{
	
	return 0;
}

int Retweet::draw( TweetInstance *instance,int w )
{
	
	return 0;
}
void favoriteTweet( void *data )
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->favoriteCreate(tweet->id))=="");
	debug("fav: %s\n",tmpString.c_str());
}

void unfavoriteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->favoriteDestroy(tweet->id))=="");
	debug("unfav: %s\n",tmpString.c_str());
}
void retweetTweet(void *data)
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
}

void deleteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	while((tmpString=twit->statusDestroyById(tweet->id))=="");
	debug("delete: %s\n",tmpString.c_str());
}

int Tweet::drawButtons( int _x,int _y,int w,int h,bool highlighted )
{
	return 0;
}

void Tweet::write( FILE *fp )
{
	wuchar(_type,fp);
	wstr(id,fp);
	wuint(timeTweetedInSeconds,fp);
	//fwrite(&timeTweeted,sizeof(int),9,fp);
	fputc('\n',fp);
	wstr(text,fp);
	wstr(userid,fp);
	wuchar(favorited,fp);
	wuchar(retweeted,fp);
	wuchar(read,fp);
	wuchar(entities.size(),fp);
	for(int i=0;i<entities.size();i++)
		entities[i]->write(fp);
}

int Tweet::cachedDraw( TweetInstance *instance )
{
///	if(user()->pic()!=instance->pic)
	///	instance->refresh(instance->w);
	return 0;
}

std::string Tweet::getHtml(string columnName)
{
	//if(html.empty())
	{
		string content=f2s("resources/tweet.html");
		replace(content,string("$ID"),id);
		string htmlText=text;
		replace(content,string("$TEXT"),escape(htmlText));
		{
			char date[100];
			if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
			{
				tm *t=localtime(&timeTweetedInSeconds);
				strftime(date,1000,"%I:%M",t);
				if(date[0]=='0')
					strcpy(date,date+1);
			}
			else
				strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
			replace(content,string("$TIME"),escape(date));
		}
		replace(content,string("$USERNAME"),escape(user()->username));
		replace(content,string("$NAME"),escape(user()->username));
		replace(content,string("$FULLNAME"),escape(user()->name));
		replace(content,string("$AVATAR"),user()->picURL);
		replace(content,string("$COLUMN"),columnName);
		if(user()->username==username)
			replace(content,string("$HIDENOTMINE"),"inline");
		else
			replace(content,string("$HIDENOTMINE"),"none");
		if(read)
			replace(content,"$READ","read");
		else
			replace(content,"$READ","unread");
		if(favorited)
			replace(content,"$FAVORITED","_on");
		else
			replace(content,"$FAVORITED","");
		html=content;
	}
	return html;
}


std::string Retweet::getHtml( string columnName )
{
	//if(html.empty())
	{
		string content=f2s("resources/retweet.html");
		replace(content,string("$ID"),id);
		replace(content,string("$ORIGINALID"),originalID);
		string htmlText=text;
		{
			char date[100];
			if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
			{
				tm *t=localtime(&timeTweetedInSeconds);
				strftime(date,1000,"%I:%M",t);
				if(date[0]=='0')
					strcpy(date,date+1);
			}
			else
				strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
			replace(content,string("$TIME"),escape(date));
		}
		replace(content,string("$TEXT"),escape(htmlText));
		replace(content,string("$USERNAME"),escape(user()->username));
		replace(content,string("$NAME"),escape(user()->username));
		replace(content,string("$RETWEETER"),escape(retweetedBy->username));
		replace(content,string("$FULLNAME"),escape(user()->name));
		replace(content,string("$AVATAR"),user()->picURL);
		replace(content,string("$RTAVATAR"),retweetedBy->picURL);
		replace(content,string("$COLUMN"),columnName);
		if(retweetedBy->username==username)
			replace(content,string("$HIDENOTMINE"),"inline");
		else
			replace(content,string("$HIDENOTMINE"),"none");
		if(read)
			replace(content,"$READ","read");
		else
			replace(content,"$READ","unread");

		if(favorited)
			replace(content,"$FAVORITED","_on");
		else
			replace(content,"$FAVORITED","");
		html=content;
	}
	return html;
}
int Retweet::cachedDraw( TweetInstance *instance )
{
	return 0;
}

void Retweet::write( FILE *fp )
{
	Tweet::write(fp);
	wstr(retweetedBy->id,fp);
	wstr(originalID,fp);
	wuint(nRetweet,fp);
	wuint(timeRetweetedInSeconds,fp);
	//	fwrite(&timeRetweeted,sizeof(int),9,fp);
	fputc('\n',fp);
}

TweetInstance::TweetInstance( Item *_tweet,int _w,int _background  )	:
	tweet(_tweet),
	background(_background)
{
	w=_w;
	read=tweet->read;
	//surface=NULL;
	replyTo=NULL;
	drawReply=0;
	//refresh(w);
}

int TweetInstance::draw( int x,int y )
{
	/*if(surface==NULL || read!=tweet->read)
		refresh(w);
	int h=0;
	drawSprite(surface,screen,0,0,x,y,surface->w,surface->h);
	{{drawEntities2}}
	tweet->cachedDraw(this);
	h+=surface->h;
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	int newx;
	if(mx>x && mx<x+surface->w && my>y && my<surface->h+y)
	{
		//	background=2;
		newx=tweet->drawButtons(x,y,surface->w,surface->h,1);
		if(newx<0)
		{
			switch(newx)
			{
			case TOGGLECONVODISPLAY:
				drawReply=!drawReply;
				break;
			}
		}
	}
	if(hoverButton(x+5,y+5,48,48))
		drawSprite(reply[2],screen,0,0,x+5,y+5,reply[2]->w,reply[2]->h);
	if(drawReply)
	{
		if(replyTo==NULL && tweet->replyTo.size())
			replyTo=new TweetInstance(getTweet(tweet->replyTo),surface->w-12,!background);
		if(replyTo!=NULL)
			h+=replyTo->draw(x+12,y+h)+0;
	}
	return h;*/
	return 0;
}

TweetInstance::~TweetInstance()
{
	/*if(surface)
	{
		SDL_FreeSurface(surface);
		surface=NULL;
	}
	if(p)
	{
		delete p;
		p=NULL;
	}*/
}

void TweetInstance::refresh( int w )
{
	//print("redraw");
	//if(surface)
	//	SDL_FreeSurface(surface);
	widths.clear();
	//tweet->draw(this,w);
	if(replyTo!=NULL)
	{
		delete replyTo;
		replyTo=NULL;
	}
}

std::string Favorite::getHtml( string columnName )
{
	//if(html.empty())
	{
		string content=f2s("resources/favorite.html");
		replace(content,string("$ID"),id);
		string htmlText=text;
		replace(content,string("$TEXT"),escape(htmlText));
		{
			char date[100];
			if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
			{
				tm *t=localtime(&timeTweetedInSeconds);
				strftime(date,1000,"%I:%M",t);
				if(date[0]=='0')
					strcpy(date,date+1);
			}
			else
				strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
			replace(content,string("TIME"),escape(date));
		}
		replace(content,string("$USERNAME"),escape(favoriter->username));
		replace(content,string("$FULLNAME"),escape(favoriter->name));
		replace(content,string("$AVATAR"),favoriter->picURL);
		replace(content,string("$COLUMN"),columnName);
		replace(content,string("$ACTION"),action);
		html=content;
	}
	return html;
}

void Favorite::write( FILE *fp )
{
	//throw std::exception("The method or operation is not implemented.");
}

std::string Follow::getHtml( string columnName )
{
	//if(html.empty())
	{
		string content=f2s("resources/follow.html");
		replace(content,string("$ID"),id);
		string htmlText=text;
		replace(content,string("$TEXT"),escape(htmlText));
		{
			char date[100];
			if(timeTweeted.tm_yday==localtime(&currentTime)->tm_yday)//time
			{
				tm *t=localtime(&timeTweetedInSeconds);
				strftime(date,1000,"%I:%M",t);
				if(date[0]=='0')
					strcpy(date,date+1);
			}
			else
				strftime(date,1000,settings::dateFormat.c_str(),&timeTweeted);
			replace(content,string("TIME"),escape(date));
		}
		replace(content,string("$USERNAME"),escape(follower->username));
		replace(content,string("$FULLNAME"),escape(follower->name));
		replace(content,string("$AVATAR"),follower->picURL);
		replace(content,string("$COLUMN"),columnName);
		html=content;
	}
	return html;
}

void Follow::write( FILE *fp )
{
	//throw std::exception("The method or operation is not implemented.");
}
