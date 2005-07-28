/************************************************************************
 * File: cstructureddataadapter.h                                       *
 * Project: AIPS                                                        *
 * Description: Abstract adapter for structured data sets               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : PreAlpha                                                    *
 * Created: 22-12-2004                                                  *
 * Changed: 25-01-2004 Added method setYSwapping and corr. data member  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSTRUCTUREDDATAADAPTER_H
#define CSTRUCTUREDDATAADAPTER_H

// AIPS includes
#include "cdataadapter.h"
#include "aipsdatatraits.h"

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
		const std::string &sClassVersion_ = "0.1", const std::string &sDerivedFrom_ = "CDataAdapter ") throw(); 
	/// Constructor with additional parameter for internal data
  CStructuredDataAdapter( boost::shared_ptr<CDataSet> internalDataPtr_, 
		const std::string &sClassName_ = "CStructuredDataAdapter", const std::string &sClassVersion_ = "0.1", 
		const std::string &sDerivedFrom_ = "CDataAdapter " ) throw();  
	/// Destructor - pure virtual
  virtual ~CStructuredDataAdapter() throw() = 0;
/* Accessors */
	/// Sets the internal data pointer to the given dataset
  void setInternalData( TDataSetPtr internalDataPtr_ );
/* Other methods */		
	/// Converts the external data into an internal representation - pure virtual
  virtual TDataSetPtr convertToInternal() = 0;
	/// Determines if Y-Axis should be swapped (conversion of left-handed to right-handed coordinate system)
	void setYSwapping( bool bSwapYAxis_ );
protected:  
  /// Smart pointer to a representation of the internal data
  TDataSetPtr internalDataPtr;
	/// Stores if returned conversions should swap the y-axis
	bool bSwapYAxis;
};

} 
#endif //CSTRUCTUREDDATAADAPTER_H

