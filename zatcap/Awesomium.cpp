#include "stdafx.h"
#include "Awesomium.h"
#include "zatcap.h"
HtmlSource *htmlSource;
MethodHandler *methodHandler;
WebCore *web_core;
WebSession *session;
WebView *view=NULL;


HtmlSource::HtmlSource()
{
	data[WSLit("error")]="<h3>INVALID NAME</h3>";
}

void DirectorySource::OnRequest( int request_id, const WebString& name )
{
	FILE *fp;
	string n=ToString(name);
	string p=path+getFile(n);
	fp=fopen((p).c_str(),"rb");
	fseek(fp,0,SEEK_END);
	unsigned int len=ftell(fp);
	fseek(fp,0,SEEK_SET);
	unsigned char *data=new unsigned char[len];
	fread(data,len,1,fp);
	SendResponse(request_id,len,data,WSLit(("")));
	delete[] data;
	fclose(fp);
}

DirectorySource::DirectorySource( string _path )
{
	path=_path;
}

Awesomium::JSValue MethodHandler::OnMethodCallWithReturnValue( Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args )
{
//	throw std::exception("The method or operation is not implemented.");
	return JSValue(0);
}

void MethodHandler::OnMethodCall( Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args )
{
	auto it=funcs.find(method_name);
//	assert(it!=funcs.end(),"invalid funciton");
	it->second(args);
}
void MethodHandler::reg( WebString str,std::function<void (JSArray)> func )
{
	funcs[str]=func;
	obj.SetCustomMethod(str,0);
}

MethodHandler::MethodHandler( WebView *view,WebCore *core )
{
	view->set_js_method_handler(this);
	while (view->IsLoading())
		core->Update();
	obj=view->CreateGlobalJavascriptObject(WSLit("cpp")).ToObject();
}

WebString FS( string str )
{
	WebString ret=WebString::CreateFromUTF8(str.c_str(),str.length());
	int i=ret.length();
	return ret;
}
Mutex jsMutex;
vector<string> jsToRun;

void runJS( string js )
{
	enterMutex(jsMutex);
	jsToRun.push_back(js);
	leaveMutex(jsMutex);
}
