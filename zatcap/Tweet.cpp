#include "Tweet.h"
#include "zatcap.h"
#include "Textbox.h"
using namespace colors;


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
	SDL_Surface* pic=user()->pic(); \
	instance->p=new textPos[entities.size()*2]; \
	for(int i=0;i<entities.size();i++) \
	{ \
		instance->p[i*2].pos=entities[i]->start; \
		instance->p[i*2].x=-10000; \
		instance->p[i*2+1].pos=entities[i]->end; \
		instance->p[i*2+1].x=-10000; \
	} \
	height=drawTextWrappedp(text.c_str(),x+7+pic->w+8,height+1,textWidth,settings::textSize,NULL,0,instance->p,entities.size()*2,instance->widths,256,256,256,tempSurface); \
	for(int i=0;i<entities.size();i++) \
	{ \
		if(instance->p[i*2].y==instance->p[i*2+1].y) \
		{ \
			bool hover=0; \
			if(colors::entityColorR[entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[entities[i]->type+hover*3]!=colors::entityColorR[entities[i]->type+hover*3] || colors::entityColorB[entities[i]->type+hover*3]!=colors::textColorB) \
			{ \
				boxColor(tempSurface,instance->p[i*2].x,instance->p[i*2].y,instance->p[i*2+1].x,instance->p[i*2].y+instance->p[i*2].h,getBackgroundColor(instance->background,read)); \
				drawText(entities[i]->text.c_str(),instance->p[i*2].x,instance->p[i*2].y,settings::textSize,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3],tempSurface); \
			} \
			if(!settings::underlineLinksWhenHovered) \
			{ \
				boxColor(tempSurface,instance->p[i*2].x,instance->p[i*2].y+settings::textSize,instance->p[i*2+1].x,instance->p[i*2].y+settings::textSize+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
			} \
		} \
		else \
		{ \
	/*boxRGBA(screen,instance->p[i*2].x,instance->p[i*2].y,x+7+pic->w+8+widths[instance->p[i*2].line],instance->p[i*2].y+instance->p[i*2].h,255,0,255,64);
	//boxRGBA(screen,x+7+pic->w+8,instance->p[i*2+1].y,instance->p[i*2+1].x,instance->p[i*2+1].y+instance->p[i*2+1].h,255,0,255,64);*/ \
			bool hover=0; \
			if(colors::entityColorR[entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[entities[i]->type+hover*3]!=colors::entityColorR[entities[i]->type+hover*3] || colors::entityColorB[entities[i]->type+hover*3]!=colors::textColorB) \
			{ \
				if(instance->p[i*2].w) \
				{ \
					boxColor(tempSurface,instance->p[i*2].x,	instance->p[i*2].y,	x+7+pic->w+8+instance->widths[instance->p[i*2].line],	instance->p[i*2].y+instance->p[i*2].h,	getBackgroundColor(instance->background,read)); \
					drawText(entities[i]->text.substr(0,instance->p[i*2].w).c_str(),instance->p[i*2].x,instance->p[i*2].y,settings::textSize,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3],tempSurface); \
				} \
				if(instance->p[i*2+1].w) \
				{ \
					boxColor(tempSurface,x+7+pic->w+8,	instance->p[i*2+1].y,	instance->p[i*2+1].x,	instance->p[i*2+1].y+instance->p[i*2+1].h,getBackgroundColor(instance->background,read)); \
					drawText(entities[i]->text.substr(instance->p[i*2].w,entities[i]->text.size()-instance->p[i*2].w).c_str(),x+7+pic->w+8,instance->p[i*2+1].y,settings::textSize,entityColorR[entities[i]->type+hover*3],entityColorG[entities[i]->type+hover*3],entityColorB[entities[i]->type+hover*3],tempSurface); \
				} \
			} \
			if(!settings::underlineLinksWhenHovered) \
			{ \
				boxColor(tempSurface,instance->p[i*2].x,instance->p[i*2].y+settings::textSize,x+7+pic->w+8+instance->widths[instance->p[i*2].line],instance->p[i*2].y+settings::textSize+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
				boxColor(tempSurface,x+7+pic->w+8,instance->p[i*2+1].y+settings::textSize,instance->p[i*2+1].x,instance->p[i*2+1].y+settings::textSize+1,colors::entityUnderlineColor[entities[i]->type+hover*3]); \
			} \
		} \
	/*boxRGB(screen,instance->p[i*2].x,instance->p[i*2].y,instance->p[i*2].x+1,instance->p[i*2].y+instance->p[i*2].h,255,0,255); boxRGB(screen,instance->p[i*2+1].x,instance->p[i*2+1].y,instance->p[i*2+1].x+1,instance->p[i*2+1].y+instance->p[i*2+1].h,0,255,255); */ \
	} \
	}} \

