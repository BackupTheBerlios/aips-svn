//
// C++ Implementation: cdiscrepancymeasures
//
// Description: 
//
//
// Author:  <>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cdiscrepancymeasures.h"

using namespace std;
using namespace boost;

CDiscrepancyMeasures::CDiscrepancyMeasures( ulong ulID )
 throw()
:	CFilter( ulID, "Discrepancy measures", 2, 0, "CDiscrepancyMeasures", "0.1", "CFilter" )
{
  setModuleID( "Bla" );
  
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
  bModuleReady = false;
  if ( getInput() )
    call<Length<imageTL>::value - 1>();
  else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
FEND; 
}

template<typename ImageType> bool CDiscrepancyMeasures::compute() throw()
{
  shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
	shared_ptr<ImageType> referenceSPtr = static_pointer_cast<ImageType>( getInput(1) );
  if ( !checkInput<ImageType>( inputSPtr, 2, 3 ) || !checkInput<ImageType>( referenceSPtr, 2, 3 ) )
    return false;
  if ( inputSPtr->getExtent(0) != referenceSPtr->getExtent(0)
  	|| inputSPtr->getExtent(1) != referenceSPtr->getExtent(1) )
  	return false;
  if( inputSPtr->getDimension() > 2 && inputSPtr->getExtent(2) != referenceSPtr->getExtent(2) )
  	return false;
  bModuleReady = true;
	// Coefficients
  double dDiceCoefficient = 0.0;
  double dTanimotoCoefficient = 0.0;
  // Compute region size and dice coefficient. Also mark boundary voxels for later
  // distance computation
  std::list<TPoint3D> anInputList;
  std::list<TPoint3D> aReferenceList;
  ulong ulInputRegionSize = 0;
  ulong ulReferenceRegionSize = 0;
  ulong ulSharedRegionSize = 0;
  ulong ulInputSurface = 0;
  ulong ulReferenceSurface = 0;
  ulong ulCombinedArea = 0;
  ulong ulFalsePositives = 0;
  ulong ulFalseNegatives = 0;
  typename ImageType::TDataType theLabel =
    static_cast<typename ImageType::TDataType>( parameters.getUnsignedLong( "Label" ) );
  cerr << "Computing area sizes... ";
  typename ImageType::iterator inputIt = inputSPtr->begin();
  typename ImageType::iterator referenceIt = referenceSPtr->begin();
  while( inputIt != inputSPtr->end() )
  {
  	if ( (*inputIt) == theLabel )
  	{
  		ulInputRegionSize++;
  		ushort x,y,z;
  		inputIt.getPos( x,y,z );
  		if ( !( (*inputSPtr)(x+1,y,z) > 0 && (*inputSPtr)(x-1,y,z) > 0
  			&& (*inputSPtr)(x,y+1,z) > 0 && (*inputSPtr)(x,y-1,z) > 0
  			&& (*inputSPtr)(x,y,z+1) > 0 && (*inputSPtr)(x,y,z-1) > 0 ) )
  		{
  			anInputList.push_back( TPoint3D(x,y,z) );
  			ulInputSurface++;
  		}
  		ulCombinedArea++;
  	}
  	if ( (*referenceIt) == theLabel )
  	{
  		ulReferenceRegionSize++;
  		ushort x,y,z;
  		referenceIt.getPos( x,y,z );
  		if ( !( (*referenceSPtr)(x+1,y,z) > 0 && (*referenceSPtr)(x-1,y,z) > 0
  			&& (*referenceSPtr)(x,y+1,z) > 0 && (*referenceSPtr)(x,y-1,z) > 0
  			&& (*referenceSPtr)(x,y,z+1) > 0 && (*referenceSPtr)(x,y,z-1) > 0 ) )
  		{
  			aReferenceList.push_back( TPoint3D(x,y,z) );
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
  cerr << "done.\nFound " << anInputList.size() << " segmentation and " << aReferenceList.size()
   << " reference voxels" << endl;
  cerr << "Computing coefficients... ";
  // Compute Dice and Tanimoto coefficient
	dDiceCoefficient = static_cast<double>( 2 * ulSharedRegionSize )
		/ static_cast<double>( ulInputRegionSize + ulReferenceRegionSize );
	dTanimotoCoefficient = static_cast<double>( ulSharedRegionSize )
		/ static_cast<double>( ulCombinedArea );
  // Compute distance measures between the two surfaces
  std::list<TPoint3D>::iterator it = anInputList.begin();
  std::list<TPoint3D>::iterator jt = aReferenceList.begin();
  double dMinDist = inputSPtr->getExtent(0)*inputSPtr->getExtent(1)*inputSPtr->getExtent(2);
  double dSumDist = 0.0;
  double dMaxMin = 0.0;
  while( it != anInputList.end() )
  {
  	jt = aReferenceList.begin();
  	dMinDist = inputSPtr->getExtent(0)*inputSPtr->getExtent(1)*inputSPtr->getExtent(2);
		while( jt != aReferenceList.end() )
		{
			double dDistance = sqrt( ((*it)[0]-(*jt)[0])*((*it)[0]-(*jt)[0])
				+	((*it)[1]-(*jt)[1])*((*it)[1]-(*jt)[1])
				+ ((*it)[2]-(*jt)[2])*((*it)[2]-(*jt)[2]) );
			if ( dMinDist > dDistance )
				dMinDist = dDistance;
			++jt;
		}
		dSumDist += dMinDist;
		dMaxMin = std::max( dMaxMin, dMinDist );
		++it;
  }
  double dSumDistA = dSumDist;
  double dHDA = dMaxMin;
  dSumDist = 0.0;
  dMaxMin = 0.0;
  jt = aReferenceList.begin();
  cerr << dSumDistA << dMaxMin << "... ";
  while( jt != aReferenceList.end() )
  {
  	it = anInputList.begin();
  	dMinDist = inputSPtr->getExtent(0)*inputSPtr->getExtent(1)*inputSPtr->getExtent(2);
		while( it != anInputList.end() )
		{
			double dDistance = sqrt( static_cast<double>( ((*it)[0]-(*jt)[0])*((*it)[0]-(*jt)[0])
				+	((*it)[1]-(*jt)[1])*((*it)[1]-(*jt)[1])
				+ ((*it)[2]-(*jt)[2])*((*it)[2]-(*jt)[2]) ) );
			if ( dMinDist > dDistance )
				dMinDist = dDistance;
			++it;
		}
		dSumDist += dMinDist;
		dMaxMin = std::max( dMaxMin, dMinDist );
		++jt;
  }
  cerr << dSumDist << dMaxMin << "done." << endl;
  double dSumDistB = dSumDist;
  double dHDB = dMaxMin;
  double dHausdorffDistance = std::max( dHDA, dHDB );
  double dMeanDistance = ( dSumDistA + dSumDistB ) / ( ulReferenceRegionSize + ulInputRegionSize );
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
  return true;
}

DEFINE_CALL_MACRO( CDiscrepancyMeasures::call, CDiscrepancyMeasures::compute, imageTL )

