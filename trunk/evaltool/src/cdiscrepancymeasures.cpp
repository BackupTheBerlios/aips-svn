//
// C++ Implementation: cdiscrepancymeasures
//
// Description: 
//
//
// Author:  Hendrik Belitz <hbelitz@users.berlios.de>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cdiscrepancymeasures.h"

using namespace std;
using namespace boost;

/** \param ulID Unique item ID */
CDiscrepancyMeasures::CDiscrepancyMeasures( ulong ulID )
 throw()
:	CFilter( ulID, "Discrepancy measures", 2, 0, "CDiscrepancyMeasures",
	CDISCREPANCYMEASURES_VERSION, "CFilter" )
{
  setModuleID( "Discrepancy measures" );
  
  sDocumentation = "Creates a gradient field from the input image via a gaussian derivative\n"
                   "** Input ports:\n"
                   "0: A scalar single channel data set (image data, required)\n"
									 "1: A scalar single channel data set (region of interest, optional)\n"
                   "** Output ports:\n"
                   "0: A 2D or 3D vector field which is normalized\n"
									 "** Parameters:\n"
									 "Sigma: Width of gaussian";

  parameters.initUnsignedLong( "Label", 1, 0, 10000000 );
	parameters.initDouble( "DiceCoefficient", 0.5, 0.0, 100000.0 );
	parameters.initDouble( "TanimotoCoefficient", 0.5, 0.0, 100000.0 );
	parameters.initDouble( "HausdorffDistance", 0.5, 0.0, 100000.0 );
	parameters.initDouble( "MeanDistance", 0.5, 0.0, 100000.0 );
	parameters.initUnsignedLong( "InputRegionSize", 0, 0, 10000000 );
	parameters.initUnsignedLong( "ReferenceRegionSize", 0, 0, 10000000 );
	parameters.initUnsignedLong( "SharedRegionSize", 0, 0, 10000000 );
	parameters.initUnsignedLong( "InputSurface", 0, 0, 10000000 );
	parameters.initUnsignedLong( "ReferenceSurface", 0, 0, 10000000 );
	parameters.initUnsignedLong( "CombinedArea", 0, 0, 10000000 );
	parameters.initUnsignedLong( "FalsePositives", 0, 0, 10000000 );
	parameters.initUnsignedLong( "FalseNegatives", 0, 0, 10000000 );
  inputsVec[0].portType = CPipelineItem::IOInteger;
	inputsVec[1].portType = CPipelineItem::IOInteger;
}

CDiscrepancyMeasures::~CDiscrepancyMeasures() throw()
{
}

