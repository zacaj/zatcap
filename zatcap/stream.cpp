#include "stream.h"
#include "zatcap.h"
#include "twitter.h"
#include "Tweet.h"
void addUsername(string str);
int streaming=0;
void parseStream(Json::Value root,string str)
{debugHere2();
debug("\nQ\n%s\nQ\n", str.c_str());
	if(!root["friends"].isNull() && root["friends"].isArray())//opening friends message
	{
		print("Streaming connection established.\n");debugHere2();
		for (int i = 0; i < root["friends"].size(); i++)
		{
			int ii = i;
			//print("%i\n", i);
			Json::Value v = root["friends"][i];
			User *u = getUser(i2s(v.asUInt()));
			if (u!=NULL)
				addUsername(u->username);
		}
		//todo figure out a use for this like loading initial friends list or should that be done using rest api since streaming api might not be used and new users should be handled anyways...
	}
	else if(!root["text"].isNull() && !root["user"].isNull() && !root["favorited"].isNull())//is a tweet?
	{
		//printf("new tweet from stream: %s\n", root["text"].asCString());
		processTweet(root);debugHere2();
	}
	else if(!root["direct_message"].isNull())
	{
		processTweet(root["direct_message"]);
	}
	/*else if(!root["limit"].isNull())//limit notice
	{
		root=root["limit"];debugHere2();

	}*/
	else if(!root["event"].isNull())//event
	{
		User *target=getUser(root["target"]);
		User *source=getUser(root["source"]);
		if (target == NULL || source == NULL)
			return;
		string type=root["event"].asString();
		struct tm otm=convertTimeStringToTM(root["created_at"].asString());
		time_t ott=mtimegm(&otm);
		struct tm timeTweeted=*localtime(&ott);
		if(type=="favorite" || type=="unfavorite")
		{
			if(source->username!=username)
			{
				Favorite *fav=new Favorite(root["target_object"]["text"].asString(),root["target_object"]["id_str"].asString(),source,type+"d",timeTweeted);
				addTweet((Item**)&fav);
			}
			else
			{
				Tweet *tweet=(Tweet*)getTweet(root["target_object"]["id_str"].asString());
				tweet->favorited=type=="favorite";
				addTweet((Item**)&tweet);
			}
		}
		if(type=="follow")
		{
			if(target->username==username)
			{
				Follow *fav=new Follow(source,timeTweeted);
				addTweet((Item**)&fav);
				addFollower(source->id);
			}
		}
	}
	else if(!root["delete"].isNull())//delete notice
	{
		deleteTweet(root["delete"]["status"]["id_str"].asString());debugHere2();
	}
	/*else if(!root["target"].isNull())//todo events
	{
		debugHere2();
	}
	else if(0)
	{

	}*/
/*	else if(root.isObject())//make sure to put this at end
	{
		//print("keep alive\n");
	}*/
	else
	{debugHere2();
	debug("new stream message:\n%s\n",str.c_str());
 		FILE *fp=fopen("unhandled.txt","a+");
		fprintf(fp,"couldnt handle (%s):\n%s\n\n\n",VERSION,str.c_str());
		print("\nERROR: Twitter message not handleable by ZATCAP, pleas email unhandled.txt to zatcap@zacaj.com!\n\n");
		fclose(fp);debugHere2();
	}debugHere2();

}
struct ParseStream
{
	Json::Value root;
	string str;
};
void parseStreamThread(void *ptr)
{
	ParseStream *str=(ParseStream*)ptr;
	parseStream(str->root,str->str);
	delete ptr;
}
string removeLeadingTrailing(string s,string not)
{
	if (s.size() == 0)
		return s;
	size_t notSpace = 0;
	while (not.find(s[notSpace]) != string::npos) notSpace++;
	if (notSpace != string::npos && notSpace != 0)
		s.erase(0, notSpace);
	while (1)
	{
		int i;
		if (not.find(s.back()) == string::npos)
			break;
		s.pop_back();
	}
	return s;
}
size_t callback_func(void *ptr, size_t size, size_t count, void *userdata)
{
	if(streaming<=0)
	{
		//startThread(loadMissingTweets,NULL);
	}

	streaming=60*45;debugHere2();
	string str = (char*)ptr;
	if (str.size() < 10)
		return size*count; debugHere2();
	FILE *fp=fopen("stream debug.txt","a+");
	if (fp)
	fprintf(fp,"Q%sQ\n\n\n\n\n\n\n\n",str.c_str());
	else
	{
		debug("fopen error: %s\n", strerror(errno));
	}
 	size_t pos=0,lpos=0;debugHere2();
	debug("\nT\n%s\nT\n", str.c_str());
	bool isEnd =removeLeadingTrailing(str, "\n\r ").back() == '}';
	size_t spos;
	/*while (true)
	{
		spos = str.rfind('}');
		if (spos == string::npos)
		{
			debugHere2();
			break;
		}
		if (spos >= str.size() - 2 || spos <=str.size()-4)
		{
			debugHere2();
			break;
		}
		if (str[spos + 1] != ',')
		{
			debugHere2();
			isEnd = true;
			break;
		}
		break;
	}*/
	//if(str.find('\r',lpos+1)!=string::npos)
	if (isEnd)
	{debugHere2();
	if (fp)
	fprintf(fp, "end\n"); 
	string str2 = *((string*)userdata);
	debug("\n2\n%s\n2\n", str2.c_str());
	str2.append(removeLeadingTrailing(str, "\n\r "));
	str = removeLeadingTrailing(str2, "\n\r ");
	debug("\nS\n%s\nS\n", str.c_str());
	str.push_back('\r');
		while((pos=str.find('\r',lpos+1))!=string::npos)
		{
			string s;debugHere2();
			if(lpos==-1)
				s=str;
			else
				s=str.substr(lpos,pos-lpos);
			if(s.length()<10)
			{debugHere2();
				lpos=pos;
				continue;
			}debugHere2();
			Json::Reader reader;
			Json::Value root;debugHere2();
			reader.parse(s,root);debugHere2();
			ParseStream *strm=new ParseStream;
			strm->root=root;
			strm->str=s;
			startThread(parseStreamThread,strm);//parseStream(root,s);debugHere2();
			lpos = pos; debugHere2();
		} //i will be so amazed if this actually works
		((string*)userdata)->clear(); debugHere2();
	}
	else//no carriage return->broken transmission
	{
		debugHere2();
		if (fp)
		fprintf(fp, "not end\n");
		if(str[str.size()-1]=='}' && 0)//end of a tweet
		{debugHere2();
			string str2=*((string*)userdata);
			str2.append(str);
			//fprintf(fp,"F%sF\n\n",str2.c_str());
			Json::Reader reader;
//			fflush(fp);
			Json::Value root;debugHere2();
			reader.parse(str2,root);debugHere2();
			ParseStream *strm=new ParseStream;
			strm->root=root;
			strm->str=str2;
			startThread(parseStreamThread,strm);
			//parseStream(root,str2);debugHere2();
			((string*)userdata)->clear();debugHere2();
		}
		else
		{debugHere2();
			string *str2=((string*)userdata);debugHere2();
			str2->append(removeLeadingTrailing(str,"\n\r "));debugHere2();
		}
	//	fprintf(fp,"LOOKHERE");
	}debugHere2();
	if (fp)
	fclose(fp);
	return size*count;
}