#define drawEntities2 { \
{ \
	SDL_Surface* pic=tweet->user()->pic(); \
	for(int i=0;i<tweet->entities.size();i++) \
	{ \
		if(p[i*2].y==p[i*2+1].y) \
		{ \
			bool hover=hoverButton(p[i*2].x+x,p[i*2].y+y,p[i*2+1].x-p[i*2].x,p[i*2].h); \
			if(hover) \
				if(colors::entityColorR[tweet->entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[tweet->entities[i]->type+hover*3]!=colors::entityColorR[tweet->entities[i]->type+hover*3] || colors::entityColorB[tweet->entities[i]->type+hover*3]!=colors::textColorB) \
				{ \
					boxColor(screen,p[i*2].x+x,p[i*2].y+y,p[i*2+1].x+x,p[i*2].y+p[i*2].h+y,getBackgroundColor(background,tweet->read)); \
					drawText(tweet->entities[i]->text.c_str(),p[i*2].x+x,p[i*2].y+y,settings::textSize,entityColorR[tweet->entities[i]->type+hover*3],entityColorG[tweet->entities[i]->type+hover*3],entityColorB[tweet->entities[i]->type+hover*3]); \
				} \
			if(hover && settings::underlineLinksWhenHovered) \
			{ \
				boxColor(screen,p[i*2].x+x,p[i*2].y+settings::textSize+y,p[i*2+1].x+x,p[i*2].y+settings::textSize+1+y,colors::entityUnderlineColor[tweet->entities[i]->type+hover*3]); \
			} \
			if(doButton(p[i*2].x+x,p[i*2].y+y,p[i*2+1].x-p[i*2].x,p[i*2].h)) \
			{ \
				tweet->entities[i]->click(); \
			} \
	} \
	else \
	{ \
	/*boxRGBA(screen,p[i*2].x,p[i*2].y,x+7+pic->w+8+widths[p[i*2].line],p[i*2].y+p[i*2].h,255,0,255,64);
	//boxRGBA(screen,x+7+pic->w+8,p[i*2+1].y,p[i*2+1].x,p[i*2+1].y+p[i*2+1].h,255,0,255,64);*/ \
		bool hover=hoverButton(p[i*2].x+x,	p[i*2].y+y,	x+7+pic->w+8+widths[p[i*2].line]-p[i*2].x,	p[i*2].h) \
		||	hoverButton(x+7+pic->w+8,	p[i*2+1].y+y,	p[i*2+1].x-(x+7+pic->w+8),	p[i*2+1].h); \
		if(hover) \
			if(colors::entityColorR[tweet->entities[i]->type+hover*3]!=colors::textColorR || colors::entityColorR[tweet->entities[i]->type+hover*3]!=colors::entityColorR[tweet->entities[i]->type+hover*3] || colors::entityColorB[tweet->entities[i]->type+hover*3]!=colors::textColorB) \
			{ \
				if(p[i*2].w) \
				{ \
					boxColor(screen,p[i*2].x+x,	p[i*2].y+y,	x+7+pic->w+8+widths[p[i*2].line],	p[i*2].y+p[i*2].h+y,	getBackgroundColor(background,tweet->read)); \
					drawText(tweet->entities[i]->text.substr(0,p[i*2].w).c_str(),p[i*2].x+x,p[i*2].y+y,settings::textSize,entityColorR[tweet->entities[i]->type+hover*3],entityColorG[tweet->entities[i]->type+hover*3],entityColorB[tweet->entities[i]->type+hover*3]); \
				} \
				if(p[i*2+1].w) \
				{ \
					boxColor(screen,x+7+pic->w+8,	p[i*2+1].y+y,	p[i*2+1].x+x,	p[i*2+1].y+p[i*2+1].h+y,getBackgroundColor(background,tweet->read)); \
					drawText(tweet->entities[i]->text.substr(p[i*2].w,tweet->entities[i]->text.size()-p[i*2].w).c_str(),x+7+pic->w+8,p[i*2+1].y+y,settings::textSize,entityColorR[tweet->entities[i]->type+hover*3],entityColorG[tweet->entities[i]->type+hover*3],entityColorB[tweet->entities[i]->type+hover*3]); \
				} \
			} \
		if(hover && settings::underlineLinksWhenHovered) \
		{ \
			boxColor(screen,p[i*2].x+x,p[i*2].y+settings::textSize+y,x+7+pic->w+8+widths[p[i*2].line],p[i*2].y+settings::textSize+1+y,colors::entityUnderlineColor[tweet->entities[i]->type+hover*3]); \
			boxColor(screen,x+7+pic->w+8,p[i*2+1].y+settings::textSize+y,p[i*2+1].x+x,p[i*2+1].y+settings::textSize+1+y,colors::entityUnderlineColor[tweet->entities[i]->type+hover*3]); \
		} \
		if(	doButton(p[i*2].x+x,	p[i*2].y+y,	x+7+pic->w+8+widths[p[i*2].line]-p[i*2].x,	p[i*2].h) \
			||	doButton(x+7+pic->w+8,	p[i*2+1].y+y,	p[i*2+1].x-(x+7+pic->w+8),	p[i*2+1].h) \
			) \
		{ \
			tweet->entities[i]->click(); \
		} \
	} \
	/*boxRGB(screen,p[i*2].x,p[i*2].y,p[i*2].x+1,p[i*2].y+p[i*2].h,255,0,255); boxRGB(screen,p[i*2+1].x,p[i*2+1].y,p[i*2+1].x+1,p[i*2+1].y+p[i*2+1].h,0,255,255); */ \
	} \
}} 

