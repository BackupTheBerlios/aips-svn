#include "renderwindowinteractor.h"

#include "vtkMath.h"
#include "vtkLightCollection.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkMath.h"

/*! \class RenderWindowInteractor
    \brief Gives a RenderWindow the ability to interact with the mouse

This class implements the interaction to the RenderWindow. There is
the possibility to set the according function dynamically
during run time for each mouse button.
There are 2 interaction possibilites: interaction
with an actor and interaction with the camera.

The following interaction functions for interacting with
the \b camera are possible:

      \arg CameraRotate : Rotates the camera about the focal point
      \arg CameraTranslate : Translates the camera parallel to the viewplane
      \arg CameraZoom : Translates the camera on the viewplane normal
      \arg CameraRoll : Rotates the camera about the viewplane normal

The following interaction functions for interacting with
an \b actor are possible:

      \arg ActorRotate : Rotates the actor about a specific point
      \arg ActorTranslate : Translates the actor on the viewplane
      \arg ActorZoom : Moves the actor on the viewplane normal
      \arg ActorRoll : Rotates the actor around the viewplane normal
      \arg ActorPlaneRoll : Rotates the actor about a specific plane
      \arg ActorPlaneMove : Moves the actor on a specific plane

Additionally, you can set the speed of an interaction with SetInteractionTime(),
you can define the rotation point of actors with SetActorRotation()
and you can set the sensitivity of the interaction with SetSensitivity().
    \version $Revision: 1.3 $
    \author Jan Stifter
    \date $Date: 2000/09/14 14:56:10 $
    \par Based on:
        A lot of code is based on the original vtkQGL package, written by
        Jan Ehrhardt <a
href="mailto:ehrhardt@medinf.mu-luebeck.de">ehrhardt@medinf.mu-luebeck.de</a>.
    \par History:
    \code
      $Log: renderwindowinteractor.cpp,v $
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
   \brief Constructs a new RenderWindowInteractor with the default settings.

   The default functions are:
   \arg LeftButtonFunction : CameraRotate()
   \arg MiddleButtonFunction : CameraTranslate()
   \arg RightButtonFunction : CameraZoom()
   \arg ShiftLeftButtonFunction : CameraRoll()

   The time between two interactions is set to 50 milliseconds, the
   sensitvity is set to 10.

   \param parent directly passed to constructor of RenderWindow
   \param name directly passed to constructor of RenderWindow
*/
RenderWindowInteractor::RenderWindowInteractor( QWidget *parent,
               const char *name )
   : RenderWindow( parent, name ) {
   this->InitRenderWindowInteractor();
}

/*!
   \brief Constructs a new RenderWindowInteractor with the default settings.

   The default functions are:
   \arg LeftButtonFunction : CameraRotate()
   \arg MiddleButtonFunction : CameraTranslate()
   \arg RightButtonFunction : CameraZoom()
   \arg ShiftLeftButtonFunction : CameraRoll()

   The time between two interactions is set to 50 milliseconds, the
   sensitvity is set to 10.
   \param glf the desired QGLFormat
   \param parent directly passed to constructor of RenderWindow
   \param name directly passed to constructor of RenderWindow
*/
RenderWindowInteractor::RenderWindowInteractor(
      QGLFormat glf, QWidget *parent, const char *name )
   : RenderWindow( glf, parent, name ) {
   this->InitRenderWindowInteractor();
}


/*!
   \brief Destructor of RenderWindowInteractor.

*/
RenderWindowInteractor::~RenderWindowInteractor() {
   delete this->stepTimer;
}


