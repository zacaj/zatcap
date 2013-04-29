CC=g++
CFLAGS=-c -Wall -Wall -std=c++0x -fno-rtti -I../libtwitcurl -I ../libtwitcurl/curl -DLINUX -I/usr/include/SDL

LDFLAGS= -L/usr/lib -lSDLmain -lSDL -lSDL_gfx -lSDL_image -lSDL_ttf -lcurl /usr/lib/libGLEW.a -lawesomium-1-7 -lGL ../libtwitcurl/libtwitcurl.a

SOURCES=AvitarDownloader.cpp Awesomium.cpp Button.cpp Column.cpp HomeColumn.cpp MentionColumn.cpp TimedEventProcess.cpp Tweet.cpp WaitIndicator.cpp file.cpp gl_texture_surface.cpp json_reader.cpp json_value.cpp json_writer.cpp stream.cpp twitter.cpp zatcap.cpp
OBJECTS=$(patsubst %.cpp,linux/obj/%.o,$(SOURCES))
EXECUTABLE=../DebugRelease/zatcap

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE):$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o linux/obj/$@
