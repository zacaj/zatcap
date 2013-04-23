#include "HomeColumn.h"
#include "twitter.h"

HomeColumn::HomeColumn(float w ):Column(w,"timeline")
{
	int r=rand()%3;
	switch(r)
	{
	case 0:
		emptyColumnText="No tweets loaded yet";
		break;
	case 1:
		emptyColumnText="No one wants to talk to you";
		break;
	case 2:
		emptyColumnText="Now with unanonymous data collection!";
		break;
	}
	//updateScreen=1;
}


HomeColumn::~HomeColumn(void)
{
}


void HomeColumn::newTweet( Tweet *tweet )
{
	if(tweet->_type<0)
		return;
	//enterMutex(drawingMutex);
	TweetInstance *instance=new TweetInstance(tweet,rw,onOff);
	m_tweets[tweet->id]=instance;
	add(tweet);
	/*if(scroll>100)
	{
		SDL_Surface *surface=instance->surface;
		scroll+=instance->surface->h;
	}*/
	//updateScreen=1;
	//printf("o %i\n",tweetHeight);
	tweetHeight=-1;
	//leaveMutex(drawingMutex);
}

void homeColumnRefresh(void *data)
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
	printf("refresh completed\n");
}

void HomeColumn::draw()
{
	Column::draw();
	if(drawRefreshButton())
		startThread(homeColumnRefresh,this);
}
