// zatcap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "twitcurl.h"
#include "curl.h"
#include <string>
#include <stdio.h>
#include "twitter.h"
#include "file.h"
#include "stream.h"
#include "zatcap.h"
#include "WaitIndicator.h"
#include <SDL.h>
#include <SDL_thread.h>
#include <assert.h>
#include "HomeColumn.h"
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include "MentionColumn.h"
#include <SDL_image.h>
#include <SDL_rotozoom.h>
#include "Button.h"
#include <SDL_syswm.h>
#include "Textbox.h"
#ifdef WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "TimedEventProcess.h"
extern SDL_Surface *defaultSmallUserPic;
extern SDL_Surface *defaultMediumUserPic;
Process *mouseDragReceiver;
int version=
#include "../Debug/version.txt"
	;
int updateScreen=1;
Uint8 *keystate;
bool newVersion=0;
Textbox *tweetbox;
string username;
SDL_Surface* arrowDown;
SDL_Surface* arrowUp;
SDL_Surface* favorite[3];
SDL_Surface* retweet[3];
SDL_Surface* reply[3];
SDL_Surface* deleteButton[3];
 SDL_Surface* refresh[2];
 SDL_Surface* top[2];
 SDL_Surface* convo[2];
SDL_Surface* tempSurface;
string tempString;
Process *keyboardInputReceiver;
map<float,Process*> processes;
map<int,TTF_Font *> fonts;
int start=-1;
/* XPM */
static const char *arrow[] = {
	/* width height num_colors chars_per_pixel */
	"    32    32        3            1",
	/* colors */
	"X c #000000",
	". c #ffffff",
	"  c None",
	/* pixels */
	"X                               ",
	"XX                              ",
	"X.X                             ",
	"X..X                            ",
	"X...X                           ",
	"X....X                          ",
	"X.....X                         ",
	"X......X                        ",
	"X.......X                       ",
	"X........X                      ",
	"X.....XXXXX                     ",
	"X..X..X                         ",
	"X.X X..X                        ",
	"XX  X..X                        ",
	"X    X..X                       ",
	"     X..X                       ",
	"      X..X                      ",
	"      X..X                      ",
	"       XX                       ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"0,0"
};
bool g_redrawAllTweets=0;
string cscanf(FILE *fp,char *text)
{
	char ret[1024];
	fscanf(fp,text,ret);
	return ret;
}

SDL_Surface *screen=NULL;
int quit=0;
namespace settings
{
	string userInfoFile="user.txt";
	int tweetsToLoadOnStartup=5;//800
	int tweetArchiveAge=0; //todo 7
	string defaultUserPicPath="defaultpic.png";
	int multipleRetweet=1;
	int retweeteePicSize=36;
	int retweeterPicSize=28;
	int arrowsOnScrollbar=1;
	int scrollSpeed=20;
	string dateFormat;
	int tweetBackgrounds;
	 int tweetOnEnter;
	 int addSFZ=1;
	int windowWidth=1024;
	int windowHeight=768;
	int separatorHeight=0;
	int enableStreaming=0;
	int backupTime=3600;
	string browserCommand="C:\\Users\\Zack\\AppData\\Local\\Google\\Chrome\\Application\\chrome.exe";
	int underlineLinksWhenHovered=1;
	int tempInt;
	int markReadAfterTweeting=0;
	int textSize=13,timeSize=13,columnTitleTextSize=25,userNameTextSize=15,retweetTextSize=12,editorTextSize=14;
	int pinLogin=0;
	int maxTweets=1000;
}
namespace colors
{
	 Uint32 unreadTweetColor;
	 Uint32 readTweetColor;
	 Uint32 hoverTweetColor;
	 Uint32 columnBackgroundColor;
	 Uint32 buttonHoverColor;
	Uint32 buttonColor;
	int columnTitleTextColorR,columnTitleTextColorG,columnTitleTextColorB;
	int textColorR,textColorG,textColorB;
	Uint32 buttonBackgroundColor;
	Uint32 buttonBorderColor;
	Uint32 scrollbarBackgroundColor;
	Uint32 scrollbarColor;
	Uint32 scrollbarHoverColor;
	int retweetTextColorR,retweetTextColorG,retweetTextColorB;
	int usernameTextColorR,usernameTextColorG,usernameTextColorB;
	int timeTextColorR,timeTextColorG,timeTextColorB;
	Uint32 unreadTweetColor2;
	Uint32 readTweetColor2;
	Uint32 separatorColor;
	int entityColorR[6],entityColorG[6],entityColorB[6];
	Uint32 entityUnderlineColor[6];
}

