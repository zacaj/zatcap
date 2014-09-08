#ifndef _TWITCURL_H_
#define _TWITCURL_H_

#include <string>
#include <sstream>
#include <cstring>
#include "oauthlib.h"
#include "curl/curl.h"

namespace twitCurlTypes
{
    typedef enum _eTwitCurlApiFormatType
    {
        eTwitCurlApiFormatXml = 0,
        eTwitCurlApiFormatJson,
        eTwitCurlApiFormatMax
    } eTwitCurlApiFormatType;
};

/* Default values used in twitcurl */
namespace twitCurlDefaults
{
    /* Constants */
    const int TWITCURL_DEFAULT_BUFFSIZE = 1024;
    const std::string TWITCURL_COLON = ":";
    const char TWITCURL_EOS = '\0';
    const unsigned int MAX_TIMELINE_TWEET_COUNT = 200;

    /* Miscellaneous data used to build twitter URLs*/
    const std::string TWITCURL_STATUSSTRING = "status=";
    const std::string TWITCURL_TEXTSTRING = "text=";
    const std::string TWITCURL_QUERYSTRING = "query=";  
    const std::string TWITCURL_SEARCHQUERYSTRING = "q=";      
    const std::string TWITCURL_SCREENNAME = "screen_name=";
    const std::string TWITCURL_USERID = "user_id=";
    const std::string TWITCURL_EXTENSIONFORMATS[2] = { ".xml",
                                                       ".json"
                                                     };
    const std::string TWITCURL_TARGETSCREENNAME = "target_screen_name=";
    const std::string TWITCURL_TARGETUSERID = "target_id=";
    const std::string TWITCURL_SINCEID = "since_id=";
    const std::string TWITCURL_MAXID = "max_id=";
    const std::string TWITCURL_TRIMUSER = "trim_user=1";
    const std::string TWITCURL_INCRETWEETS = "include_rts=1";
    const std::string TWITCURL_INCLUDEENTITIES= "include_entities=true";
    const std::string TWITCURL_COUNT = "count=";

    /* URL separators */
    const std::string TWITCURL_URL_SEP_AMP = "&";
    const std::string TWITCURL_URL_SEP_QUES = "?";
};
struct twitInstance;
/* Default twitter URLs */
namespace twitterDefaults
{

    /* Search URLs */
    const std::string TWITCURL_SEARCH_URL = "http://search.twitter.com/search";

    /* Status URLs */
    const std::string TWITCURL_STATUSUPDATE_URL = "https://api.twitter.com/1.1/statuses/update";
    const std::string TWITCURL_STATUSSHOW_URL = "https://api.twitter.com/1.1/statuses/show";
    const std::string TWITCURL_STATUDESTROY_URL = "https://api.twitter.com/1.1/statuses/destroy/";

    /* Timeline URLs */
    const std::string TWITCURL_HOME_TIMELINE_URL = "https://api.twitter.com/1.1/statuses/home_timeline";
    const std::string TWITCURL_PUBLIC_TIMELINE_URL = "http://api.twitter.com/1/statuses/public_timeline";
    const std::string TWITCURL_FEATURED_USERS_URL = "http://api.twitter.com/1/statuses/featured";
    const std::string TWITCURL_FRIENDS_TIMELINE_URL = "http://api.twitter.com/1/statuses/friends_timeline";
    const std::string TWITCURL_MENTIONS_URL = "https://api.twitter.com/1.1/statuses/mentions_timeline";
    const std::string TWITCURL_USERTIMELINE_URL = "https://api.twitter.com/1.1/statuses/user_timeline";
    const std::string TWITCURL_RETWEETTIMELINE_URL = "https://api.twitter.com/1.1/statuses/retweets_of_me";

    /* Users URLs */
    const std::string TWITCURL_SHOWUSERS_URL = "https://api.twitter.com/1.1/users/show";
    const std::string TWITCURL_SHOWFRIENDS_URL = "https://api.twitter.com/1.1/friends/ids";
    const std::string TWITCURL_SHOWFOLLOWERS_URL = "https://api.twitter.com/1.1/followers/ids";

    /* Direct messages URLs */
    const std::string TWITCURL_DIRECTMESSAGES_URL = "https://api.twitter.com/1.1/direct_messages";
    const std::string TWITCURL_DIRECTMESSAGENEW_URL = "https://api.twitter.com/1.1/direct_messages/new";
    const std::string TWITCURL_DIRECTMESSAGESSENT_URL = "https://api.twitter.com/1.1/direct_messages/sent";
    const std::string TWITCURL_DIRECTMESSAGEDESTROY_URL = "https://api.twitter.com/1.1/direct_messages/destroy";

