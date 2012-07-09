#include "zatcap.h"
#include <algorithm>
#include <SDL_image.h>
#include "twitter.h"
#include "utf8.h"
using namespace colors;
SDL_mutex *fontMutex;
inline TTF_Font* getFont(int size)//makes me cringe, make better somehow, possible alternative TTF_* method?
{
	SDL_LockMutex(fontMutex);
	map<int,TTF_Font *>::iterator it=fonts.find(size);
	if(it==fonts.end())
	{
		TTF_Font *font=TTF_OpenFont("resources/font.ttf",size);
		fonts.insert(pair<unsigned int,TTF_Font *>(size,font));
		SDL_UnlockMutex(fontMutex);
		assert(font);
		return font;
	}
	else
	{
		assert(it->second);
		SDL_UnlockMutex(fontMutex);
		return it->second;
	}
}

SDL_Surface* drawTexts(const char* string,int size,int fR, int fG, int fB)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	SDL_Color foregroundColor = { fR, fG, fB };
	SDL_Surface* textSurface =  TTF_RenderUTF8_Blended(getFont(size),string, foregroundColor);
	return textSurface;
}


int drawTextWrapped(const char* string, int _x, int _y,int w,int size,bool dontDraw, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
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
				//	pos++;
				s.erase(pos+1);
				break;
			}
			s=s.substr(0,pos);
			TTF_SizeUTF8(font,s.c_str(),&tw,&th);
		}
		size_t p=s.find_first_of('\n');
		if(p!=string::npos)
		{
			pos=p+1;
			s=s.substr(0,p);
		}
		if(!rest.size())
			break;

		if(!dontDraw && s.find_first_not_of(' ')!=string::npos)
			drawText(s.substr(s.find_first_not_of(' ')).c_str(),x,y,size,fR,fG,fB,_screen);
		y+=h;
		s=rest.substr(pos,rest.size()-pos+1);
		if(s.find_first_not_of(' ')!=string::npos)
			s=s.substr(s.find_first_not_of(' '));
	}
	return y;
}

int drawTextWrappedw(const char* string, int _x, int _y,int &w,int size,bool dontDraw, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	int maxWidth=0;
	if(w<20)
	{
		return 0;		
	}
	if(_screen==NULL)
		_screen=screen;
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
				//	pos++;
				s=s.substr(0,pos);
				break;
			}
			//s=s.substr(0,pos);
			s.erase(pos+1);
			TTF_SizeUTF8(font,s.c_str(),&tw,&th);
		}
		if(tw>maxWidth)
			maxWidth=tw;
		size_t p=s.find_first_of('\n');
		if(p!=string::npos)
		{
			pos=p+1;
			s=s.substr(0,p);
		}
		if(!rest.size())
			break;

		if(!dontDraw && s.find_first_not_of(' ')!=string::npos)
			drawText(s.substr(s.find_first_not_of(' ')).c_str(),x,y,size,fR,fG,fB,_screen);
		y+=h;
		s=rest.substr(pos,rest.size()-pos+1);
		if(s.find_first_not_of(' ')!=string::npos)
			s=s.substr(s.find_first_not_of(' '));
	}
	w=maxWidth;
	return y;
}