size_t function( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return size*nmemb;
}

void msystem( string str )
{

#ifdef USE_WINDOWS
	string cmd=string("cmd /C \"")+str+"\"";
#else
	string cmd=str;
#endif
	printf("Command to be run: \n%s\n\nOutput from command (empty if successful):\n\n",cmd.c_str());
	system(cmd.c_str());
	printf("\n<end of output>\n");
}

int collectDeviousData(void *p)
{
	int columnTitleTextSize;
	CURL *curl=curl_easy_init();
	string url= (string("http://zacaj.com/zatcap.php?id="+username));
	curl_easy_setopt(  curl, CURLOPT_URL,url.c_str());//
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	{
		CURL *curl2=curl_easy_init();
		assert(curl2);
		string url= (string("http://zacaj.com/zatcap/zatcap-"+i2s(version+1)+".zip"));
		curl_easy_setopt(  curl2, CURLOPT_URL,url.c_str());//
		curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, function);
		curl_easy_setopt(curl2, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl2, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl2, CURLOPT_FAILONERROR , 1);
		//curl_easy_setopt(  curl2, CURLOPT_VERBOSE, 1 );
		CURLcode res;
		res=curl_easy_perform(curl2);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		else
			newVersion=1;
		curl_easy_cleanup(curl2);
	}
	return 0;
}
SDL_mutex *tempSurfaceMutex;
size_t callback_func(void *ptr, size_t size, size_t count, void *userdata);
void loadUser(twitCurl *twit)
{
	FILE *fp=fopen("user.txt","r");
	if(!fp)
	{
		printf("ERROR:  Could not read %s (user info file specified in config.txt)\n");
		SDL_Quit();
		system("pause");
		exit(0);
	}
	else
	{
		skipToString(fp,"password");
		if(feof(fp))
		{
			fseek(fp,0,SEEK_SET);
			username=cscanf(fp,"username = %s\n");
			string key=cscanf(fp,"token key = %s\n");
			string secret=cscanf(fp,"token secret = %s\n");
			fclose(fp);

			twit->setTwitterUsername(username);
			twit->getOAuth().setConsumerKey("1Ysjec2smtfSHfTaZeOAA");
			twit->getOAuth().setConsumerSecret("fMzPJj4oFBgSlW1Ma2r79Y1kE0t7S7r1lvQXBnXSk");
			twit->getOAuth().setOAuthTokenKey(key);
			twit->getOAuth().setOAuthTokenSecret(secret);
			//fclose(fp);
		}
		else
		{
			string password="FUCKPASSWORDS";

			fseek(fp,0,SEEK_SET);
			username=cscanf(fp,"username = %s\n");
			password=cscanf(fp,"password = %s\n");debug("%i username: %s\n",__LINE__,username.c_str());
			debug("length %i,%i\n",username.size(),password.size());
		//	debug("%i pass: %s\n",__LINE__,password.c_str());
			if(!username.size() || !password.size())
			{
				printf("ERROR:  Could not read credentials from %s (user info file specified in config.txt)\n");
				SDL_Quit();
				system("pause");
				exit(0);
			}
			twit->setTwitterUsername(username);
			twit->setTwitterPassword(password);
			twit->getOAuth().setConsumerKey("1Ysjec2smtfSHfTaZeOAA");
			twit->getOAuth().setConsumerSecret("fMzPJj4oFBgSlW1Ma2r79Y1kE0t7S7r1lvQXBnXSk");
			string authURL;
			assert(twit->oAuthRequestToken(authURL)!="");
			if(settings::pinLogin)
			{
				string url;
				twit->oAuthRequestToken(url);
				printf("Please go to %s and enter the pin\n",url.c_str());
				char pin[100];
				scanf("%s",pin);
				printf("Pin entered: %s\n",pin);
				twit->getOAuth().setOAuthPin(pin);
			}
			else if(twit->oAuthHandlePIN(authURL)=="")
			{
				printf("Login failure\n");
				system("pause");
				exit(0);
			}//use pin?

			assert(twit->oAuthAccessToken()!="");

			string key,secret;
			twit->getOAuth().getOAuthTokenKey(key);
			twit->getOAuth().getOAuthTokenSecret(secret);
			fp=fopen("user.txt","w");
			fprintf(fp,"username = %s\n",username.c_str());
			fprintf(fp,"token key = %s\n",key.c_str());
			fprintf(fp,"token secret = %s\n",secret.c_str());
			fclose(fp);

		}
	}
	string tmpString;
	if((tmpString=twit->accountRateLimitGet())=="")
		loadUser(twit);
	Json::Value root;
	Json::Reader reader;
	reader.parse(tmpString,root);
	if(root["reset_time"].isNull())
	{
		printf("ERROR:  log in failure (is your password correct in %s?)\n",settings::userInfoFile.c_str());
		SDL_Quit();
		system("pause");
		exit(0);
	}
	printf("Successfully logged in to twitter account: %s\n",username.c_str());debugHere();
	{
		TimedEventProcess *timer=new TimedEventProcess;
		timer->data=NULL;
		timer->fn=collectDeviousData;
		timer->maxTicks=10000;
		timer->ticks=9999;
		processes[23478]=timer;
	}
}

