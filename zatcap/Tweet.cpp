#include "Tweet.h"
#include "zatcap.h"
#include "Awesomium.h"

int Tweet::draw( int x,int y,int w,int background)
{
	return 0;
}



int Retweet::draw( int x,int y,int w,int background )
{
	
	return 0;
}

void favoriteTweet( void *data )
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	int nTries = 0;
	while((tmpString=twit->favoriteCreate(tweet->id))=="") if(nTries++>20) return;
	debug("fav: %s\n",tmpString.c_str());
}

void unfavoriteTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	int nTries = 0;
	while ((tmpString = twit->favoriteDestroy(tweet->id)) == "") if (nTries++>20) return;
	debug("unfav: %s\n",tmpString.c_str());
}
void retweetTweet(void *data)
{
	Tweet *tweet=(Tweet*)data;
	string tmpString;
	int nTries = 0;
	while ((tmpString = twit->retweetCreate(tweet->id)) == "") if (nTries++>20) return;
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
	runJS("if(document.getElementById('tweetbox').value.length==0) document.getElementById('tweetbox').value='"+escape(tweet->originalText)+"';");
	string tmpString;
	int nTries = 0;
	while ((tmpString = twit->statusDestroyById(tweet->id)) == "") if (nTries++>20) return;
	debug("delete: %s\n",tmpString.c_str());
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
		replace(content,"$USERID",user()->id);
		replace(content,string("$REPLYID"),replyTo);
		string htmlText=text;
		replace(htmlText,"\n","<br />");
		replace(content,string("$TEXT"),escape(htmlText));
		{
			char date[10000];
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
		if(user()->username==username)
			replace(content,string("$HIDEMINE"),"none");
		else
			replace(content,string("$HIDEMINE"),"inline");
		if(replyTo.size())
			replace(content,string("$HIDENOTREPLY"),"inline");
		else
			replace(content,string("$HIDENOTREPLY"),"none");
		if(read)
			replace(content,"$READ","read");
		else
			replace(content,"$READ","unread");
		if(favorited)
			replace(content,"$FAVORITED","_on");
		else
			replace(content,"$FAVORITED","");
		replace(content,"$TYPEPREFIX",isDM?"d ":"@");
		replace(content,"$FOLLOWS",followers.find(user()->id)!=followers.end()?"following":"");
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
		replace(content,string("$ORIGINALID"),originalID);;
		replace(content,string("$REPLYID"),replyTo);
		string htmlText=text;
		{
			char date[10000];
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
		replace(content,string("$RTUSERNAME"),escape(retweetedBy->username));
		replace(content,string("$NAME"),escape(user()->username));
		replace(content,string("$RETWEETER"),escape(retweetedBy->username));
		replace(content,string("$FULLNAME"),escape(user()->name));
		replace(content,string("$AVATAR"),user()->picURL);
		replace(content,string("$RTAVATAR"),retweetedBy->picURL);
		replace(content,string("$COLUMN"),columnName);
		if(retweetedBy->username==username || user()->username==username)
			replace(content,string("$HIDENOTMINE"),"inline");
		else
			replace(content,string("$HIDENOTMINE"),"none");
		if(user()->username==username || retweetedBy->username==username)
			replace(content,string("$HIDEMINE"),"none");
		else
			replace(content,string("$HIDEMINE"),"inline");
		if(read)
			replace(content,"$READ","read");
		else
			replace(content,"$READ","unread");

		if(favorited)
			replace(content,"$FAVORITED","_on");
		else
			replace(content,"$FAVORITED","");
		replace(content,"$TYPEPREFIX",isDM?"d ":"@");
		replace(content,"$USERID",user()->id);
		replace(content,"$RTUSERID",retweetedBy->id);
		replace(content,"$FOLLOWS",followers.find(user()->id)!=followers.end()?"following":"");
		replace(content,"$RTFOLLOWS",followers.find(retweetedBy->id)!=followers.end()?"following":"");
		if(replyTo.size())
			replace(content,string("$HIDENOTREPLY"),"inline");
		else
			replace(content,string("$HIDENOTREPLY"),"none");
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


std::string Favorite::getHtml( string columnName )
{
	//if(html.empty())
	{
		string content=f2s("resources/favorite.html");
		replace(content,string("$ID"),id);
		string htmlText=text;
		replace(content,string("$TEXT"),escape(htmlText));
		{
			char date[10000];
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
			replace(content,string("$TIME"),escape(date));
		}
		replace(content,string("$USERNAME"),escape(follower->username));
		replace(content,string("$FULLNAME"),escape(follower->name));
		replace(content,string("$AVATAR"),follower->picURL);
		replace(content,string("$COLUMN"),columnName);
		html=content;
	}
	return html;
}

std::string Activity::getHtml( string columnName )
{
	{
		string content=f2s("resources/activity.html");
		replace(content,string("$ID"),id);
		string htmlText=text;
		replace(content,string("$TEXT"),escape(htmlText));
		{
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
			replace(content,string("$TIME"),escape(date));
		}
		replace(content,string("$USERNAME"),escape(username));
		replace(content,string("$FULLNAME"),escape(username));
		replace(content,string("$AVATAR"),"asset://resource/activity.gif");
		replace(content,string("$COLUMN"),columnName);
		replace(content,string("$STOPJS"),escape(stoppable));
		if(stoppable.empty())
			replace(content,string("$HIDENOTMINE"),"none");
		else
			replace(content,string("$HIDENOTMINE"),"inline");
		html=content;
	}
	return html;
}

void Activity::write( FILE *fp )
{

}


void Follow::write( FILE *fp )
{
	//throw std::exception("The method or operation is not implemented.");
}

bool Item::operator!=(const Item &i ) const
{
	return text!=i.text || id!=i.id || _type!=i._type || timeTweetedInSeconds!=i.timeTweetedInSeconds || read!=i.read;
}