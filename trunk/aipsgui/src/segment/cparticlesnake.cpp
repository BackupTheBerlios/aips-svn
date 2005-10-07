/************************************************************************
 * File: cparticlesnake.cpp                                             *
 * Project: AIPS                                                        *
 * Description: Implementation of the discrete particle snake           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-11-15                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cparticlesnake.h"
#include "libid.h"
#include <cmath>
#include <cfloat>

using namespace blitz;
using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
CParticleSnake::CParticleSnake( ulong ulID_ ) throw() 
	: CFilter ( ulID_, "Snake", 4, 1, "CSnake", "0.1", "CFilter" )
{
  setModuleID( sLibID );

	sDocumentation = "An implementation of the 2D particle snake\n"
									 "** Input ports:\n"
                   " 0: 2D input image\n"
									 " 1: 2D gradient field (external force)\n"
									 " 2: Start polygon (Array of points)\n"
									 " 3: (Optional) Mask image to restrict area of movement\n"
                   "** Output ports:\n"
                   " 0: 2D binary image: Contour representing the segmentation"
									 "** Parameters:\n"
									 " Iterations: Maximum number of iterations\n"
									 " MeanEdgeDistance: Starting mean distance of two particles\n"
									 " InternalWeight: Weight of internal forces\n"
									 " BalloonForce: Weight of the balloon force\n"
									 " OnlyUseDirection: Normalize external force field\n"
									 " StopCriterion: # ierations after which a particle is declared as stable\n"
									 " DeltaT: Timestep\n"
									 " MultiScale: Use multiscale approach (with minimum distance given)\n"
									 " Template: Use template polygon\n"
									 " Mask: Use movement restriction mask\n"
									 " ShowEvery: Only show every n-th frame\n"
									 " Movie: Save segmentation as movie\n"
									 " ShowNormals: Display normal vectors";
	
	parameters.initUnsignedLong( "Iterations", 1000, 0, 10000 );
  parameters.initDouble( "MeanEdgeDistance", 8.0, 1.0, 1000.0 );
  parameters.initDouble( "InternalWeight", 0.0, 0.0, 10.0 );
  parameters.initDouble( "BalloonForce", 0.0, -10.0, 10.0 );
  parameters.initUnsignedLong( "StopCriterion", 50, 0, 1000 );
  parameters.initDouble( "DeltaT", 0.5, 0.0, 10.0 );
	parameters.initUnsignedLong( "MultiScale", 0, 0, 100 );
	parameters.initDouble( "TemplateWeight", 0.0, 0.0, 10.0 );
	parameters.initUnsignedLong( "Mask", 0, 0, 1000 );
	parameters.initBool( "Movie", false );
	parameters.initUnsignedLong( "ShowEvery", 1, 1, 1000 );
	parameters.initBool( "ShowNormals", false );
	parameters.initBool( "OnlyUseDirection", false );
	
// 	parameters.initDouble( "Template weight", 0.0, 0.0, 100.0 );					  
//	parameters.initLong( "Search tendency", 0, -1, 1 );

  inputsVec[0].portType = CPipelineItem::IO2DVector;
  inputsVec[1].portType = CPipelineItem::IO2DInteger;
  inputsVec[2].portType = CPipelineItem::IO1DVector;
	inputsVec[3].portType = CPipelineItem::IO2DInteger;
/* HB 28-06-05 */		
	theDialog.reset( new CSnakeDialog() );
	setModuleDialog( theDialog );
}

CParticleSnake::~CParticleSnake() throw()
{
}

/*****************
 * Other methods *
 *****************/

