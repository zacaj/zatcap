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
	rw=w;
	columnName=_name;
	scroll=0;
	x=y=0;
	highlightScrollbar=0;
	minimized=0;
	tweetHeight=-1;
	emptyColumnText="No tweets";
	drawingMutex=createMutex();
	onOff=0;
	redrawAllTweets=0;
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
	deleteMutex(drawingMutex);
}
using namespace colors;
void Column::draw()
{
	
}

void Column::update()
{

}

bool Column::mouseButtonEvent( int x,int y,int button,int pressed )
{
	/*int mx,my;
	SDL_GetMouseState(&mx,&my);
	if(button!=-1)
	{
		if(y>screen->h-footerHeight)
			return 0;
		x-=columnHorizontalScroll;
	}
	if(button==1 && pressed)
	{
		if((x>this->x && x<this->x+w) || (x>this->x+w-3 && x<this->x+w+3))
		{
			mouseDragReceiver=this;
			updateScreen=1;
			return 1;
		}
	}
	if(mouseDragReceiver==this && pressed==0)
	{
		mouseDragReceiver=NULL;
		updateScreen=1;
		return 1;
	}
	if(button==-1)//drag
	{
		if(mx>this->x && mx<this->x+w)
			updateScreen=1;
		if(pressed)
		{
			if(mx>this->x+w-3-abs(x) && mx<this->x+w+3+abs(x) && !highlightScrollbar)//resize
			{
				rw+=x;
				if(rw<200)
					rw=200;
				updateScreen=1;
				redrawAllTweets=1;
			}
			else
			{
				if(highlightScrollbar)
					scroll+=(float)y*tweetHeight/(screen->h-COLUMNHEADERHEIGHT);
				else
					scroll-=y;
				updateScreen=1;
			}
		}
		else
		{
			if(mx>this->x+w-3-8 && mx<this->x+w-3)
			{
				if(!highlightScrollbar)
					updateScreen=1;
				highlightScrollbar=1;
			}
			else
			{
				if(highlightScrollbar)
					updateScreen=1;
				highlightScrollbar=0;
			}
			if(mx>this->x+w-3 && mx<this->x+w+3)
			{
				cursor=IDC_SIZEWE;
				updateScreen=1;
				return 1;
			}
			else
				cursor=IDC_ARROW;
		}
	}
	if(button==SDL_BUTTON_WHEELDOWN && x>this->x && x<this->x+w)
	{
		scroll+=settings::scrollSpeed;
		updateScreen=1;
	}
	if(button==SDL_BUTTON_WHEELUP && x>this->x && x<this->x+w)
	{
		scroll-=settings::scrollSpeed;
		updateScreen=1;
	}
	return 0;*/
	return 0;
}

void Column::deleteTweet(string id)
{
	if(m_tweets.find(id)!=m_tweets.end())
	{
		m_tweets[id]->tweet->instances.erase(this);
		m_tweets.erase(m_tweets.find(id));
		runJS("removeTweet('"+columnName+"','"+id+"');");
	}

}

bool Column::drawRefreshButton()
{
	/*if(!minimized)
	{
		int bx=columnHorizontalRenderAt-20-refresh[0]->w;
		int by=COLUMNHEADERHEIGHT-25;
		drawSprite(refresh[hoverButton(bx,by,refresh[0]->w,refresh[0]->h)],screen,0,0,bx,by,refresh[0]->w,refresh[0]->h);
		return doButton(bx,by,refresh[0]->w,refresh[0]->h);
	}
	return 0;*/
	return 0;
}
