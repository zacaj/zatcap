CC=g++
CFLAGS= -c -w -std=c++0x -fno-rtti -I-./ -I../libtwitcurl -I ../libtwitcurl/curl -DLINUX -I/usr/include/SDL

LDFLAGS= -L/usr/lib -lSDLmain -lSDL -lcurl -lGLEW -lawesomium-1-7 -lGL ../libtwitcurl/libtwitcurl.so.1.0

SOURCES=Awesomium.cpp Column.cpp CustomColumn.cpp Tweet.cpp file.cpp gl_texture_surface.cpp json_reader.cpp json_value.cpp json_writer.cpp stream.cpp twitter.cpp zatcap.cpp TimedEventProcess.cpp 

OBJECTS=$(patsubst %.cpp,linux/obj/%.o,$(SOURCES))
EXECUTABLE=../DebugRelease/zatcap

all: $(EXECUTABLE)
	
$(EXECUTABLE):$(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

linux/obj/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

