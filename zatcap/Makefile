CC=g++

CFLAGS=-c -m32 -Wall -std=c++0x -fno-rtti -I../libtwitcurl -I ../libtwitcurl/curl -I/usr/include/ -DLINUX  -D_GNU_SOURCE=1 -D_THREAD_SAFE -I/Users/thahn/utils/homebrew/include -I./


LDFLAGS= -L/usr/lib -L/Users/thahn/codestuff/zatcap/libtwitcurl -L/Users/thahn/utils/glew-1.10.0/lib -L/Users/thahn/codestuff/SDL32 -Wl,-framework,Cocoa -lSDLmain -lSDL  -lcurl -lGLEW -framework OpenGL -m32 -framework Awesomium -ltwitcurl


SOURCES=Awesomium.cpp Column.cpp CustomColumn.cpp Tweet.cpp file.cpp gl_texture_surface.cpp json_reader.cpp json_value.cpp json_writer.cpp stream.cpp twitter.cpp zatcap.cpp TimedEventProcess.cpp 

OBJECTS=$(patsubst %.cpp,linux/obj/%.o,$(SOURCES))
EXECUTABLE=../DebugRelease/zatcap

all: $(EXECUTABLE)
	
$(EXECUTABLE):$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

linux/obj/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

