#pragma once
#include <string>
using namespace std;
class twitCurl;

/**
	Opens a twitter User Stream using the Streaming API

	\param twit Already logged in twitCurl
	\returns 1 for now
	\note currently doesnt return until disconnect
	\todo thread this
	*/
bool openUserStream(twitCurl *twit);
int openUserStream(void *twit);
string uploadPhoto(string path,string tweet="");
extern int streaming;