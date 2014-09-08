#ifndef __GL_TEXTURE_SURFACE_H__
#define __GL_TEXTURE_SURFACE_H__

#include <GL/glew.h>
#include <Awesomium/Surface.h>
#include <set>
using namespace std;
class GLRAMTextureSurface;
extern set<GLRAMTextureSurface*> activeSurfaces;
class GLTextureSurface : public Awesomium::Surface {
public:
  virtual void Paint(unsigned char* src_buffer,
                     int src_row_span,
                     const Awesomium::Rect& src_rect,
                     const Awesomium::Rect& dest_rect) = 0;

  virtual void Scroll(int dx,
                      int dy,
                      const Awesomium::Rect& clip_rect) = 0;

  virtual GLuint GetTexture() const = 0;
  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual int size() const = 0;
};

class GLRAMTextureSurface : public GLTextureSurface {
public:
  GLuint texture_id_;
  unsigned char* buffer_;
  int bpp_, rowspan_, width_, height_;
  bool needs_update_;

  GLRAMTextureSurface(int width, int height);
  virtual ~GLRAMTextureSurface();

  GLuint GetTexture() const;

  int width() const { return width_; }

  int height() const { return height_; }

  int size() const { return rowspan_ * height_; }

  void UpdateTexture();
 //protected:
  virtual void Paint(unsigned char* src_buffer,
                     int src_row_span,
                     const Awesomium::Rect& src_rect,
                     const Awesomium::Rect& dest_rect);

  virtual void Scroll(int dx,
                      int dy,
                      const Awesomium::Rect& clip_rect);

};

class GLTextureSurfaceFactory : public Awesomium::SurfaceFactory {
public:
  GLTextureSurfaceFactory();

  virtual ~GLTextureSurfaceFactory();

  virtual Awesomium::Surface* CreateSurface(Awesomium::WebView* view,
                                            int width,
                                            int height);

  virtual void DestroySurface(Awesomium::Surface* surface);
};

#endif  // __GL_TEXTURE_SURFACE_H__