/*!
   \brief Initialize the RenderWindowInteractor

   This function is called from the two constructors
*/
void RenderWindowInteractor::InitRenderWindowInteractor() {
   this->SetLeftButtonFunction( SLOT(CameraRotate()) );
   this->SetMiddleButtonFunction( SLOT(CameraTranslate()) );
   this->SetRightButtonFunction( SLOT(CameraZoom()) );
   this->SetShiftLeftButtonFunction( SLOT(CameraRoll()) );

   this->mouseEventActive = false;
   this->SetInteractionTime( 50 );
   this->SetSensitivity( 10.0 );

   this->stepTimer = new QTimer();
   CHECK_PTR( this->stepTimer );

   this->currentCamera = this->GetRenderer()->GetActiveCamera();
   CHECK_PTR( this->currentCamera );

   // get the light we are using
   vtkLightCollection *lightCollection = this->GetRenderer()->GetLights();
   lightCollection->InitTraversal();
   this->currentLight = lightCollection->GetNextItem();
   CHECK_PTR( this->currentLight );

   this->interactionActor = NULL;
   this->collisionActor = NULL;
}


/*!
   \brief Returns the class name
*/
const char *RenderWindowInteractor::GetClassName() {
   return( "RenderWindowInteractor" );
}


/*!
   \brief Sets the plane for the actor interaction ActorPlaneMove() and ActorPlaneRoll().

   \param ebene pointer to a double  array with the four values, which specifiy a plane (ax+by+cz+d=1).
*/
void RenderWindowInteractor::SetPlane(double  *ebene) {

   this->normale[0]     = ebene[0];
   this->normale[1]     = ebene[1];
   this->normale[2]     = ebene[2];
   this->ebenekonstante = ebene[3];

}

/*!
   \brief Sets the sensitivity of all interactions

   \param t the sensitivity, default is 10.0
   \sa GetSensitivity()
*/
void RenderWindowInteractor::SetSensitivity( double  t ) {
   this->trackballFactor = t;
}

/*!
   \brief Returns the current sensitivity of all interactions

   \sa SetSensitivity()
*/
double  RenderWindowInteractor::GetSensitivity() {
   return this->trackballFactor;
}


/*!
   \brief Sets the function, which will be used, when the left mouse button is pressed

   The function can be:
   \arg  ActorRotate()
   \arg  ActorTranslate()
   \arg  ActorZoom()
   \arg  ActorRoll()
   \arg  ActorPlaneMove()
   \arg  ActorPlaneRoll()
   \arg  CameraRotate()
   \arg  CameraTranslate()
   \arg  CameraZoom()
   \arg  CameraRoll()
   \arg  DoNothing()

   \param function the name of the function, wrapped in a slot, e.g.
   \c SLOT(CameraRotate()). If function is NULL,  DoNothing() will be used.
*/
void RenderWindowInteractor::SetLeftButtonFunction( const char* function ) {
   if( function == NULL ) {
      strncpy( this->leftButtonFunction, SLOT(DoNothing()), 100 );
   }
   else {
      strncpy( this->leftButtonFunction, function, 100 );
   }
}


/*!
   \brief Sets the function, which will be used, when the middle mouse button is pressed

   The function can be:
   \arg  ActorRotate()
   \arg  ActorTranslate()
   \arg  ActorZoom()
   \arg  ActorRoll()
   \arg  ActorPlaneMove()
   \arg  ActorPlaneRoll()
   \arg  CameraRotate()
   \arg  CameraTranslate()
   \arg  CameraZoom()
   \arg  CameraRoll()
   \arg  DoNothing()

   \param function the name of the function, wrapped in a slot, e.g.
   \c SLOT(CameraRotate()). If function is NULL,  DoNothing() will be used.
*/
void RenderWindowInteractor::SetMiddleButtonFunction( const char* function ) {
   if( function == NULL ) {
      strncpy( this->middleButtonFunction, SLOT(DoNothing()), 100 );
   }
   else {
      strncpy( this->middleButtonFunction, function, 100 );
   }
}


