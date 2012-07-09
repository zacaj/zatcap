#include "MentionColumn.h"
#include "twitter.h"

MentionColumn::MentionColumn(string _term, int w ):Column(w,"mentions")
{
	term=_term;
	emptyColumnText="No one loves you";
	updateScreen=1;
}


MentionColumn::~MentionColumn(void)
{
}

void MentionColumn::newTweet( Tweet *tweet )
{
	if(tweet->text.find(term)!=string::npos)
	{
		SDL_LockMutex(drawingMutex);
		m_tweets[tweet->id]=new TweetInstance(tweet,rw,onOff);
		updateScreen=1;
		SDL_UnlockMutex(drawingMutex);
	}
}

int mentionColumnRefresh(void *data)
{
	MentionColumn *col=(MentionColumn*)data;
	if(col->m_tweets.empty())
		return 0;
	auto rit=col->m_tweets.rbegin();
	rit++;
	string tmpString;
	while((tmpString=twit->mentionsGet(rit->first,""))=="");//settings::tweetsToLoadOnStartup+50
	parseRestTweets(tmpString);
	return 0;
}

void MentionColumn::draw()
{
	Column::draw();
	if(drawRefreshButton())
		SDL_CreateThread(mentionColumnRefresh,this);
}
