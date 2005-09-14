//
// C++ Implementation: citkadapter
//
// Description: 
//
//
// Author: Hendrik Belitz <hendrik@darkon.info>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "citkadapter.h"
#ifdef USE_ITK

namespace aips
{

CITKAdapter::CITKAdapter() throw()
  : CStructuredDataAdapter( "CITKAdapter", ITKADAPTER_VERSION, "CStructuredDataAdapter" ) 
{
}

CITKAdapter::CITKAdapter( TDataSetPtr internalDataSPtr_ ) throw()
  : CStructuredDataAdapter( internalDataSPtr_, "CITKAdapter", ITKADAPTER_VERSION, "CStructuredDataAdapter") 
{
}

CITKAdapter::~CITKAdapter() throw()
{
}
  
/* Other methods */    
  
TDataSetPtr CITKAdapter::convertToInternal()
{  
  return externalDataPtr;
}

} /* namespace aips */
#endif
