/************************************************************************
 * File: cparticlesnake.h                                               *
 * Project: AIPS                                                        *
 * Description: Implementation of the discrete particle snake           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-11-15                                                  *
 * Changed: 2004-12-08 Added balloon forces and intersection solver     *
 *          2004-12-15 Remade representation of internal forces         *
 *          2005-01-20 Added optional mask parameter                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CPARTICLESNAKE_H
#define CPARTICLESNAKE_H

#include <cfilter.h>
#include <aipsnumeric.h>
 #include <csnakedialog.h>
#include <cglobalprogress.h>
  #include <qimage.h>
  #include <qpixmap.h>
  #include <qpainter.h>

using namespace aips;

/** Representation of a model particle */
struct SParticle
{
	TVector2D position;  ///< Actual position of particle
	TVector2D normal;    ///< Contour normal at particle position
	TVector2D curvature; ///< Curvature at particle position
	TVector2D force;     ///< Force to apply in actual timestep
	TVector2D oldpos;    ///< Position in last timestep
	int stability;       ///< Stability counter
	bool isReference;
	SParticle* reference;
	double angle;
};

/**
 * Implementation of the discrete particle snake
 * @author Hendrik Belitz
 */
class CParticleSnake : public CFilter
{
public:
/* Structors */
	/// Constructor
  CParticleSnake( ulong ulID_ ) throw();
	/// Destructor
  virtual ~CParticleSnake() throw();
/* Other methods */	
	/// Reimplemented from CFilter
  virtual CPipelineItem* newInstance( ulong ulID_ ) const
    throw();
	/// Reimplemented from CFilter
  virtual void apply()
    throw();
private:
	/// Draw the input image into the display buffer
	void drawImage( TImagePtr img );
	/// Generates start polygon from input port data
	void generateInitialContour( TField2DPtr poly );
	/// Adaptive remeshing scheme
	void remesh();		
	/// Heuristic for intersection avoidance
	void checkProximity();
	/// Calculate all contour normals
	void calculateNormals();
	/// Computes the internal forces
	void computeInternalForces();
	/// Interpolation of external forces
	void computeExternalForces();		
	/// Computation of balloon forces
	void computeBalloonForces();
	/// Computation of template forces
	void computeTemplateForces();
	/// Checks for particle stability
	void checkStability();		
	/// Updates the whole model
	void updateSnake();
	/// Display the actual state of the model
	void displaySnake();
	/// Generates a new contour model for the next multiscale step
	void generateNewContour( std::list<SParticle>& polygon );
	/// Computes the minimal distance of two edges
	TVector2D edgeDistance( const TVector2D& p0, const TVector2D& p1, 
		const TVector2D& q0, const TVector2D& q1 );	
private:	
	QPixmap theBuffer;                         ///< Buffer to draw vector field into
	QImage theImage;                           ///< Corresponding image to be shown
	int iWidth;                                ///< Input image width
	int iHeight;                               ///< Input image height
	boost::shared_ptr<CSnakeDialog> theDialog; ///< Associated model dialog
	
	typedef std::list<SParticle> TPartList;
	typedef std::list<SParticle>::iterator TPartIterator;
	TPartList vertexList;   ///< List of all model particles
	TPartList templateList;   ///< List of all template polygon vertices
	TField2DPtr externalForceFieldPtr; ///< Pointer to external forcefield
	TImagePtr maskPtr;                 ///< Pointer to image mask
	bool bStop;                        ///< Use to signal a computation stop
	bool bStable;                      ///< Signals a stable model
	bool bNormalize;									 ///< External forces should be normalized
	double dTemplateWeight;						 ///< Weight of template forces	
	double dDelta;                     ///< Timestep
	double dInternalWeight;            ///< Weight of internal forces
	double dBalloonWeight;             ///< Weight of balloon forces
	double dMeanDistance;              ///< Mean distance between particles
	double dMeanForce;                 ///< Mean force in one timestep
	ulong ulStop;                      ///< Stop criterion
	ulong ulStableNodes;               ///< Number of model particles which are already in the stable condition
	ushort usMaskIntensity;            ///< Intensity in mask image to use as a mask
	bool bMovie;
	bool bShowNormals;
};

#endif