    /* Friendships URLs */
    const std::string TWITCURL_FRIENDSHIPSCREATE_URL = "https://api.twitter.com/1.1/friendships/create";
    const std::string TWITCURL_FRIENDSHIPSDESTROY_URL = "https://api.twitter.com/1.1/friendships/destroy";
    const std::string TWITCURL_FRIENDSHIPSSHOW_URL = "https://api.twitter.com/1.1/friendships/show";

    /* Social graphs URLs */
    const std::string TWITCURL_FRIENDSIDS_URL = "https://api.twitter.com/1.1/friends/ids";
    const std::string TWITCURL_FOLLOWERSIDS_URL = "https://api.twitter.com/1.1/followers/ids";

    /* Account URLs */
    const std::string TWITCURL_ACCOUNTRATELIMIT_URL = "https://api.twitter.com/1.1/application/rate_limit_status";

    /* Favorites URLs */
    const std::string TWITCURL_FAVORITESGET_URL = "https://api.twitter.com/1.1/favorites/list";
    const std::string TWITCURL_FAVORITECREATE_URL = "https://api.twitter.com/1.1/favorites/create";
	const std::string TWITCURL_FAVORITEDESTROY_URL = "https://api.twitter.com/1.1/favorites/destroy";
	const std::string TWITCURL_RETWEETCREATE_URL = "https://api.twitter.com/1.1/statuses/retweet/";

    /* Block URLs */
    const std::string TWITCURL_BLOCKSCREATE_URL = "http://api.twitter.com/1/blocks/create/";
    const std::string TWITCURL_BLOCKSDESTROY_URL = "http://api.twitter.com/1/blocks/destroy/";
    
    /* Saved Search URLs */
    const std::string TWITCURL_SAVEDSEARCHGET_URL = "http://api.twitter.com/1/saved_searches";
    const std::string TWITCURL_SAVEDSEARCHSHOW_URL = "http://api.twitter.com/1/saved_searches/show/";
    const std::string TWITCURL_SAVEDSEARCHCREATE_URL = "http://api.twitter.com/1/saved_searches/create";
    const std::string TWITCURL_SAVEDSEARCHDESTROY_URL = "http://api.twitter.com/1/saved_searches/destroy/";

    /* Trends URLs */
    const std::string TWITCURL_TRENDS_URL = "http://api.twitter.com/1/trends";
    const std::string TWITCURL_TRENDSDAILY_URL = "http://api.twitter.com/1/trends/daily";
    const std::string TWITCURL_TRENDSCURRENT_URL = "http://api.twitter.com/1/trends/current";
    const std::string TWITCURL_TRENDSWEEKLY_URL = "http://api.twitter.com/1/trends/weekly";
    const std::string TWITCURL_TRENDSAVAILABLE_URL = "http://api.twitter.com/1/trends/available";
    
};

/* twitCurl class */
class twitCurl
{
public:
    twitCurl();
    ~twitCurl();

    /* Twitter OAuth authorization methods */
    oAuth& getOAuth();
    std::string oAuthRequestToken( std::string& authorizeUrl /* out */ );
    std::string oAuthAccessToken();
    std::string oAuthHandlePIN( const std::string& authorizeUrl /* in */ );

    /* Twitter login APIs, set once and forget */
    std::string& getTwitterUsername();
    std::string& getTwitterPassword();
    void setTwitterUsername( std::string& userName /* in */ );
    void setTwitterPassword( std::string& passWord /* in */ );

    /* Twitter API type */
    void setTwitterApiType( twitCurlTypes::eTwitCurlApiFormatType eType );

    /* Twitter search APIs */
    std::string search( std::string& searchQuery /* in */ );

    /* Twitter status APIs */
    std::string statusUpdate( std::string& newStatus /* in */,std::string inReplyTo="" );
    std::string statusShowById( std::string& statusId /* in */ );
    std::string statusDestroyById( std::string& statusId /* in */ );

    /* Twitter timeline APIs */
    std::string timelinePublicGet();
    std::string timelineFriendsGet();
    std::string timelineUserGet( bool trimUser /* in */, bool includeRetweets /* in */, unsigned int tweetCount /* in */, std::string userInfo = "" /* in */, bool isUserId = false /* in */ );
    std::string featuredUsersGet();
	std::string mentionsGet(std::string sinceId /* in */, std::string maxId, int tweetCount=20);
    std::string retweetsGet();