int drawTextWrappedc(const char* string, int _x, int _y,int w,int size, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
	int x=_x,y=_y;
	int tw,th;
	std::string str=string;
	TTF_Font *font=getFont(size);
	int h=TTF_FontLineSkip(font);
	std::string s=str;
	while(s.size())
	{
		TTF_SizeUTF8(font,s.c_str(),&tw,&th);
		size_t pos=s.size();
		std::string rest=s;
		while(tw>w)
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
				//	pos++;
				s=s.substr(0,pos);
				break;
			}
			TTF_SizeUTF8(font,s.c_str(),&tw,&th);
		}
		size_t p=s.find_first_of('\n');
		if(p!=string::npos)
		{
			pos=p+1;
			s=s.substr(0,p);
		}
		if(s.find_first_not_of(' ')!=string::npos)
			drawTextc(s.substr(s.find_first_not_of(' ')).c_str(),x,y,size,fR,fG,fB,_screen);
		y+=h;
		s=rest.substr(pos,rest.size()-pos+1);
		if(s.find_first_not_of(' ')!=string::npos)
			s=s.substr(s.find_first_not_of(' '));
	}
	return y;
}
int drawTextWrappedp(const char* string, int _x, int _y,int &w,int size,textPos *textPositions,int nPos,textPos *textCoordinates,int nCoord,vector<int> &widths, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(_screen==NULL)
		_screen=screen;
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	int maxWidth=0;
	int x=_x,y=_y;
	int tw,th;
	std::string str=string;
	TTF_Font *font=getFont(size);
	int h=TTF_FontLineSkip(font);
	std::string s=str;
	std::string rest;
	int linePos=0;
	int lineNumber=0;
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
					if(s[pos]<0)
					{
						while(s[pos]<0)
							pos-=1;
					//	pos++;
					}
					
					goto again;
				}
				s.erase(pos);
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
		if(tw>maxWidth)
			maxWidth=tw;
		widths.push_back(tw);
		size_t p=s.find_first_of('\n');
		if(p!=string::npos)
		{
			pos=p+1;
			s=s.substr(0,p);
		}/*
		if(s.size() && s.back()<0 && 0)
		{
			for(int i=0;i<nCoord;i++)
				if(textCoordinates[i].x==-10000)
				textCoordinates[i].pos--;
		}

		while(s.size() && s.back()<0)
		{
			rest.insert(rest.begin(),s.back());
			s.erase(s.size()-1);
			pos++;
			for(int i=0;i<nCoord;i++)
				textCoordinates[i].pos++;
		}*/
		if(!rest.size())
			break;
		if(((int)_screen)!=1)
			drawText(s.c_str(),x,y,size,fR,fG,fB,_screen);
		for(int i=0;i<nCoord;i++)
		{
			if(textCoordinates[i].pos>=0  && textCoordinates[i].pos<s.size() && textCoordinates[i].x==-10000)
			{
				if(textCoordinates[i].pos==s.size())
				{
					textCoordinates[i].x=_x;
					textCoordinates[i].y=y+h;
					textCoordinates[i].h=h*2;
					textCoordinates[i].line=lineNumber+1;
					textCoordinates[i].w=0;
				}
				else
				{
					std::string tempStr=s.substr(0,textCoordinates[i].pos);
					int lw=0,lh;
					TTF_SizeUTF8(font,tempStr.c_str(),&lw,&lh);
					textCoordinates[i].x=x+lw;
					textCoordinates[i].y=y;
					textCoordinates[i].h=lh;
					textCoordinates[i].w=s.size()-textCoordinates[i].pos;
					textCoordinates[i].line=lineNumber;
				}
			}
			else if(textCoordinates[i].x==-10000)
				textCoordinates[i].pos-=pos;
		}
		for(int i=0;i<nPos;i++)
		{
			if(textPositions[i].pos==-1 && textPositions[i].y>=y && textPositions[i].y<y+h && textPositions[i].x>=x && textPositions[i].x<x+w)//cursor clicked this line
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
					if(textPositions[i].x>=px && textPositions[i].x<px+lw)//this letter clicked
					{
						textPositions[i].pos=linePos+i;
					}
				}
				//cursor past end of line
				textPositions[i].pos=linePos+s.size()-1;
			}
		}
		linePos+=s.size();
		s=rest.substr(pos,rest.size()-pos);
		y+=h;
		lineNumber++;
		for(int i=0;i<nCoord;i++)
			if(textCoordinates[i].pos==0 && !s.size() && p!=string::npos)//fix for cursor being on empty line
			{
				textCoordinates[i].x=x;
				textCoordinates[i].y=y;
			}
	}
	for(int i=0;i<nCoord;i++)
	{
		if(textCoordinates[i].x==-10000)
		{
			textCoordinates[i].x=_x+widths[widths.size()-1];
			textCoordinates[i].y=y-h;
			textCoordinates[i].h=h/2;
			textCoordinates[i].line=lineNumber-1;
		}
	}
	w=maxWidth;
	return y;
}
void drawText(const char* string, int x, int y,int size, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
	SDL_Surface* textSurface =  drawTexts(string,size,fR,fG,fB);
	SDL_Rect textLocation = { x, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, _screen, &textLocation);
	SDL_FreeSurface(textSurface);
}
int drawTextc(const char* string, int x, int y,int size, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
	SDL_Surface* textSurface =  drawTexts(string,size,fR,fG,fB);
	SDL_Rect textLocation = { x-textSurface->w/2, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, _screen, &textLocation);
	SDL_FreeSurface(textSurface);
	return 0;
}
int drawTextr(const char* string, int x, int y,int size, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
	SDL_Surface* textSurface =  drawTexts(string,size,fR,fG,fB);
	SDL_Rect textLocation = { x-textSurface->w, y, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, _screen, &textLocation);
	int w=textSurface->w;
	SDL_FreeSurface(textSurface);
	return w;
}
int drawTextcc(const char* string, int x, int y,int size, int fR, int fG, int fB,SDL_Surface* _screen)
{
	if(fR==256)
		fR=colors::textColorR;
	if(fG==256)
		fG=colors::textColorG;
	if(fB==256)
		fB=colors::textColorB;
	if(_screen==NULL)
		_screen=screen;
	SDL_Surface* textSurface =  drawTexts(string,size,fR,fG,fB);
	SDL_Rect textLocation = { x-textSurface->w/2, y-textSurface->h/2, 0, 0 };
	SDL_BlitSurface(textSurface, NULL, _screen, &textLocation);
	SDL_FreeSurface(textSurface);
	return 0;
}
void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
{
	SDL_Rect srcRect;
	srcRect.x = srcX;
	srcRect.y = srcY;
	srcRect.w = width;
	srcRect.h = height;

	SDL_Rect dstRect;
	dstRect.x = dstX;
	dstRect.y = dstY;
	dstRect.w = width;
	dstRect.h = height;

	SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
}

