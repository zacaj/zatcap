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
class By:public Rule
{
public:
	string user;
	By(string _user)
	{
		user=_user;

	}
	By(char a,FILE *fp)
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
		switch(item->_type)
		{
		case 0:
			{
				Tweet *tweet=(Tweet*)item;
				return tweet->user()->username==user;
			}
		case 1:
			{
				Retweet *retweet=(Retweet*)item;
				return retweet->user()->username==user || retweet->retweetedBy->username==user;
			}
		default: return 0;
		}
	}
	static string cmd(){return "by";}
};
class Favorited:public Rule
{
public:
	Favorited()
	{
	}
	Favorited(char a,FILE *fp)
	{
		add=a;
		fscanf(fp,"\n");
	}
	bool check(Item *item)
	{
		if(item->_type>1)
			return 0;
		Tweet *tweet=(Tweet*)item;
		return tweet->favorited;
	}
	static string cmd(){return "fav";}
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

