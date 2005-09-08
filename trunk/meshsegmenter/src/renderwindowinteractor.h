#ifndef RenderWindowInteractor_h
#define RenderWindowInteractor_h

//
// $Id: renderwindowinteractor.h,v 1.2 2000/09/14 13:25:43 jstifter Exp $
//

#include "renderwindow.h"

#include <qtimer.h>

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkCellPicker.h"

class RenderWindowInteractor : public RenderWindow
{
   Q_OBJECT

public:

   RenderWindowInteractor( QWidget *parent = 0, const char *name = 0 );
   RenderWindowInteractor( QGLFormat glf, QWidget *parent = 0, const char *name = 0 );

   ~RenderWindowInteractor();

   void SetLeftButtonFunction( const char* function );
   void SetMiddleButtonFunction( const char* function );
   void SetRightButtonFunction( const char* function );
   void SetShiftLeftButtonFunction( const char* function );

   void SetInteractionActor( vtkActor* actor );
   void SetActorRotation( double  x, double  y, double  z );
   void SetPlane(double  *ebene);
   void SetInteractionTime( int time );
   void SetSensitivity( double  t );

   int GetInteractionTime( void );
   vtkActor *GetInteractionActor( void );
   double  GetSensitivity();
   const char *GetClassName();


signals:
   /*!
      \brief This signal is emitted, whenever we have rotated an actor.
      \param xf the angle in degrees around the view-up vector of the current
      camera.
      \param yf the angle in degrees around the
      cross of the viewplane-normal and the view-up vector.
   */
   void ActorRotated( double  xf, double  yf );

   /*!
      \brief This signal is emitted, whenever we have rolled an actor
      Rolling is rotating in the viewplane of the camera
      \param angle the angle in degrees around the viewplane-normal of
      the current camera.
   */
   void ActorRolled( double  angle );

   /*!
      This signal is emitted, when the interaction is finished.
      \param x the x coordinate of the pointer
      \param y the y coordinate of the pointer
   */
   void Released( int x, int y );

protected:
   void mousePressEvent( QMouseEvent *event );

   void mouseReleaseEvent( QMouseEvent *event );
   void mouseMoveEvent( QMouseEvent *event );

protected slots:

   void ActorRotate();
   void ActorTranslate();
   void ActorZoom();
   void ActorRoll();
   void ActorPlaneRoll();
   void ActorPlaneMove();
   
   void CameraRotate();
   void CameraTranslate();
   void CameraZoom();
   void CameraRoll();
   
   void DoNothing();


private:
   void DisplayToWorld( double  x, double  y, double  z,
                                                double  *worldPoint );

   void WorldToDisplay( double x, double  y, double  z,
                                                double  *displayPoint );

   void InitRenderWindowInteractor();

   vtkCellPicker *cellpicker;

   vtkCamera   *currentCamera;
   vtkActor    *interactionActor;
   vtkActor    *collisionActor;
   vtkLight    *currentLight;

   double viewUp[3];
   double vpNormal[3];
   double  rotateY[3];
   double  renCenter[3];
   double  viewFocus[3];
   double  camPos[3];
   double  deltaElevation;
   double  deltaAzimuth;
   double  normale[3];
   double  ebenekonstante;

   QTimer   *stepTimer;
   bool     mouseEventActive;
   bool     firstCall;
   double     trackballFactor;
   double     cameraRotation[3];
   int      interactionTime;
   int      currentMousePos[2];
   int      lastMousePos[2];

   char leftButtonFunction[200];
   char middleButtonFunction[200];
   char rightButtonFunction[200];
   char shiftLeftButtonFunction[200];
};

#endif  // RenderWindowInteractor_h
