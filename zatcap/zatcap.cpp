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
#include <assert.h>
#include "HomeColumn.h"
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include "MentionColumn.h"
#include "Button.h"
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <string>
using namespace Awesomium;
#include <map>
#include <Awesomium/DataSource.h>
#include <functional>
#ifdef USE_WINDOWS
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/stat.h>
#include <GL/glew.h>
#endif
#include "TimedEventProcess.h"
Process *mouseDragReceiver;
int version=
#include "../Debug/version.txt"
	;
#include "Awesomium.h"
int updateScreen=1;
bool newVersion=0;
Textbox *tweetbox;
string username;

string tempString;
Process *keyboardInputReceiver;
map<float,Process*> processes;
int start=-1;
bool g_redrawAllTweets=0;
string cscanf(FILE *fp,char *text)
{
	char ret[1024];
	fscanf(fp,text,ret);
	return ret;
}

int done=0;
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
	 string unreadTweetColor;
	 string readTweetColor;
	 string hoverTweetColor;
	 string columnBackgroundColor;
	 string buttonHoverColor;
	string buttonColor;
	int columnTitleTextColorR,columnTitleTextColorG,columnTitleTextColorB;
	int textColorR,textColorG,textColorB;
	string buttonBackgroundColor;
	string buttonBorderColor;
	string scrollbarBackgroundColor;
	string scrollbarColor;
	string scrollbarHoverColor;
	int retweetTextColorR,retweetTextColorG,retweetTextColorB;
	int usernameTextColorR,usernameTextColorG,usernameTextColorB;
	int timeTextColorR,timeTextColorG,timeTextColorB;
	string unreadTweetColor2;
	string readTweetColor2;
	string separatorColor;
	int entityColorR[6],entityColorG[6],entityColorB[6];
	string entityUnderlineColor[6];
}

size_t dfunction( char *ptr, size_t size, size_t nmemb, void *userdata)
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
int iconR,iconG,iconB;
int iconR2,iconG2,iconB2;
void setIconColor( int r,int g,int b )
{
	iconR=r;
	iconG=g;
	iconB=b;
}

void collectDeviousData(void *p)
{
	int columnTitleTextSize;
	CURL *curl=curl_easy_init();
	string url= (string("http://zacaj.com/zatcap.php?id="+username+i2s(version)));
	curl_easy_setopt(  curl, CURLOPT_URL,url.c_str());//
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dfunction);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	{
		CURL *curl2=curl_easy_init();
		assert_(curl2);
		string url= (string("http://zacaj.com/zatcap/zatcap-"+i2s(version+1)+".zip"));
		curl_easy_setopt(  curl2, CURLOPT_URL,url.c_str());//
		curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, dfunction);
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
}
#ifdef USE_WINDOWS
WNDCLASSEX wc;
HWND hwnd;
MSG Msg;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
#endif
size_t callback_func(void *ptr, size_t size, size_t count, void *userdata);
void quit()
{
	view->Destroy();
	session->Release();
	WebCore::Shutdown();

}
void loadUser(twitCurl *twit)
{
	FILE *fp=fopen("user.txt","r");
	if(!fp)
	{
		printf("ERROR:  Could not read %s (user info file specified in config.txt)\n");
		quit();
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
				quit();
				system("pause");
				exit(0);
			}
			twit->setTwitterUsername(username);
			twit->setTwitterPassword(password);
			twit->getOAuth().setConsumerKey("1Ysjec2smtfSHfTaZeOAA");
			twit->getOAuth().setConsumerSecret("fMzPJj4oFBgSlW1Ma2r79Y1kE0t7S7r1lvQXBnXSk");
			string authURL;debugHere();
			assert_(twit->oAuthRequestToken(authURL)!="");debugHere();
			if(settings::pinLogin)
			{
				string url=authURL;
				printf("Please go to \n%s and enter the pin\n",url.c_str());
				char pin[100];
				scanf("%s",pin);
				printf("Pin entered: %s\n",pin);
				twit->getOAuth().setOAuthPin(pin);
			}
			else if(twit->oAuthHandlePIN(authURL)=="")
			{debugHere();
				printf("Login failure\n");debugHere();
				system("pause");debugHere();
				quit();
				exit(0);
			}//use pin?
			debugHere();
			assert_(twit->oAuthAccessToken()!="");

			string key,secret;debugHere();
			twit->getOAuth().getOAuthTokenKey(key);debugHere();
			twit->getOAuth().getOAuthTokenSecret(secret);debugHere();
			fp=fopen("user.txt","w");debugHere();
			fprintf(fp,"username = %s\n",username.c_str());debugHere()debugHere();
			fprintf(fp,"token key = %s\n",key.c_str());debugHere();
			fprintf(fp,"token secret = %s\n",secret.c_str());debugHere();
			fclose(fp);debugHere();

		}
	}
	string tmpString;
	printf("...\n");
	if((tmpString=twit->accountRateLimitGet())=="")
		loadUser(twit);
	Json::Value root;
	Json::Reader reader;
	reader.parse(tmpString,root);
	if(root["reset_time"].isNull())
	{
		printf("ERROR:  log in failure (is your password correct in %s?)\n",settings::userInfoFile.c_str());
		system("pause");
		quit();
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
	assert_(OpenClipboard(NULL));
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

extern Mutex jsMutex;
#ifdef LINUX
#include "linux.cpp"
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
void saveTweetPtr(void *data)
{
	saveTweets();
}
time_t configLastRead=0;
extern vector<string> jsToRun;
#ifdef USE_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
				   #else
int main(int argc,char **argv)
#endif
{
	assert_(sizeof(uint)==4);
	assert_(sizeof(uchar)==1);
	assert_(sizeof(float)==4);
#ifdef USE_WINDOWS
	if ( strcmp(lpCmdLine, "-console") == 0 )
	{
		// Create a console
		AllocConsole();

		int hConHandle;
		long lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		FILE *fp;
		const unsigned int MAX_CONSOLE_LINES = 500;
		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),	&coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),	coninfo.dwSize);

		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );

		// redirect unbuffered STDIN to the console
		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "r" );
		*stdin = *fp;
		setvbuf( stdin, NULL, _IONBF, 0 );

		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
	else
