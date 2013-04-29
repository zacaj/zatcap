#pragma once
#include "column.h"
#include "zatcap.h"

class HomeColumn :
	public Column
{
public:
	HomeColumn(float w);
	virtual ~HomeColumn(void);
	virtual void newTweet( Item *tweet );

	virtual void draw();

};