/** \param imagePtr Pointer to image to be drawn */
void CParticleSnake::drawImage( TImagePtr imagePtr )
{
/*  if ( imagePtr.get() != NULL && imagePtr->getExtent( 0 ) > 0 && imagePtr->getExtent( 1 ) > 0 )
  {
    theImage.create( imagePtr->getExtent( 0 ), imagePtr->getExtent(1), 32, 256 * 256 * 256 );
    float fIntRange = 1.0 / static_cast<float>
      ( imagePtr->getMaximum() - imagePtr->getMinimum() + 1 ) * 256.0;
    for ( uint x = 0; x < imagePtr->getExtent( 0 ); x++ )
      for ( uint y = 0; y < imagePtr->getExtent( 1 ); y++ )
      {
        if ( imagePtr->getDataDimension() == 1 )
        {
          ushort usValue = static_cast<ushort>( static_cast<float>
            ( (*imagePtr)( x, y ) - imagePtr->getMinimum() ) * fIntRange );
          if ( usValue < 256 ) 
						theImage.setPixel( x, imagePtr->getExtent(1) - 1 - y, qRgb( usValue, usValue, usValue ) );
          else
          { 
						alog << LWARN << "\n **** Pixel value " << usValue << " too high (" 
							<< imagePtr->getMaximum() << ") ****" << endl; 
					}
        }
        else if ( imagePtr->getDataDimension() == 3 )
          theImage.setPixel( x, imagePtr->getExtent(1) - 1 - y, 
						qRgb( (*imagePtr)( x, y, 0 ), (*imagePtr)( x, y, 1 ), (*imagePtr)( x, y, 2 ) ) );
        else if ( imagePtr->getDataDimension() == 4 )
          theImage.setPixel( x, imagePtr->getExtent(1) - 1 - y, qRgba( (*imagePtr)( x, y, 0 ),
						(*imagePtr)( x, y, 1 ), (*imagePtr)( x, y, 2 ), (*imagePtr)( x, y , 3 ) ) );
      }
  }*/
}

CPipelineItem* CParticleSnake::newInstance( ulong ulID_ ) const throw()
{
  return new CParticleSnake( ulID_ );
}

void CParticleSnake::displaySnake() 
{
/*	static ulong frame = 0;
	theBuffer.resize( 2*iWidth, 2*iHeight );
	QPainter p( &theBuffer );
	p.scale(2.0,2.0);
  p.drawImage( 0, 0, theImage );
  p.moveTo( static_cast<int>( round( (*vertexList.begin()).position[0] ) ), 
		static_cast<int>( iHeight - 1 - round( (*vertexList.begin()).position[1] ) ) );
	p.setPen(Qt::red);	
	TPartIterator listEnd = vertexList.end();
  for ( TPartIterator actv = vertexList.begin(); 
		actv != listEnd; ++actv )
  {
    p.lineTo( static_cast<int>( round( (*actv).position[0]) ), 
			iHeight - 1 - static_cast<int>( round( (*actv).position[1] ) ) );
		if ( bShowNormals )
		{
			p.setPen( Qt::green );
			p.drawLine( static_cast<int>( round( (*actv).position[0] ) ), 
				iHeight - 1 - static_cast<int>( round( (*actv).position[1] ) ), 
				static_cast<int>( round( (*actv).position[0] + 5 * (*actv).normal[0] ) ), 
				iHeight - 1 - static_cast<int>( round( (*actv).position[1] + 5 * (*actv).normal[1] ) ) );
		}
		p.setPen( Qt::white );
		p.moveTo( static_cast<int>( round( (*actv).position[0] ) ), 
			iHeight - 1 - static_cast<int>( round( (*actv).position[1] ) ) );
	}
  p.lineTo( static_cast<int>( round( (*vertexList.begin() ).position[0] ) ),
		iHeight - 1 - static_cast<int>( round( (*vertexList.begin() ).position[1] ) ) );
  p.flush();		
  p.end();	
	theDialog->setPixmap( theBuffer );
	if ( bMovie )
	{
		string number = lexical_cast<string>( frame );
		while ( number.length() < 5 )
			number = "0" + number;
		theBuffer.save( "snake_f"+number+".bmp", "BMP", 100);
		frame++;
	}*/
}

