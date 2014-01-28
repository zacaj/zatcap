// zatcap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "twitcurl.h"
#include "curl.h"
#include <string>
#include <stdio.h>
#include <thread>
#include "twitter.h"
#include "file.h"
#include "stream.h"
#include "zatcap.h"
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
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
#include "../DebugRelease/version.txt"
	;
#include "Awesomium.h"
#include <dirent.h>
#include "CustomColumn.h"
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
	string proxyServer;
	string proxyPort;
	string notificationSound;
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
	print("Command to be run: \n%s\n\nOutput from command (empty if successful):\n\n",cmd.c_str());
	system(cmd.c_str());
	print("\n<end of output>\n");
}
int iconR,iconG,iconB;
int iconR2,iconG2,iconB2;
void setIconColor( int r,int g,int b )
{
	iconR=r;
	iconG=g;
	iconB=b;
}
size_t urlfunction( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	string *str=(string*)userdata;
	str->append(ptr,size*nmemb);
	return size*nmemb;
}
int curl_debug_callback2(CURL *curl,curl_infotype infotype,char *data,size_t size,void *userptr);
string getSite(string url)
{
	CURL *curl=curl_easy_init();
	curl_easy_setopt(  curl, CURLOPT_URL,url.c_str());//

	if(settings::proxyServer!="none")
	{
		curl_easy_setopt( curl, CURLOPT_PROXY, (settings::proxyServer+":"+settings::proxyPort).c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
	}
	string *ret=new string();
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, urlfunction);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)ret);
	curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYPEER, 0);
	//curl_easy_setopt( curl, CURLOPT_DEBUGFUNCTION, curl_debug_callback2 );
	curl_easy_setopt(  curl, CURLOPT_HTTPGET, 1 );//
	curl_easy_setopt(  curl, CURLOPT_VERBOSE, 0 );
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); 
	//curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return *ret;
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
void addColumnButtons()
{
	for (map<float,Process*>::iterator it=processes.begin();it!=processes.end();it++)
	{
		if(it->second->isColumn())
		{
			Column *column=(Column*)it->second;
			string html=f2s("resources/columnbutton.html");
			replace(html,"$COLUMNNAME",column->columnName);
			runJS("document.getElementById('columnButtonContainer').appendChild(nodeFromHtml('"+escape(html)+"'));");
		}
	}
}
int w=-1;
#ifdef USE_WINDOWS
WNDCLASSEX wc;
HWND hwnd;
MSG Msg;
bool hasFocus=1;
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
	case WM_SIZE:
		{
			if(LOWORD(lParam)==0)
				break;
			if(LOWORD(lParam)<550 &&(w==-1 || w>=550))
			{
				runJS("tweetContent=document.getElementById('tweetbox').value;");
				runJS("document.getElementById('bottom').innerHTML='"+escape(f2s("resources/bottomnarrow.html"))+"';");
				runJS("document.getElementById('bottom').style.height='100px';");
				runJS("document.getElementById('columns').style.bottom='100px';");
				runJS("document.getElementById('columns').style.overflow='hidden';");
				runJS("	document.getElementById('tweetbox').addEventListener('keydown',tweetboxKeydown,true);");
				runJS("	document.getElementById('tweetbox').addEventListener('paste',tweetboxPaste,true);");
				runJS("document.getElementById('tweetbox').value=tweetContent;");
				runJS("hideColumns();");
				addColumnButtons();
			}
			else if(LOWORD(lParam)>550 && (w==-1 || w<=550))
			{
				runJS("tweetContent=document.getElementById('tweetbox').value;");
				runJS("document.getElementById('bottom').innerHTML='"+escape(f2s("resources/bottom.html"))+"';");
				runJS("document.getElementById('bottom').style.height='75px';");
				runJS("document.getElementById('columns').style.bottom='75px';");
				runJS("document.getElementById('columns').style.overflow='auto';");
				runJS("	document.getElementById('tweetbox').addEventListener('keydown',tweetboxKeydown,true);");
				runJS("	document.getElementById('tweetbox').addEventListener('paste',tweetboxPaste,true);");
				runJS("document.getElementById('tweetbox').value=tweetContent;");
				runJS("showColumns();");
			}
			if(view)
			view->Resize(LOWORD(lParam),HIWORD(lParam));
			w=LOWORD(lParam);
		}
		break;
	case WM_SETFOCUS:
		hasFocus=1;
		//printf("focus\n");
		notifyIcon(0);
		break ;

	case WM_KILLFOCUS:
		hasFocus=0;
		//printf("unfocus\n");
		break;

	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_ACTIVE )
		{
			//printf("active\n");
			hasFocus=1;
			notifyIcon(0);
		}
		else 
		{
			//printf("deactive\n");
			
			hasFocus=0;
		}
		//	puts( "I AM NOW INACTIVE." ) ;
		break ;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