    /* Twitter user APIs */
    std::string userGet( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    std::string friendsGet( std::string userInfo = "" /* in */, bool isUserId = false /* in */ );
    std::string followersGet( std::string userInfo = "" /* in */, bool isUserId = false /* in */ );

    /* Twitter direct message APIs */
    std::string directMessageGet();
    std::string directMessageSend( std::string& userInfo /* in */, std::string& dMsg /* in */, bool isUserId = false /* in */ );
    std::string directMessageGetSent();
    std::string directMessageDestroyById( std::string& dMsgId /* in */ );

    /* Twitter friendships APIs */
    std::string friendshipCreate( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    std::string friendshipDestroy( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    std::string friendshipShow( std::string& userInfo /* in */, bool isUserId = false /* in */ );

    /* Twitter social graphs APIs */
    std::string friendsIdsGet( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    std::string followersIdsGet( std::string& userInfo /* in */, bool isUserId = false /* in */ );

    /* Twitter account APIs */
    std::string accountRateLimitGet();

    /* Twitter favorites APIs */
    std::string favoriteGet();
    std::string favoriteCreate( std::string& statusId /* in */ );
	std::string favoriteDestroy( std::string& statusId /* in */ );
	std::string retweetCreate( std::string& statusId /* in */ );

    /* Twitter block APIs */
    std::string blockCreate( std::string& userInfo /* in */ );
    std::string blockDestroy( std::string& userInfo /* in */ );

    /* Twitter search APIs */
    std::string savedSearchGet();
    std::string savedSearchCreate( std::string& query /* in */ );
    std::string savedSearchShow( std::string& searchId /* in */ );
    std::string savedSearchDestroy( std::string& searchId /* in */ );

    /* Twitter trends APIs (JSON) */
    std::string trendsGet();
    std::string trendsDailyGet();
    std::string trendsWeeklyGet();
    std::string trendsCurrentGet();
    std::string trendsAvailableGet();

    /* Internal cURL related methods */
    int saveLastWebResponse( char*& data, size_t size, twitInstance *instance  );

    /* cURL proxy APIs */
    std::string& getProxyServerIp();
    std::string& getProxyServerPort();
    std::string& getProxyUserName();
    std::string& getProxyPassword();
    void setProxyServerIp( std::string& proxyServerIp /* in */ );
    void setProxyServerPort( std::string& proxyServerPort /* in */ );
    void setProxyUserName( std::string& proxyUserName /* in */ );
    void setProxyPassword( std::string& proxyPassword /* in */ );

    /* cURL data */
    CURL* m_curlHandle;
	bool handleInUse;
    char m_errorBuffer[twitCurlDefaults::TWITCURL_DEFAULT_BUFFSIZE];
    //std::string m_callbackData;

    /* cURL proxy data */
    std::string m_proxyServerIp;
    std::string m_proxyServerPort;
    std::string m_proxyUserName;
    std::string m_proxyPassword;

    /* Twitter data */
    std::string m_twitterUsername;
    std::string m_twitterPassword;

    /* Twitter API type */
    twitCurlTypes::eTwitCurlApiFormatType m_eApiFormatType;

    /* OAuth data */
    oAuth m_oAuth;

    /* Private methods */
    void clearCurlCallbackBuffers();
    void prepareCurlProxy(twitInstance *instance);
    void prepareCurlCallback(twitInstance *instance);
    void prepareCurlUserPass(twitInstance *instance);
    void prepareStandardParams(twitInstance *instance);
    std::string performGet( const std::string& getUrl );
    std::string performGet( const std::string& getUrl, const std::string& oAuthHttpHeader );
    std::string performDelete( const std::string& deleteUrl );
    std::string performPost( const std::string& postUrl, std::string dataStr = "" );

    /* Internal cURL related methods */
    static int curlCallback( char* data, size_t size, size_t nmemb, twitInstance *instance);
	std::string timelineHomeGet( bool trimUser, bool includeRetweets, unsigned int tweetCount  , std::string sinceId ,std::string maxId);
};


struct twitInstance
{
	twitCurl *twit;
	int instance;
	std::string data;
	CURL* m_curlHandle;
};

/* Private functions */
void utilMakeCurlParams( std::string& outStr, std::string& inParam1, std::string& inParam2 );
void utilMakeUrlForUser( std::string& outUrl, const std::string& baseUrl, std::string& userInfo, bool isUserId );

#endif // _TWITCURL_H_