string getExt(string path)
{
	size_t pos=path.rfind('.');
	size_t pos2=path.rfind('/');
	if(pos!=path.npos && pos>pos2)
		return(path.substr(pos+1,path.length()-pos-1));
	else
		return "";
}

SDL_Surface* loadImage(string path)
{
	string ext=getExt(path);debug("%i %s\n",__LINE__,path.c_str());
//	std::transform(ext.begin(),ext.end(),ext.begin(),tolower);
	if(ext=="bmp" || ext=="BMP")
		return SDL_LoadBMP(path.c_str());
	else
	//if(ext=="png" || ext=="jpg" || ext=="jpeg" || ext=="PNG" || ext=="JPG" || ext=="JPEG")//wish was cleaner...
	{//now dangerous
		int tries=0;
			tryagain:
		SDL_Surface* ret=IMG_Load(path.c_str());
		if(ret==NULL)
		{
			if(tries<10)//not sure why this is needed, but it is
				goto tryagain;
			debug("img_load error: %s\n",IMG_GetError());
			printf("img_load error: %s\n",IMG_GetError());
			return defaultUserPic;
		}
		return ret;
	}
	//else if(ext=="bmp" || ext=="BMP")
	//	return SDL_LoadBMP(path.c_str());
	//els
	printf("ERROR: Image format %s not supported!\n",ext.c_str());
	return NULL;
}

void boxRGB(SDL_Surface* screen,int x1,int y1,int x2,int y2,int r,int g,int b,int a)
{
	//boxRGBA(screen,x1,y1,x2,y2,r,g,b,255);
	//return;
	SDL_Rect rect;
	rect.x=x1;
	rect.y=y1;
	rect.w=x2-x1;
	rect.h=y2-y1;
	SDL_FillRect(screen,&rect,SDL_MapRGB(screen->format,r,g,b));
}
void boxColor(SDL_Surface* screen,int x1,int y1,int x2,int y2,Uint32 color)
{
	//boxRGBA(screen,x1,y1,x2,y2,r,g,b,255);
	//return;
	SDL_Rect rect;
	rect.x=x1;
	rect.y=y1;
	rect.w=x2-x1;
	rect.h=y2-y1;
	SDL_FillRect(screen,&rect,color);
}
void rectangleColor(SDL_Surface* screen,int x1,int y1,int x2,int y2,Uint32 color)
{
	SDL_Rect rect;
	rect.x=x1;
	rect.y=y1;
	rect.w=x2-x1;
	rect.h=1;
	SDL_FillRect(screen,&rect,color);
	rect.y=y2;
	SDL_FillRect(screen,&rect,color);
	rect.h=y2-y1;
	rect.w=1;
	rect.y=y1;
	SDL_FillRect(screen,&rect,color);
	rect.x=x2;
	SDL_FillRect(screen,&rect,color);
}

bool textButton(int x,int y,char *str)
{
	SDL_Surface *text=drawTexts(str,12,255,255,255);
	boxColor(screen,x,y,x+text->w+4,y+text->h+4,columnBackgroundColor);
	if(hoverButton(x,y,text->w+4,text->h+4))
		rectangleColor(screen,x,y,x+text->w+4,y+text->h+4,buttonHoverColor);
	else
		rectangleColor(screen,x,y,x+text->w+4,y+text->h+4,buttonColor);
	drawSprite(text,screen,0,0,x+2,y+2,text->w,text->h);
	return doButton(x,y,text->w+4,text->h+4);
}