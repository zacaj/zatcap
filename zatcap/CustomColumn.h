#pragma once
#include "column.h"
class Rule
{
public:
	char add;
	virtual bool check(Item *item)=0;
};
class All:public Rule
{
public:
	All(char a,FILE *fp)
	{
		add=a;
		fscanf(fp,"\n");
	}
	bool check(Item *item)
	{
		return 1;
	}
	static string cmd(){return "all";}
};
class String:public Rule
{
public:
	string term;
	String(string str)
	{
		term=str;
	}
	String(char a,FILE *fp)
	{
		add=a;
		skipWhile(fp,' ');
		back(fp);
		char str[100];
		fscanf(fp,"%s\n",str);
		term=str;
	}
	bool check(Item *item)
	{
		return item->text.find(term)!=string::npos;
	}
	static string cmd(){return "str";}
};
class Type:public Rule
{
public:
	int n;
	Type(int _t)
	{
		n=_t;
		
	}
	Type(char a,FILE *fp)
	{
		add=a;
		fscanf(fp,"%i\n",&n);
	}
	bool check(Item *item)
	{
		return item->_type==n;
	}
	static string cmd(){return "type";}
};
class RT:public Rule
{
public:
	string user;
	RT(string _user)
	{
		user=_user;
		
	}
	RT(char a,FILE *fp)
	{
		add=a;
		skipWhile(fp,' ');
		back(fp);
		char str[100];
		fscanf(fp,"%s\n",str);
		user=str;
	}
	bool check(Item *item)
	{
		if(item->_type!=1)
			return 0;
		Retweet *tweet=(Retweet*)item;
		return tweet->user()->username==user;
	}
	static string cmd(){return "rt";}
};
class CustomColumn :
	public Column
{
public:
	vector<Rule *> rules;
	CustomColumn(string name,string path);
	~CustomColumn(void);

	virtual void newTweet( Item *tweet );

};

