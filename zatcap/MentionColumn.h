#pragma once
#include "column.h"
#include "zatcap.h"
class MentionColumn :
	public Column
{
public:
	string term;
	MentionColumn(string name,int w);
	~MentionColumn(void);

	virtual void newTweet( Item *tweet );

	virtual void draw();

};