/*!
   \brief Sets the function, which will be used, when the right mouse button is pressed

   The function can be:
   \arg  ActorRotate()
   \arg  ActorTranslate()
   \arg  ActorZoom()
   \arg  ActorRoll()
   \arg  ActorPlaneMove()
   \arg  ActorPlaneRoll()
   \arg  CameraRotate()
   \arg  CameraTranslate()
   \arg  CameraZoom()
   \arg  CameraRoll()
   \arg  DoNothing()

   \param function the name of the function, wrapped in a slot, e.g.
   SLOT(CameraRotate()). If function is NULL, DoNothing() will be used.
*/
void RenderWindowInteractor::SetRightButtonFunction( const char* function ) {
   if( function == NULL ) {
      strncpy( this->rightButtonFunction, SLOT(DoNothing()), 100 );
   }
   else {
      strncpy( this->rightButtonFunction, function, 100 );
   }
}


/*!
   \brief Sets the function, which will be used, when the left button and the shift key are pressed

    The function can be:
   \arg  ActorRotate()
   \arg  ActorTranslate()
   \arg  ActorZoom()
   \arg  ActorRoll()
   \arg  ActorPlaneMove()
   \arg  ActorPlaneRoll()
   \arg  CameraRotate()
   \arg  CameraTranslate()
   \arg  CameraZoom()
   \arg  CameraRoll()
   \arg  DoNothing()

   \param function the name of the function, wrapped in a slot, e.g.
   SLOT(CameraRotate()). If function is NULL, DoNothing() will be used.
*/
void RenderWindowInteractor::SetShiftLeftButtonFunction(
            const char* function ) {
   if( function == NULL ) {
      strncpy( this->shiftLeftButtonFunction, SLOT(DoNothing()), 100 );
   }
   else {
      strncpy( this->shiftLeftButtonFunction, function, 100 );
   }
}


/*!
   \brief Sets the actor, which will be used in actor interactions.
*/
void RenderWindowInteractor::SetInteractionActor( vtkActor* actor ) {
   this->interactionActor = actor;
}


/*!
   \brief Returns the actor, which is used in actor interactions.
*/
vtkActor *RenderWindowInteractor::GetInteractionActor( void ) {
   return this->interactionActor;
}

/*!
   \brief Sets the point, in actor coordinate system, around which we rotate the
   current actor.

   \b Important: Set first the actor with
   SetInteractionActor() before using this function.
   \param x x of the rotation point
   \param y y of the rotation point
   \param z z of the rotation point
*/
void RenderWindowInteractor::SetActorRotation( double  x, double  y, double  z ) {
   ASSERT( this->interactionActor != NULL );

   // set it to the interactionActor
   this->interactionActor->SetOrigin( x, y, z );
}


/*!
   \brief catches the mouse press to react appropriate

   Overriden to catch mousePressEvents and to start an internal
   timer, which calls the appropriate interaction routine.
*/
void RenderWindowInteractor::mousePressEvent( QMouseEvent *event ) {
   if( this->mouseEventActive ) {
      return;
   }

   if( event->button() & LeftButton ) {
      if( event->state() & ShiftButton ) {
         // left mouse mouse and shift button
         this->mouseEventActive = true;
         QObject::connect( this->stepTimer, SIGNAL(timeout()),
                           this->shiftLeftButtonFunction );
      }
      else {
         // left mouse button without shift button
         this->mouseEventActive = true;
         QObject::connect( this->stepTimer, SIGNAL(timeout()),
                           this->leftButtonFunction );
      }
   }
   else if( event->button() & MidButton ) {
      // middle mouse button
      this->mouseEventActive = true;
      QObject::connect( this->stepTimer, SIGNAL(timeout()),
                        this->middleButtonFunction );
   }
   else if( event->button() & RightButton ) {
      // right mouse button
      this->mouseEventActive = true;
      QObject::connect( this->stepTimer, SIGNAL(timeout()),
                        this, this->rightButtonFunction );
   }

   if( this->mouseEventActive ) {
      this->currentMousePos[0] = event->x();
      this->currentMousePos[1] = event->y();
      this->lastMousePos[0] = event->x();
      this->lastMousePos[1] = event->y();
      this->firstCall = true;
      this->stepTimer->start( this->interactionTime );
   }
}


