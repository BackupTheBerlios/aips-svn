/***********************************************************************
 * File: cvectorflow.h                                                 *
 * Project: AIPS                                                       *
 * Description: Implementation of the GVF and GGVF algorithms          *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 01.09.03                                                   *
 ***********************************************************************/
 
#include "cvectorflow.h"

using namespace std;
using namespace boost;

/************* 
 * Structors *
 *************/

CVectorFlow::CVectorFlow( ulong ulID ) throw()
  : CFilter ( ulID, "Gradient vector flow", 2, 1, "CVectorFlow", "0.7", "CFilter" )
{
  setModuleID( sLibID );

	sDocumentation = "Computes the gradient vector flow of a vector field\n"
                   "** Input ports:\n"
                   "0: A 2D or 3D single channel vector field.\n"
									 "1: Area of interest binary mask (optional).\n"
                   "** Output ports:\n"
                   "1: A 2D or 3D single channel vector field \n"
									 "** Parameters:\n"
									 "Iterations: Number of iterations of the finite difference scheme\n"
									 "Method: 0 - GVF, 1 - GGVF, 2 - GGVF with blitz++\n"
									 "DeltaT: time step\n"
									 "Mu:\n"
									 "Kappa: Smoothing parameter\n";

  parameters.initUnsignedLong( "Iterations", 10UL, 1UL, 10000UL );
#ifdef USE_BLITZ
  parameters.initUnsignedLong( "Method", 2UL, 0UL, 2UL );
#else
  parameters.initUnsignedLong( "Method", 1UL, 0UL, 1UL );
#endif
  parameters.initDouble( "DeltaT", 0.125, 0.000001, 1.0 );
  parameters.initDouble( "Mu", 1.0, 0.0, 10.0 );
  parameters.initDouble( "Kappa", 0.25, 0.0, 10.0 );
   
  inputsVec[0].portType = CPipelineItem::IOVector;
	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOVector;
}

CVectorFlow::~CVectorFlow() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CVectorFlow::apply() throw()
{
BENCHSTART;
	// First check the input dimensions	
	bModuleReady = false;
	if ( getInput().get() == 0 )
	{
		alog << LWARN << "Empty NULL dataset" << endl; 
		return;
	}
	if ( getInput()->getDimension() > 3 || getInput()->getDimension() < 2 
		|| getInput()->getDataDimension() > 1 )
	{
		alog << LWARN << "Input type is no 2D/3D single channel vector field!" << endl; 
		return;
	}
	if ( getInput()->getDimension() == 2 )
	{	  
		shared_ptr<TField2D> gradp = static_pointer_cast<TField2D>( getInput() );
	  if ( gradp.get() == NULL )
	  {
  	  alog << LWARN << "Input type is no 2D vector field!" << endl;
    	return;
	  }
		bModuleReady = true;
		bRoiSelf = false;
		roiPtr = static_pointer_cast<TImage>( getInput(1) );
		if ( roiPtr.get() == NULL || roiPtr->getMaximum() > 1 )
		{
			bRoiSelf = true;
		}		
	  deleteOldOutput();		
		if ( parameters.getUnsignedLong( "Method" ) == 0 )
			gvf2D( gradp );
		else if ( parameters.getUnsignedLong( "Method" ) == 1 )
			ggvf2D( gradp );
#ifdef USE_BLITZ
		else if ( parameters.getUnsignedLong( "Method" ) == 2 )
			gvf2Dblitz( gradp );
#endif
	}
	else
	{
		shared_ptr<TField3D> gradp = static_pointer_cast<TField3D>( getInput() );
	  if ( gradp == NULL || gradp->getDimension() != 3 )
	  {
  	  alog << LWARN << "Input type is no 3D vector field!" << endl;
	    return;
	  }
		bModuleReady = true;
		bRoiSelf = false;
		roiPtr = static_pointer_cast<TImage>( getInput(1) );
		if ( roiPtr == NULL || roiPtr->getMaximum() > 1 )
		{
			bRoiSelf = true;
		}		
	  deleteOldOutput();
		if ( parameters.getUnsignedLong( "Method" ) == 0 )
			gvf3D( gradp );
		else if ( parameters.getUnsignedLong( "Method" ) == 1 )
			ggvf3D( gradp );
#ifdef USE_BLITZ
		else if ( parameters.getUnsignedLong( "Method" ) == 2 )
			gvf3Dblitz( gradp );
#endif					
	}
BENCHSTOP;
}

