CC=g++

CFLAGS=-c -Wall -Wall -std=c++0x -fno-rtti -I../libtwitcurl -I ../libtwitcurl/curl -I/usr/include/ -DLINUX  -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Users/thahn/utils/homebrew/include -I./


LDFLAGS= -L/usr/lib -L/Users/thahn/utils/homebrew/lib -Wl,-framework,Cocoa -lSDLmain -lSDL  -lSDL_image -lSDL_ttf -lcurl -lGLEW -framework OpenGL -m32 -framework Awesomium ../libtwitcurl/libtwitcurl.dylib


SOURCES=Awesomium.cpp Column.cpp CustomColumn.cpp Tweet.cpp file.cpp gl_texture_surface.cpp json_reader.cpp json_value.cpp json_writer.cpp stream.cpp twitter.cpp zatcap.cpp TimedEventProcess.cpp 

OBJECTS=$(patsubst %.cpp,linux/obj/%.o,$(SOURCES))
EXECUTABLE=../DebugRelease/zatcap

all: $(EXECUTABLE)
	
$(EXECUTABLE):$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

linux/obj/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