void CParticleSnake::apply() throw()
{
FBEGIN;
	bModuleReady = false;
  TField2DPtr inputForcePtr = static_pointer_cast<TField2D>( getInput() );
	TImagePtr inputImagePtr = static_pointer_cast<TImage>( getInput(1) );
	TField2DPtr inputPolyPtr = static_pointer_cast<TField2D>( getInput(2) );
  if ( !checkInput<TField2D>( inputForcePtr ) || !checkInput<TImage>( inputImagePtr ) || !checkInput<TField2D>( inputPolyPtr )) 
		return; 
  
	maskPtr = static_pointer_cast<TImage>(getInput(3));
  if ( !checkInput<TImage>( maskPtr ) ) 
	{ 
		alog << LINFO << "No mask image given. Substituting global mask" << endl; 
		maskPtr.reset( new TImage( inputImagePtr->getDimension(), inputImagePtr->getExtents() ) );
		(*maskPtr) = 1;
		maskPtr->setMinimum( 0 );
		maskPtr->setMaximum( 1 );
	}
	
  bModuleReady = true;
	deleteOldOutput();  
	
	dInternalWeight = parameters.getDouble("InternalWeight");
	dDelta = parameters.getDouble("DeltaT");
	dMeanDistance = parameters.getDouble( "MeanEdgeDistance" );
	dBalloonWeight = parameters.getDouble( "BalloonForce" );
	ulStop = parameters.getUnsignedLong( "StopCriterion" );
	usMaskIntensity = parameters.getUnsignedLong ( "Mask" );
	ulong fr = parameters.getUnsignedLong ( "ShowEvery" );
	dTemplateWeight = parameters.getDouble( "TemplateWeight" );
	bNormalize = parameters.getBool( "OnlyUseDirection" );
	bStop = false;	
	bStable = false;

  iWidth = inputImagePtr->getExtent( 0 );
  iHeight = inputImagePtr->getExtent( 1 );
  //theBuffer.resize( iWidth, iHeight );
	
	ulong ulMultiScale = parameters.getUnsignedLong("MultiScale");
  TVector2D maxForce = 0.0;
  ulong ulMaxIterations = parameters.getUnsignedLong( "Iterations" ); 
	ulong ulTotalIterations = 0;
	
	externalForceFieldPtr.reset( new TField2D( *inputForcePtr ) );
  
  ulong t = 0; // First iteration is Zero

	drawImage( inputImagePtr );
	generateInitialContour( inputPolyPtr );
	displaySnake();
	bMovie = parameters.getBool("Movie");
	bShowNormals = parameters.getBool("ShowNormals");
PROG_MAX( ulMaxIterations );
	bool bTotalStop = false;
	// Global loop (for multiscale model)	
DBG3( "Entering main model loop" );
	while ( bTotalStop == false )
	{
DS( "-- Multiscale loop" );		
		// Model loop
	  while ( t < ulMaxIterations && bStop == false )
  	{
APP_PROC();
PROG_VAL( t );	
	    remesh();		
			checkProximity();
			calculateNormals();
			computeInternalForces();
			computeExternalForces();
			if ( dBalloonWeight > 0.0 )
				computeBalloonForces();
			if ( dTemplateWeight > 0.0 )
				computeTemplateForces();
			checkStability();		
			updateSnake();
			if ( t % fr == 0 ) displaySnake();
	    t++;	
			if ( ulStableNodes == vertexList.size() ) 
			{
				bStop = true;	
				ulTotalIterations += t;
			}
  	}
		// Check and update multiscale discretization
		if ( ulMultiScale > 0 && round(dMeanDistance) > static_cast<double>( ulMultiScale ) ) 
		{
			cerr << "Reducing scaling from " << dMeanDistance << " to " << dMeanDistance / 2.0 << endl;
			dMeanDistance /= 2.0;
			dInternalWeight *= 4.0;
			dBalloonWeight /= 2.0;
			generateNewContour( vertexList );
			bStop = false;			
			t = 0;
			bStable = false;
		}
		else bTotalStop = true;		
	}
	
		
	alog << LINFO << "Particle snake finished. Resulting polygon has " << vertexList.size() << " vertices" << endl;
	alog << "-- Algorithm took " << ulTotalIterations << " iterations " << endl;

	// Draw output contour	
  TImagePtr outputPtr ( new TImage( 2, inputImagePtr->getExtents() ) );
	(*outputPtr) = 0;
  outputPtr->setMaximum(255);
  outputPtr->setMinimum(0); 
/*  TPartIterator listEnd = vertexList.end();
   for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
   {
     (*outputPtr)(static_cast<int>( round( (*actv).position[0] ) ),
 			static_cast<int>( round( (*actv).position[1] ) ) ) = 255;
   }*/
/* 	listEnd = vertexList.end();
   for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
   {
     pa.setPoint( index, static_cast<int>( round( (*actv).position[0] ) ),
 			static_cast<int>( round( (*actv).position[1] ) ) );

   }*/
  QPixmap pic( inputImagePtr->getExtent(0), inputImagePtr->getExtent(1) );
	pic.fill( Qt::black );
  QPainter p(&pic);
  QPointArray pa( vertexList.size() * 2 );
  p.setPen( Qt::red );
  p.setBrush( Qt::red );
  int index = 0;
	TPartIterator listEnd = vertexList.end();
  for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
  {
    pa.setPoint( index, static_cast<int>( round( (*actv).position[0] ) ),
			static_cast<int>( round( (*actv).position[1] ) ) );
    ++index;
  }
	listEnd = vertexList.end();
  for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
  {
    pa.setPoint( index, static_cast<int>( round( (*actv).position[0] ) ),
			static_cast<int>( round( (*actv).position[1] ) ) );
    ++index;
  }

	p.drawPolyline( pa );
  p.end();
  QImage i = pic.convertToImage();
  for ( ushort y = 0; y < outputPtr->getExtent(1); ++y )
    for ( ushort x = 0; x < outputPtr->getExtent(0); ++x )
      (*outputPtr)( x, y ) = qRed( i.pixel( x, y ) );
  vertexList.clear();
  setOutput( outputPtr );
PROG_RESET();		
FEND;
}