/** Reimplemented from CPipelineItem */
CPipelineItem* CVectorFlow::newInstance( ulong ulID_ ) const throw()
{
  return new CVectorFlow( ulID_ );
}

/******************* 
 * Private methods *
 *******************/

void CVectorFlow::gvf2D( shared_ptr<TField2D> gradientFieldPtr ) throw()
{
  shared_ptr<TField2D> outputPtr ( new TField2D( 2, gradientFieldPtr->getExtents() ) );

	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
	double dMu = parameters.getDouble( "Mu" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );
	
  // Generate some reference to make reading the code easier
  TField2D& grad = ( *gradientFieldPtr );
  ushort w = gradientFieldPtr->getExtent(0);
  ushort h = gradientFieldPtr->getExtent(1);
  
	TField2D& flowt = *outputPtr;
  TField2D flowdt( 2, grad.getExtents() );
    
  CTypedData<double> b ( 2, grad.getExtents() );
  CTypedData<double> c1 ( 2, grad.getExtents() );
  CTypedData<double> c2 ( 2, grad.getExtents() );        
	flowt = grad;
  
	for ( int y = 0; y < h; ++y )
		for ( int x = 0; x < w; ++x )    
    {
      b(x,y) = ( flowt(x,y)[0] * flowt(x,y)[0] ) + ( flowt(x,y)[1] * flowt(x,y)[1] );
      c1(x,y) = flowt(x,y)[0] * b(x,y);
      c2(x,y) = flowt(x,y)[1] * b(x,y);
    }
		
  ushort time = 0; ushort maxtime = ulIterations;
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
		ushort x = 1; ushort y = 1;
		TField2D::iterator it = flowt.begin() + w + 1;
		TField2D::iterator itdt = flowdt.begin() + w + 1;
		while( y < h )
    {
			TVector2D neigh = *(it+1) + *(it-1) + *(it+w) + *(it-w) - ( 4 * (*it) );
      (*itdt)[0] = ( 1.0 - dDeltaT * b(x,y) ) * (*it)[0] 
				+ dMu * dDeltaT * neigh[0] + c1(x,y) * dDeltaT;
      (*itdt)[1] = ( 1.0 - dDeltaT * b(x,y) ) * (*it)[1] 
				+ dMu	* dDeltaT * neigh[1] + c2(x,y) * dDeltaT;
			++it; ++itdt; ++x;
			if ( x == w )
			{
				y++; x = 1; it+=2; itdt+=2;
			}			
    }
		flowt.swap( flowdt );
    time++;      
		if ( time % 10 == 0 )
		{
			PROG_VAL( time );
	  	if ( time % 50 == 0 )
				APP_PROC();
		}
  }
  PROG_RESET();
	outputPtr->setMinimum( TVector2D( -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector2D( 1.0, 1.0 ) );	
  setOutput( outputPtr );
}

/**
 * GGVF 2d case
 * \param gradientFieldPtr Gradient vector field 
 */
