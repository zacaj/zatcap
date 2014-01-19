#pragma once
#include "process.h"
#include "twitter.h"

struct profilepic
{
	SDL_Surface **img;
	string url;
	string name;
	User *user;
};
class AvitarDownloader :
	public Process
{
public:
	vector<profilepic*> pics;
	AvitarDownloader(void);
	~AvitarDownloader(void);

	virtual void update();

	virtual void draw();

	bool busy;
};

