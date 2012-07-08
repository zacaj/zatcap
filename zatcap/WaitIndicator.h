#pragma once
#include "process.h"
#include "zatcap.h"

class WaitIndicator :
	public Process
{
public:
	string text;
	WaitIndicator(string _text);
	~WaitIndicator(void);

	virtual void update();
	virtual void draw();

};

