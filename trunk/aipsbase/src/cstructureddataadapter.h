/************************************************************************
 * File: cstructureddataadapter.h                                       *
 * Project: AIPS                                                        *
 * Description: Abstract adapter for structured data sets               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2004-12-22                                                  *
 * Changed: 2005-01-25 Added method setYSwapping and corr. data member  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSTRUCTUREDDATAADAPTER_H
#define CSTRUCTUREDDATAADAPTER_H

#define CSTRUCTUREDDATAADAPTER_VERSION "0.2"

// AIPS includes
#include "cdataadapter.h"
#include "aipstypelist.h"

namespace aips { 

/**
 * Abstract adapter for image datasets
 */
class CStructuredDataAdapter : public CDataAdapter 
{
private:
	/// Standard constructor
	CStructuredDataAdapter();
	/// Copy constructor
	CStructuredDataAdapter( const CStructuredDataAdapter& );
	/// Assignment operator
	//CStructuredDataAdapter operator=( const CStructuredDataAdapter& ) //icc returning abtract class is not allowed;
public:
/* Structors */
	/// Constructor
  CStructuredDataAdapter( const std::string &sClassName_ = "CStructuredDataAdapter", 
		const std::string &sClassVersion_ = CSTRUCTUREDDATAADAPTER_VERSION, 
		const std::string &sDerivedFrom_ = "CDataAdapter ") throw(); 
	/// Constructor with additional parameter for internal data
  CStructuredDataAdapter( TDataSetPtr internalDataSPtr_, 
		const std::string &sClassName_ = "CStructuredDataAdapter", 
		const std::string &sClassVersion_ = CSTRUCTUREDDATAADAPTER_VERSION, 
		const std::string &sDerivedFrom_ = "CDataAdapter " ) throw();  
	/// Destructor - pure virtual
  virtual ~CStructuredDataAdapter() throw() = 0;
/* Accessors */
	/// Sets the internal data pointer to the given dataset
  void setInternalData( TDataSetPtr internalDataSPtr_ );
/* Other methods */		
	/// Converts the external data into an internal representation - pure virtual
  virtual TDataSetPtr convertToInternal() = 0;
	/// Determines if Y-Axis should be swapped (conversion of left-handed to right-handed coordinate system)
	void setYSwapping( bool bSwapYAxis_ );
protected:  
  /// Smart pointer to a representation of the internal data
  TDataSetPtr internalDataSPtr;
	/// Stores if returned conversions should swap the y-axis
	bool bSwapYAxis;
};

} 
#endif //CSTRUCTUREDDATAADAPTER_H

