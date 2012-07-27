#include "Textbox.h"
#include "zatcap.h"
#include <SDL_gfxPrimitives.h>
#include "twitcurl.h"
#include "twitter.h"
#include "Tweet.h"

TTF_Font* getFont(int size);
#define ITR str.begin()+
int _drawTextWrapped(const char* string, int _x, int _y,int w,int size,int cp,int &cx,int &cy, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(_screen==NULL)
		_screen=screen;
	if(((int)_screen)==1)//finding click
	{
		//_screen==NULL;
		cp=0;
	}
	int x=_x,y=_y;
	int tw,th;
	std::string str=string;
	TTF_Font *font=getFont(size);
	int h=TTF_FontLineSkip(font);
	std::string s=str;
	std::string rest;
	while(s.size())
	{
		TTF_SizeUTF8(font,s.c_str(),&tw,&th);
		size_t pos=s.size();
		rest=s;
		bool wordSplit=1;
		while(tw>w && s.size())
		{
			pos=s.rfind(' ',pos-1);
			if(pos==string::npos)
			{
				pos=s.size()-1;
again:
				TTF_SizeUTF8(font,s.substr(0,pos).c_str(),&tw,&th);
				if(tw>w)
				{
					pos-=1;
					goto again;
				}
				s.erase(pos+1);
				wordSplit=1;
				break;
			}
			s=s.substr(0,pos);
			TTF_SizeUTF8(font,s.c_str(),&tw,&th);
			wordSplit=0;
		}
		if(!wordSplit && s.size())
		{
			pos++;
			s+=" ";
		}
		size_t p=s.find_first_of('\n');
		if(p!=string::npos)
		{
			pos=p+1;
			s=s.substr(0,p);
		}
		if(!rest.size())
			break;
		if(((int)_screen)!=1)
			drawText(s.c_str(),x,y,size,fR,fG,fB,_screen);
		if(cp>=0  && cp<=s.size() && cx==-10000)
		{
			cy=y;
			cx=x;
			std::string tempStr;
			for(int i=0;i<cp && i<s.size();i++)
			{
				if(i==s.size()-1 && s[i]==' ' 
					&& s.size()!=rest.size())//make sure tehres another line
				{
					cx=_x;
					cy=y+h;
					break;
				}
				int lw=0,lh;
				char cstr[2000];
				tempStr.push_back(s[i]);
				sprintf(cstr,"%s",tempStr.c_str());
				TTF_SizeUTF8(font,cstr,&lw,&lh);
				cx=x+lw;
			}
		}
		else if(((int)_screen)==1)
		{
			if(cy>=y && cy<y+h && cx>=x && cx<x+w)//cursor clicked this line
			{
				int px=x;
				char cstr[2000];
				std::string tempStr;
				int i;
				for(i=0;i<s.size();i++)
				{
					int lw=0,lh;
					tempStr.push_back(s[i]);
					sprintf(cstr,"%s",tempStr.c_str());
					TTF_SizeUTF8(font,cstr,&lw,&lh);
					if(cx>=px && cx<px+lw)//this letter clicked
					{

						return -cp+i;
					}
					//px+=lw;
				}
				//cursor past end of line
				return -cp+s.size()-1;
			}
		}
		cp-=pos;
		s=rest.substr(pos,rest.size()-pos+1);
		y+=h;
		if(cp==0 && !s.size() && p!=string::npos)//fix for cursor being on empty line
		{
			cx=x;
			cy=y;
		}
	}
	if(((int)_screen)==1)
	{
		return str.size();
	}
	return y;
}

Textbox::Textbox(void)
{
	cursorBlink=0;
	str="";
	cursorPos=0;
	holdTimer=0;
	heldKey=-1;
	scroll=0;
	lastTextChangeTime=10000;
}


Textbox::~Textbox(void)
{
}

void Textbox::update()
{
	x=(screen->w-w)/2;
	y=screen->h-footerHeight+10;
	if(keyboardInputReceiver==this)
	{
		cursorBlink++;
		lastTextChangeTime++;
		if(cursorBlink>40)
		{
			updateScreen=1;
			cursorBlink=-30;
		}
		if(cursorBlink==0)
			updateScreen=1;
	}
	else
		cursorBlink=0;
}