void CVectorFlow::ggvf2D( shared_ptr<TField2D> gradientFieldPtr ) throw()
{
  shared_ptr<TField2D> outputPtr ( new TField2D( 2, gradientFieldPtr->getExtents() ) );

	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
	double dMu = parameters.getDouble( "Mu" );
	double dKappa = parameters.getDouble( "Kappa" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );

  // Generate some reference to make reading the code easier
  TField2D& grad = (*gradientFieldPtr);
  ushort w = grad.getExtent(0);
  ushort h = grad.getExtent(1);
  TField2D& flowt = *outputPtr;
  TField2D flowdt( 2, grad.getExtents() ); 
  CTypedData<double> g ( 2, grad.getExtents() );
  CTypedData<double> hf ( 2, grad.getExtents() );

  for ( int y = 0; y < h; ++y )
    for ( int x = 0; x < w; ++x )
    {
      // Define the time-invariant weighting functions
      g(x,y) = exp ( -1.0 * norm( grad(x,y) ) / dKappa );
      hf(x,y) = 1.0 - g(x,y);
    }

  flowt = grad;
  // Time loop
  ushort time = 0; ushort maxtime = ulIterations;
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
    if ( time % 10 == 0 )
		{
			PROG_VAL( time );
			if ( time % 50 == 0 )
      	APP_PROC();
		}
		for( ushort y = 0; y < h; y++ )
			for( ushort x = 0; x < w; x++ )
			{
        //Determine u(x,y,t)
				if ( bRoiSelf || (*roiPtr)( x, y ) == 1 )
				{
	       // Determine u(x,y,t)
  	      TVector2D neigh ( 0.0, 0.0 );
    	   // Value of discrete Laplacian operator
	        ushort times = 0;
  	      if ( x < w-1 ) { neigh += flowt( x+1,y  ); times++; }
    	    if ( x > 0 )   { neigh += flowt( x-1,y  ); times++; }
      	  if ( y < h-1 ) { neigh += flowt( x  ,y+1); times++; }
        	if ( y > 0 )   { neigh += flowt( x  ,y-1); times++; }
        neigh -= static_cast<double>( times ) * flowt(x,y);
        //Determine u(x,y,t+1)
        flowdt(x,y)[0] = ( 1.0 - dDeltaT * hf(x,y) ) 
					* flowt(x,y)[0] + dMu * dDeltaT * neigh[0] * g(x,y) 
					+ grad(x,y)[0] * hf(x,y) * dDeltaT;
        //Do the same thing for v(x,y,t)
        flowdt(x,y)[1] = ( 1.0 - dDeltaT * hf(x,y) ) 
					* flowt(x,y)[1] + dMu * dDeltaT * neigh[1] * g(x,y) 
					+ grad(x,y)[1] * hf(x,y) * dDeltaT;
				}
     }
      
		flowt.swap( flowdt );  
    time++;     		
  }
	outputPtr->setMinimum( TVector2D( -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector2D( 1.0, 1.0 ) );
	PROG_RESET();
	setOutput( outputPtr );
}

void CVectorFlow::gvf3D( shared_ptr<TField3D> gradientFieldPtr ) throw()
{
	shared_ptr<TField3D> outputPtr ( new TField3D( 3, gradientFieldPtr->getExtents() ) );

  TVector3D maxVec; maxVec[0] = 1.0; maxVec[1] = 1.0;
  outputPtr->setMaximum( maxVec );
	
	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
	double dMu = parameters.getDouble( "Mu" );
	double dKappa = parameters.getDouble( "Kappa" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );

  // Generate some reference to make reading the code easier
  TField3D& grad = (*gradientFieldPtr);
  ushort w = grad.getExtent(0);
  ushort h = grad.getExtent(1);
	ushort d = grad.getExtent(2);
  TField3D& flowt = *outputPtr;
  TField3D flowdt( 3, grad.getExtents() ); 	
  flowt = grad;
  // Time loop
  ushort time = 0; ushort maxtime = ulIterations;;
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
    APP_PROC();
		for ( ushort z = 0; z < d; ++z )
    	for ( ushort y = 0; y < h; ++y )
      	for ( ushort x = 0; x < w; ++x )
	      {
  	      // Determine u(x,y,t)
    	    TVector3D neigh( 0.0, 0.0, 0.0 ); // Value of discrete Laplacian operator
      	  ushort times = 0;
	        if ( x < w-1) { neigh+=flowt(x+1,y,z); times++; }
  	      if ( x > 0) { neigh+=flowt(x-1,y,z); times++; }
      	  if ( y < h-1) { neigh+=flowt(x,y+1,z); times++; }
	        if ( y > 0) { neigh+=flowt(x,y-1,z); times++; }
  	      if ( z < d-1) { neigh+=flowt(x,y,z+1); times++; }
        	if ( z > 0) { neigh+=flowt(x,y,z-1); times++; }
     	   	neigh -= static_cast<double>( times ) * flowt(x,y,z);
	        // Determine u(x,y,t+1)
	      	double g = exp ( -1.0 * norm( grad(x,y,z) ) / dKappa );
    	  	double hf = 1.0 - g;					
  	      flowdt(x,y,z)[0] = ( 1.0 - dDeltaT * hf ) 
						* flowt(x,y,z)[0] + dMu * dDeltaT * neigh[0] * g
						+ grad(x,y,z)[0] * hf * dDeltaT;
		      // Do the same thing for v(x,y,t)
    	    flowdt(x,y,z)[1] = ( 1.0 - dDeltaT * hf ) 
						* flowt(x,y,z)[1] + dMu * dDeltaT * neigh[1] * g
						+ grad(x,y,z)[1] * hf * dDeltaT;
		      // Do the same thing for v(x,y,t)
    	    flowdt(x,y,z)[2] = ( 1.0 - dDeltaT * hf ) 
						* flowt(x,y,z)[2] + dMu * dDeltaT * neigh[2] * g
						+ grad(x,y,z)[2] * hf * dDeltaT;
	      }      
		flowt.swap( flowdt );  
    time++;     
		PROG_VAL( time );
  }  
	outputPtr->setMinimum( TVector3D( -1.0, -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );
	PROG_RESET();
	setOutput( outputPtr );  
}