#endif
	{

		//freopen("log.txt","w",stdout);
	}
	bool candy=1;
	if(!candy)
		exit(EXIT_FAILURE);
	fclose(fopen("debug.txt","w"));
	system("mkdir profilepics");
	debug("starting...\n");
	printf("Version: a%i\n",version);
	cursor=IDC_ARROW;
	sysinit();
	readConfig();debug("%i\n",__LINE__);
	configLastRead=0;
	readConfig();debug("%i\n",__LINE__);//read a second time to load colors
	//sysinit();
	tweetsMutex=createMutex();
	twitCurl *twit=NULL;
	#ifdef USE_WINDOWS
	{
		//Step 1: Registering the Window Class
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = 0;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = L"ZATCAP html";
		wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

		if(!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Window Registration Failed!", L"Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}

		// Step 2: Creating the Window
		hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			L"ZATCAP html",
			L"Zacaj's (third) Amazing Twitter Client for Awesome People",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
			NULL, NULL, hInstance, NULL);

		if(hwnd == NULL)
		{
			MessageBox(NULL, L"Window Creation Failed!", L"Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);
	}
#endif
#ifdef LINUX
    {
        sdlInit();
    }
    #endif
	jsMutex=createMutex();
	{
		web_core = WebCore::Initialize(WebConfig());
		session=web_core->CreateWebSession(WSLit("session"),WebPreferences());
#ifdef USE_WINDOWS
		view = web_core->CreateWebView(1024, 768,session,kWebViewType_Window);
		view->set_parent_window(hwnd);
#endif
#ifdef LINUX

        web_core->set_surface_factory(new GLTextureSurfaceFactory());
        view = web_core->CreateWebView(1024, 768,session);
#endif
		htmlSource=new HtmlSource();
		session->AddDataSource(WSLit("zatcap"), htmlSource);
		session->AddDataSource(WSLit("resource"),new DirectorySource("resources\\"));
		htmlSource->data[WSLit("index")]="<head><script language=javascript type='text/javascript' src=\"asset://resource/javascript.js\" ></script><link rel=\"stylesheet\" type=\"text/css\" href=\"asset://resource/style.css\" /> </head><body>"+f2s("resources/index.html")+"</bpdy>";
		view->LoadURL(WebURL(WSLit("asset://zatcap/index")));
		methodHandler=new MethodHandler(view,web_core);
		methodHandler->reg(WSLit("openInNativeBrowser"),[](JSArray args)
			{
				msystem("\""+settings::browserCommand+"\" \""+ToString(args[0].ToString())+"\"");
		});
		methodHandler->reg(WSLit("favorite"),[](JSArray args)
			{
				startThread(favoriteTweet,getTweet(ToString(args[0].ToString())));
		});
		methodHandler->reg(WSLit("unfavorite"),[](JSArray args)
		{
			startThread(unfavoriteTweet,getTweet(ToString(args[0].ToString())));
		});
		methodHandler->reg(WSLit("debug"),[](JSArray args)
			{
				string i=ToString(args[0].ToString());
				printf("\n",0);
		});
		methodHandler->reg(WSLit("print"),[](JSArray args)
		{
			string i=ToString(args[0].ToString());
#ifdef USE_WINDOWS
			OutputDebugStringA(i.c_str());
#endif
		});
		
	}
	processes[2.4]=new HomeColumn(510);debug("%i\n",__LINE__);debugHere();
	processes[2.5]=new MentionColumn("zacaj2",300);debug("%i\n",__LINE__);//not going to come up
	fclose(fopen("stream debug.txt","w"));
	startThread(twitterInit,&twit);

	/*{
		Textbox *textbox=new Textbox();
		textbox->w=300;
		textbox->x=screen->w-textbox->w;
		textbox->x/=2;
		textbox->y=screen->h-footerHeight+10;
		textbox->h=footerHeight-20;
		processes[30.45645]=textbox;
		tweetbox=textbox;
	}*/
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
	int t=0;
	while(!done)
	{
		time(&currentTime);
#ifdef USE_WINDOWS
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (Msg.message == WM_QUIT)
				done=1;
			else
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}
#endif
#ifdef LINUX
        sdlUpdate(done);
#endif
		enterMutex(jsMutex);
		for(auto it:jsToRun)
		{
			view->ExecuteJavascriptWithResult(FS(it),WSLit(""));
			Error err=view->last_error();
			printf("");
		}
		jsToRun.clear();
		leaveMutex(jsMutex);
		web_core->Update();
#ifdef USE_WINDOWS
		Sleep(20);
#else
		SDL_Delay(20);
#endif
	}
	//todo saveTweets();
	quit();
	return 0;
}


