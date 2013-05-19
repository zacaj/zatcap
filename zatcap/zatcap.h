#pragma once

#include "twitcurl.h"
#include "curl.h"
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <stdio.h>
#include "json/reader.h"
#define VERSION "0.0001  " __TIMESTAMP__
using namespace std;
using namespace Json;
using namespace std;
//if !0, the app will close
extern int done;

#define dprint print

#ifdef LINUX
#include <SDL/SDL.h>
#define LPCWSTR int
#define IDC_SIZEWE 1
#define IDC_ARROW 0
#endif
#include <set>

string cscanf(FILE *fp,char *text);

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
	extern int maxTweets;
	extern string proxyServer;
	extern string proxyPort;
	extern string notificationSound;
}
namespace colors
{
	extern string unreadTweetColor;
	extern string readTweetColor;
	extern string hoverTweetColor;
	extern string columnBackgroundColor;
	extern string buttonHoverColor;
	extern string buttonColor;
	extern int columnTitleTextColorR,columnTitleTextColorG,columnTitleTextColorB;
	extern int textColorR,textColorG,textColorB;
	extern string buttonBackgroundColor;
	extern string buttonBorderColor;
	extern string scrollbarBackgroundColor;
	extern string scrollbarColor;
	extern string scrollbarHoverColor;
	extern int retweetTextColorR,retweetTextColorG,retweetTextColorB;
	extern int usernameTextColorR,usernameTextColorG,usernameTextColorB;
	extern int timeTextColorR,timeTextColorG,timeTextColorB;
	extern string unreadTweetColor2;
	extern string readTweetColor2;
	extern string separatorColor;
	extern int entityColorR[6];
	extern int entityColorG[6];
	extern int entityColorB[6];
	extern string entityUnderlineColor[6];
}

FILE *fopenf(string path,char *mode);

extern time_t currentTime;
#define assert_(a) a
void loadUser(twitCurl *twit);
extern string tempString;
extern string username;

#define debugHere() debug("%s:%i %s\n",__FILE__,__LINE__,__FUNCTION__);

class Process;
extern map<float,Process*> processes;
extern Process *mouseDragReceiver;
extern Process *keyboardInputReceiver;


struct tm convertTimeStringToTM(string str);

string i2s(int i);


void debug(const char* msg, ...);
void print(const char* msg, ...);
#define prints(s) print(s,0)

extern int footerHeight;


class Textbox;
extern Textbox *tweetbox;

char* getClipboardText();
extern int mousex,mousey;

bool fileExists(string path);

extern bool g_redrawAllTweets;

#define MOUSEPREPRESS -7

string getFile(string path);
string getPath(string path);

extern int version;

void msystem(string str);

void setIconColor(int r,int g,int b);
extern int iconR,iconG,iconB;

void startThread(void(*functionPointer)(void*),void *data);
void delayedThreadStart(void(*functionPointer)(void*),void *data,float delay);
void startLambdaThread(function<void ()> func);

#ifdef USE_WINDOWS
typedef CRITICAL_SECTION Mutex;
#endif
#ifdef LINUX
typedef SDL_mutex* Mutex;
#endif
Mutex createMutex();
void deleteMutex(Mutex &mutex);
void enterMutex(Mutex &mutex);
void leaveMutex(Mutex &mutex);
void replace(std::string& str, const std::string& oldStr, const std::string& newStr);
string escape(string str,bool forPrintf =false);
void doing(int i);
void notifyIcon(bool on);
string tolower( string str);
extern int nUnread;

extern map<string,time_t> mute;
extern set<string> followers;
void addFollower(string id);