void CVectorFlow::ggvf3D( shared_ptr<TField3D> gradientFieldPtr ) throw()
{
  shared_ptr<TField3D> outputPtr ( new TField3D( 3, gradientFieldPtr->getExtents() ) );

  TVector3D maxVec; maxVec[0] = 1.0; maxVec[1] = 1.0;
  outputPtr->setMaximum( maxVec );

	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
	double dMu = parameters.getDouble( "Mu" );
	double dKappa = parameters.getDouble( "Kappa" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );

  // Generate some reference to make reading the code easier
  TField3D& grad = (*gradientFieldPtr);
  ushort w = grad.getExtent(0);
  ushort h = grad.getExtent(1);
	ushort d = grad.getExtent(2);
  TField3D& flowt = *outputPtr;
  TField3D flowdt( 3, grad.getExtents() ); 
  CTypedData<double> g ( 3, grad.getExtents() );
  CTypedData<double> hf ( 3, grad.getExtents() );
	PROG_MAX( d );
	for ( int z = 0; z < d; ++z )
	{
		PROG_VAL( z );
  	for ( int y = 0; y < h; ++y )
    	for ( int x = 0; x < w; ++x )
    	{
      	// Define the time-invariant weighting functions
	      g(x,y,z) = exp ( -1.0 * norm( grad(x,y,z) )  / dKappa );
    	  hf(x,y,z) = 1.0 - g(x,y,z);
    	}
	}
	
	PROG_RESET();
  flowt = grad;
  // Time loop
  ushort time = 0; ushort maxtime = ulIterations;;
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
  	cerr << "GGVF iteration " << time << endl;
    APP_PROC();
		for ( ushort z = 0; z < d; ++z )
    	for ( ushort y = 0; y < h; ++y )
      	for ( ushort x = 0; x < w; ++x )
	      {
  	      // Determine u(x,y,t)
    	    TVector3D neigh( 0.0, 0.0, 0.0 ); // Value of discrete Laplacian operator
      	  ushort times = 0;
	        if ( x < w-1) { neigh+=flowt(x+1,y,z); times++; }
  	      if ( x > 0) { neigh+=flowt(x-1,y,z); times++; }
      	  if ( y < h-1) { neigh+=flowt(x,y+1,z); times++; }
	        if ( y > 0) { neigh+=flowt(x,y-1,z); times++; }
  	      if ( z < d-1) { neigh+=flowt(x,y,z+1); times++; }
        	if ( z > 0) { neigh+=flowt(x,y,z-1); times++; }
     	   	neigh -= static_cast<double>( times ) * flowt(x,y,z);
	        // Determine u(x,y,t+1)
  	      flowdt(x,y,z)[0] = ( 1.0 - dDeltaT * hf(x,y,z) ) 
						* flowt(x,y,z)[0] + dMu * dDeltaT * neigh[0] * g(x,y,z) 
						+ grad(x,y,z)[0] * hf(x,y,z) * dDeltaT;
 		      // Do the same thing for v(x,y,t)
    	    flowdt(x,y,z)[1] = ( 1.0 - dDeltaT * hf(x,y,z) ) 
						* flowt(x,y,z)[1] + dMu * dDeltaT * neigh[1] * g(x,y,z) 
						+ grad(x,y,z)[1] * hf(x,y,z) * dDeltaT;
 		      // Do the same thing for w(x,y,t)
    	    flowdt(x,y,z)[2] = ( 1.0 - dDeltaT * hf(x,y,z) ) 
						* flowt(x,y,z)[2] + dMu * dDeltaT * neigh[2] * g(x,y,z) 
						+ grad(x,y,z)[2] * hf(x,y,z) * dDeltaT;
	      }      
		flowt = flowdt;  
    time++;      
		PROG_VAL( time );
  }  
	outputPtr->setMinimum( TVector3D( -1.0, -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );	
	PROG_RESET();
	setOutput( outputPtr );
}

