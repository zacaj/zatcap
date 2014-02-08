# Hey Emacs, this is a -*- makefile -*-
# The twitcurl library.. a Makefile for OpenWRT
# Makefile-fu by John Boiles
# 28 September 2009

LIBNAME = twitcurl
SRC = $(LIBNAME).cpp
STAGING_DIR = 
INCLUDE_DIR = $(STAGING_DIR)/usr/include
LINCLUDE_DIR = $(STAGING_DIR)/usr/local/include
LIBRARY_DIR = $(STAGING_DIR)/usr/lib
LLIBRARY_DIR = $(STAGING_DIR)/usr/local/lib
#LDFLAGS += -Wl,-rpath-link=$(STAGING_DIR)/usr/lib
CC = g++ -g
REMOVE = rm -f
COPY = cp
REMOTEIP = 192.168.1.30

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
# CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS =

# Place -I options here
CINCS = 

CFLAGS =$(CDEFS) $(CINCS) $(CSTANDARD)

all: target

target: $(SRC) $(LIBNAME).h
	$(CC) -Wall -fPIC -c -I$(INCLUDE_DIR) $(SRC) oauthlib.cpp urlencode.cpp base64.cpp HMAC_SHA1.cpp SHA1.cpp -m32
	ar —r lib$(LIBNAME).a twitcurl.o urlencode.cpp

#clean project.
clean:
	$(REMOVE) $(LIBNAME)/libtwitcurl.a
	$(REMOVE) $(LIBNAME).o
	$(REMOVE) $(LIBRARY_DIR)/lib$(LIBNAME).a
	
# Install library to local openwrt library directory
install: all
	$(COPY) lib$(LIBNAME).dylib $(LIBRARY_DIR)
	$(COPY) lib$(LIBNAME).dylib $(LLIBRARY_DIR)
	$(COPY) $(LIBNAME).h $(INCLUDE_DIR)/
	$(COPY) $(LIBNAME).h $(LINCLUDE_DIR)/
	$(COPY) oauthlib.h $(INCLUDE_DIR)/
	$(COPY) oauthlib.h $(LINCLUDE_DIR)/
	ln -sf $(LIBRARY_DIR)/lib$(LIBNAME).dylib $(LIBRARY_DIR)/lib$(LIBNAME).dylib
	ln -sf $(LLIBRARY_DIR)/lib$(LIBNAME).dylib $(LLIBRARY_DIR)/lib$(LIBNAME).dylib