void CParticleSnake::remesh()
{
  for ( TPartIterator it = vertexList.begin();	it != vertexList.end(); ++it )
  {
    TPartIterator next = it; 
		++next;    
    if ( next == vertexList.end() )
    {
			next = vertexList.begin();
    }
		TVector2D particleDistance = next->position - it->position;
    double dLength = norm( particleDistance );
		// Check if edge is too large. In this case, insert another vertex
    if ( dLength > dMeanDistance * 2.0 ) 
    {
      SParticle newVertex;
      newVertex.position = it->position + ( 0.5 * particleDistance );
			newVertex.stability = 0;
			newVertex.isReference = false;
			newVertex.reference = NULL;
      vertexList.insert( next, newVertex );
      ++it;
    }
		// Now check if one of the adjacent edges is too small. In this case, try to erase the
		// current vertex. If the current vertex is a ref vertex, erase its successor instead.
		// If this is also not possible ( next is also a ref vertex), do nothing and pray nothing
		// goes wrong ;-)
    else if ( dLength < ( dMeanDistance * 0.5 ) )
		{
			if  ( !it->isReference )
			// Edge is too small
  	  {
	      next->position = it->position + ( 0.5 * particleDistance );						
  	    vertexList.erase( it );
    	  it = next;
	    }		
/*			else if ( next->stability > -1 )
			{
 FIXME 
				DBG("Yuk!");
	      //it->position = next->position - ( 0.5 * difference );						
  	    vertexList.erase( next );
			}*/
		}		
  }   
}

void CParticleSnake::checkProximity()
{
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
	{
    TPartIterator successor = actv;
    ++successor;
    if ( successor == listEnd ) 
			successor = vertexList.begin();
    for ( TPartIterator partner = successor; partner != listEnd; ++partner )
		{					
			list<SParticle>::iterator partnersSuccessor = partner;
	    partnersSuccessor++;
  	  if ( partnersSuccessor  == listEnd ) 
				partnersSuccessor = vertexList.begin();
			if ( partner != actv && partnersSuccessor != actv 
				&& partner != successor && partnersSuccessor != successor )
			{
				TVector2D particleDistance = edgeDistance( actv->position, successor->position, 
					partner->position, partnersSuccessor->position );
				double dMinimalDistance = dMeanDistance * 0.1;
				double dDistance = norm( particleDistance );
				if ( trueIn( dDistance, numeric_limits<double>::epsilon(), dMinimalDistance ) )
				{
					double dFactor = ( dMinimalDistance - dDistance ) / ( 10.0 * dDistance );
					actv->force += particleDistance * dFactor;
					successor->force += particleDistance * dFactor;
					partner->force += -particleDistance * dFactor;
					partnersSuccessor->force += -particleDistance * dFactor;
				}
			}
		}		
	}
}