#ifdef USE_BLITZ
using namespace blitz;	
/**
 * GGVF 2d case with blitz support
 * \param gradientFieldPtr Gradient vector field 
 */
void CVectorFlow::gvf2Dblitz( shared_ptr<TField2D> gradientFieldPtr ) throw()
{
  shared_ptr<TField2D> outputPtr ( new TField2D( 2, gradientFieldPtr->getExtents() ) );
	
	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
//	double dMu = parameters.getDouble( "Mu" );
	double dKappa = parameters.getDouble( "Kappa" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );
  
	// Generate some reference to make reading the code easier
  TField2D& grad = (*gradientFieldPtr);
  ushort w = grad.getExtent(0);
  ushort h = grad.getExtent(1);
  Array<double,2> bg( w, h, AIPSArray<2>() );
  Array<double,2> bhf( w, h, AIPSArray<2>() );
  Array<TVector2D, 2> bgradient( grad.getArray(), shape( w, h ),
    neverDeleteData, AIPSArray<2>() );
  Array<TVector2D, 2> bflowt( outputPtr->getArray(), shape( w, h ), 
		neverDeleteData, AIPSArray<2>() ); 
	bflowt = bgradient;
  Array<TVector2D, 2> bflowdt( w, h, AIPSArray<2>() );
   for ( int y = 0; y < h; ++y )
     for ( int x = 0; x < w; ++x )
     {
       // Transform gradient to [0.0 ... 1.0]
       // Define the time-invariant weighting functions
       bg(x,y) = exp ( -1.0 *  norm( bgradient( x, y ) ) / dKappa ) ;
     }
  bhf = 1.0 - bg;
  // Time loop
  ushort time = 0; ushort maxtime = ulIterations;;    
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
    if ( time % 10 == 0 )
      APP_PROC();
    for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
				if ( bRoiSelf || (*roiPtr)( x, y ) == 1 )
				{
	        // Determine u(x,y,t)
  	      TVector2D neigh ( 0.0, 0.0 );
    	    // Value of discrete Laplacian operator
	        ushort times = 0;
  	      if ( x < w-1 ) { neigh += bflowt( x+1,y  ); times++; }
    	    if ( x > 0 )   { neigh += bflowt( x-1,y  ); times++; }
      	  if ( y < h-1 ) { neigh += bflowt( x  ,y+1); times++; }
        	if ( y > 0 )   { neigh += bflowt( x  ,y-1); times++; }
	        neigh -= static_cast<double>( times ) * bflowt(x,y);
  	      // Determine u(x,y,t+1)
    	    bflowdt(x,y)[0] = ( 1.0 - dDeltaT * bhf(x,y) )
						* bflowt(x,y)[0]						
						+ dDeltaT * neigh[0] * bg(x,y)						
						+ bgradient(x,y)[0] * bhf(x,y) * dDeltaT;
	        bflowdt(x,y)[1] = ( 1.0 - dDeltaT * bhf(x,y) )
						* bflowt(x,y)[1] 
						+ dDeltaT * neigh[1] * bg(x,y) 
						+ bgradient(x,y)[1] * bhf(x,y) * dDeltaT;
// 					bflowdt(x,y)[0] = ( 1.0 - dDeltaT * bhf(x,y) )
// 						* bflowt(x,y)[0] 
// 						
// 						+ dMu
// 						* dDeltaT * neigh[0] * bg(x,y) 
// 						
// 						+ bgradient(x,y)[0] * bhf(x,y) * dDeltaT;
// 	        bflowdt(x,y)[1] = ( 1.0 - dDeltaT * bhf(x,y) )
// 						* bflowt(x,y)[1] + dMu 
// 						* dDeltaT * neigh[1] * bg(x,y) 
// 						+ bgradient(x,y)[1] * bhf(x,y) * dDeltaT;
				}
      }
		cycleArrays( bflowt, bflowdt );
    time++;
		PROG_VAL( time );
  }
	outputPtr->setMinimum( TVector2D( -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector2D( 1.0, 1.0 ) );			
	PROG_RESET();
  setOutput( outputPtr );
}

/**
 * GGVF 3d case with blitz++ support
 * \param gradientFieldPtr Gradient vector field 
 */
