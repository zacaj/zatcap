#include "Column.h"
#include "twitter.h"
#include <SDL_rotozoom.h>

int columnHorizontalScroll=0;
int columnHorizontalRenderAt=0;

Column::Column(float _w ,string _name)
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
	drawing=0;
	onOff=0;
	redrawAllTweets=0;
}


Column::~Column(void)
{
}
using namespace colors;
void Column::draw()
{
	drawing=1;
	if(scroll<0)
		scroll=0;
	y=COLUMNHEADERHEIGHT-scroll;
	x=columnHorizontalRenderAt;
	if(columnHorizontalRenderAt>screen->w)
		return;
	{
		if(minimized)
		{
			w=34;
			columnHorizontalRenderAt+=1;
			boxColor(screen,columnHorizontalRenderAt,0,columnHorizontalRenderAt+32,screen->h-footerHeight,columnBackgroundColor);
			{
				int bx=columnHorizontalRenderAt+9,by=COLUMNHEADERHEIGHT-24;
				if(hoverButton(bx,by,16,16))
				{
					boxColor(screen,bx+3,by+5,bx+13,by+7,buttonHoverColor);
					boxColor(screen,bx+7,by+1,bx+9,by+11,buttonHoverColor);
				}
				else
				{
					boxColor(screen,bx+3,by+5,bx+13,by+7,buttonColor);
					boxColor(screen,bx+7,by+1,bx+9,by+11,buttonColor);
				}
				if(doButton(bx,by,16,16))
				{
					minimized=0;
				}
			}
			SDL_Surface *otext=drawTexts(columnName.c_str(),25,columnTitleTextColorR,columnTitleTextColorG,columnTitleTextColorB);
			SDL_Surface *text=rotateSurface90Degrees(otext,3);
			drawSprite(text,screen,0,0,columnHorizontalRenderAt+(32-text->w)/2,COLUMNHEADERHEIGHT,text->w,text->h);
			SDL_FreeSurface(text);
			SDL_FreeSurface(otext);
			columnHorizontalRenderAt+=33;
			drawing=0;
			return;
		}
		else
		{
			w=rw;
			boxColor(screen,columnHorizontalRenderAt,0,columnHorizontalRenderAt+w,screen->h-footerHeight,columnBackgroundColor);
		}
	}
	int tweetsDrawn=0;
	int firstDrawn=-1,i=0;
	if(m_tweets.size()==0)
		drawTextWrappedc(emptyColumnText.c_str(),columnHorizontalRenderAt+w/2,100,w-30,14);
	TweetInstance *lastInstance=NULL;
	for(auto it=m_tweets.rbegin();it!=m_tweets.rend();it++,i++)
	{
		TweetInstance *instance=it->second;
		Tweet *tweet=instance->tweet;

		//tweetHeight=height-y;
		if(lastInstance!=NULL && settings::separatorHeight)
		{
			if(lastInstance->tweet->read!=tweet->read)
			{
				boxColor(screen,columnHorizontalRenderAt,y,x+w-4,y+settings::separatorHeight,colors::separatorColor);
				y+=settings::separatorHeight;
			}
			if(lastInstance->background==instance->background)
			{
				instance->background=!instance->background;
				instance->needsRefresh=1;
			}
		}
		int oldy=y;
		if(instance->needsRefresh || redrawAllTweets)
		{
			if(lastInstance==NULL)
				it->second=new TweetInstance(instance->tweet,rw,onOff);
			else
				it->second=new TweetInstance(instance->tweet,rw,!lastInstance->background);

			delete instance;
			instance=it->second;
		}
		instance->draw(columnHorizontalRenderAt,y);
		y+=instance->surface->h;
		int y2=y-oldy;
		if(oldy+y2>screen->h-footerHeight)
			y2+=screen->h-footerHeight-(y2+oldy);
		if(y>0)
			if(doButton(columnHorizontalRenderAt,oldy,w-3-8,y2,SDL_BUTTON_RIGHT))
			{
				for(auto rit=it;rit!=m_tweets.rend();rit++)
				{
					rit->second->tweet->read=1;
					rit->second->needsRefresh=1;
				}
			}
		if(y>COLUMNHEADERHEIGHT)
		{
			tweetsDrawn++;
			if(firstDrawn==-1)
				firstDrawn=i;
		}
		if(y>screen->h-footerHeight && tweetHeight!=-1)
			break;
		onOff=!onOff;
		lastInstance=instance;
	}
	if(tweetHeight==-1)
	{
		tweetHeight=y+scroll;
		//printf("n %i\n",tweetHeight);
	}
	boxColor(screen,columnHorizontalRenderAt+w-3-8,COLUMNHEADERHEIGHT,columnHorizontalRenderAt+w-3,screen->h-footerHeight,scrollbarBackgroundColor);//scrollbar background

	if((m_tweets.size()!=0))
	{
		scrollBarHeight=(float)(screen->h-COLUMNHEADERHEIGHT-footerHeight)/tweetHeight*(screen->h-COLUMNHEADERHEIGHT-footerHeight);
		scrollBarY=(float)(scroll)/(tweetHeight)*(screen->h-COLUMNHEADERHEIGHT-footerHeight);
		if(scrollBarHeight>(screen->h-COLUMNHEADERHEIGHT-footerHeight))
			scrollBarHeight=(screen->h-COLUMNHEADERHEIGHT-footerHeight);
 		if(highlightScrollbar)//scrollbar
			boxColor(screen,columnHorizontalRenderAt+w-3-8,scrollBarY+COLUMNHEADERHEIGHT,columnHorizontalRenderAt+w-3,scrollBarY+scrollBarHeight+COLUMNHEADERHEIGHT,scrollbarHoverColor);
		else
			boxColor(screen,columnHorizontalRenderAt+w-3-8,scrollBarY+COLUMNHEADERHEIGHT,columnHorizontalRenderAt+w-3,scrollBarY+scrollBarHeight+COLUMNHEADERHEIGHT,scrollbarColor);

		boxColor(screen,columnHorizontalRenderAt,0,columnHorizontalRenderAt+w,COLUMNHEADERHEIGHT,columnBackgroundColor);//header bg

		/*if(settings::arrowsOnScrollbar)
		{
			drawSprite(arrowUp,screen,0,0,columnHorizontalRenderAt+w-3-8,COLUMNHEADERHEIGHT,8,8);
			drawSprite(arrowDown,screen,0,0,columnHorizontalRenderAt+w-3-8,screen->h-8,8,8);
		}*///todo
	}
	drawText(columnName.c_str(),columnHorizontalRenderAt+4,COLUMNHEADERHEIGHT-30,25,200,200,200);
	{//minimize
		int bx=columnHorizontalRenderAt+w-15,by=COLUMNHEADERHEIGHT-24;
		if(doButton(bx,by,16,16))
		{
			minimized=1;
		}
		if(hoverButton(bx,(by),16,16))
			boxColor(screen,(bx)+3,(by)+5,(bx)+13,(by)+7,buttonHoverColor);
		else
			boxColor(screen,(bx)+3,(by)+5,(bx)+13,(by)+7,buttonColor);
	}
	{
		char str[100];
		sprintf(str,"%i",m_tweets.size());
		drawText(str,columnHorizontalRenderAt+w-80,COLUMNHEADERHEIGHT-30,13,255,255,255);
	}/**/
	{
		char str[100];
		sprintf(str,"%i",scroll);
		drawText(str,columnHorizontalRenderAt+w-120,COLUMNHEADERHEIGHT-30,13,255,255,255);
	}/**/
	{//top
		int bx=columnHorizontalRenderAt+w-20-24-top[0]->w;
		int by=COLUMNHEADERHEIGHT-25;
		drawSprite(top[hoverButton(bx,by,top[0]->w,top[0]->h)],screen,0,0,bx,by,top[0]->w,top[0]->h);
		if(doButton(bx,by,16,16))
			scroll=0;
	}/**/
	if(y<screen->h-COLUMNHEADERHEIGHT-footerHeight)
	{
		scroll-=screen->h-y-COLUMNHEADERHEIGHT-footerHeight;
		SDL_CreateThread(loadOlderTweets,NULL);
		if(scroll>0)
		{
			boxColor(screen,columnHorizontalRenderAt,0,columnHorizontalRenderAt+w,screen->h,columnBackgroundColor);
			return draw();//erase and draw again to prevent jitter
		}
	}
	columnHorizontalRenderAt+=w;
	drawing=0;
	redrawAllTweets=0;
}

void Column::update()
{

}

bool Column::mouseButtonEvent( int x,int y,int button,int pressed )
{
	int mx,my;
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
	return 0;
}

void Column::deleteTweet(string id)
{
	while(drawing);
	if(m_tweets.find(id)!=m_tweets.end())
	{
		m_tweets.erase(m_tweets.find(id));
		updateScreen=1;
	}
}

bool Column::drawRefreshButton()
{
	if(!minimized)
	{
		int bx=columnHorizontalRenderAt-20-refresh[0]->w;
		int by=COLUMNHEADERHEIGHT-25;
		drawSprite(refresh[hoverButton(bx,by,refresh[0]->w,refresh[0]->h)],screen,0,0,bx,by,refresh[0]->w,refresh[0]->h);
		return doButton(bx,by,refresh[0]->w,refresh[0]->h);
	}
	return 0;
}