void notifyIcon(bool on)
{
	FLASHWINFO info;
	info.cbSize = sizeof(info);
	info.hwnd = hwnd;
	if(on)
	{
		if(GetFocus()==hwnd || hasFocus)
		{
			return;
		}
#ifdef NDEBUG
		if(settings::notificationSound.size())
			PlaySoundA(settings::notificationSound.c_str(), NULL, SND_FILENAME|SND_ASYNC|SND_NOSTOP);
#endif
		alert=1;
		info.dwFlags = FLASHW_TRAY;
		info.dwTimeout = 0;
		info.uCount = 3000;
		//FlashWindowEx(&info);
	}
	else 
	{
		info.dwFlags = FLASHW_STOP;
		info.dwTimeout = 0;
		info.uCount = 3;
		//FlashWindowEx(&info);
		alert=0;
		//printf("stop\n");
	}
}
#else
void notifyIcon(bool on)
{
//todo
}
#endif

void saveMute() 
{
	FILE *fp=fopen("mute.txt","wb");
	for(auto it:mute)
	{
		fprintf(fp,"%s\n",it.first.c_str());
		fwrite(&it.second,sizeof(time_t),1,fp);
	}
	fclose(fp);
}

size_t callback_func(void *ptr, size_t size, size_t count, void *userdata);
void quit()
{
	saveMute();
	view->Destroy();
	session->Release();
	WebCore::Shutdown();

}
void loadUser(twitCurl *twit)
{
	FILE *fp=fopen("user.txt","r");
	if(!fp)
	{
		print("ERROR:  Could not read %s (user info file specified in config.txt)\n");
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
				print("ERROR:  Could not read credentials from %s (user info file specified in config.txt)\n");
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
			if(settings::pinLogin || 1)
			{
				string url=authURL;
				print("Please go to \n%s and enter the pin\n",url.c_str());
				char pin[100];
				scanf("%s",pin);
				print("Pin entered: %s\n",pin);
				twit->getOAuth().setOAuthPin(pin);
			}
			else if(twit->oAuthHandlePIN(authURL)=="")
			{debugHere();
				print("Login failure\n");debugHere();
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
	print("...\n");
	if((tmpString=twit->accountRateLimitGet())=="")
		loadUser(twit);
	if(tmpString.find("error")!=string::npos)
	{
		print("ERROR:  log in failure (is your password correct in %s?)\n",settings::userInfoFile.c_str());
		system("pause");
		quit();
		exit(0);
	}
	print("Successfully logged in to twitter account: %s\n",username.c_str());debugHere();
	{
		TimedEventProcess *timer=new TimedEventProcess;
		timer->data=NULL;
		timer->fn=collectDeviousData;
		timer->interval=60*10;
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
void saveMute() ;
void saveTweetPtr(void *data)
{
	saveTweets();
	saveMute();
}
time_t configLastRead=0;
struct tweetData
{
	string str;
	string replyId;
	string user;
	bool split;
	tweetData(string _str,string _r,string _u,bool _split)
	{
		str=_str;
		replyId=_r;
		user=_u;
		split=_split;
	}
};

void addUsername( string name ) 
{
	runJS("usernames.push('"+name+"');");
}
bool alert=0,alert2=0;;
int nUnread=0,nUnread2=0;
Mutex debugMutex;
map<int,bool> pendingTweets;
size_t removeLeadingTrailingSpaces( string &str )
{
	if(str.empty())
		return 0;
	size_t notSpace=str.find_first_not_of(' ');
	if(notSpace!=string::npos)
		str.erase(str.begin(),str.begin()+notSpace);
	while(str.back()==' ') str.erase(str.size()-1);
	if(notSpace==string::npos)
		return 0;
	return notSpace;
}
void sendTweet(void *_data)
{
	tweetData *data=(tweetData*)_data;
	string ostr=data->str;
	vector<string> tweets;
	if(ostr.size()<=140 || !data->split)
		tweets.push_back(ostr);
	else
	{
		string prefix="";
		{
			volatile int pos=0;
			if(ostr[0]=='@')
				while(pos!=string::npos)
				{
					if(ostr[pos]=='@')
					{
						pos=ostr.find(' ',pos);
						pos++;
					}
					else break;
				}
				prefix.append(ostr.substr(0,pos));
			int start=pos;
			while(pos<=ostr.size())
			{
				pos+=137-prefix.size();
				if(tweets.size()!=0)
					pos-=3;
				if(pos>ostr.size())
					pos=ostr.size()-1;
				else
				while(ostr[pos]!=' ' && pos>start)
					pos--;
				if(pos==start)
				{
					//tweets[tweets.size()-1].erase(tweets[tweets.size()-1].begin()+tweets[tweets.size()-1].size()-2,tweets[tweets.size()-1].end());
					break;
				}
				else
				{
					if(tweets.size())
					tweets.back()+="...";
				}
				string tweet=prefix;
				if(tweets.size()!=0)
					tweet+="...";
				string excerpt=ostr.substr(start,pos-start+1);
				removeLeadingTrailingSpaces(excerpt);
				tweet+=excerpt;
				tweets.push_back(tweet);
				start=pos;
			}
		}
	}
	for(int i=0;i<tweets.size();i++)
	{
		string str=tweets[i];
		time_t t=time(NULL);
		Activity *activity=new Activity(str,"cpp.stopTweet("+i2s(t)+");",data->user);
		pendingTweets[t]=0;
		addTweet((Item**)&activity);
		int nTries = 0;
		while (twit->statusUpdate(str, data->replyId) == "" && !pendingTweets[t]) if (nTries++>20) return;
		deleteTweet(activity->id);
		print("Tweet sent successfully: %s (%s,%s)\n",str.c_str(),data->replyId.c_str(),replyId.c_str());
	}
	delete data;
}
extern vector<string> jsToRun;
map<string,time_t> mute;
set<string> followers;
#ifdef USE_WINDOWS
HBITMAP icon[3];
BOOL SaveToFile(HBITMAP hBitmap3, const char* lpszFileName)
{   
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal,hOldPal2=NULL;
	hDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24; 
	GetObject(hBitmap3, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight =-Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount +31) & ~31) /8
		* Bitmap0.bmHeight; 
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{ 
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap3, 0, (UINT) Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFileA(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE; 

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
} 
HCURSOR CreateAlphaCursor(void)
{
	HDC hMemDC;
	DWORD dwWidth, dwHeight;
	BITMAPV5HEADER bi;
	HBITMAP hBitmap, hOldBitmap;
	void *lpBits;
	DWORD x,y;
	HCURSOR hAlphaCursor = NULL;

	dwWidth  = 32;  // width of cursor
	dwHeight = 32;  // height of cursor

	ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
	bi.bV5Size           = sizeof(BITMAPV5HEADER);
	bi.bV5Width           = dwWidth;
	bi.bV5Height          = dwHeight;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	// The following mask specification specifies a supported 32 BPP
	// alpha format for Windows XP.
	bi.bV5RedMask   =  0x00FF0000;
	bi.bV5GreenMask =  0x0000FF00;
	bi.bV5BlueMask  =  0x000000FF;
	bi.bV5AlphaMask =  0xFF000000; 

	HDC hdc;
	hdc = GetDC(NULL);
	char str[100];
	sprintf(str,"%i",nUnread);
	// Create the DIB section with an alpha channel.
	hBitmap = (HBITMAP)CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void **)&lpBits, NULL, (DWORD)0);//LoadImage(NULL,L"resources/icon.bmp",IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE|LR_LOADTRANSPARENT);//
	if(nUnread==0)
		GetDIBits(hdc,icon[0],0,32,lpBits,(BITMAPINFO *)&bi,DIB_RGB_COLORS);
	else if(nUnread<10)
		GetDIBits(hdc,icon[1],0,32,lpBits,(BITMAPINFO *)&bi,DIB_RGB_COLORS);
	else
		GetDIBits(hdc,icon[2],0,32,lpBits,(BITMAPINFO *)&bi,DIB_RGB_COLORS);
	hMemDC = CreateCompatibleDC(hdc);
	ReleaseDC(NULL,hdc);

	// Draw something on the DIB section.
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	//PatBlt(hMemDC,0,0,dwWidth,dwHeight,WHITENESS);
	if(alert)
	SetTextColor(hMemDC,RGB(255,0,0));
	else
	SetTextColor(hMemDC,RGB(255,255,255));
	SetBkMode(hMemDC,TRANSPARENT);
	if(nUnread>0)
	{
		
		TextOutA(hMemDC,1,17,str,strlen(str));
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);

	// Create an empty mask bitmap.
	HBITMAP hMonoBitmap = CreateBitmap(dwWidth,dwHeight,1,1,NULL);

	// Set the alpha values for each pixel in the cursor so that
	// the complete cursor is semi-transparent.
	/*DWORD *lpdwPixel;
	lpdwPixel = (DWORD *)lpBits;
	for (x=0;x<dwWidth;x++)
		for (y=0;y<dwHeight;y++)
		{
			// Clear the alpha bits
			*lpdwPixel &= 0x00FFFFFF;
			// Set the alpha bits to 0x9F (semi-transparent)
			*lpdwPixel |= 0x9F000000;
			lpdwPixel++;
		}*/

		ICONINFO ii;
		ii.fIcon = FALSE;  // Change fIcon to TRUE to create an alpha icon
		ii.xHotspot = 0;
		ii.yHotspot = 0;
		ii.hbmMask = hMonoBitmap;
		ii.hbmColor = hBitmap;

		// Create the alpha cursor with the alpha DIB section.
		hAlphaCursor = CreateIconIndirect(&ii);

		DeleteObject(hBitmap);          
		DeleteObject(hMonoBitmap); 

		return hAlphaCursor;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
				   #else
//#undef main
int main(int argc,char **argv)
#endif
{
	assert_(sizeof(uint)==4);
	assert_(sizeof(uchar)==1);
	assert_(sizeof(float)==4);
#ifdef USE_WINDOWS
#ifdef NDEBUG
	if ( strcmp(lpCmdLine, "-console") == 0 )
#else 
	if(1)
#endif
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

		HWND hwndC = GetConsoleWindow() ; 
#ifdef _NDEBUG
		ShowWindow(hwndC,SW_HIDE);
#endif
	}
	else
	if( strcmp(lpCmdLine, "-noredirect") != 0 )
#endif
	{
		//freopen("log.txt","w",stdout);
	}
	debugMutex=createMutex();
	bool candy=1;
	if(!candy)
		exit(EXIT_FAILURE);
	fclose(fopen("debug.txt","w"));
	fclose(fopen("log.txt","w"));
	debug("starting...\n");
	print("Version: a%i\n",version);
	cursor=IDC_ARROW;
	sysinit();
	readConfig();debug("%i\n",__LINE__);
	configLastRead=0;
	readConfig();debug("%i\n",__LINE__);//read a second time to load colors
	//sysinit();
	tweetsMutex=createMutex();
	jsMutex=createMutex();
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
			WS_EX_LEFT,
			L"ZATCAP html",
			L"Zacaj's (third) Amazing Twitter Client for Awesome People",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, settings::windowWidth, settings::windowHeight,
			NULL, NULL, hInstance, NULL);

		if(hwnd == NULL)
		{
			MessageBox(NULL, L"Window Creation Failed!", L"Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);
		icon[0]=(HBITMAP)LoadImage(NULL,L"resources/iconbig.bmp",IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE);
		icon[1]=(HBITMAP)LoadImage(NULL,L"resources/iconmed.bmp",IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE);
		icon[2]=(HBITMAP)LoadImage(NULL,L"resources/iconsmall.bmp",IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_LOADFROMFILE);
		{
			SendMessage(hwnd, WM_SETICON,
				ICON_BIG,(LPARAM)CreateAlphaCursor());
		}
		POINT pt;
		pt.x=0;
		pt.y=0;
		ClientToScreen(hwnd,&pt);
		RECT rect;
		GetWindowRect(hwnd,&rect);
		MoveWindow(hwnd,rect.left-(pt.x-rect.left),rect.top-(pt.y-rect.top),settings::windowWidth+(pt.x-rect.left)*2,settings::windowHeight+(pt.y-rect.top)+(pt.x-rect.left),0);
	}
#endif
#ifdef LINUX
    {
        sdlInit();
    }
    #endif
	{
		web_core = WebCore::Initialize(WebConfig());
		session=web_core->CreateWebSession(WSLit("session"),WebPreferences());
#ifdef USE_WINDOWS
		view = web_core->CreateWebView(settings::windowWidth, settings::windowHeight,session,kWebViewType_Window);
		view->set_parent_window(hwnd);
#endif
#ifdef LINUX

        web_core->set_surface_factory(new GLTextureSurfaceFactory());
        view = web_core->CreateWebView(1024, 768,session);
#endif
		htmlSource=new HtmlSource();
		session->AddDataSource(WSLit("zatcap"), htmlSource);
		session->AddDataSource(WSLit("resource"),new DirectorySource("resources//"));
		string index=f2s("resources/index.html");
		replace(index,"$BOTTOM",f2s("resources/bottom.html"));
		htmlSource->data[WSLit("index")]="<head><script language=javascript type='text/javascript' src=\"asset://resource/javascript.js\" ></script><link rel=\"stylesheet\" type=\"text/css\" href=\"asset://resource/style.css\" /> <script type=\"text/javascript\" src=\"asset://resource/selection_range.js\"></script>			<script type=\"text/javascript\" src=\"asset://resource/string_splitter.js\"></script>			<script type=\"text/javascript\" src=\"asset://resource/cursor_position.js\"></script><script type=\"text/javascript\" src=\"asset://resource/twitter-text.js\"></script></head><body onload=\"init();\" id='body'>"+index+"</body>";
		view->LoadURL(WebURL(WSLit("asset://zatcap/index")));
		runJS("init();");
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
		methodHandler->reg(WSLit("retweet"),[](JSArray args)
		{
			startThread(retweetTweet,getTweet(ToString(args[0].ToString())));
		});
		methodHandler->reg(WSLit("_delete"),[](JSArray args)
		{
			startThread(deleteTweet,getTweet(ToString(args[0].ToString())));
		});
		methodHandler->reg(WSLit("sendTweet"),[](JSArray args)
		{
			string tweet=ToString(args[0].ToString());
			string inReplyTo=ToString(args[1].ToString());
			startThread(sendTweet,new tweetData(tweet,inReplyTo,username,ToString(args[2].ToString())=="true"));
		});
		methodHandler->reg(WSLit("debug"),[](JSArray args)
			{
				string i=ToString(args[0].ToString());
				print("\n",0);
		});
		methodHandler->reg(WSLit("print"),[](JSArray args)
		{
			string i=ToString(args[0].ToString());
#ifdef USE_WINDOWS
			OutputDebugStringA(i.c_str());
#endif
		});
		methodHandler->reg(WSLit("handleImage"),[](JSArray args)
			{
				string ourl=ToString(args[0].ToString());
				string url=ourl;
				if(ourl.find("twitter.com")!=string::npos)
				{
					string src=getSite(ourl);
					size_t pos=src.find("<img class=\"large media-slideshow-image\" alt=\"\" src=\"");
					if(pos!=string::npos)
					{
						pos+=strlen("<img class=\"large media-slideshow-image\" alt=\"\" src=\"");
						int a=pos;
						url.clear();
						while(a<src.size() && src[a]!='\"') url.push_back(src[a++]);
					}
				}
				else if(ourl.find("twitpic.com")!=string::npos)
				{
					string src=getSite(ourl+"/full");
					size_t pos=src.find("Return to photo page</a></p");
					if(pos!=string::npos)
					{
						pos=src.find('\"',pos);
						pos++;
						int a=pos;
						url.clear();
						while(a<src.size() && src[a]!='\"') url.push_back(src[a++]);
					}
				}
				runJS("lightbox('"+url+"','"+url+"');");
		});
		methodHandler->reg(WSLit("refresh"),[](JSArray args)
		{
			startThread(refreshTweets,NULL);
		});
		methodHandler->reg(WSLit("source"),[](JSArray args)
		{
			FILE *fp=fopen("source.html","w");
			string source=ToString(args[0].ToString());
			replace(source,"asset://resource/","resources/");
			fwrite(source.c_str(),source.size(),1,fp);
			fclose(fp);
		});
		methodHandler->reg(WSLit("read"),[](JSArray args)
		{
			string id=ToString(args[0].ToString());
			Item *i=getTweet(id);
			if(i==NULL)
				return;
			auto it=tweets.begin();
			for(;it!=tweets.end();++it)
			{
				if(!it->second->read && it->second->timeTweetedInSeconds<=i->timeTweetedInSeconds)
				{
					it->second->read=1;
					nUnread--;
					for(auto c:it->second->instances)
					{
						runJS("document.getElementById('"+it->second->id+"_"+c->columnName+"bg').className='readtweetbg';");
					}
				}
			}
		});
		methodHandler->reg(WSLit("loadBackTill"),[](JSArray args)
		{
			string sHours=ToString(args[0].ToString());
			int hours=-1;
			sscanf(sHours.c_str(),"%i",&hours);
			startThread(loadBackTill,new int(hours));

		});
		methodHandler->reg(WSLit("mute"),[](JSArray args)
		{
			string str=ToString(args[0].ToString());
			string sHours=ToString(args[1].ToString());
			int hours=-1;
			sscanf(sHours.c_str(),"%i",&hours);
			time_t end=(uint)-1;
			int secs=hours*60*60;
			if(secs>0)
				 end=time(NULL)+secs;
			mute[str]=end;
			saveMute();
		});
		methodHandler->reg(WSLit("toggleConsole"),[](JSArray args)
		{
#ifdef WINDOWS
			HWND hwndC = GetConsoleWindow() ; 

		  ShowWindow(hwndC, IsWindowVisible(hwndC)?SW_HIDE:SW_SHOW);
#endif
		});
		methodHandler->reg(WSLit("stopTweet"),[](JSArray args)
		{
			int t=args[0].ToInteger();
			pendingTweets[t]=1;
		});
		methodHandler->reg(WSLit("setIdToTweetHtml"),[](JSArray args)
		{
			string htmlid=ToString(args[0].ToString());
			string tweetid=ToString(args[1].ToString());
			string parent=ToString(args[2].ToString());
			int n=args[3].ToInteger();
			startLambdaThread([=]()
			{
				Tweet *item=(Tweet*)getTweet(tweetid);
				string html=item->getHtml(parent);
				runJS("document.getElementById('"+htmlid+"').innerHTML='"+escape(html)+"';");
				runJS("document.getElementById('"+htmlid+"').style.display='block';");
				if(n-1>0 && item->replyTo.size())
				{
					JSArray a;
					a.Push(JSValue(FS(item->id+"_"+parent+"_replyShell")));
					a.Push(JSValue(FS(item->replyTo)));
					a.Push(JSValue(FS(item->id+"_"+parent+"_replyShell")));
					a.Push(JSValue(n-1));
					methodHandler->funcs[WSLit("setIdToTweetHtml")](a);
				}
			});
		});
		methodHandler->reg(WSLit("pasteImage"),[](JSArray args)
		{
			string htmlid=ToString(args[0].ToString());
#ifdef WINDOWS
			assert_(OpenClipboard(NULL));
			{
				HANDLE hData = GetClipboardData(CF_BITMAP);
				{
					Activity *activity=new Activity("Uploading photo...");
					addTweet((Item**)&activity);
					string file="t"+i2s(time(NULL))+".bmp";
					SaveToFile((HBITMAP)hData, file.c_str());
					startLambdaThread([=](){
					string url=uploadPhoto(file);
					runJS("insertText('"+escape(url)+"',"+htmlid+");");
					system((string("erase \"")+file+"\"").c_str());});
					deleteTweet(activity->id);
				}
				CloseClipboard();
			}
#endif
		});
		methodHandler->reg(WSLit("showUser"),[&](JSArray args)
		{
			string id=ToString(args[0].ToString());
			string url=ToString(args[1].ToString());
			runJS("popup('<img src=\"asset://resource/activity.gif\">','"+escape(url)+"');");
			startLambdaThread([=]()
			{
				string html=getUser(id)->getHtml();
				runJS("document.getElementById('popup_content').innerHTML='"+escape(html)+"';");
				string json=twit->timelineUserGet(false,true,200,id,true);
				Json::Reader reader;
				Json::Value root;
				reader.parse(json,root);
				assert(root.isArray());
				for(int i=0;i<root.size();i++)
				{
					Json::Value tweet=root[i];debugHere();
					if(tweet.isNull())
						continue;
					Tweet *t=processTweet(root[i]);debugHere();
					runJS("document.getElementById('userDisplayColumn').insertBefore(nodeFromHtml('"+escape(t->getHtml("userDisplayColumn"))+"'),null);");
				}
				//runJS("cpp.source(document.body.parentNode.outerHTML);");
			});
		});
		methodHandler->reg(WSLit("follow"),[&](JSArray args)
		{
			string id=ToString(args[0].ToString());
			int to=args[1].ToInteger();
			startLambdaThread([=]()
			{
				string id_=id;
				int to_=to;
				twitCurl *twit_=twit;
				if(to==-1)
				{
					string json;
					int nTries = 0;
					while ((json = twit->userGet(id_, true)) == "") if (nTries++>20) return;
					Json::Reader reader;
					Json::Value root;
					reader.parse(json,root);
					to_=!root["following"].asBool();
				}
				if(to_==1)
					twit->friendshipCreate(id_,true);
				else if(to_==0)
					twit->friendshipDestroy(id_,true);
				runJS((string("document.getElementById('followbutton').innerHTML='")+(!to_?"Follow":"Unfollow")+"';"));
			});
		});
	}
	/*processes[2.4]=new HomeColumn(510);debug("%i\n",__LINE__);debugHere();
	processes[2.5]=new MentionColumn("zacaj2",300);debug("%i\n",__LINE__);//not going to come up*/
	{
		DIR *pDIR;
		struct dirent *entry;
		if( pDIR=opendir("columns") ){
			while(entry = readdir(pDIR)){
			    int d_namlen;
			    #ifdef USE_WINDOWS
			    d_namlen=entry->d_namlen;
			    #else
			    d_namlen=strlen(entry->d_name);
			    #endif
				if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
				{
					struct stat s;
					if( stat((string("columns/")+entry->d_name).c_str(),&s) == 0 )
					{
						if( s.st_mode & S_IFREG )
						{
							float n=0;
							int a=0;
							while(a<d_namlen && entry->d_name[a]!='_')
							{
								n+=entry->d_name[a]-'0';
								n*=10;
								a++;
							}
							a++;
							string name;
							while(a<d_namlen)
								name.push_back(entry->d_name[a++]);
							name.erase(name.end()-4,name.end());
							processes[200+n]=new CustomColumn(name,(string("columns/")+entry->d_name));
						}
					}
				}
			}
			closedir(pDIR);
		}
	}
	fclose(fopen("stream debug.txt","w"));
	fclose(fopen("img debug.txt","w"));
	startThread(twitterInit,&twit);
	runJS("	document.getElementById('tweetbox').addEventListener('keydown',tweetboxKeydown,true);");
	//addTweet(new Favorite("lorem ipsum est luditorium con meguieligum son locos","3453245234624563456",))
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
			timer->interval=settings::backupTime;
			processes[2345]=timer;
		}
		//atexit(saveTweets);
	}
	{
		TimedEventProcess *timer=new TimedEventProcess;
		timer->data=NULL;
		timer->fn=refreshTweets;
		timer->interval=5*60;
		processes[2345]=timer;
	}
	bool notSet=1;
	int t=0;
	addColumnButtons();
	{
		FILE *fp=fopen("mute.txt","rb");
		if(fp)
		{
			while(!feof(fp))
			{
				char str[100];
				time_t i=-1;
				fscanf(fp,"%s\n",str);
				fread(&i,sizeof(time_t),1,fp);
				if(i!=-1)
					mute[str]=i;
			}
			fclose(fp);
		}
	}
	while(!done)
	{
		if(notSet && loggedIn)
		{

			while (view->IsLoading())
				web_core->Update();
			JSObject v=view->CreateGlobalJavascriptObject(WSLit("globals")).ToObject();
			v.SetProperty(WSLit("username"),JSValue(FS(username)));
			notSet=0;
		}
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
			print("");
		}
		jsToRun.clear();
		leaveMutex(jsMutex);
		web_core->Update();
		for (map<float,Process*>::reverse_iterator it=processes.rbegin();it!=processes.rend();it++)
			it->second->update();
		if(nUnread!=nUnread2 || alert2!=alert)
		{
#ifdef USE_WINDOWS
			SendMessage(hwnd, WM_SETICON,
				ICON_BIG,(LPARAM)CreateAlphaCursor());
#endif			
			nUnread2=nUnread;
			alert2=alert;
		}
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
    //printf("%i\n",ftell(fp));
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
	settings::browserCommand=readTo(fp,'\n');
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
    printf("%i\n",ftell(fp));
	jumpToSetting(fp,"proxy server");
    printf("%i\n",ftell(fp));
	settings::proxyServer=cscanf(fp,"%s\r");
	jumpToSetting(fp,"proxy port");
	settings::proxyPort=cscanf(fp,"%s\n");
    proxyServer="none";
   // exit(0);
	jumpToSetting(fp,"notification sound");
	settings::notificationSound=cscanf(fp,"%s\n");

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
	print("Scanned config file\n");
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
		print("cannot decipher day %s, please annoy zacaj!\n",day);

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
		print("cannot decipher month %s, please annoy zacaj!\n",month);
	tmm.tm_isdst=-1;

	return tmm;
}

std::string i2s(unsigned int n )
{
	char t[50];
	sprintf(t,"%u",n);
	return string(t);
}

void debug(const char* msg, ...)
{

	va_list fmtargs;
	char buffer[20024];
	va_start(fmtargs, msg);
	vsnprintf(buffer, sizeof(buffer) - 1, msg, fmtargs);
	va_end(fmtargs);
	FILE *fp;
	enterMutex(debugMutex);
	fp=fopen("debug.txt","a");
	if(!fp)
	{
		leaveMutex(debugMutex);
		return;
	}
	fprintf(fp,buffer);
	fclose(fp);
	leaveMutex(debugMutex);
}
void print(const char* msg, ...)
{
	va_list fmtargs;
	char buffer[2024];
	va_start(fmtargs, msg);
	vsnprintf(buffer, sizeof(buffer) - 1, msg, fmtargs);
	va_end(fmtargs);
	FILE *fp;
	fp=fopen("log.txt","a");
	if(!fp)
		return;
	fprintf(fp,buffer);
	fclose(fp);
	printf(buffer);
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

std::string escape( string str,bool forPrintf )
{
	string special="\"\'\n\r/";
	for(int i=0;i<str.size();i++)
	{
			if(str[i]=='%' && forPrintf)
				str.insert(str.begin()+i++,'%');
			else
			{
				int j=0;
				bool found=0;
				for(j=0;j<special.size();j++)
				{
					if(str[i]==special[j])
					{
							found=1;
							if(i==0 || str[i-1]!='\\')
							{
								str.insert(str.begin()+i++,'\\');
								if(special[j]=='\n')
									str[i]='n';
								if(special[j]=='\r')
									str[i]='r';
								break;
							}
					}
				}
				if(i!=0 && !found && str[i]!='n' && str[i]!='r' && str[i]!='\\'  && str[i-1]=='\\')
					str.insert(str.begin()+i++,'\\');
			}

	}
	if(str[str.size()-1]=='\\')
		str.insert(str.begin()+str.size()-1,'\\');
	return str;
}

int doin=0;
bool progress=0;
void doing( int i )
{
	doin+=i;
	if(doin>0  && !progress)
	{
		progress=1;
		runJS("document.getElementById('activity').style.display='inline';");
	}
	if(doin<=0 && progress)
	{
		progress=0;
		runJS("document.getElementById('activity').style.display='none';");
	}
}
void lambdaThreadStarter(void *data)
{
	function<void()> func=*(function<void()>*)data;
	func();
}
void startLambdaThread( function<void ()> func )
{
	startThread(lambdaThreadStarter,new function<void()>(func));
}

std::string tolower( string str )
{
	for(int i=0;i<str.size();i++)
		str[i]=tolower(str[i]);
	return str;
}

void addFollower( string id )
{
	followers.insert(id);
	runJS("addFollower('"+id+"');");
}

tm getTime()
{
	time_t t=time(NULL);
	return *localtime(&t);
}