/*!
   \brief Overriden to catch mouseReleaseEvents and to stop the internal timer.
*/
void RenderWindowInteractor::mouseReleaseEvent( QMouseEvent *event ) {
   if( this->mouseEventActive ) {
      this->stepTimer->stop();
      this->mouseEventActive = false;
      QObject::disconnect( this->stepTimer, 0, this, 0 );
   }

   emit( Released(event->x(), this->height() - event->y()) );

}


/*!
   \brief catches all movements of the mouse

   Overriden to save the mouse coords, which are needed for the interaction
   routines.
*/
void RenderWindowInteractor::mouseMoveEvent( QMouseEvent *event ) {
   this->currentMousePos[0] = event->x();
   this->currentMousePos[1] = event->y();
}


/*!
   \brief Set the time in milliseconds between two interactions. 

   If you have huge
   objects and the interaction is too fast for the rendering, set the value
   bigger.
   Note that on some systems, the timer of qt (QTimer) can not be faster
   than the internal clock, which is around 16 msec (measured on WinNT 4.0).
   \param time the time in milliseconds
*/
void RenderWindowInteractor::SetInteractionTime( int time ) {
   this->interactionTime = time;
}

/*!
   \brief Get the time in milliseconds between two interactions.
*/
int RenderWindowInteractor::GetInteractionTime( void ) {
   return this->interactionTime;
}


