#include "CustomColumn.h"


CustomColumn::CustomColumn(string name,string path)
{
	FILE *fp=fopen(path.c_str(),"rb");
	fscanf(fp,"%i\n",&w);
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
	init(w,name);
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
	m_tweets[tweet->id]=tweet;
	add(tweet);
}
