#ifndef _RenderWindow_H_
#define _RenderWindow_H_

//
// $Id: renderwindow.h,v 1.2 2000/09/14 13:25:43 jstifter Exp $
//

#include <qgl.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkLight.h"

class RenderWindow : public QGLWidget {

Q_OBJECT

public:
   RenderWindow( QGLFormat glf, QWidget *parent = 0, const char *name = 0 );
   RenderWindow( QWidget *parent = 0, const char *name = 0 );

   ~RenderWindow();

   vtkRenderer *GetRenderer(); // Returns a vtkRenderer (Mmmh, are we limited to one renderer per Widget???
   vtkRenderWindow *GetRenderWindow(); // Returns the render window (maybe we could use this handle to add more Renderers???)

protected:
   virtual void paintGL( );
   virtual void initializeGL( );
   virtual void resizeGL( int w, int h );

private:
   void InitRenderWindow();
   bool initNeeded;
   vtkRenderer*     renderer;
   vtkRenderWindow* renderWindow;
   vtkLight*        light;

};

#endif // _RenderWindow_H_