void CVectorFlow::gvf3Dblitz( shared_ptr<TField3D> gradientFieldPtr ) throw()
{
	shared_ptr<TField3D> outputPtr ( new TField3D( 3, gradientFieldPtr->getExtents() ) );

	// Get the parameters
	double dDeltaT = parameters.getDouble( "DeltaT" );
	double dMu = parameters.getDouble( "Mu" );
	double dKappa = parameters.getDouble( "Kappa" );
	ulong ulIterations = parameters.getUnsignedLong( "Iterations" );
	
  // Generate some reference to make reading the code easier
  TField3D& grad = (*gradientFieldPtr);
  ushort w = grad.getExtent(0);
  ushort h = grad.getExtent(1);
	ushort d = grad.getExtent(2);
	Array<double,3> bg( w, h, d, AIPSArray<3>() );
  Array<double,3> bhf( w, h, d, AIPSArray<3>() );
  Array<TVector3D, 3> bgradient( grad.getArray(), shape( w, h, d ),
    neverDeleteData, AIPSArray<3>() );
  Array<TVector3D, 3> bflowt( outputPtr->getArray(), shape( w, h, d ),
    neverDeleteData, AIPSArray<3>() );
	bflowt = bgradient;
  Array<TVector3D, 3> bflowdt( w, h, d, AIPSArray<3>() );	
  PROG_MAX( d );
  for ( int z = 0; z < d; ++z )
	{
		PROG_VAL( z );
    for ( int y = 0; y < h; ++y )
     	for ( int x = 0; x < w; ++x )
      {
        // Transform gradient to [0.0 ... 1.0]
        // Define the time-invariant weighting functions
        bg(x,y,z) = exp ( -1.0 *  norm( bgradient( x, y, z ) ) / dKappa );
      }
	}
	bhf = 1.0 - bg;
	
	PROG_RESET();  
  // Time loop
  ushort time = 0; ushort maxtime = ulIterations;
	PROG_MAX( maxtime );
  while ( time <= maxtime )
  {
    APP_PROC();
	  for ( int z = 0; z < d; ++z )
      for ( int y = 0; y < h; ++y )
   	  	for ( int x = 0; x < w; ++x )
     	  {
       	  // Determine u(x,y,t)
         	TVector3D neigh( 0.0, 0.0, 0.0 );
   	      // Value of discrete Laplacian operator
     	    ushort times = 0;
       	  if ( x < w-1) { neigh += bflowt( x+1,y,z  ); times++; }
         	if ( x > 0)   { neigh += bflowt( x-1,y,z  ); times++; }
	        if ( y < h-1) { neigh += bflowt( x  ,y+1,z); times++; }
  	      if ( y > 0)   { neigh += bflowt( x  ,y-1,z); times++; }
          if ( z < d-1) { neigh += bflowt( x  ,y,z+1); times++; }
     	    if ( z > 0)   { neigh += bflowt( x  ,y,z-1); times++; }
	        neigh -= static_cast<double>( times ) * bflowt(x,y,z);
	        // Determine u(x,y,t+1)
  	      bflowdt(x,y,z)[0] = ( 1.0 - dDeltaT * bhf(x,y,z) )
						* bflowt(x,y,z)[0] + dMu
						* dDeltaT * neigh[0] * bg(x,y,z) 
						+ bgradient(x,y,z)[0] * bhf(x,y,z) * dDeltaT;
         	bflowdt(x,y,z)[1] = ( 1.0 - dDeltaT * bhf(x,y,z) )
						* bflowt(x,y,z)[1] + dMu 
						* dDeltaT * neigh[1] * bg(x,y,z) 
						+ bgradient(x,y,z)[1] * bhf(x,y,z) * dDeltaT;
          bflowdt(x,y,z)[2] = ( 1.0 - dDeltaT * bhf(x,y,z) )
						* bflowt(x,y,z)[2] + dMu 
						* dDeltaT * neigh[2] * bg(x,y,z) 
						+ bgradient(x,y,z)[2] * bhf(x,y,z) * dDeltaT;
       	}
	  cycleArrays( bflowt, bflowdt );
	  time++;
		PROG_VAL( time );
  } 
	outputPtr->setMinimum( TVector3D( -1.0, -1.0, -1.0 ) );
	outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );
	PROG_RESET();
  // Produce some output
  setOutput( outputPtr );	
}
#endif