void readConfig();
extern SDL_Surface *defaultUserPic;

int mousex=-10000,mousey,mousebutton;


time_t currentTime;
LPCWSTR cursor;

int footerHeight=100;

#ifdef USE_WINDOWS
void sysinit()
{
}

char* getClipboardText()
{
	assert(OpenClipboard(NULL));
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char * buffer = (char*)GlobalLock(hData);
		GlobalUnlock(hData);
		CloseClipboard();
		if (buffer != NULL)
		{
			int bytes = strlen(buffer);
			char *data = (char *) malloc(bytes+1);
			strcpy(data,buffer);
			return data;
		}
	}
	return "";
}

#endif
#ifdef LINUX
void SetCursor(int c)
{

}
int LoadCursor(int i,LPCWSTR s)
{

}
void sysinit()
{

}
char* getClipboardText()
{
    return "Not implemented yet, because fragmentation";
}
#endif
int saveTweetPtr(void *data)
{
	saveTweets();
	return 0;
}
static SDL_Cursor *init_system_cursor(const char *image[]);
time_t configLastRead=0;
int main(int argc, char* argv[])
{
	assert(sizeof(uint)==4);
	assert(sizeof(uchar)==1);
	assert(sizeof(float)==4);
	bool candy=1;
	if(!candy)
		exit(EXIT_FAILURE);
	fclose(fopen("debug.txt","w"));
	system("mkdir profilepics");
	debug("starting...\n");
	printf("Version: a%i\n",version);
	SDL_Init(SDL_INIT_EVERYTHING);debug("%i\n",__LINE__);
	cursor=IDC_ARROW;
	sysinit();
	readConfig();debug("%i\n",__LINE__);
	configLastRead=0;
	screen=SDL_SetVideoMode(settings::windowWidth,settings::windowHeight,32,SDL_SWSURFACE| SDL_RESIZABLE);debug("%i\n",__LINE__);
	readConfig();debug("%i\n",__LINE__);//read a second time to load colors
	//sysinit();
	tempSurface=SDL_CreateRGBSurface(SDL_SWSURFACE,3000,1000,32,screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
	twitCurl *twit=NULL;
	TTF_Init();debug("%i\n",__LINE__);
	SDL_WM_SetCaption( "Zacaj's Amazing Twitter Client for Awesome People", NULL );
	SDL_SetCursor(init_system_cursor(arrow));
	//SetCursor(LoadCursor(NULL, IDC_ARROW));
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);debug("%i\n",__LINE__);
	debug("%i %i\n",__LINE__,fopen(string("profilepics/"+settings::defaultUserPicPath).c_str(),"r"));
	defaultUserPic=loadImage("resources/"+settings::defaultUserPicPath);debug("%i defaultUserPic=%i\n",__LINE__,defaultUserPic);
	{
		defaultSmallUserPic=zoomSurface(defaultUserPic,(float)settings::retweeterPicSize/defaultUserPic->w,(float)settings::retweeterPicSize/defaultUserPic->h,1);debug("%i\n",__LINE__);
		defaultMediumUserPic=zoomSurface(defaultUserPic,(float)settings::retweeteePicSize/defaultUserPic->w,(float)settings::retweeteePicSize/defaultUserPic->h,1);debug("%i\n",__LINE__);

	}debug("%i\n",__LINE__);

	assert(arrowDown=IMG_Load("resources/arrowDown.PNG"));
	assert(arrowUp=IMG_Load("resources/arrowUp.PNG"));
	assert(favorite[0]=IMG_Load("resources/favorite.png"));
	assert(favorite[1]=IMG_Load("resources/favorite_hover.png"));
	assert(favorite[2]=IMG_Load("resources/favorite_on.png"));
	assert(retweet[0]=IMG_Load("resources/retweet.png"));
	assert(retweet[1]=IMG_Load("resources/retweet_hover.png"));
	assert(retweet[2]=IMG_Load("resources/retweet_on.png"));
	assert(reply[0]=IMG_Load("resources/reply.png"));
	assert(reply[1]=IMG_Load("resources/reply_hover.png"));
	assert(reply[2]=IMG_Load("resources/reply_large.png"));
	assert(deleteButton[0]=IMG_Load("resources/delete.png"));
	assert(deleteButton[1]=IMG_Load("resources/delete_hover.png"));
	assert(deleteButton[2]=IMG_Load("resources/delete_hover2.png"));
	assert(refresh[0]=IMG_Load("resources/refresh.png"));
	assert(refresh[1]=IMG_Load("resources/refresh_hover.png"));
	assert(top[0]=IMG_Load("resources/top.png"));
	assert(top[1]=IMG_Load("resources/top_hover.png"));
	assert(convo[0]=IMG_Load("resources/convo.png"));
	assert(convo[1]=IMG_Load("resources/convo_hover.png"));


	SDL_Thread *thread=SDL_CreateThread(twitterInit,&twit);debug("%i\n",__LINE__);
	processes[2.4]=new HomeColumn(510);debug("%i\n",__LINE__);debugHere();
	processes[2.5]=new MentionColumn("zacaj2",300);debug("%i\n",__LINE__);//not going to come up
	fclose(fopen("stream debug.txt","w"));
	fontMutex=SDL_CreateMutex();
	tempSurfaceMutex=SDL_CreateMutex();
	tweetsMutex=SDL_CreateMutex();

	{
		Textbox *textbox=new Textbox();
		textbox->w=300;
		textbox->x=screen->w-textbox->w;
		textbox->x/=2;
		textbox->y=screen->h-footerHeight+10;
		textbox->h=footerHeight-20;
		processes[30.45645]=textbox;
		tweetbox=textbox;
	}
	{
		if(settings::backupTime)
		{
			TimedEventProcess *timer=new TimedEventProcess;
			timer->data=NULL;
			timer->fn=saveTweetPtr;
			timer->maxTicks=settings::backupTime;
			processes[2345]=timer;
		}
		//atexit(saveTweets);
	}
	/*{
		Tweet *tweet=new Tweet;
		tweet->text="@zacaj2 support.amd.com/us/gpudownload… #hashtag blah @zacaj2 support.amd.com/us/gpudownload…";
		tweet->userid="533546294";
		tweet->id="201332280944369666";
		tweet->timeTweetedInSeconds=time(NULL);
		tweet->timeTweeted=*localtime(&tweet->timeTweetedInSeconds);
		{
			URLEntity *url=new URLEntity;
			url->displayUrl="support.amd.com/us/gpudownload…";
			url->text="support.amd.com/us/gpudownload…";
			url->realUrl="http://support.amd.com/us/gpudownload/windows/Pages/radeonaiw_vista64.aspx#1"	;
			url->start=64;
			url->end=97;
			tweet->entities.push_back(url);
		}
		{
			URLEntity *url=new URLEntity;
			url->displayUrl="support.amd.com/us/gpudownload…";
			url->text="support.amd.com/us/gpudownload…";
			url->realUrl="http://support.amd.com/us/gpudownload/windows/Pages/radeonaiw_vista64.aspx#1"	;
			url->start=8;
			url->end=41;
			tweet->entities.push_back(url);
		}
		addTweet(tweet);
	}/**/
	SDL_EnableUNICODE(1);//todo make these only enable when typing?
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
//	SDL_CreateCursor()
	int t=0;
	while(!quit)
	{
		//if(cursor!=IDC_ARROW)
		//	SetCursor(LoadCursor(NULL, cursor));

		time(&currentTime);
		if(mousex!=-10000)
		{
			//if(ev.button.y<screen->h-80) //todo no tweet bar yet
			{
				map<float,Process*>::reverse_iterator end=processes.rend();
				for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
					if(it->second->mouseButtonEvent(mousex,mousey,mousebutton,1))
						break;
			}
			mousex=-10000;
		}
		if(start!=-1)
		{
			t=SDL_GetTicks()-start;
			if(t>0 && t<17)
				SDL_Delay(17-t);
		}
		start=SDL_GetTicks();
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
			case SDL_KEYDOWN:
				{
					switch(ev.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						quit=1;//not permenant
						break;
					default:
						{
							if(keyboardInputReceiver==NULL)
							{
								//todo maybe if it actually comes up
							}
							else
							{
								if(ev.key.keysym.unicode>30)
									keyboardInputReceiver->keyboardEvent(ev.key.keysym.unicode,1,ev.key.keysym.mod);
								else
									keyboardInputReceiver->keyboardEvent(ev.key.keysym.sym,1,ev.key.keysym.mod);
							}
						}
						break;
					}
				}
				break;
			case SDL_KEYUP:
				if(keyboardInputReceiver==NULL)
				{
					//todo maybe if it actually comes up
				}
				else
				{
					if(ev.key.keysym.unicode)
						keyboardInputReceiver->keyboardEvent(ev.key.keysym.unicode,0,ev.key.keysym.mod);
					else
						keyboardInputReceiver->keyboardEvent(ev.key.keysym.sym,0,ev.key.keysym.mod);
				}
				break;
			case SDL_QUIT:
				quit=1;
				break;
			case SDL_VIDEORESIZE:
				screen=SDL_SetVideoMode(ev.resize.w,ev.resize.h,32,SDL_SWSURFACE| SDL_RESIZABLE);
				updateScreen=2;
				printf("Window resized to %ix%i\n",screen->w,screen->h);
				break;
			case SDL_ACTIVEEVENT:
				switch(ev.active.type)
				{
				case SDL_APPMOUSEFOCUS :
				case SDL_APPACTIVE :
					//debugHere();
					if(ev.active.gain)
						readConfig();
					//debugHere();
				break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(ev.button.button!=SDL_BUTTON_WHEELDOWN && ev.button.button!=SDL_BUTTON_WHEELUP)
				{
					mousex=ev.button.x;
					mousey=ev.button.y;
					mousebutton=ev.button.button;
					if(mousey>screen->h-footerHeight)//todo horrible hack
					{
						map<float,Process*>::reverse_iterator end=processes.rend();
						for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
							if(it->second->mouseButtonEvent(mousex,mousey,mousebutton,1))
							{
								mousex=-10000;
								break;
							}
					}
				}
				else
				{
					mousex=-10000;
					map<float,Process*>::reverse_iterator end=processes.rend();
					for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
						if(it->second->mouseButtonEvent(ev.button.x,ev.button.y,ev.button.button,1))
						{
							break;
						}
				}
				updateScreen=1;
				break;
			case SDL_MOUSEBUTTONUP:
				//if(ev.button.y<screen->h-80) //todo no tweet bar yet
				{
					ev.button.x-=columnHorizontalScroll;
					for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
						if(it->second->mouseButtonEvent(ev.button.x,ev.button.y,ev.button.button,0))
							break;
				}
				updateScreen=1;
				break;
			case SDL_MOUSEMOTION:
				{
					if(ev.motion.state==SDL_PRESSED)
					{
						if(mouseDragReceiver!=NULL)
							mouseDragReceiver->mouseButtonEvent(ev.motion.xrel,ev.motion.yrel,-1,1);
					}
					else
					{
						//ev.motion.x-=columnHorizontalScroll;
						for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
							if(it->second->mouseButtonEvent(ev.motion.x,ev.motion.y,-1,0))
								break;
					}
				}
				//updateScreen=1;
				break;
			}
		}
		keystate=SDL_GetKeyState(NULL);
		if(g_redrawAllTweets)
			g_redrawAllTweets=0;
		if(updateScreen || !rand()%100 || mousex!=-10000)//immediate mode gui done in draw often
		{
			SDL_FillRect(screen,0,0);
			columnHorizontalRenderAt=0;
			for (map<float,Process*>::iterator it=processes.begin();it!=processes.end();it++)//go in reverse so higher priority=first
			{
				it->second->draw();
			}
			if(textButton(screen->w-125,screen->h-40,"Force reload config.txt"))
			{
				configLastRead=0;
				readConfig();
			}
			if(textButton(screen->w-125,screen->h-20,"Redraw cached tweets"))
				g_redrawAllTweets=1;
			updateScreen--;
			if(newVersion)
				drawText("A new version is available, download at zacaj.com/zatcap/",5,screen->h-40,13);
		}
		for (map<float,Process*>::iterator it=processes.begin();it!=processes.end();it++)//go in reverse so higher priority=first
		{
			if(it->second->shouldRemove)
			{
				//processes.erase(processes.begin()+i);
				map<float,Process*>::iterator it2=it;
				it++;
				Process *process=it2->second;
				processes.erase(it2->first);//may invalidate it?
				delete process;
				if(it==processes.end())
					break;
				continue;
			}
			it->second->update();
		}
		if(streaming)
		{
			boxRGB(screen,0,0,3,3,rand(),rand(),rand(),255);
			streaming--;
		}
		{
			boxRGB(screen,screen->w-100,0,screen->w,30,0,0,0);
			char str[100];
			sprintf(str,"MSPF: %4i",t);
			drawTextr(str,screen->w,0,12);
		}
		SDL_Flip(screen);
	}
	//todo saveTweets();
	SDL_Quit();
	return 0;
}


