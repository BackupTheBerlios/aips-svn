/************************************************************************
 * File: cstructureddataadapter.h                                       *
 * Project: AIPS                                                        *
 * Description: Abstract adapter for structured data sets               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Alpha                                                       *
 * Created: 2004-12-22                                                  *
 * Changed: 2005-01-25 Added method setYSwapping and corr. data member  *
 *          2005-11-22 Added documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSTRUCTUREDDATAADAPTER_H
#define CSTRUCTUREDDATAADAPTER_H

#define CSTRUCTUREDDATAADAPTER_VERSION "0.3"

// AIPS includes
#include "cdataadapter.h"
#include "aipstypelist.h"

namespace aips { 

/**
 * \brief Abstract adapter for image datasets
 *
 * Data mapped through his adapter is always represented by rectangular grids.
 * This is a virtual class defining the interface for setting and getting
 * internal data (\see CDataSet ) only.
 *
 * \todo Add a dump function and verbosity support.
 * \todo Check if swapYAxis is really needed (I doubt that!).
 * \todo Write test cases and example code.
 * \todo Write a reset function.
 */
class CStructuredDataAdapter : public CDataAdapter 
{
private:
	/// Standard constructor
	CStructuredDataAdapter();
	/// Copy constructor
	CStructuredDataAdapter( const CStructuredDataAdapter& );
	/// Assignment operator
	CStructuredDataAdapter& operator=( const CStructuredDataAdapter& );
public:
/** \name Structors */
  //@{
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
  //@}
/** \name Accessors */
  //@{
	/// Sets the internal data pointer to the given dataset
  void setInternalData( TDataSetPtr internalDataSPtr_ );
  //@}
/** \name Other methods */
  //@{
	/// Converts the external data into an internal representation - pure virtual
  virtual TDataSetPtr convertToInternal() = 0;
	/// Determines if Y-Axis should be swapped (conversion of left-handed to right-handed coordinate system)
	void setYSwapping( bool bSwapYAxis_ );
  //@} 
protected:    
  TDataSetPtr internalDataSPtr; ///< Pointer to a representation of the internal data
	bool bSwapYAxis; ///< Stores if returned conversions should swap the y-axis
};

} 
#endif //CSTRUCTUREDDATAADAPTER_H