void CParticleSnake::calculateNormals()
{
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
  {		
    TPartIterator successor = actv;
    successor++;
    if ( successor == listEnd ) successor = vertexList.begin();
    TPartIterator predecessor = actv;
    if ( predecessor == vertexList.begin() ) predecessor = listEnd;
    predecessor--;
		
		TVector2D d1 = predecessor->position / 2.0;
		TVector2D d2 = successor->position / 2.0;
		actv->curvature = d1 + d2 - actv->position;
		TVector2D m1 ( predecessor->position[1] - actv->position[1], actv->position[0] - predecessor->position[0] ); 
		TVector2D m2 ( actv->position[1] - successor->position[1], successor->position[0] - actv->position[0] );
		actv->normal = m1 + m2; 
		actv->normal /= norm( actv->normal );
		if ( isnan( actv->normal[0] ) || isnan( actv->normal[1] ) )
		{
			DBG( "Extreme curvature alert!" );
			// This means pre- and successor are on equal positions. We need to define the normal manually
			actv->normal = actv->position - predecessor->position;
			actv->normal /= norm( actv->normal );
		}
	}
}

void CParticleSnake::computeInternalForces()
{
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
  {
    TPartIterator successor = actv;
    successor++;
    if ( successor == listEnd ) successor = vertexList.begin();
    TPartIterator predecessor = actv;
    if ( predecessor == vertexList.begin() ) predecessor = listEnd;
    predecessor--;
		TVector2D innerForce = 0.0;
		
		if ( dInternalWeight > numeric_limits<double>::epsilon() 
			&& norm(actv->curvature) > numeric_limits<double>::epsilon() )
		{
			innerForce = actv->curvature; 
			double dForceStrength = dot( actv->curvature, actv->normal );
			innerForce *= dForceStrength;
			TVector2D d1 = predecessor->position / 2.0;
			TVector2D d2 = successor->position / 2.0;
			innerForce *= -dInternalWeight;
			actv->curvature = d1 + d2 - ( actv->position + innerForce );
			TVector2D unshrinkForce = actv->curvature;
			dForceStrength = dot( actv->curvature, actv->normal );
			unshrinkForce *= dForceStrength;
			unshrinkForce *= ( dInternalWeight * 1.1 );
			innerForce += unshrinkForce;
			// If force is too high, this can lead to problems. So we set the maximum norm of the force to 1.0
			if ( norm( innerForce ) > 1.0 ) 
				innerForce /= norm( innerForce );
				
			// Again, we may have problems if the force does not converge
			if ( isnan( innerForce[0] ) || isnan( innerForce[1] ) )
			{
				DBG("Inner force is NaN");
				innerForce = 0.0;
			}
	  }
		if ( norm( innerForce ) > numeric_limits<double>::epsilon() )
			actv->force += innerForce;
	}
}

void CParticleSnake::computeExternalForces()
{
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
  {
		const TVector2D& pos = actv->position;
		TVector2D ro, ru, lo, lu;
  	double rol, rul, lol, lul;
	  ro[0] = static_cast<int>( ceil( pos[0] ) );  ro[1] = static_cast<int>( floor( pos[1] ) );
  	ru[0] = static_cast<int>( ceil( pos[0] ) );  ru[1] = static_cast<int>( ceil( pos[1] ) );
	  lo[0] = static_cast<int>( floor( pos[0] ) ); lo[1] = static_cast<int>( floor( pos[1] ) );
  	lu[0] = static_cast<int>( floor( pos[0] ) ); lu[1] = static_cast<int>( ceil( pos[1] ) );
	  rul = sqrt(2.0) - norm( ru - pos );
  	rol = sqrt(2.0) - norm( ro - pos );
	  lul = sqrt(2.0) - norm( lu - pos );
  	lol = sqrt(2.0) - norm( lo - pos );
	  double sum = rol + rul + lul + lol;
  	TVector2D eforce =
    	( rol /sum ) * (*externalForceFieldPtr)( static_cast<ushort>( ro[0] ), static_cast<ushort>( ro[1] ) )
	    + ( rul /sum ) * (*externalForceFieldPtr)( static_cast<ushort>( ru[0] ), static_cast<ushort>( ru[1] ) )
  	  + ( lol /sum ) * (*externalForceFieldPtr)( static_cast<ushort>( lo[0] ), static_cast<ushort>( lo[1] ) )
    	+ ( lul /sum ) * (*externalForceFieldPtr)( static_cast<ushort>( lu[0] ), static_cast<ushort>( lu[1] ) );
    if ( bNormalize )
    	eforce /= norm(eforce);
    eforce = dot(actv->normal,eforce) * actv->normal;
  	actv->force += eforce;
	}
}

