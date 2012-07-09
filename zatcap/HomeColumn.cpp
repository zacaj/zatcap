#include "HomeColumn.h"
#include "twitter.h"

HomeColumn::HomeColumn(float w ):Column(w,"timeline")
{
	emptyColumnText="No one wants to talk to you";
	emptyColumnText="Now with unanonymous data collection!";
	updateScreen=1;
}


HomeColumn::~HomeColumn(void)
{
}


void HomeColumn::newTweet( Tweet *tweet )
{
	if(tweet->_type<0)
		return;
	SDL_LockMutex(drawingMutex);
	m_tweets[tweet->id]=new TweetInstance(tweet,rw,onOff);
	if(scroll!=0)
	{
		scroll+=m_tweets[tweet->id]->surface->h;
	}
	updateScreen=1;
	//printf("o %i\n",tweetHeight);
	tweetHeight=-1;
	SDL_UnlockMutex(drawingMutex);
}

int homeColumnRefresh(void *data)
{
	HomeColumn *col=(HomeColumn*)data;
	string tmpString;
	if(!col->m_tweets.empty())
	{
		auto rit=col->m_tweets.rbegin();
		rit++;
		while((tmpString=twit->timelineHomeGet(false,true,800,rit->first,""))=="");//settings::tweetsToLoadOnStartup+50
	}
	else
	{
		while((tmpString=twit->timelineHomeGet(false,true,20,"",""))=="");
	}
	parseRestTweets(tmpString);
	return 0;
}

void HomeColumn::draw()
{
	Column::draw();
	if(drawRefreshButton())
		SDL_CreateThread(homeColumnRefresh,this);
}
