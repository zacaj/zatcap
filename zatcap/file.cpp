#include "file.h"
#include "zatcap.h"

void skipToString(FILE *fp,const char *str)
{
	while(!feof(fp))
	{
		char c=fgetc(fp);
		if(c=='\n')
		{
			int i=0;
			while(str[i]!='\0')
			{
				char c2=fgetc(fp);
				if(c2!=str[i])
				{
					i=-1;
					break;
				}
				else
					i++;
			}
			if(i!=-1)
				return;
		}
	}
}
void skipTo(FILE *fp,char g)
{
	while(!feof(fp))
	{
		char c=fgetc(fp);
		if(c==g)
			return;
	}
}
void skipBackTo(FILE *fp,char g)
{
	while(!feof(fp))
	{
		fseek(fp,-2,SEEK_CUR);
		char c=fgetc(fp);
		if(c==g)
			return;
	}
}
char skipBackWhile(FILE *fp,char g)
{
	while(!feof(fp))
	{
		fseek(fp,-2,SEEK_CUR);
		char c=fgetc(fp);
		if(c==g)
			continue;
		return c;
	}
	return 0;
}
char skipWhile(FILE *fp,char g)
{
	while(!feof(fp))
	{
		char c=fgetc(fp);
		if(c==g)
			continue;
		return c;
	}
	return 0;
}
string readTo(FILE *fp,char g)
{
	string ret;
	while(!feof(fp))
	{
		char c=fgetc(fp);
		if(c==g)
		{
			break;
		}
		else
			ret+=c;
	}
	return ret;
}
#define WASCII
#ifndef WASCII
void wuchar( uchar n,FILE *fp )
{
	fwrite(&n,sizeof(uchar),1,fp);
}
void w_char( char n,FILE *fp )
{
	fwrite(&n,sizeof(char),1,fp);
}
void wfloat( float n,FILE *fp )
{
	fwrite(&n,sizeof(float),1,fp);
}

void wstr( string str,FILE *fp )
{
	wuint(str.size(),fp);
	for(int iLetter=0;iLetter<str.size();iLetter++)
		wuchar(str[iLetter],fp);
}

void wuint( uint n,FILE *fp )
{
	fwrite(&n,sizeof(uint),1,fp);
}


uchar ruchar( FILE *fp )
{
	uchar n;
	fread(&n,sizeof(uchar),1,fp);
	return n;
}
char rchar( FILE *fp )
{
	char n;
	fread(&n,sizeof(char),1,fp);
	return n;
}


uint ruint( FILE *fp )
{
	uint n;
	fread(&n,sizeof(uint),1,fp);
	return n;
}
float rfloat( FILE *fp )
{
	float n;
	fread(&n,sizeof(float),1,fp);
	return n;
}

std::string rstr( FILE *fp )
{
	uint l=ruint(fp);
	string r;
	for(int i=0;i<l;i++)
	{
		r.push_back(fgetc(fp));
	}
	return r;
}


uchar rid( FILE *fp )
{
	return ruchar(fp);
}

#else
void wuchar( uchar n,FILE *fp )
{
	fprintf(fp,"%i\n",(int)n);
}

void wfloat( float n,FILE *fp )
{
	fprintf(fp,"%f\n",n);
}

void wstr( string str,FILE *fp )
{
	wuint(str.size(),fp);
	fprintf(fp,"%s\n",str.c_str());
}

void wuint( uint n,FILE *fp )
{
	fprintf(fp,"%i\n",n);
}

uchar ruchar( FILE *fp )
{
	uint n;
	fscanf(fp,"%i\n",&n);
	return n;
}

uint ruint( FILE *fp )
{
	uint n;
	fscanf(fp,"%i\n",&n);
	return n;
}

float rfloat( FILE *fp )
{
	float n;
	fscanf(fp,"%f\n",&n);
	return n;
}

std::string rstr( FILE *fp )
{
	uint l=ruint(fp);
	string r;
	r.reserve(l);
	for(int i=0;i<l;i++)
	{
		r.push_back(fgetc(fp));
	}
	fgetc(fp);
	return r;
}

std::string f2s( string path )
{
	string ret;
	FILE *fp=fopen(path.c_str(),"rb");
	while(!feof(fp))
		ret.push_back(fgetc(fp));
	ret.erase(--ret.end());
	return ret;
}

void back( FILE *fp )
{
	fseek(fp,-1,SEEK_CUR);
}

#endif