void CParticleSnake::computeTemplateForces()
{
	for( TPartIterator it = templateList.begin(); it != templateList.end(); ++it )
	{
		TPartIterator next;	next = it; ++next;
		TPartIterator last;	
		if ( it == templateList.begin() )
		{
			last = templateList.end();
			--last;
		}
		else
		{			
			last = it; 
			--last;
		}
			
		if ( next == templateList.end() )
		{
			next = templateList.begin();	
		}
			
		TVector2D a = it->reference->position - last->reference->position;
		TVector2D b = next->reference->position - it->reference->position;
		TVector2D n1( -a[1], a[0] ); 
		TVector2D n2( -b[1], b[0] ); 
		TVector2D vertexNormal( n1 + n2 );
		vertexNormal /= norm( vertexNormal );			
		it->reference->angle = 2 * M_PI - acos( dot( a * - 1.0, vertexNormal ) / norm( a ) )
			- acos( dot(b,vertexNormal) / norm(b) );
		double dFactor = it->reference->angle / it->angle;
		if ( dFactor < 1.0 )
			dFactor = - 1.0 * it->angle / it->reference->angle;
		dFactor *= dTemplateWeight;
		it->reference->force += it->reference->normal * dFactor;
//alog << "Template force strength is " << factor << endl;
	}
}

void CParticleSnake::checkStability()
{
}

void CParticleSnake::updateSnake()
{
	ulStableNodes = 0;
	dMeanForce = 0.0;
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
	{
		if ( actv->stability != -1 )
		{
			// Cap forces that are too large
			if ( norm( actv->force ) > 1.0 )
				actv->force /= norm( actv->force );
			actv->oldpos = actv->position;
			actv->position += ( actv->force * dDelta );
			dMeanForce += ( norm( actv->force ) * dDelta );
			
			// Cap positions outside of image space
			if ( actv->position[0] < 0.0 ) 
				actv->position[0] = 0.0;
			else if ( actv->position[0] > iWidth ) 
				actv->position[0] = iWidth;
			if ( actv->position[1] < 0.0 ) 
				actv->position[1] = 0.0;
			else if ( actv->position[1] > iHeight ) 
				actv->position[1] = iHeight;
				
			if ( (*maskPtr)( static_cast<ushort>( actv->position[0] ), static_cast<ushort>( actv->position[1] ) ) 
				!= usMaskIntensity 
				&& (*maskPtr)( static_cast<ushort>( actv->oldpos[0] ), static_cast<ushort>( actv->oldpos[1] ) ) 
				== usMaskIntensity )
				actv->position = actv->oldpos;
			if ( bStable )
			{
				if ( (static_cast<ushort>( round( actv->position[0] ) ) 
					== static_cast<ushort>( round( actv->oldpos[0] ) ) )
					&& (static_cast<ushort>( round( actv->position[1] ) ) 
					== static_cast<ushort>( round( actv->oldpos[1] ) ) ) )
					actv->stability++;
				if ( actv->stability >= static_cast<long>( ulStop ) )
					actv->stability = -1;
			}
		}
		else
		{
			ulStableNodes++;
		}		
		actv->force = 0.0;
	}
	if ( ( dMeanForce / static_cast<double>( vertexList.size() ) ) > 0.075 ) 
		bStable = false;
	else 
		bStable = true;
}

