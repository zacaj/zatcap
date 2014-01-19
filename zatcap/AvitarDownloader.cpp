#include "AvitarDownloader.h"
int loadProfilePic(void *ptr);

AvitarDownloader::AvitarDownloader(void)
{
	busy=0;
}


AvitarDownloader::~AvitarDownloader(void)
{
}

void AvitarDownloader::update()
{
	if(pics.size() && !busy)
	{
		SDL_CreateThread(loadProfilePic,pics[0]);
		busy=1;
	}
}

void AvitarDownloader::draw()
{
	if(pics.size())
	{
		char str[100];
		sprintf(str,"Downloading %s's avatar (%i remaining)",pics[0]->user->username.c_str(),pics.size());
		drawText(str,3,screen->h-20,12);
	}
}
