#include "Column.h"
#include "twitter.h"
#include <SDL_rotozoom.h>
#include "algorithm"
#include "zatcap.h"
#include "Awesomium.h"

int columnHorizontalScroll=0;
int columnHorizontalRenderAt=0;

void Column::init(float _w ,string _name)
{
	w=_w;
	columnName=_name;
	x=y=0;
	minimized=0;
	string src=f2s("resources/column.html");
	string column=escape(src);
	replace(column,string("$COLUMNNAME"),columnName);
	replace(column,string("$WIDTH"),i2s(w));
	runJS("addColumn(nodeFromHtml(\""+column+"\"),'"+columnName+"');");
}


void Column::add( Item* tweet )
{
	runJS("addTweetHtml('"+columnName+"',\""+escape(tweet->getHtml(columnName))+"\",'"+ tweet->id +"');");
	tweet->instances.insert(this);
}

Column::~Column(void)
{
}
void Column::draw()
{
	
}

void Column::update()
{

}

void Column::deleteTweet(string id)
{
	if(m_tweets.find(id)!=m_tweets.end())
	{
		m_tweets[id]->instances.erase(this);
		m_tweets.erase(m_tweets.find(id));
		runJS("removeTweet('"+columnName+"','"+id+"');");
	}

}
