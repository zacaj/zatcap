#pragma once

#include "twitcurl.h"
#include "curl.h"
#include <string>
#include <SDL_gfxPrimitives.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <json/reader.h>
#define VERSION "0.0001  " __TIMESTAMP__
using namespace std;
using namespace Json;
using namespace std;
//if !0, the app will close
extern int quit;

#define dprintf printf

#ifdef LINUX
#define LPCWSTR int
#define IDC_SIZEWE 1
#define IDC_ARROW 0
#endif

string cscanf(FILE *fp,char *text);
struct SDL_Surface;
extern SDL_Surface* tempSurface;
extern SDL_Surface *screen;
extern SDL_Surface* arrowDown;
extern SDL_Surface* arrowUp;

extern LPCWSTR cursor;

namespace settings
{
	extern string userInfoFile;
	extern int tweetsToLoadOnStartup;//800
	extern int tweetArchiveAge; //todo 7
	extern string defaultUserPicPath;
	extern int multipleRetweet;
	extern int retweeterPicSize;
	extern int retweeteePicSize;
	extern int arrowsOnScrollbar;
	extern int scrollSpeed;
	extern string dateFormat;
	extern int tweetBackgrounds;
	extern int tweetOnEnter;
	extern int addSFZ;
	extern int windowWidth;
	extern int windowHeight;
	extern int separatorHeight;
	extern int enableStreaming;
	extern int backupTime;
	extern string browserCommand;
	extern int underlineLinksWhenHovered;
	extern int tempInt;
	extern int markReadAfterTweeting;
	extern int textSize,timeSize,columnTitleTextSize,userNameTextSize,retweetTextSize,editorTextSize;
	extern int pinLogin;
}
namespace colors
{
	extern Uint32 unreadTweetColor;
	extern Uint32 readTweetColor;
	extern Uint32 hoverTweetColor;
	extern Uint32 columnBackgroundColor;
	extern Uint32 buttonHoverColor;
	extern Uint32 buttonColor;
	extern int columnTitleTextColorR,columnTitleTextColorG,columnTitleTextColorB;
	extern int textColorR,textColorG,textColorB;
	extern Uint32 buttonBackgroundColor;
	extern Uint32 buttonBorderColor;
	extern Uint32 scrollbarBackgroundColor;
	extern Uint32 scrollbarColor;
	extern Uint32 scrollbarHoverColor;
	extern int retweetTextColorR,retweetTextColorG,retweetTextColorB;
	extern int usernameTextColorR,usernameTextColorG,usernameTextColorB;
	extern int timeTextColorR,timeTextColorG,timeTextColorB;
	extern Uint32 unreadTweetColor2;
	extern Uint32 readTweetColor2;
	extern Uint32 separatorColor;
	extern int entityColorR[6];
	extern int entityColorG[6];
	extern int entityColorB[6];
	extern Uint32 entityUnderlineColor[6];
}

FILE *fopenf(string path,char *mode);

extern time_t currentTime;

void loadUser(twitCurl *twit);
extern Uint8 *keystate;
extern string tempString;
extern string username;

#define debugHere() debug("%s:%i\n",__FILE__,__LINE__);

class Process;
extern map<float,Process*> processes;
extern Process *mouseDragReceiver;
extern Process *keyboardInputReceiver;

void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height);
SDL_Surface* drawTexts(const char* string,int size,int fR, int fG, int fB);
void drawText(const char* string, int x, int y,int size, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextc(const char* string, int x, int y,int size, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextr(const char* string, int x, int y,int size, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextcc(const char* string, int x, int y,int size, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextWrapped(const char* string, int x, int y,int w,int size,bool dontDraw=0, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextWrappedw(const char* string, int x, int y,int &w,int size,bool dontDraw=0, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
int drawTextWrappedc(const char* string, int x, int y,int w,int size, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);
struct textPos  
{
	int pos;
	int x,y;
	int w,h;
	int line;
};
int drawTextWrappedp(const char* string, int x, int y,int &w,int size,textPos *textPositions,int nPos,textPos *textCoordinates,int nCoord,vector<int> &widths, int fR=256, int fG=256, int fB=256,SDL_Surface* _screen=NULL);

struct tm convertTimeStringToTM(string str);

SDL_Surface* loadImage(string path);

string i2s(int i);

extern map<int,TTF_Font *> fonts;

void debug(const char* msg, ...);

bool hoverButton(int x,int y,int w,int h);
int doButton(int x,int y,int w,int h,int button=SDL_BUTTON_LEFT );

extern SDL_Surface* favorite[3];
extern SDL_Surface* retweet[3];
extern SDL_Surface* reply[3];
extern SDL_Surface* deleteButton[3];
extern SDL_Surface* refresh[2];
extern SDL_Surface* top[2];
extern SDL_Surface* convo[2];

void boxRGB(SDL_Surface* screen,int x1,int y1,int x2,int y2,int r,int g,int b,int a=255);
void boxColor(SDL_Surface* screen,int x1,int y1,int x2,int y2,Uint32 color);
void rectangleColor(SDL_Surface* screen,int x1,int y1,int x2,int y2,Uint32 color);
extern int updateScreen;

extern int footerHeight;

bool textButton(int x,int y,char *str);

class Textbox;
extern Textbox *tweetbox;

char* getClipboardText();
extern int mousex,mousey;

bool fileExists(string path);

extern bool g_redrawAllTweets;

#define MOUSEPREPRESS -7

extern SDL_mutex *fontMutex;
extern SDL_mutex *tempSurfaceMutex;
string getFile(string path);
string getPath(string path);

extern int version;

void msystem(string str);