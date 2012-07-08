#include "UserRetryProcess.h"
#include "twitter.h"


UserRetryProcess::UserRetryProcess( User *_user )
{
	user=_user;
	ticks=100;
}


UserRetryProcess::~UserRetryProcess(void)
{
}

extern twitCurl *twit;

int fixUser(void *ptr)
{
	UserRetryProcess *u=(UserRetryProcess*)ptr;
	while(!twit->userGet(u->user->id,true));
	twit->getLastWebResponse(tempString);

	Json::Reader reader;
	Json::Value root;
	if(reader.parse(tempString,root))
	{
		getUser(root);
		u->shouldRemove=1;
	}
	return 0;
}

void UserRetryProcess::update()
{
	if(ticks++>3600)
	{
		ticks=0;
		SDL_CreateThread(fixUser,this);
	}
}

void UserRetryProcess::newTweet( Tweet *tweet )
{
	
}

void UserRetryProcess::draw()
{

}
