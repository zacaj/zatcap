#pragma once
#include "process.h"
#include <string>
#include <stack>
using namespace std;
class Textbox :
	public Process
{
public:
	int x,y,w,h;
	int cursorBlink;
	int cursorPos;	
	int holdTimer;
	int heldKey,heldMod;
	int scroll;
	string str;
	int lastTextChangeTime;
	stack<string> lstr;//for undo
	stack<int> lpos;//for undo
	Textbox(void);
	virtual ~Textbox(void);

	virtual void update();

	virtual bool mouseButtonEvent( int x,int y,int button,int pressed );

	virtual void draw();

	void doTweet();
	void strGoingToChange();

	virtual bool keyboardEvent( int key,int pressed,int mod );
};