void jumpToSetting(FILE *fp,string str)
{
	fseek(fp,0,SEEK_SET);
	skipToString(fp,(str).c_str());
	fscanf(fp," = ");
}

Uint32 readColor(FILE *fp)
{
	int r,g,b;
	fscanf(fp,"%i,%i,%i\n",&r,&g,&b);
	if(!screen)
		return 0;
	return SDL_MapRGB(screen->format,r,g,b);
}

void readConfig()
{
	struct stat st;//debug("%i\n",__LINE__);
	stat("config.txt",&st);
	if(st.st_mtime<=configLastRead)
		return;
	FILE *fp=fopen("config.txt","rb");//debugHere();
	using namespace settings;

	jumpToSetting(fp,"user info file");
	userInfoFile=cscanf(fp,"%s\n");
	jumpToSetting(fp,"tweets to load");
	fscanf(fp,"%i\n",&tweetsToLoadOnStartup);
	jumpToSetting(fp,"tweet archive age");
	fscanf(fp,"%i\n",&tweetArchiveAge);
	jumpToSetting(fp,"default profile pic");
	defaultUserPicPath=cscanf(fp,"%s\n");
	jumpToSetting(fp,"show retweet every time");
	fscanf(fp,"%i\n",&multipleRetweet);
	jumpToSetting(fp,"retweeter pic size");
	fscanf(fp,"%i\n",&retweeterPicSize);
	jumpToSetting(fp,"retweetee pic size");
	fscanf(fp,"%i\n",&settings::retweeteePicSize);
	jumpToSetting(fp,"arrows on scroll bar");
	fscanf(fp,"%i\n",&settings::arrowsOnScrollbar);
	jumpToSetting(fp,"scroll speed");
	fscanf(fp,"%i\n",&settings::scrollSpeed);
	jumpToSetting(fp,"date format");
	settings::dateFormat=readTo(fp,'\r');
	jumpToSetting(fp,"show tweet backgrounds");
	fscanf(fp,"%i\n",&settings::tweetBackgrounds);
	jumpToSetting(fp,"tweet on enter");
	fscanf(fp,"%i\n",&settings::tweetOnEnter);
	jumpToSetting(fp,"add sfz");
	fscanf(fp,"%i\n",&settings::addSFZ);
	jumpToSetting(fp,"window width");
	fscanf(fp,"%i\n",&settings::windowWidth);
	jumpToSetting(fp,"window height");
	fscanf(fp,"%i\n",&settings::windowHeight);
	jumpToSetting(fp,"enable streaming");
	fscanf(fp,"%i\n",&settings::enableStreaming);
	jumpToSetting(fp,"separator bar height");
	fscanf(fp,"%i\n",&settings::separatorHeight);
	jumpToSetting(fp,"backup tweet time");
	fscanf(fp,"%i\n",&settings::backupTime);
	jumpToSetting(fp,"browser command");
	settings::browserCommand=readTo(fp,'\r');
	jumpToSetting(fp,"show url underline only when hovering");
	fscanf(fp,"%i\n",&settings::underlineLinksWhenHovered);
	jumpToSetting(fp,"temp");
	fscanf(fp,"%i\n",&settings::tempInt);
	jumpToSetting(fp,"mark all tweets as read after tweeting");
	fscanf(fp,"%i\n",&settings::markReadAfterTweeting);
	jumpToSetting(fp,"pin login");
	fscanf(fp,"%i\n",&pinLogin);
	jumpToSetting(fp,"max tweets in ram");
	fscanf(fp,"%i\n",&maxTweets);

	using namespace colors;
	jumpToSetting(fp,"unread tweet color");
	unreadTweetColor=readColor(fp);
	jumpToSetting(fp,"unread tweet color 2");
	unreadTweetColor2=readColor(fp);
	jumpToSetting(fp,"read tweet color");
	readTweetColor=readColor(fp);
	jumpToSetting(fp,"read tweet color 2");
	readTweetColor2=readColor(fp);
	jumpToSetting(fp,"hover tweet color");
	hoverTweetColor=readColor(fp);
	jumpToSetting(fp,"column background color");
	columnBackgroundColor=readColor(fp);
	jumpToSetting(fp,"button hover color");
	buttonHoverColor=readColor(fp);
	jumpToSetting(fp,"button color");
	buttonColor=readColor(fp);
	jumpToSetting(fp,"column title text color");
	fscanf(fp,"%i,%i,%i\n",&columnTitleTextColorR,&columnTitleTextColorB,&columnTitleTextColorG);
	jumpToSetting(fp,"column title text size");
	fscanf(fp,"%i\n",&columnTitleTextSize);
	jumpToSetting(fp,"text color");
	fscanf(fp,"%i,%i,%i\n",&textColorR,&textColorB,&textColorG);
	jumpToSetting(fp,"text size");
	fscanf(fp,"%i\n",&textSize);
	jumpToSetting(fp,"button background color");
	buttonBackgroundColor=readColor(fp);
	jumpToSetting(fp,"button border color");
	buttonBorderColor=readColor(fp);
	jumpToSetting(fp,"scrollbar background color");
	scrollbarBackgroundColor=readColor(fp);
	jumpToSetting(fp,"scrollbar color");
	scrollbarColor=readColor(fp);
	jumpToSetting(fp,"scrollbar hover color");
	scrollbarHoverColor=readColor(fp);
	jumpToSetting(fp,"retweet text color");
	fscanf(fp,"%i,%i,%i\n",&retweetTextColorR,&retweetTextColorB,&retweetTextColorG);
	jumpToSetting(fp,"retweet text size");
	fscanf(fp,"%i\n",&retweetTextSize);
	jumpToSetting(fp,"username text color");
	fscanf(fp,"%i,%i,%i\n",&usernameTextColorR,&usernameTextColorB,&usernameTextColorG);
	jumpToSetting(fp,"username text size");
	fscanf(fp,"%i\n",&userNameTextSize);
	jumpToSetting(fp,"time text color");
	fscanf(fp,"%i,%i,%i\n",&timeTextColorR,&timeTextColorB,&timeTextColorG);
	jumpToSetting(fp,"time text size");
	fscanf(fp,"%i\n",&timeSize);
	jumpToSetting(fp,"editor text size");
	fscanf(fp,"%i\n",&editorTextSize);
	jumpToSetting(fp,"separator color");
	separatorColor=readColor(fp);

	jumpToSetting(fp,"url color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[0],&entityColorG[0],&entityColorB[0]);
	jumpToSetting(fp,"url hover color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[3],&entityColorG[3],&entityColorB[3]);
	jumpToSetting(fp,"user color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[1],&entityColorG[1],&entityColorB[1]);
	jumpToSetting(fp,"user hover color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[4],&entityColorG[4],&entityColorB[4]);
	jumpToSetting(fp,"hashtag color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[2],&entityColorG[2],&entityColorB[2]);
	jumpToSetting(fp,"hashtag hover color");
	fscanf(fp,"%i,%i,%i\n",&entityColorR[5],&entityColorG[5],&entityColorB[5]);

	jumpToSetting(fp,"url underline color");
	entityUnderlineColor[0]=readColor(fp);
	jumpToSetting(fp,"url underline hover color");
	entityUnderlineColor[3]=readColor(fp);
	jumpToSetting(fp,"user underline color");
	entityUnderlineColor[1]=readColor(fp);
	jumpToSetting(fp,"user underline hover color");
	entityUnderlineColor[4]=readColor(fp);
	jumpToSetting(fp,"hashtag underline color");
	entityUnderlineColor[2]=readColor(fp);
	jumpToSetting(fp,"hashtag underline hover color");
	entityUnderlineColor[5]=readColor(fp);

	fclose(fp);//debugHere();
	printf("Scanned config file\n");
	configLastRead=st.st_mtime;
	return;
/*
jumpToSetting(fp,"");
	fscanf(fp,"%i\n",&);
	=cscanf(fp,"%s\n");
	*/
}

struct tm convertTimeStringToTM( string str )
{
	struct tm tmm;
//	strptime(str.c_str(),"%a %b %d %R %z %Y",&tmm);
	char day[20],month[20];
	char date[20];
	sscanf(str.c_str(),"%s %s %s %u:%d:%d +0000 %d",day,month,date,&tmm.tm_hour,&tmm.tm_min,&tmm.tm_sec,&tmm.tm_year);
	tmm.tm_year-=1900;
	tmm.tm_hour++;//todo check this
	sscanf(date,"%d",&tmm.tm_mday);
	if(strcmp(day,"Sun")==0)
		tmm.tm_wday=0;
	else if(strcmp(day,"Mon")==0)
		tmm.tm_wday=1;
	else if(strcmp(day,"Tue")==0)
		tmm.tm_wday=2;
	else if(strcmp(day,"Wed")==0)
		tmm.tm_wday=3;
	else if(strcmp(day,"Thu")==0)
		tmm.tm_wday=4;
	else if(strcmp(day,"Fri")==0)
		tmm.tm_wday=5;
	else if(strcmp(day,"Sat")==0)
		tmm.tm_wday=6;
	else
		printf("cannot decipher day %s, please annoy zacaj!\n",day);

	if(strcmp(month,"Jan")==0)
		tmm.tm_mon=0;
	else if(strcmp(month,"Feb")==0)
		tmm.tm_mon=1;
	else if(strcmp(month,"Mar")==0)
		tmm.tm_mon=2;
	else if(strcmp(month,"Apr")==0)
		tmm.tm_mon=3;
	else if(strcmp(month,"May")==0)
		tmm.tm_mon=4;
	else if(strcmp(month,"Jun")==0)
		tmm.tm_mon=5;
	else if(strcmp(month,"Jul")==0)
		tmm.tm_mon=6;
	else if(strcmp(month,"Aug")==0)
		tmm.tm_mon=7;
	else if(strcmp(month,"Sep")==0)
		tmm.tm_mon=8;
	else if(strcmp(month,"Oct")==0)
		tmm.tm_mon=9;
	else if(strcmp(month,"Nov")==0)
		tmm.tm_mon=10;
	else if(strcmp(month,"Dec")==0)
		tmm.tm_mon=11;
	else
		printf("cannot decipher month %s, please annoy zacaj!\n",month);
	tmm.tm_isdst=-1;

	return tmm;
}

std::string i2s( int n )
{
	char t[50];
	sprintf(t,"%i",n);
	return string(t);
}

void debug(const char* msg, ...)
{
	va_list fmtargs;
	char buffer[2024];
	va_start(fmtargs, msg);
	vsnprintf(buffer, sizeof(buffer) - 1, msg, fmtargs);
	va_end(fmtargs);
	FILE *fp;
	fp=fopen("debug.txt","a");
	if(!fp)
		return;
	fprintf(fp,buffer);
	fclose(fp);

}

bool hoverButton( int x,int y,int w,int h )
{
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	if(mx>x && mx<x+w && my>y && my<y+h)
		return 1;
	return 0;
}

int doButton( int x,int y,int w,int h,int button)
{
	if(mousex==-10000)
		return 0;
	if(mousebutton==button && mousex>x && mousex<x+w && mousey>y && mousey<y+h)
	{
		mousex=-10000;
		return mousebutton;
	}
	return 0;
}
#ifdef LINUX
void mkdir(const char *str)
{
    mkdir(str,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
#endif
FILE * fopenf( string path,char *mode )
{
	string folder=path.substr(0,path.rfind('/'));
	mkdir(folder.c_str());
	return fopen(path.c_str(),mode);
}

bool fileExists( string path )
{
	FILE *fp=fopen(path.c_str(),"r");
	if(fp)
	{
		fclose(fp);
		return 1;
	}
	return 0;
}

string getFile(string path)
{
	int i;
	for(i=path.size()-1;i>=0;i--)
	{
		if(path[i]=='/' || path[i]=='\\')
			break;
	}
	if(i!=0)
	{
		i++;
		return(path.substr(i,path.length()-i));
	}
	else
		return path;
}
string getPath(string path)
{
	int i;
	for(i=path.size()-1;i>=0;i--)
	{
		if(path[i]=='/' || path[i]=='\\')
			break;
	}
	if(i!=0)
	{
		i++;
		return(path.substr(0,i));
	}
	else
		return path;
}

static SDL_Cursor *init_system_cursor(const char *image[])
{
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row ) {
		for ( col=0; col<32; ++col ) {
			if ( col % 8 ) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
			case 'X':
				data[i] |= 0x01;
				mask[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}