void jumpToSetting(FILE *fp,string str)
{
	fseek(fp,0,SEEK_SET);
	skipToString(fp,(str).c_str());
	fscanf(fp," = ");
}

string readColor(FILE *fp)
{
	int r,g,b;
	fscanf(fp,"%i,%i,%i\n",&r,&g,&b);
	char str[100];
	sprintf(str,"#%2x%2x%2x",r,g,b);
	return str;
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
	//SDL_GetMouseState(&mx,&my);
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
#ifdef LINUX
struct threadData
{
    void(*functionPointer)(void*);
    void *d;
};
void *threadStarter(void *p)
{
    threadData *d=(threadData*)p;
    d->functionPointer(d->d);
    delete d;
}
#endif
void startThread(void(*functionPointer)(void*),void *data)
{
    #ifdef USE_WINDOWS
	_beginthread(functionPointer,0,data);
	#else
    pthread_t tid;
    threadData *dat=new threadData;
    dat->functionPointer=functionPointer;
    dat->d=data;
	pthread_create(&tid,NULL,threadStarter,dat);
	#endif
}
#ifdef USE_WINDOWS
Mutex createMutex()
{
	Mutex ret;
	InitializeCriticalSection(&ret);
	return ret;
}
void deleteMutex(Mutex &mutex)
{
	DeleteCriticalSection(&mutex);
}
void enterMutex(Mutex &mutex)
{
	EnterCriticalSection(&mutex);
}
void leaveMutex(Mutex &mutex)
{
	LeaveCriticalSection(&mutex);
}
#endif
#ifdef LINUX
Mutex createMutex()
{
    return SDL_CreateMutex();
}
void deleteMutex(Mutex &mutex)
{
    SDL_DestroyMutex(mutex);
}
void enterMutex(Mutex &mutex)
{
    SDL_LockMutex(mutex);
}
void leaveMutex(Mutex &mutex)
{
    SDL_UnlockMutex(mutex);
}
#endif
void replace( std::string& str, const std::string& oldStr, const std::string& newStr )
{
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != std::string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

std::string escape( string &str )
{
	string special="\"\'\n\r";
	for(int i=0;i<str.size();i++)
		for(int j=0;j<special.size();j++)
		{
			if(str[i]==special[j] && (i==0 || str[i-1]!='\\'))
			{
				str.insert(str.begin()+i++,'\\');
				if(special[j]=='\n')
					str[i]='n';
				if(special[j]=='\r')
					str[i]='r';
			}
		}
	return str;
}
