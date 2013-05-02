#include "CustomColumn.h"


CustomColumn::CustomColumn(string name,string path):Column(0,name)
{
	FILE *fp=fopen(path.c_str(),"rb");
	while(!feof(fp))
	{
		skipWhile(fp,' ');
		back(fp);
		char add=fgetc(fp);
		skipWhile(fp,' ');
		back(fp);
		string type;
		while(!feof(fp))
		{
			char c=fgetc(fp);
			if(!isalnum(c))
				break;
			type.push_back(c);
		}
#define CHECK(a) if(type==a::cmd())	rules.push_back(new a(add,fp));
		CHECK(All);
		CHECK(Type);
		CHECK(RT);
		CHECK(String);
		CHECK(By);
		CHECK(Favorited);
	}
}


CustomColumn::~CustomColumn(void)
{
}

void CustomColumn::newTweet( Item *tweet )
{
	bool had=0;
	for(auto it:rules)
	{
		if(had && it->add!='-')
			continue;
		bool match=it->check(tweet);
		if(it->add=='-' && match)
			return;
		if(it->add=='+' && match)
			had=1;
	}
	if(!had)
		return;
	TweetInstance *instance=new TweetInstance(tweet,rw,onOff);
	m_tweets[tweet->id]=instance;
	add(tweet);
}