void CDiscrepancyMeasures::apply() throw()
{
FBEGIN;
	// Set Module inactive, since no valid input is given yet
  bModuleReady = false;
  // Check if input present. Call specific function for all possible types
  if ( getInput() && getInput(1) )
    call<Length<imageTL>::value - 1>();
  else
  {
  	// No function for image type found. Send out a warning and return
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
FEND; 
}

/** \returns true if computation was successful, false if image type was wrong */
template<typename ImageType> bool CDiscrepancyMeasures::compute() throw()
{
FBEGIN;
	// Get inputs
  shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
	shared_ptr<ImageType> referenceSPtr = static_pointer_cast<ImageType>( getInput(1) );

  // Validate inputs
  if ( !checkInput<ImageType>( inputSPtr, 2, 3 ) || !checkInput<ImageType>( referenceSPtr, 2, 3 )
    || inputSPtr->getExtents() != referenceSPtr->getExtents() )
    return false;

  // We can start computation now, inputs are valid
  bModuleReady = true;
  
	// Initialize all coefficients
  double dDiceCoefficient = 0.0;
  double dTanimotoCoefficient = 0.0;
  std::list<TPoint3D> anInputList; // List of surface voxels for input image
  std::list<TPoint3D> aReferenceList; // List of surface voxels for reference image
  ulong ulInputRegionSize = 0;
  ulong ulReferenceRegionSize = 0;
  ulong ulSharedRegionSize = 0;
  ulong ulInputSurface = 0;
  ulong ulReferenceSurface = 0;
  ulong ulCombinedArea = 0;
  ulong ulFalsePositives = 0;
  ulong ulFalseNegatives = 0;
  // What's the label we should look out for
  typename ImageType::TDataType theLabel =
    static_cast<typename ImageType::TDataType>( parameters.getUnsignedLong( "Label" ) );
DBG3( "Computing area sizes." );
  // Compute region sizes and dice coefficient. Also mark boundary voxels for later
  // distance computation
  typename ImageType::iterator inputIt = inputSPtr->begin();
  typename ImageType::iterator referenceIt = referenceSPtr->begin();
  // For all voxels, check if the voxel is a surface voxel. In that case, save it to the
  // appropiate list and increase individual surface area.
  // For all object voxels, increase the individual volume counts.
  // For all voxels belonging to both objects, increase the combined volume count.
  // Additionally, record the number of false positives and false negatives
  while( inputIt != inputSPtr->end() )
  {
  	if ( (*inputIt) == theLabel )
  	{
  		ulInputRegionSize++;
      TPoint3D p;
      referenceIt.getPos( p );
      if ( inputSPtr->getDimension() == 3
  			&& !( ( p[0] < inputSPtr->getExtent(0) && (*inputSPtr)( p[0]+1, p[1], p[2] ) ) == theLabel
  			&& ( p[0] > 0 && (*inputSPtr)( p[0]-1, p[1], p[2] ) ) == theLabel
  			&& ( p[1] < inputSPtr->getExtent(1) && (*inputSPtr)( p[0], p[1]+1, p[2] ) ) == theLabel
  			&& ( p[1] > 0 && (*inputSPtr)( p[0], p[1]-1, p[2] ) ) == theLabel
  			&& ( p[2] < inputSPtr->getExtent(2) && (*inputSPtr)( p[0], p[1], p[2]+1 ) ) == theLabel
  			&& ( p[2] > 0 && (*inputSPtr)( p[0], p[1], p[2]-1 ) ) == theLabel ) )
  		{
  			anInputList.push_back( p );
  			ulInputSurface++;
  		}
  		else if ( inputSPtr->getDimension() == 2
  			&& !( ( p[0] < inputSPtr->getExtent(0) && (*inputSPtr)( p[0]+1, p[1], p[2] ) ) == theLabel
  			&& ( p[0] > 0 && (*inputSPtr)( p[0]-1, p[1], p[2] ) == theLabel
  			&& ( p[1] < inputSPtr->getExtent(1) && (*inputSPtr)( p[0], p[1]+1, p[2] ) ) == theLabel
  			&& ( p[1] > 0 && (*inputSPtr)( p[0], p[1]-1, p[2] ) == theLabel ) ) ) )
  		{
  			anInputList.push_back( p );
  			ulInputSurface++;
  		}
  		ulCombinedArea++;
  	}
  	if ( (*referenceIt) == theLabel )
  	{
  		ulReferenceRegionSize++;
  		TPoint3D p;
  		referenceIt.getPos( p );
  		if ( inputSPtr->getDimension() == 3
  			&& !( ( p[0] < inputSPtr->getExtent(0) && (*referenceSPtr)( p[0]+1, p[1], p[2] ) ) == theLabel
  			&& ( p[0] > 0 && (*referenceSPtr)( p[0]-1, p[1], p[2] ) ) == theLabel
  			&& ( p[1] < inputSPtr->getExtent(1) && (*referenceSPtr)( p[0], p[1]+1, p[2] ) ) == theLabel
  			&& ( p[1] > 0 && (*referenceSPtr)( p[0], p[1]-1, p[2] ) ) == theLabel
  			&& ( p[2] < inputSPtr->getExtent(2) && (*referenceSPtr)( p[0], p[1], p[2]+1 ) ) == theLabel
  			&& ( p[2] > 0 && (*referenceSPtr)( p[0], p[1], p[2]-1 ) ) == theLabel ) ) 
  		{
  			aReferenceList.push_back( p );
  			ulReferenceSurface++;
  		}
  		else if ( inputSPtr->getDimension() == 2
  			&& !( ( p[0] < inputSPtr->getExtent(0) && (*referenceSPtr)( p[0]+1, p[1], p[2] ) ) == theLabel
  			&& ( p[0] > 0 && (*referenceSPtr)( p[0]-1, p[1], p[2] ) == theLabel
  			&& ( p[1] < inputSPtr->getExtent(1) && (*referenceSPtr)( p[0], p[1]+1, p[2] ) ) == theLabel
  			&& ( p[1] > 0 && (*referenceSPtr)( p[0], p[1]-1, p[2] ) == theLabel ) ) ) )
  		{
  			aReferenceList.push_back( p );
  			ulReferenceSurface++;
  		}  		
  		if ( !( (*inputIt) == theLabel ) )
  			ulCombinedArea++;
  	}
  	if ( (*inputIt) == theLabel && (*referenceIt) == theLabel ) ulSharedRegionSize++;
  	if ( (*inputIt) == theLabel && !(*referenceIt) == theLabel ) ulFalsePositives++;
  	if ( !(*inputIt) == theLabel && (*referenceIt) == theLabel ) ulFalseNegatives++;
  	++inputIt;
  	++referenceIt;
  }
DBG3( "Found " << anInputList.size() << " segmentation and " << aReferenceList.size()
 << " reference voxels." );
DBG3( "Computing coefficients." );

  /* Compute Dice and Tanimoto coefficient */
	dDiceCoefficient = static_cast<double>( 2 * ulSharedRegionSize )
		/ static_cast<double>( ulInputRegionSize + ulReferenceRegionSize );
	dTanimotoCoefficient = static_cast<double>( ulSharedRegionSize )
		/ static_cast<double>( ulCombinedArea );
  
  /* Compute distance measures between the two surfaces */

  // Initilize all variables
  std::list<TPoint3D>::iterator it;
  std::list<TPoint3D>::iterator jt;
  double dMinDist = inputSPtr->getSize();
  double dSumDist = 0.0;
  double dMaxMin = 0.0;

  // Loop over all input surface voxels
  for( it = anInputList.begin(); it != anInputList.end(); ++it )
  {  	
  	dMinDist = inputSPtr->getSize();
  	// For each input surface voxel, loop over all reference surface voxels
		for( jt = aReferenceList.begin(); jt != aReferenceList.end(); ++jt )
		{
			// Compute distance and update minimum distance
			double dDistance = sqrt( ((*it)[0]-(*jt)[0])*((*it)[0]-(*jt)[0])
				+	((*it)[1]-(*jt)[1])*((*it)[1]-(*jt)[1])
				+ ((*it)[2]-(*jt)[2])*((*it)[2]-(*jt)[2]) );
			dMinDist = std::min( dMinDist, dDistance );
		}
		// Sum of distances
		dSumDist += dMinDist;
		// Maximum of minimum distances
		dMaxMin = std::max( dMaxMin, dMinDist );
  }
  double dSumDistA = dSumDist;
  double dHDA = dMaxMin;
  dSumDist = 0.0;
  dMaxMin = 0.0;

  // Do the same in the other direction
  for( jt = aReferenceList.begin(); jt != aReferenceList.end(); ++jt )
  {
  	dMinDist = inputSPtr->getSize();
		for( it = anInputList.begin(); it != anInputList.end(); ++it )
		{
			double dDistance = sqrt( static_cast<double>( ((*it)[0]-(*jt)[0])*((*it)[0]-(*jt)[0])
				+	((*it)[1]-(*jt)[1])*((*it)[1]-(*jt)[1])
				+ ((*it)[2]-(*jt)[2])*((*it)[2]-(*jt)[2]) ) );
			if ( dMinDist > dDistance )
				dMinDist = dDistance;
		}
		dSumDist += dMinDist;
		dMaxMin = std::max( dMaxMin, dMinDist );
  }
  double dSumDistB = dSumDist;
  double dHDB = dMaxMin;

  // Compute Hausdorff distance and mean surface distance
  double dHausdorffDistance = std::max( dHDA, dHDB );
  double dMeanDistance = ( dSumDistA + dSumDistB ) / ( ulReferenceRegionSize + ulInputRegionSize );
  
  // Put all computed metrics into parameter list
	parameters.setDouble( "DiceCoefficient", dDiceCoefficient );
	parameters.setDouble( "TanimotoCoefficient", dTanimotoCoefficient );
	parameters.setDouble( "HausdorffDistance", dHausdorffDistance );
	parameters.setDouble( "MeanDistance", dMeanDistance );
	parameters.setUnsignedLong( "InputRegionSize", ulInputRegionSize );
	parameters.setUnsignedLong( "ReferenceRegionSize", ulReferenceRegionSize );
	parameters.setUnsignedLong( "SharedRegionSize", ulSharedRegionSize );
	parameters.setUnsignedLong( "InputSurface", ulInputSurface );
	parameters.setUnsignedLong( "ReferenceSurface", ulReferenceSurface );
	parameters.setUnsignedLong( "CombinedArea", ulCombinedArea );
	parameters.setUnsignedLong( "FalsePositives", ulFalsePositives );
	parameters.setUnsignedLong( "FalseNegatives", ulFalseNegatives );
FEND;  
  return true;
}

DEFINE_CALL_MACRO( CDiscrepancyMeasures::call, CDiscrepancyMeasures::compute, imageTL );