/*!
   \brief Rotates an actor around a specific point

   The rotation point can be set with SetActorRotation(),
   the actor must be set with SetInteractionActor()
*/
void RenderWindowInteractor::ActorRotate() {
   double  xf;
   double  yf;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // basic idea: the change in x of the mouse is used to rotate around the
   // view-up vector. the change in y of the mouse is used to rotate around the
   // cross of the viewplane-normal and the view-up vector,

   // first time we do some preprocessing
   if( firstCall ) {
      int *size = this->GetRenderWindow()->GetSize();
      double *vp = this->GetRenderer()->GetViewport();

      this->deltaAzimuth = -20.0/((vp[2] - vp[0])*size[0]);
      this->deltaElevation = -20.0/((vp[3] - vp[1])*size[1]);

      // get the rotation axis: cross of view-up and the normal in the viewplane
      this->currentCamera->GetViewUp( this->viewUp );
      this->currentCamera->GetViewPlaneNormal( this->vpNormal );
      vtkMath::Cross( this->vpNormal, this->viewUp, this->rotateY );

      this->firstCall = false;
   }

   // how much to rotate
   xf = (this->currentMousePos[0] - this->lastMousePos[0])
            * this->trackballFactor * deltaAzimuth * (-1);
   yf = (this->currentMousePos[1] - this->lastMousePos[1])
            * this->trackballFactor * deltaElevation;

   this->interactionActor->RotateWXYZ( xf, this->viewUp[0], this->viewUp[1],
                                           this->viewUp[2] );
   this->interactionActor->RotateWXYZ( yf, this->rotateY[0], this->rotateY[1],
                                           this->rotateY[2] );

   emit ActorRotated( xf, yf );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Translates the actor on the viewplane

   The actor must be set with SetInteractionActor()
*/
void RenderWindowInteractor::ActorTranslate() {
   double *center;
   double dispCenter[4];
   double newP[4];
   double oldP[4];

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   center = this->interactionActor->GetCenter();
   this->WorldToDisplay( center[0], center[1], center[2], dispCenter );

   this->DisplayToWorld( double (this->currentMousePos[0]),
                         double (this->height() - this->currentMousePos[1]),
                         dispCenter[2],
                         newP );

   this->DisplayToWorld( double (this->lastMousePos[0]),
                         double (this->height() - this->lastMousePos[1]),
                         dispCenter[2],
                         oldP);

   this->interactionActor->AddPosition( newP[0]-oldP[0],
                                        newP[1]-oldP[1],
                                        newP[2]-oldP[2] );
   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
  \brief Zooms the actor on the normal of the viewplane

   The actor must be set with SetInteractionActor()
*/
void RenderWindowInteractor::ActorZoom() {
   double  yf;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // first time we do some preprocessing
   if( firstCall ) {
      this->currentCamera->GetViewPlaneNormal( this->vpNormal );
      firstCall = false;
   }

   yf = double (this->lastMousePos[1] - this->currentMousePos[1]) /
              double (this->height()) * this->trackballFactor * -10.0;

   this->interactionActor->AddPosition( this->vpNormal[0] * yf,
                                        this->vpNormal[1] * yf,
                                        this->vpNormal[2] * yf );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Turns an actor on the viewplane of the camera

   The actor can be set with SetInteractionActor()
*/
void RenderWindowInteractor::ActorRoll() {
   double  angle;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // first time we do some preprocessing
   if( firstCall ) {
      this->currentCamera->GetViewPlaneNormal( this->vpNormal );
      this->renCenter[0] = this->GetRenderer()->GetCenter()[0];
      this->renCenter[1] = this->GetRenderer()->GetCenter()[1];
      this->renCenter[2] = this->GetRenderer()->GetCenter()[2];
      firstCall = false;
   }

   int diffX1 = this->currentMousePos[0] - int(this->renCenter[0]);
   int diffY1 = this->currentMousePos[1] - int(this->renCenter[1]);
   int diffX2 = this->lastMousePos[0] - int(this->renCenter[0]);
   int diffY2 = this->lastMousePos[1] - int(this->renCenter[1]);

   double a1 = atan2( double(diffY1), double(diffX1) );
   double a2 = atan2( double(diffY2), double(diffX2) );
   angle = (a2 - a1) / (2.0 * 3.1415926535) * 360.0 / 10.0 *
           this->trackballFactor;
   this->interactionActor->RotateWXYZ( angle, this->vpNormal[0],
                                              this->vpNormal[1],
                                              this->vpNormal[2] );

   emit ActorRolled( angle );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Rotates the camera around the point, which was set with SetCameraRotation().

*/
void RenderWindowInteractor::CameraRotate() {
   double  xf;
   double  yf;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // first time we do some preprocessing
   if( firstCall ) {
      int *size = this->GetRenderWindow()->GetSize();
      double  *vp = this->GetRenderer()->GetViewport();

      this->deltaAzimuth = -20.0/((vp[2] - vp[0])*size[0]);
      this->deltaElevation = -20.0/((vp[3] - vp[1])*size[1]);
      firstCall = false;
   }

   xf = (this->currentMousePos[0] - this->lastMousePos[0]) * this->deltaAzimuth
            * this->trackballFactor;
   yf = (this->lastMousePos[1] - this->currentMousePos[1]) * this->deltaElevation
            * this->trackballFactor;

   this->currentCamera->Azimuth( xf );
   this->currentCamera->Elevation( yf );
   this->currentCamera->OrthogonalizeViewUp();
   this->GetRenderer()->ResetCameraClippingRange();

   // light follows the camera
   this->currentLight->SetPosition( this->currentCamera->GetPosition() );
   this->currentLight->SetFocalPoint( this->currentCamera->GetFocalPoint() );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Translates the camera along the viewplane.
*/
void RenderWindowInteractor::CameraTranslate() {
   double  newP[4];
   double  viewFocus[4];
   double  viewPoint[3];
   double motionVector[3];

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   this->currentCamera->GetFocalPoint( viewFocus );
   this->WorldToDisplay( viewFocus[0], viewFocus[1], viewFocus[2],
                         viewFocus );
   double focalDepth = viewFocus[2];

   this->DisplayToWorld( double (this->width()/2.0 + (this->currentMousePos[0]
                                 - this->lastMousePos[0])),
                         double (this->height()/2.0 - (this->currentMousePos[1]
                                 - this->lastMousePos[1])),
                         focalDepth,
                         newP );

   this->currentCamera->GetFocalPoint( viewFocus );
   this->currentCamera->GetPosition( viewPoint );

   motionVector[0] = this->trackballFactor / 10.0 * ( viewFocus[0] - newP[0] );
   motionVector[1] = this->trackballFactor / 10.0 * ( viewFocus[1] - newP[1] );
   motionVector[2] = this->trackballFactor / 10.0 * ( viewFocus[2] - newP[2] );

   this->currentCamera->SetFocalPoint( motionVector[0] + viewFocus[0],
                                       motionVector[1] + viewFocus[1],
                                       motionVector[2] + viewFocus[2]);
   this->currentCamera->SetPosition( motionVector[0] + viewPoint[0],
                                     motionVector[1] + viewPoint[1],
                                     motionVector[2] + viewPoint[2]);

   // light follows the camera
   this->currentLight->SetPosition( this->currentCamera->GetPosition() );
   this->currentLight->SetFocalPoint( this->currentCamera->GetFocalPoint() );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Zooms the camera along the normal of the viewplane.

*/
void RenderWindowInteractor::CameraZoom() {
   double  yf;

   // do nothing if mouse is still on the same y position
   if( this->currentMousePos[1] == this->lastMousePos[1] ) {
      return;
   }

   yf = double (this->currentMousePos[1]-this->lastMousePos[1]) /
              double (this->height()) * this->trackballFactor;
   // if yf < 0, we need to be between 0..1
   yf = pow( 1.1, yf );

   this->currentCamera->Dolly( yf );
   this->GetRenderer()->ResetCameraClippingRange();

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}

   
/*!
   \brief Rotates the camera on the viewplane

   This function corresponds to a roll-operation
   like in airplanes
*/
void RenderWindowInteractor::CameraRoll() {
   double  angle;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // first time we do some preprocessing
   if( firstCall ) {
      this->renCenter[0] = this->GetRenderer()->GetCenter()[0];
      this->renCenter[1] = this->GetRenderer()->GetCenter()[1];
      this->renCenter[2] = this->GetRenderer()->GetCenter()[2];
      firstCall = false;
   }

   // calculate the angle in radians and roll the camera
   int diffX1 = this->currentMousePos[0] - int(this->renCenter[0]);
   int diffY1 = this->currentMousePos[1] - int(this->renCenter[1]);
   int diffX2 = this->lastMousePos[0] - int(this->renCenter[0]);
   int diffY2 = this->lastMousePos[1] - int(this->renCenter[1]);

   double a1 = atan2( double(diffY1), double(diffX1) );
   double a2 = atan2( double(diffY2), double(diffX2) );
   angle = (a2 - a1) / (2.0 * 3.1415926535) * 360.0 / 10.0 *
           this->trackballFactor;
   this->currentCamera->Roll( angle );

   // light follows the camera
   this->currentLight->SetPosition( this->currentCamera->GetPosition() );
   this->currentLight->SetFocalPoint( this->currentCamera->GetFocalPoint() );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Moves the actor

   Moves the actor, which was set with SetInteractionActor()
   in the plane, which was set with SetPlane().
*/
void RenderWindowInteractor::ActorPlaneMove() {

   double  *center;
   double  dispCenter[4];
   double  newP[4];
   double  oldP[4];
   double  p0[3],p1[3],t;
   double  tkonstante = (this->normale[0])*(this->normale[0]) +
                      (this->normale[1])*(this->normale[1]) +
                      (this->normale[2])*(this->normale[2]) ;

   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   center = this->interactionActor->GetCenter();
   this->WorldToDisplay( center[0], center[1], center[2], dispCenter );

   this->DisplayToWorld( double (this->currentMousePos[0]),
                         double (this->height() - this->currentMousePos[1]),
                         dispCenter[2],
                         newP );

   this->DisplayToWorld( double (this->lastMousePos[0]),
                         double (this->height() - this->lastMousePos[1]),
                         dispCenter[2],
                         oldP);

   oldP[3] = oldP[0]*(this->normale[0]) +
             oldP[1]*(this->normale[1]) +
             oldP[2]*(this->normale[2]) ;

   t = (this->ebenekonstante - oldP[3])/tkonstante;

   p0[0] = oldP[0] + t*(this->normale[0]);
   p0[1] = oldP[1] + t*(this->normale[1]);
   p0[2] = oldP[2] + t*(this->normale[2]);

   newP[3] = newP[0]*(this->normale[0]) +
             newP[1]*(this->normale[1]) +
             newP[2]*(this->normale[2]);

   t = (this->ebenekonstante - newP[3])/tkonstante;

   p1[0] = newP[0] + t*(this->normale[0]);
   p1[1] = newP[1] + t*(this->normale[1]);
   p1[2] = newP[2] + t*(this->normale[2]);

   this->interactionActor->AddPosition( p1[0]-p0[0],
                                        p1[1]-p0[1],
                                        p1[2]-p0[2] );
   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];
}


/*!
   \brief Rolls the actor

   Rolls the actor, which was set with SetInteractionActor()
   in the plane, which was set with SetPlane().
*/
void RenderWindowInteractor::ActorPlaneRoll() {

   double  angle;

   // do nothing if mouse is still on the same pos
   if( (this->currentMousePos[0] == this->lastMousePos[0]) &&
       (this->currentMousePos[1] == this->lastMousePos[1]) ) {
      return;
   }

   // first time we do some preprocessing
   if( firstCall ) {
      this->renCenter[0] = this->GetRenderer()->GetCenter()[0];
      this->renCenter[1] = this->GetRenderer()->GetCenter()[1];
      this->renCenter[2] = this->GetRenderer()->GetCenter()[2];
      firstCall = false;
   }

   int diffX1 = this->currentMousePos[0] - int(this->renCenter[0]);
   int diffY1 = this->currentMousePos[1] - int(this->renCenter[1]);
   int diffX2 = this->lastMousePos[0] - int(this->renCenter[0]);
   int diffY2 = this->lastMousePos[1] - int(this->renCenter[1]);

   double a1 = atan2( double(diffY1), double(diffX1) );
   double a2 = atan2( double(diffY2), double(diffX2) );
   angle = (a2 - a1) / (2.0 * 3.1415926535) * 360.0 / 10.0 *
           this->trackballFactor;
   this->interactionActor->RotateWXYZ( -angle, this->normale[0],
                                              this->normale[1],
                                              this->normale[2] );

   // update
   this->updateGL();

   // save mouse pos
   this->lastMousePos[0] = this->currentMousePos[0];
   this->lastMousePos[1] = this->currentMousePos[1];

}


/*!
   \brief Does nothing.

   This is needed, when a Button should have no interaction meaning.
*/
void RenderWindowInteractor::DoNothing() {
}


/*!
   \brief Computes the display point defined with \a x, \a y and \a z to world coordinates.

   \param x x of the display point
   \param y y of the display point
   \param z z of the display point
   \param worldPoint the resulting world point (size 4 double s).
*/
void RenderWindowInteractor::DisplayToWorld( double  x, double  y, double  z,
                                             double  *worldPoint ) {
   this->GetRenderer()->SetDisplayPoint( x, y, z );
   this->GetRenderer()->DisplayToWorld();
   this->GetRenderer()->GetWorldPoint( worldPoint );

   // normalize
   if (worldPoint[3]) {
      worldPoint[0] = worldPoint[0] / worldPoint[3];
      worldPoint[1] = worldPoint[1] / worldPoint[3];
      worldPoint[2] = worldPoint[2] / worldPoint[3];
      worldPoint[3] = 1.0;
   }
}


/*!
   \brief Computes the world point defined with \a x, \a y and \a z to display coordinates.

   \param x x of the world point
   \param y y of the world point
   \param z z of the world point
   \param displayPoint the resulting display point (size 3 double s).
*/
void RenderWindowInteractor::WorldToDisplay( double  x, double  y, double  z,
                                             double  *displayPoint ) {
  this->GetRenderer()->SetWorldPoint(x, y, z, 1.0 );
  this->GetRenderer()->WorldToDisplay();
  this->GetRenderer()->GetDisplayPoint( displayPoint );
}
