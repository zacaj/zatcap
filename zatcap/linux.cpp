 
#include <SDL1/SDL.h>
//#include <GL\glew.h>
#include "gl_texture_surface.h"
 
#define mapKey(a, b) case SDLK_##a: return Awesomium::KeyCodes::AK_##b;
 
int getWebKeyFromSDLKey(SDLKey key) {
        switch (key) {
                mapKey(BACKSPACE, BACK)
                        mapKey(TAB, TAB)
                        mapKey(CLEAR, CLEAR)
                        mapKey(RETURN, RETURN)
                        mapKey(PAUSE, PAUSE)
                        mapKey(ESCAPE, ESCAPE)
                        mapKey(SPACE, SPACE)
                        mapKey(EXCLAIM, 1)
                        mapKey(QUOTEDBL, 2)
                        mapKey(HASH, 3)
                        mapKey(DOLLAR, 4)
                        mapKey(AMPERSAND, 7)
                        mapKey(QUOTE, OEM_7)
                        mapKey(LEFTPAREN, 9)
                        mapKey(RIGHTPAREN, 0)
                        mapKey(ASTERISK, 8)
                        mapKey(PLUS, OEM_PLUS)
                        mapKey(COMMA, OEM_COMMA)
                        mapKey(MINUS, OEM_MINUS)
                        mapKey(PERIOD, OEM_PERIOD)
                        mapKey(SLASH, OEM_2)
                        mapKey(0, 0)
                        mapKey(1, 1)
                        mapKey(2, 2)
                        mapKey(3, 3)
                        mapKey(4, 4)
                        mapKey(5, 5)
                        mapKey(6, 6)
                        mapKey(7, 7)
                        mapKey(8, 8)
                        mapKey(9, 9)
                        mapKey(COLON, OEM_1)
                        mapKey(SEMICOLON, OEM_1)
                        mapKey(LESS, OEM_COMMA)
                        mapKey(EQUALS, OEM_PLUS)
                        mapKey(GREATER, OEM_PERIOD)
                        mapKey(QUESTION, OEM_2)
                        mapKey(AT, 2)
                        mapKey(LEFTBRACKET, OEM_4)
                        mapKey(BACKSLASH, OEM_5)
                        mapKey(RIGHTBRACKET, OEM_6)
                        mapKey(CARET, 6)
                        mapKey(UNDERSCORE, OEM_MINUS)
                        mapKey(BACKQUOTE, OEM_3)
                        mapKey(a, A)
                        mapKey(b, B)
                        mapKey(c, C)
                        mapKey(d, D)
                        mapKey(e, E)
                        mapKey(f, F)
                        mapKey(g, G)
                        mapKey(h, H)
                        mapKey(i, I)
                        mapKey(j, J)
                        mapKey(k, K)
                        mapKey(l, L)
                        mapKey(m, M)
                        mapKey(n, N)
                        mapKey(o, O)
                        mapKey(p, P)
                        mapKey(q, Q)
                        mapKey(r, R)
                        mapKey(s, S)
                        mapKey(t, T)
                        mapKey(u, U)
                        mapKey(v, V)
                        mapKey(w, W)
                        mapKey(x, X)
                        mapKey(y, Y)
                        mapKey(z, Z)
                        mapKey(DELETE, DELETE)
                        mapKey(KP0, NUMPAD0)
                        mapKey(KP1, NUMPAD1)
                        mapKey(KP2, NUMPAD2)
                        mapKey(KP3, NUMPAD3)
                        mapKey(KP4, NUMPAD4)
                        mapKey(KP5, NUMPAD5)
                        mapKey(KP6, NUMPAD6)
                        mapKey(KP7, NUMPAD7)
                        mapKey(KP8, NUMPAD8)
                        mapKey(KP9, NUMPAD9)
                        mapKey(KP_PERIOD, DECIMAL)
                        mapKey(KP_DIVIDE, DIVIDE)
                        mapKey(KP_MULTIPLY, MULTIPLY)
                        mapKey(KP_MINUS, SUBTRACT)
                        mapKey(KP_PLUS, ADD)
                        mapKey(KP_ENTER, SEPARATOR)
                        mapKey(KP_EQUALS, UNKNOWN)
                        mapKey(UP, UP)
                        mapKey(DOWN, DOWN)
                        mapKey(RIGHT, RIGHT)
                        mapKey(LEFT, LEFT)
                        mapKey(INSERT, INSERT)
                        mapKey(HOME, HOME)
                        mapKey(END, END)
                        mapKey(PAGEUP, PRIOR)
                        mapKey(PAGEDOWN, NEXT)
                        mapKey(F1, F1)
                        mapKey(F2, F2)
                        mapKey(F3, F3)
                        mapKey(F4, F4)
                        mapKey(F5, F5)
                        mapKey(F6, F6)
                        mapKey(F7, F7)
                        mapKey(F8, F8)
                        mapKey(F9, F9)
                        mapKey(F10, F10)
                        mapKey(F11, F11)
                        mapKey(F12, F12)
                        mapKey(F13, F13)
                        mapKey(F14, F14)
                        mapKey(F15, F15)
                        mapKey(NUMLOCK, NUMLOCK)
                        mapKey(CAPSLOCK, CAPITAL)
                        mapKey(SCROLLOCK, SCROLL)
                        mapKey(RSHIFT, RSHIFT)
                        mapKey(LSHIFT, LSHIFT)
                        mapKey(RCTRL, RCONTROL)
                        mapKey(LCTRL, LCONTROL)
                        mapKey(RALT, RMENU)
                        mapKey(LALT, LMENU)
                        mapKey(RMETA, LWIN)
                        mapKey(LMETA, RWIN)
                        mapKey(LSUPER, LWIN)
                        mapKey(RSUPER, RWIN)
                        mapKey(MODE, MODECHANGE)
                        mapKey(COMPOSE, ACCEPT)
                        mapKey(HELP, HELP)
                        mapKey(PRINT, SNAPSHOT)
                        mapKey(SYSREQ, EXECUTE)
        default:
                return Awesomium::KeyCodes::AK_UNKNOWN;
        }
}
void handleSDLKeyEvent(Awesomium::WebView* webView, const SDL_Event& event) {
        if (!(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP))
                return;
 
        Awesomium::WebKeyboardEvent keyEvent;
 
        keyEvent.type = event.type == SDL_KEYDOWN?
                Awesomium::WebKeyboardEvent::kTypeKeyDown :
        Awesomium::WebKeyboardEvent::kTypeKeyUp;
 
        char* buf = new char[20];
        keyEvent.virtual_key_code = getWebKeyFromSDLKey(event.key.keysym.sym);
        Awesomium::GetKeyIdentifierFromVirtualKeyCode(keyEvent.virtual_key_code,
                &buf);
        strcpy(keyEvent.key_identifier, buf);
        delete[] buf;
 
        keyEvent.modifiers = 0;
 
        if (event.key.keysym.mod & KMOD_LALT || event.key.keysym.mod & KMOD_RALT)
                keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModAltKey;
        if (event.key.keysym.mod & KMOD_LCTRL || event.key.keysym.mod & KMOD_RCTRL)
                keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModControlKey;
        if (event.key.keysym.mod & KMOD_LMETA || event.key.keysym.mod & KMOD_RMETA)
                keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModMetaKey;
        if (event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT)
                keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModShiftKey;
        if (event.key.keysym.mod & KMOD_NUM)
                keyEvent.modifiers |= Awesomium::WebKeyboardEvent::kModIsKeypad;
 
        keyEvent.native_key_code = event.key.keysym.scancode;
 
        if (event.type == SDL_KEYUP) {
                webView->InjectKeyboardEvent(keyEvent);
        } else {
                unsigned int chr;
                if ((event.key.keysym.unicode & 0xFF80) == 0)
                        chr = event.key.keysym.unicode & 0x7F;
                else
                        chr = event.key.keysym.unicode;
 
                keyEvent.text[0] = chr;
                keyEvent.unmodified_text[0] = chr;
 
                webView->InjectKeyboardEvent(keyEvent);
 
                if (chr) {
                        keyEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
                        keyEvent.virtual_key_code = chr;
                        keyEvent.native_key_code = chr;
                        webView->InjectKeyboardEvent(keyEvent);
                }
        }
}
 