bool Textbox::mouseButtonEvent( int x,int y,int button,int pressed )
{
	if(x>this->x && x<this->x+w && y>this->y && y<this->y+h)
	{
		if(button==SDL_BUTTON_LEFT && pressed)
		{
			if(keyboardInputReceiver!=this)
				keyboardInputReceiver=this;
			{
				cursorPos=_drawTextWrapped(str.c_str(),this->x+4,this->y+4,this->w-8,settings::editorTextSize,0,x,y,255,255,255,(SDL_Surface*)1);
				cursorBlink=10;
			}
		}
		return 1;
	}
	else
	{
		if(button==SDL_BUTTON_LEFT && pressed)
		{
			if(keyboardInputReceiver==this)
				keyboardInputReceiver=NULL;
		}
	}
	return 0;
}
extern twitCurl *twit;
void Textbox::draw()
{
	boxRGB(screen,0,screen->h-footerHeight,screen->w,screen->h,00,0,0);
	boxColor(screen,x,y,x+w,y+h,colors::columnBackgroundColor);
	int cx=-10000,cy;
	SDL_Surface* text=SDL_CreateRGBSurface(SDL_SWSURFACE,w,16*140,32,screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
	SDL_FillRect(text,0,SDL_MapRGB(text->format,30,30,30));
	_drawTextWrapped(str.c_str(),4,4,w-8,settings::textSize,cursorPos,cx,cy,255,255,255,text);
	if(cx==-10000)
	{
		cx=4;
		cy=4;
	}
	cx-=1;
	if(cursorBlink>=0 && keyboardInputReceiver==this)
		lineRGBA(text,cx,cy,cx,cy+16,255,255,255,255);

	if(cy<scroll)
		scroll=cy;
	if(scroll<0)
		scroll=0;
	cy+=16;
	if(cy-scroll>h)
		scroll=cy-h;
	drawSprite(text,screen,0,scroll,x,y,w,h);
	SDL_FreeSurface(text);

	if(this==keyboardInputReceiver)
		rectangleRGBA(screen,x,y,x+w,y+h,200,200,200,255);
	else if(hoverButton(x,y,w,h))
		rectangleRGBA(screen,x,y,x+w,y+h,150,150,150,255);
	else
		rectangleRGBA(screen,x,y,x+w,y+h,50,50,50,255);
	char s[1000];
	sprintf(s,"%i",140-str.size());
	drawText(s,x+w+5,y+h/2,17);

	if(textButton(x+w+5,y,"Tweet"))
		doTweet();
	if(textButton(x+w+5,y+h-16,"Clear"))
	{
		strGoingToChange();
		str.clear();
		cursorPos=0;
	}
	if(replyId.size())
	{
		sprintf(s,"Replying to @%s",tweets[replyId]->user()->username.c_str());
		drawText(s,x+w+5,y+h/2-15,14);
	}
	if(textButton(x-50,y,"Save"))
	{
		saveTweets();
	}
}

extern twitCurl *twit;

struct tweetData
{
	string str;
	string replyId;
};

int sendTweet(void *_data)
{
	tweetData *data=(tweetData*)_data;
	string str=data->str;
	while(twit->statusUpdate(str,data->replyId)=="");
	printf("Tweet sent successfully: %s (%s,%s)\n",str.c_str(),data->replyId.c_str(),replyId.c_str());
	delete data;
	return 0;
}

void Textbox::doTweet()
{
	if(str.size()<=140 && str.size() && loggedIn)
	{
		printf("sending tweet... ");
		if(str.size()<135 && settings::addSFZ)
			str+="\n#sfz";
		tweetData *data=new tweetData;
		data->str=str;
		data->replyId=replyId;
		SDL_CreateThread(sendTweet,data);
		while(!lstr.empty())
		{
			lstr.pop();
			lpos.pop();
		}
		str.erase(str.begin(),str.end());
		replyId="";
		cursorPos=0;
		if(settings::markReadAfterTweeting==1)
		{
			for(map<string,Tweet*>::iterator it=tweets.begin();it!=tweets.end();it++)
				it->second->read=1;
		}
	}
}

bool Textbox::keyboardEvent( int key,int pressed,int mod )
{
	if(keyboardInputReceiver!=this)
		return 0;
	int handled=0;
	if(pressed)
	{
		if(key==SDLK_RETURN && (mod&KMOD_SHIFT^settings::tweetOnEnter))
		{
			doTweet();
			return 1;
		}
		else if(key==SDLK_v && mod&KMOD_CTRL)
		{
			if(mod&KMOD_CTRL)
			{
				char *text=getClipboardText();
				strGoingToChange();
				str.insert(cursorPos,text);
				cursorPos+=strlen(text);
				updateScreen=1;
				handled=1;
			}
		}
		else if(key==SDLK_z && mod&KMOD_CTRL)
		{
			if(mod&KMOD_CTRL)
			{
				if(lstr.size())
				{
					str=lstr.top();
					lstr.pop();
					cursorPos=lpos.top();
					lpos.pop();
					updateScreen=1;
				}
			}
		}
		else if((key<256 && isprint(key)))//unicode?
		{
			if(key==' ' || key=='\n')
				lastTextChangeTime=1000;
			strGoingToChange();
			str.insert(ITR cursorPos,(char)key);
			cursorPos++;
			updateScreen=1;
			handled=1;
		}
		else 
		{
			int oldCursorPos=cursorPos;
			switch(key)
			{
			case SDLK_DELETE:
				if(cursorPos!=str.size())
				{
					strGoingToChange();
					str.erase(ITR cursorPos);
				}
				handled=1;
				break;
			case SDLK_BACKSPACE:
				if(cursorPos!=0)
				{
					strGoingToChange();
					str.erase(ITR cursorPos-1);
					cursorPos--;
					handled=1;
				}
				break;
			case SDLK_LEFT:
				if(cursorPos!=0)
				{
					if(mod&KMOD_CTRL)
					{
						size_t pos=str.rfind(' ',cursorPos-1);
						if(pos!=string::npos)
							cursorPos=pos;
						else
							cursorPos=0;
					}
					else
						cursorPos--;
				}
				handled=1;
				break;
			case SDLK_RIGHT:
				if(cursorPos!=str.size())
				{
					if(mod&KMOD_CTRL)
					{
						size_t pos=str.find(' ',cursorPos+1);
						if(pos!=string::npos)
							cursorPos=pos;
						else
							cursorPos=str.size();
					}
					else
						cursorPos++;
				}
				handled=1;
				break;	
			case SDLK_UP:
				{
					int cx=-10000,cy;
					_drawTextWrapped(str.c_str(),x+4,y+4,w-8,settings::editorTextSize,cursorPos,cx,cy,255,255,255,screen);
					cx+=3;
					cy-=TTF_FontLineSkip(getFont(14));
					if(cy>y-4)
						cursorPos=_drawTextWrapped(str.c_str(),this->x+4,this->y+4,this->w-8,settings::editorTextSize,0,cx,cy,255,255,255,(SDL_Surface*)1);
					else
						cursorPos=0;
					handled=1;
					break;
				}
			case SDLK_DOWN:
				{
					int cx=-10000,cy;
					_drawTextWrapped(str.c_str(),x+4,y+4,w-8,settings::editorTextSize,cursorPos,cx,cy,255,255,255,screen);
					cx+=3;
					cy+=TTF_FontLineSkip(getFont(14));
					cursorPos=_drawTextWrapped(str.c_str(),this->x+4,this->y+4,this->w-8,settings::editorTextSize,0,cx,cy,255,255,255,(SDL_Surface*)1);
					handled=1;
					break;
				}
			case SDLK_HOME:
				cursorPos=0;
				handled=2;
				break;
			case SDLK_END:
				cursorPos=str.size();
				handled=2;
				break;
			case SDLK_RETURN:
				//if(!settings::tweetOnEnter)
				{
					strGoingToChange();
					str.insert(ITR cursorPos++,'\n');
					handled=1;
				}
				//else
				//	doTweet();
				break;
			}
			updateScreen=1;
			if(oldCursorPos!=cursorPos)
			{
				cursorBlink=10;
				if(cursorPos>str.size())
					cursorPos=str.size();
			}
		}
		if(handled==1)
		{
			holdTimer=30*(pressed!=2);
			heldMod=mod;
			heldKey=key;
		}
		if(handled)
			return 1;
	}
	else if(!pressed)
	{
		if(heldKey==key)
			heldKey=-1;
	}
	return 0;
}

void Textbox::strGoingToChange()
{
	if(lastTextChangeTime>200)
	{
		lstr.push(str);
		lpos.push(cursorPos);
		lastTextChangeTime=0;
	}
}
