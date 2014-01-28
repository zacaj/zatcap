CC=g++

CFLAGS=-g -m32 -I/Library/Frameworks/Awesomium.framework/Headers  -I/Library/Frameworks/SDL.framework/Headers  -I/Library/Frameworks/OpenGL.framework/Headers -c -Wall -Wall -std=c++0x -fno-rtti -I../libtwitcurl -I ../libtwitcurl/curl -I/usr/include/ -DLINUX  -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Users/thahn/utils/homebrew/include -I./


LDFLAGS=-g -F/Library/Frameworks  -Wl,-framework,Cocoa /usr/local/lib/libSDLmain.a /usr/local/lib/libSDL.dylib -lcurl -framework OpenGL -lGLEW -m32 -framework Awesomium


SOURCES=Awesomium.cpp Column.cpp CustomColumn.cpp Tweet.cpp file.cpp gl_texture_surface.cpp json_reader.cpp json_value.cpp json_writer.cpp stream.cpp twitter.cpp zatcap.cpp TimedEventProcess.cpp ../libtwitcurl/oauthlib.cpp ../libtwitcurl/urlencode.cpp ../libtwitcurl/base64.cpp ../libtwitcurl/HMAC_SHA1.cpp ../libtwitcurl/SHA1.cpp ../libtwitcurl/twitcurl.cpp

OBJECTS=$(SOURCES:%.cpp=%.o)
EXECUTABLE=../DebugRelease/zatcap

all: $(EXECUTABLE)
	
$(EXECUTABLE):$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