void sdlInit()
{
    SDL_Init(SDL_INIT_VIDEO);
        SDL_SetVideoMode(settings::windowWidth, settings::windowHeight,32,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGL|SDL_RESIZABLE);
        glewInit();
        glClearColor(0, 0, 0, 0);
        glClearDepth(1.0f);
 
        glViewport(0, 0, 1024,768);
 
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
 
        glOrtho(0, 1024,768, 0, 1, -1);
 
        glMatrixMode(GL_MODELVIEW);
 
        glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
}
 
void sdlQuit()
{
    SDL_Quit();
}
 
void sdlUpdate(int &quit)
{
    SDL_Event e;
                while(SDL_PollEvent(&e))
                {
                        switch(e.type)
                        {
                        case SDL_QUIT:
                                quit=1;
                                break;
                        case SDL_KEYDOWN:
                                if(e.key.keysym.sym==SDLK_ESCAPE)
                                        quit=1;
                                else
                                {
                                        handleSDLKeyEvent(view,e);
                                }
                                break;
                        case SDL_MOUSEMOTION:
 
                                        view->
                                        InjectMouseMove(e.motion.x, e.motion.y);
                                break;
                        case SDL_MOUSEBUTTONDOWN:
                                        if (e.button.button == SDL_BUTTON_LEFT)
                                                view->
                                                InjectMouseDown(Awesomium::kMouseButton_Left);
                                        else if (e.button.button == SDL_BUTTON_WHEELUP)
                                                view->InjectMouseWheel(25, 0);
                                        else if (e.button.button == SDL_BUTTON_WHEELDOWN)
                                                view->InjectMouseWheel(-25, 0);
 
                                break;
                        case SDL_MOUSEBUTTONUP:
                                        if (e.button.button == SDL_BUTTON_LEFT)
                                                view->
                                                InjectMouseUp(Awesomium::kMouseButton_Left);
                                break;
                            case SDL_VIDEORESIZE:
                                
                                SDL_SetVideoMode(e.resize.w, e.resize.h,32,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGL|SDL_RESIZABLE);
                                
                                glViewport(0, 0, e.resize.w,e.resize.h);
                                
                                view->Resize(e.resize.w,e.resize.h);
                                glewInit();
                                glClearColor(0, 0, 0, 0);
                                glClearDepth(1.0f);
                                
                                
                                glMatrixMode(GL_PROJECTION);
                                glLoadIdentity();
                                
                                glOrtho(0, 1024,768, 0, 1, -1);
                                
                                glMatrixMode(GL_MODELVIEW);
                                
                                glEnable(GL_TEXTURE_2D);
                                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                                glLoadIdentity();
                                break;

                        }
                }
}