int openUserStream( void *twit ) //the fact that you need this really pisses me off
{
	return openUserStream((twitCurl*)twit);
}

int curl_debug_callback2(CURL *curl,curl_infotype infotype,char *data,size_t size,void *userptr)
{debugHere2();
	//print("%i:\n");
	//fwrite(data,size,1,stdout);
	//print("\n");
	FILE *fp=fopen("stream debug.txt","a+");
	if (!fp){
		debug("fopen error: %s\n", strerror(errno));
		return 0;
	}
	fprintf(fp,"%i:\n",size);
	fwrite(data,size,1,fp);
	fprintf(fp,"\n\n\n");
	fclose(fp);
	return 0;
}

void utilMakeCurlParams( std::string& outStr, std::string& inParam1, std::string& inParam2 );
int delay=1000;
bool openUserStream(twitCurl *twit)
{
	print("Opening a stream...\n");
	//return 0;
	/*if( !twit->isCurlInit() )
	{
		return false;
	}*/
	CURL *curl=curl_easy_init();
	std::string userNamePassword;
	utilMakeCurlParams( userNamePassword, twit->getTwitterUsername(), twit->getTwitterPassword() );//

	/* Set username and password */
	curl_easy_setopt( curl, CURLOPT_USERPWD, userNamePassword.c_str() );//
	string oAuthHttpHeader;
	struct curl_slist* pOAuthHeaderList = NULL;
	char *url="https://userstream.twitter.com/1.1/user.json?delimited=false&with=followings&stall_warnings=true";
	//char *url="https://stream.twitter.com/1/statuses/sample.json";
	tempString="";
	twit->m_oAuth.getOAuthHeader( eOAuthHttpGet,url, tempString, oAuthHttpHeader );//

	if( oAuthHttpHeader.length() )
	{
		pOAuthHeaderList = curl_slist_append( pOAuthHeaderList, oAuthHttpHeader.c_str() );//
		if( pOAuthHeaderList )
		{
			curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pOAuthHeaderList );//
		}
	}
	if(settings::proxyServer!="none")
	{
		curl_easy_setopt( curl, CURLOPT_PROXY, (settings::proxyServer+":"+settings::proxyPort).c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
		curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, NULL );
		curl_easy_setopt( curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY );
	}
	curl_easy_setopt(  curl, CURLOPT_HTTPGET, 1 );//
	curl_easy_setopt(  curl, CURLOPT_VERBOSE, 1 );
	//curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(  curl, CURLOPT_CONNECTTIMEOUT, 60);
	//curl_easy_setopt( curl, CURLOPT_CAINFO,"resources/cacert.pem");//
	curl_easy_setopt(  curl, CURLOPT_URL, url);//
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, callback_func);
	string *str=new string();
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)str);
	//curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, callback_func );
	curl_easy_setopt( curl, CURLOPT_DEBUGFUNCTION, curl_debug_callback2 );
	//todo errors
	if(CURLE_OK == curl_easy_perform(curl))
	{

	}
	streaming=0;
	/*curl_easy_setopt(  curl, CURLOPT_VERBOSE, 0 );
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, 0);
	curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, 0 );
	curl_easy_setopt( curl, CURLOPT_DEBUGFUNCTION, 0 );*/
	//twit->getLastCurlError(tempString);
	//print("Last curl error after perform? %s\n",tempString.c_str());
	//twit->getLastWebResponse(tempString);
	//print("Last web response: \n%s\n",tempString.c_str());
	//long l=87;
	//curl_easy_getinfo(curl,CURLINFO_HTTP_CONNECTCODE,&l);
	//print("curl: %i\n",l);
	print("ERROR\nERROR\nERROR\nERROR\nERROR:Stream connection lost, reconnecting after %ims wait...\nERROR\nERROR\nERROR\nERROR\n",delay);
	delete str;
	curl_easy_cleanup(curl);
	#ifdef USE_WINDOWS
	Sleep(delay);
	#else
	SDL_Delay(delay);
	#endif
	delay*=2;
	return openUserStream(twit);
}
int curl_debug_callback3(CURL *curl,curl_infotype infotype,char *data,size_t size,void *userptr)
{debugHere2();
//print("%i:\n");
//fwrite(data,size,1,stdout);
//print("\n");
FILE *fp=fopen("img debug.txt","a+");debugHere2();
fprintf(fp,"%i:\n");
fwrite(data,size,1,fp);
fprintf(fp,"\n\n\n");
fclose(fp);debugHere2();
return 0;
}
size_t img_callback_func(void *ptr, size_t size, size_t count, void *userdata)
{
	string *str=(string*)userdata;
	str->append((char*)ptr);
	return size*count;
}
string uploadPhoto(string path,string tweet)
{
doing(1);	
	CURL *curl=curl_easy_init();
	std::string userNamePassword;
	//utilMakeCurlParams( userNamePassword, twit->getTwitterUsername(), twit->getTwitterPassword() );//

	/* Set username and password */
	//curl_easy_setopt( curl, CURLOPT_USERPWD, userNamePassword.c_str() );//
	string oAuthHttpHeader;
	struct curl_slist* pOAuthHeaderList = NULL;
	char *url="http://api.twitpic.com/2/upload.json";
	tempString="";
	twit->m_oAuth.getOAuthHeader( eOAuthHttpGet,"https://api.twitter.com/1.1/account/verify_credentials.json", tempString, oAuthHttpHeader );//
	if( oAuthHttpHeader.length() )
	{
		oAuthHttpHeader.erase(oAuthHttpHeader.begin(),oAuthHttpHeader.begin()+21);
		string strr=("X-Verify-Credentials-Authorization: OAuth realm=\"http://api.twitter.com/\", "+oAuthHttpHeader);
		pOAuthHeaderList = curl_slist_append( pOAuthHeaderList, strr.c_str() );//
	}
	pOAuthHeaderList = curl_slist_append( pOAuthHeaderList, "X-Auth-Service-Provider: https://api.twitter.com/1.1/account/verify_credentials.json");
	if( pOAuthHeaderList )
	{
		curl_easy_setopt( curl, CURLOPT_HTTPHEADER, pOAuthHeaderList );//
	}
	if(settings::proxyServer!="none")
	{
		curl_easy_setopt( curl, CURLOPT_PROXY, (settings::proxyServer+":"+settings::proxyPort).c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
		curl_easy_setopt( curl, CURLOPT_PROXYUSERPWD, NULL );
		curl_easy_setopt( curl, CURLOPT_PROXYAUTH, (long)CURLAUTH_ANY );
	}
	curl_easy_setopt(  curl, CURLOPT_VERBOSE, 1 );
	//curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_easy_setopt(  curl, CURLOPT_SSL_VERIFYPEER, 0);
	//curl_easy_setopt( curl, CURLOPT_CAINFO,"resources/cacert.pem");//
	curl_easy_setopt(  curl, CURLOPT_URL, url);//
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, img_callback_func);
	string *str=new string();
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)str);
	//curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, callback_func );
	curl_easy_setopt( curl, CURLOPT_DEBUGFUNCTION, curl_debug_callback3 );
	curl_easy_setopt( curl, CURLOPT_TIMEOUT, curl_debug_callback3 );
	//todo errors

	struct curl_httppost* post = NULL;  struct curl_httppost* last = NULL; 

	/* Add simple name/content section */  curl_formadd(&post, &last, CURLFORM_COPYNAME, "key",   CURLFORM_COPYCONTENTS, "1145426acb9e63f5d615b6d97273075b", CURLFORM_END);
	/* Add simple name/content section */  curl_formadd(&post, &last, CURLFORM_COPYNAME, "message",   CURLFORM_COPYCONTENTS, tweet.c_str(), CURLFORM_END);


	/* Add simple file section */  curl_formadd(&post, &last, CURLFORM_COPYNAME, "media",   CURLFORM_FILE, path.c_str(), CURLFORM_END);

	 /* Add the content of a file as a normal post text value */ // curl_formadd(&post, &last, CURLFORM_COPYNAME, "filecontent",   CURLFORM_FILECONTENT, ".bashrc", CURLFORM_END);  /* Set the form info */  
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

	if(CURLE_OK == curl_easy_perform(curl))
	{
		curl_formfree(post);
		curl_easy_cleanup(curl);
		Json::Reader reader;
		Json::Value root;
		reader.parse(*str,root);
		doing(-1);
		return root["url"].asString();
	}
	doing(-1);
	return "";
}