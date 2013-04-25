#pragma once

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include <string>
using namespace Awesomium;
#include <map>
#include <Awesomium/DataSource.h>
#include <functional>
using namespace std;
class HtmlSource : public DataSource
{
public:
	map<WebString,string> data;
	HtmlSource();

	virtual void OnRequest(int request_id,
		const WebString& path);
};
class DirectorySource:public DataSource
{
public:
	string path;
	DirectorySource(string _path);
	virtual void OnRequest(int request_id, const WebString& name);
};
class MethodHandler:public JSMethodHandler
{
public:
	JSObject obj;
	map<WebString,std::function<void (JSArray)> > funcs;
	MethodHandler(WebView *view,WebCore *core);
	void reg(WebString str,std::function<void (JSArray)>  func);
	virtual void OnMethodCall( Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args );

	virtual Awesomium::JSValue OnMethodCallWithReturnValue( Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args );

};

extern HtmlSource *htmlSource;
extern MethodHandler *methodHandler;
extern WebCore *web_core;
extern WebSession *session;
extern WebView *view;
WebString FS(string str);
void runJS(string js);