int Tweet::draw( int x,int y,int w,int background)
{
	SDL_Surface *pic=user()->pic();
	int height=y;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
draw:
	int nameWidth=200;
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,settings::userNameTextSize,first,usernameTextColorR,usernameTextColorG,usernameTextColorB);
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
	int timeWidth=drawTextr(date,x+w-15,y+5,settings::timeSize,timeTextColorR,timeTextColorG,timeTextColorB);//time
	int heightBeforeText=height;
	//{{drawEntities}}
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


int Tweet::draw( TweetInstance *instance,int w )
{
	int x=0,y=0;
	SDL_Surface *pic=user()->pic();
	int height=0;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
	SDL_LockMutex(tempSurfaceMutex);
	SDL_FillRect(tempSurface,0,0);
draw:
	int nameWidth=textWidth;
	//SDL_FillRect(tempSurface,0,0);
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,settings::userNameTextSize,first,usernameTextColorR,usernameTextColorG,usernameTextColorB,tempSurface);
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
	int timeWidth=drawTextr(date,x+w-15,y+5,settings::timeSize,timeTextColorR,timeTextColorG,timeTextColorB,tempSurface);//time
	int heightBeforeText=height;
	{{drawEntities}}
	if(height<pic->h+10+y)
		height=pic->h+10+y;
	if(height<y+48+18+3)
		height=y+48+18+3;
	int newx;
	instance->buttonX=x;
	instance->buttonY=y;
	instance->buttonHeight=height;
	newx=drawButtons(x,y,w,height,0);
	if(newx<textWidth+(x+7+pic->w+8))
	{
		textWidth=newx-(x+7+pic->w+8)-2;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	if(first)
	{
		boxColor(tempSurface,x,y,x+w-8,height,getBackgroundColor(instance->background,read));
		first=0;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	drawSprite(pic,tempSurface,0,0,x+5,y+5,pic->w,pic->h);
	instance->surface=SDL_CreateRGBSurface(SDL_HWSURFACE,w,height,32,tempSurface->format->Rmask,tempSurface->format->Gmask,tempSurface->format->Bmask,tempSurface->format->Amask);
	drawSprite(tempSurface,instance->surface,0,0,0,0,w,height);
	SDL_FillRect(tempSurface,0,0);
	SDL_UnlockMutex(tempSurfaceMutex);
	instance->pic=user()->pic();
	return 0;
}


int Retweet::draw( int x,int y,int w,int background )
{
	SDL_Surface *pic=user()->pic();
	int height=y;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
draw:
	int nameWidth=200;
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,settings::userNameTextSize,first,usernameTextColorR,usernameTextColorG,usernameTextColorB,tempSurface);
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
	int timeWidth=drawTextr(date,x+w-15,y+4,settings::timeSize,timeTextColorR,timeTextColorG,timeTextColorB,tempSurface);//time

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
		sprintf(str,"retweeted by %s at %s (x%i)",retweetedBy->username.c_str(),date,nRetweet);
	int retweetWidth=w-7-pic->w-5-7-13-3-nameWidth+5;
	drawTextWrappedw(str,((x+7+user()->pic()->w+5)+nameWidth+5),y+5,retweetWidth,settings::retweetTextSize,1,retweetTextColorR,retweetTextColorG,retweetTextColorB,tempSurface);//get width
	if(retweetWidth+x+7+user()->pic()->w+5+nameWidth+5>x+w-15-timeWidth)//on top of time
		height=drawTextWrapped(str,x+7+user()->pic()->w+5,height,w-7-pic->w-5-7-13-3,settings::retweetTextSize,first,retweetTextColorR,retweetTextColorG,retweetTextColorB);
	else
		height=drawTextWrapped(str,x+7+user()->pic()->w+5+nameWidth+5,y+5,w-7-pic->w-5-7-13-3-nameWidth-5,settings::retweetTextSize,first,retweetTextColorR,retweetTextColorG,retweetTextColorB);
	//drawEntities

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

int Retweet::draw( TweetInstance *instance,int w )
{
	int x=0,y=0;
	SDL_Surface *pic=user()->pic();
	int height=0;
	bool first=settings::tweetBackgrounds;
	int textWidth=w-7-pic->w-5-7-13-16;
	SDL_LockMutex(tempSurfaceMutex);
	SDL_FillRect(tempSurface,0,0);
draw:
	int nameWidth=200;
	//SDL_FillRect(tempSurface,0,0);
	height=drawTextWrappedw(user()->username.c_str(),x+7+pic->w+5,y+2,nameWidth,settings::userNameTextSize,first,usernameTextColorR,usernameTextColorG,usernameTextColorB,tempSurface);
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
	int timeWidth=drawTextr(date,x+w-15,y+5,settings::timeSize,timeTextColorR,timeTextColorG,timeTextColorB,tempSurface);//time
	int heightBeforeText=height;
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
		sprintf(str,"retweeted by %s at %s (x%i)",retweetedBy->username.c_str(),date,nRetweet);
	int retweetWidth=w-7-pic->w-5-7-13-3-nameWidth+5;
	drawTextWrappedw(str,((x+7+user()->pic()->w+5)+nameWidth+5),y+5,retweetWidth,settings::retweetTextSize,1,retweetTextColorR,retweetTextColorG,retweetTextColorB,tempSurface);//get width
	if(retweetWidth+x+7+user()->pic()->w+5+nameWidth+5>x+w-15-timeWidth)//on top of time
		height=drawTextWrapped(str,x+7+user()->pic()->w+5,height,w-7-pic->w-5-7-13-3,settings::retweetTextSize,first,retweetTextColorR,retweetTextColorG,retweetTextColorB,tempSurface);
	else
		height=drawTextWrapped(str,x+7+user()->pic()->w+5+nameWidth+5,y+5,w-7-pic->w-5-7-13-3-nameWidth-5,settings::retweetTextSize,first,retweetTextColorR,retweetTextColorG,retweetTextColorB,tempSurface);
	{{drawEntities}}

	if(height<pic->h+10+y)
		height=pic->h+10+y;
	if(height<y+48+18+3)
		height=y+48+18+3;
	int newx;
	instance->buttonX=x;
	instance->buttonY=y;
	instance->buttonHeight=height;
	newx=drawButtons(x,y,w,height,0);
	if(newx<textWidth+(x+7+pic->w+8))
	{
		textWidth=newx-(x+7+pic->w+8)-2;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	if(first)
	{
		boxColor(tempSurface,x,y,x+w-4,height,getBackgroundColor(instance->background,read));
		first=0;
		goto draw;//cringe, I know, I know.  Now STFU
	}
	pic=user()->mediumPic();
	drawSprite(pic,tempSurface,0,0,x+5,y+5,pic->w,pic->h);
	pic=retweetedBy->smallPic();
	drawSprite(pic,tempSurface,0,0,x+5+48-pic->w,y+5+48-pic->h,pic->w,pic->h);
	//drawSprite(pic,tempSurface,0,0,x+5,y+5,pic->w,pic->h);
	instance->surface=SDL_CreateRGBSurface(SDL_HWSURFACE,w,height,32,tempSurface->format->Rmask,tempSurface->format->Gmask,tempSurface->format->Bmask,tempSurface->format->Amask);
	drawSprite(tempSurface,instance->surface,0,0,0,0,w,height);
	SDL_FillRect(tempSurface,0,0);
	SDL_UnlockMutex(tempSurfaceMutex);
	instance->pic=user()->mediumPic();
	instance->pic2=user()->smallPic();
	return 0;
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
	if(y+19>_y+h)	{y=_y+18;x-=16;	}
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
		y+=16;
	}
	if(y+19>_y+h)	{y=_y+18;x-=16;	}
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
		y+=16;
	}
	if(y+19>_y+h)	{y=_y+18;x-=16;	}
	if(highlighted && replyTo.size())
	{
		drawSprite(convo[hoverButton(x,y,16,16)],_screen,0,0,x,y,16,16);
		if(doButton(x,y,16,16))
		{
			return TOGGLECONVODISPLAY;
		}
	}
	return x;
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
	if(user()->pic()!=instance->pic)
		instance->refresh(instance->w);
	return 0;
}

int Retweet::cachedDraw( TweetInstance *instance )
{
	if(user()->mediumPic()!=instance->pic)
		instance->refresh(instance->w);
	if(user()->smallPic()!=instance->pic2)
		instance->refresh(instance->w);
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

TweetInstance::TweetInstance( Tweet *_tweet,int _w,int _background  )	:
	tweet(_tweet),
	background(_background)
{
	w=_w;
	surface=NULL;
	replyTo=NULL;
	drawReply=0;
	//refresh(w);
}

int TweetInstance::draw( int x,int y )	
{
	if(surface==NULL)
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
	return h;
}

TweetInstance::~TweetInstance()
{
	if(surface)
	{
		SDL_FreeSurface(surface);
		surface=NULL;
	}
	if(p)
	{
		delete p; 
		p=NULL;
	}
}

void TweetInstance::refresh( int w )
{
	printf("redraw");
	if(surface)
		SDL_FreeSurface(surface);
	widths.clear();
	tweet->draw(this,w);
	if(replyTo!=NULL)
	{
		delete replyTo;
		replyTo=NULL;
	}
}
