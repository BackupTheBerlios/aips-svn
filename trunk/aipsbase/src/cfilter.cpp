/***********************************************************************
 * File: cfilter.cpp                                                   *
 * Project: AIPS                                                       *
 * Description: An abstract base class for all image filters           *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Created: 26.08.03                                                   *
 ***********************************************************************/

#include "cfilter.h"

using namespace std;
using namespace aips;

/**
 * Constructor
 * Also sets the following defaults:
 * - One mandatory input and one output port with portType IOOther
 * - No parameters
 * \param ulID_ Unique item ID (in processing graph)
 * \param sName_ Filter name
 * \param usNoOfInputs Number of input ports
 * \param usNoOfOutputs Number of output ports 
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CFilter::CFilter(  ulong ulID_, const string &sName_,
  const ushort usNoOfInputs, const ushort usNoOfOutputs,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ )
  throw() : CPipelineItem ( ulID_, usNoOfInputs, usNoOfOutputs,
		sClassName_, sClassVersion_, sDerivedFrom_ )
{
  setModuleName( sName_ );
  setType( ITypeFilter );
  
  for( uint i = 0; i < usNoOfInputs; ++i )
  	inputsVec[i].portType = IOInteger;
  for( uint i = 0; i < usNoOfOutputs; ++i )
  	outputsVec[0].portType = IOInteger;
}

/** Destructor */
CFilter::~CFilter() throw()
{
}