void CParticleSnake::generateInitialContour( TField2DPtr polygon )
{
	vertexList.clear();
	templateList.clear();
	TField2D& initialField = (*polygon);
  for ( TField2D::iterator it = initialField.begin(); it != initialField.end(); ++it )
  {		
//cerr << *it << endl;
    SParticle newVertex;
    newVertex.position = *it;
		newVertex.oldpos = 0.0;
		if ( dTemplateWeight > 0.0 )
			newVertex.isReference = true;
		newVertex.stability = 0;
		newVertex.force = 0.0;
		vertexList.push_back( newVertex );
		if ( dTemplateWeight > 0.0 )
		{
			templateList.push_back( newVertex );
			vertexList.back().reference = &templateList.back();
			templateList.back().reference = &vertexList.back();
		}
	}
	if ( dTemplateWeight > 0.0 )
	{
		for( TPartIterator it = templateList.begin(); it != templateList.end(); ++it )
		{
			TPartIterator next;	next = it; ++next;
			TPartIterator last;	
			if ( it == templateList.begin() )
			{
				last = templateList.end();
				--last;
			}
			else
			{			
				last = it; 
				--last;
			}
			
			if ( next == templateList.end() )
			{
				next = templateList.begin();	
			}
			
			TVector2D a = it->position - last->position;
			TVector2D b = next->position - it->position;
			TVector2D n1( -a[1], a[0] ); 
			TVector2D n2( -b[1], b[0] ); 
			TVector2D vertexNormal( n1 + n2 );
			vertexNormal /= norm( vertexNormal );			
			it->angle = 2 * M_PI - acos( dot ( a * - 1.0, vertexNormal ) / norm( a ) )
				- acos( dot( b, vertexNormal ) / norm( b ) );						
		}
		for( TPartIterator it = templateList.begin(); it != templateList.end(); ++it )
		{
			alog << it->position << " --- " << it->angle << endl;
		}
	}
}

/** \param polygon old model polygon to be refined */
void CParticleSnake::generateNewContour( std::list<SParticle>& polygon )
{
	TPartIterator listEnd = polygon.end();
	for ( TPartIterator actv = polygon.begin();	actv != listEnd; ++actv )
	{
    actv->oldpos = 0.0;
		actv->stability = -1;
		actv->force = 0.0;
	}
}

void CParticleSnake::computeBalloonForces()
{	
	TPartIterator listEnd = vertexList.end();
	for ( TPartIterator actv = vertexList.begin(); actv != listEnd; ++actv )
	{
		actv->force += actv->normal * dBalloonWeight;
	}
}

/**  
 * \param p0 Starting point of first edge
 * \param p1 End point of first edge
 * \param q0 Starting point of second edge
 * \param q1 End point of second edge
 * \returns Distance vector connecting both edges
 */
TVector2D CParticleSnake::edgeDistance( const TVector2D& p0, const TVector2D& p1, 
	const TVector2D& q0, const TVector2D& q1 )
{
	TVector2D u = p1 - p0;
	TVector2D v = q1 - q0;
	TVector2D w = p0 - q0;
	double a = dot( u, u );
	double b = dot( u, v );
	double c = dot( v, v );
	double d = dot( u, w );
	double e = dot( v, w );
	double D = a * c - b * b;
	double sc, sN, sD = D;
	double tc, tN, tD = D;
	if ( D > DBL_EPSILON )
	{
		sN = ( b * e - c * d );
		tN = ( a * e - b * d );
		if ( sN < 0.0 ) 
		{
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if ( sN > sD )
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}
	else
	{
		sN = 0.0; 
		sD = 1.0; 
		tN = e; 
		tD = c;
	}

	if ( tN < 0.0 )
	{
		tN = 0.0;
		if ( -d < 0.0 )
			sN = 0.0;
		else if ( -d > a )
			sN = sD;
		else
		{
			sN = -d; 
			sD = a;
		}
	}
	else if ( tN > tD )
	{
		tN = tD;
		if ( ( -d + b ) < 0.0 )
			sN = 0;
		else if ( ( -d + b ) > a )
			sN = sD;
		else
		{
			sN = -d;
			sD = a;
		}
	}
	
	sc = ( std::abs(sN) > DBL_EPSILON ? sN/sD : 0.0 );
	tc = ( std::abs(tN) > DBL_EPSILON ? tN/tD : 0.0 );
	TVector2D dP = w + ( sc * u ) - ( tc * v );
	return dP;
}
