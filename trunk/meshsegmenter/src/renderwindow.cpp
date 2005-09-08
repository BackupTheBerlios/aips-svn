#include "renderwindow.h"

/*! \class RenderWindow
    \brief Combines a QGLWidget of \a qt with a vtkRenderWindow of \a vtk.

This class combines the great GUI toolkit qt (<a
href="http://www.trolltech.no">www.troll.no</a>) with the great visualization toolkit vtk
(<a href="http://www.kitware.com/vtk">www.kitware.com/vtk</a>).

    \version $Revision: 1.3 $
    \author Jan Stifter <a href="mailto:j.stifter@medres.ch">j.stifter@medres.ch</a>.
Original idea and code from Tom Radcliffe
<a href="mailto:radcliffe@igotechnologies.com">radcliffe@igotechnologies.com</a>.
    \date $Date: 2000/09/14 14:56:10 $
    \par Thanks to:
         Kuno Meyer <meyerk@student.ethz.ch>,
         Marcel van Straten <streets@mediaport.org>
         Kent Vander Velden <graphix@iastate.edu>

    \par History:
    \code
      $Log: renderwindow.cpp,v $
      Revision 1.3  2000/09/14 14:56:10  jstifter
      Changes proposed from Kent Vander Velden <graphix@iastate.edu>
      CameraRotate, CameraTranslate, CameraZoom problems fixed (mostly due to
      incorrect clipping)

      Revision 1.2  2000/09/14 13:25:43  jstifter
      Added Support for *parent, *name in the constructors

      Revision 1.1.1.1  2000/08/15 09:36:39  jstifter
      first cvs import

    \endcode

*/


/*!
   \brief Constructs a new RenderWindow

   The new RenderWindow has a vtkRenderWindow, a
   vtkRenderer and a vtkLight initialized
   \param glf the QGLFormat, which will be passed directly to the underlying QGLWidget
   \param parent directly passed to QGLWidget
   \param name directly passed to QGLWidget
*/
RenderWindow::RenderWindow(QGLFormat glf, QWidget * parent, const char * name )
  : QGLWidget(glf, parent, name) {
   this->InitRenderWindow();
}


/*!
   \brief Constructs a new RenderWindow

   The new RenderWindow has a vtkRenderWindow, a
   vtkRenderer and a vtkLight initialized
   \param parent directly passed to QGLWidget
   \param name directly passed to QGLWidget
*/
RenderWindow::RenderWindow( QWidget * parent, const char * name)
  : QGLWidget(parent, name) {
   this->InitRenderWindow();
}

/*!
   \brief Initialize the RenderWindow

   This method is called from the two Constructors
*/
void RenderWindow::InitRenderWindow() {
   this->initNeeded = true;
   this->renderWindow = vtkRenderWindow::New();
   this->renderer = vtkRenderer::New();
   this->renderWindow->AddRenderer( this->renderer );
   this->light = vtkLight::New();
   this->renderer->AddLight( this->light );
   // just enable one double buffering
   // by default, we use the one of vtk
   #ifdef QT_DOUBLEBUFFER
      // use the double buffering of QT
      this->renderWindow->SetSwapBuffers(0);
   #else
      // use the double buffering of vtk
      this->setAutoBufferSwap( false );
   #endif
}

/*!
   \brief Destructs the RenderWindow.
*/
RenderWindow::~RenderWindow() {
   this->renderer->Delete();
   this->renderWindow->Delete();
}

/*!
   \brief returns the vtkRenderer used
*/
vtkRenderer *RenderWindow::GetRenderer() {
   return this->renderer;
}

/*!
   \brief returns the vtkRenderWindow used
*/
vtkRenderWindow *RenderWindow::GetRenderWindow( ) {
   return this->renderWindow;
}

/*!
   \brief Initialize the OpenGL Window
*/
void RenderWindow::initializeGL( ) {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor3f(1.0, 0.0, 0.0);
}

/*!
   \brief Resize the OpenGL Window
*/
void RenderWindow::resizeGL( int w, int h) {
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0, w, 0.0, h );
  glMatrixMode( GL_MODELVIEW );

  updateGL();
}

/*!
   \brief Render the scene
*/
void RenderWindow::paintGL( )
{
   // Get the native window ID and pass it
   // to the VTK render window
   // before we render for the first time...
   if (this->initNeeded) {
      WId nId = winId();
      this->renderWindow->SetWindowId( (void*) nId );
      this->initNeeded = false;
   }
   else {
      this->renderWindow->Render();
   }
}
