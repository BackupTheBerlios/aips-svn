/************************************************************************
 * File: ccarraydataadapter.h                                           *
 * Project: AIPS base library                                           *
 * Description:   A data adapter to convert aips datasets into C-style  *
 *                arrays and vice versa                                 *
 *                                                                      *
 * Author: HendrikBelitz <hendrik@darkon.info>                          *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Alpha                                                       *
 * Created: 2005-01-24                                                  *
 * Changed: 2005-01-25 Added templatized helper functions               *
 *          2005-07-07 Added exception handling                         *
 *          2005-08-01 Added documentation                              *
 *                     Correction of minor errors                       *
 * TODO: Make this work for more than 3 dimensions                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCCARRAYDATAADAPTER_H
#define AIPSCCARRAYDATAADAPTER_H

#include <aipsdatatraits.h>
#include <cstructureddataadapter.h>

namespace aips {

/**
 * A data adapter to convert aips datasets into C-style arrays and vice versa
 * This class should only be used to convert C-style arrays into CDataSet objects and
 * to do the back-conversion if C array memory is not managed by yourself. Otherwise the
 * use of CDataSet.getArray() or CDataSet.getVoidArray() is strongly encouraged to safe
 * time and memory.
 * 
 * Some important issues of this class:
 * 1. Always provide the extents of an external C-style dataset
 * 2. Never try to support an external short array and a double array at the same time.
 *    By using one, the pointer to the other is effectively rendered illegal
 * 3. Extents and data dimensions of the internal representation are NOT converted. They can be
 *    extracted directly from the corrsponding CDataSet object via the appropiate member functions.
 * 4. If you are converting into external C Arrays, make sure that these are already allocated 
 */
class CCArrayDataAdapter : public CStructuredDataAdapter
{
private:
	/// Copy constructor
	CCArrayDataAdapter( const CCArrayDataAdapter& );
	/// Assignment operator
	CCArrayDataAdapter operator=( const CCArrayDataAdapter& );
public:
/* Structors */
	/// Constructor
  CCArrayDataAdapter() 
		throw(); 
	/// Constructor with additional parameter for internal data
  CCArrayDataAdapter( TDataSetPtr internalDataPtr_ )
		throw();  
	/// Destructor - pure virtual
  virtual ~CCArrayDataAdapter()
  	throw();  
/* Accessors */
	/// Sets the external data pointer to the given dataset (short input)
	void setExternalData( short* sImagePtr = NULL, size_t dimension = 0, 
		const size_t* const extents = NULL )
		throw( NullException ); 
	/// Sets the external data pointer to the given dataset (double input)
  void setExternalData( double* dFieldPtr = NULL, size_t dimension  = 0, 
		const size_t* const extents = NULL )
		throw( NullException );
/* Other methods */		
	/// Converts the external data into an internal representation (always returns a integer dataset)
	virtual TDataSetPtr convertToInternal()
		throw( NullException );
	/// Converts the external data into an internal representation
  TDataSetPtr convertToInternal( bool bConvertField )
  	throw( NullException );
	/// Returns external short array representing our internal data
	void convertToExternal( short* shortData )
		throw( NullException );
	/// Returns external double array representing our internal data
	void convertToExternal( double* doubleData )
		throw( NullException );
private:	
/* Internal member function templates */		
	/// Function template to convert internal data into C-Array types
	template<typename T> void actualConversion( T* dataPtr );
	/// Function template to convert external C-Array data into internal types
	template<typename T, typename U> TDataSetPtr actualBackConversion( T* inputPtr );
private:    
  short* externalShortDataPtr;     ///< Pointer to external array storing short data	
	double* externalDoubleDataPtr;   ///< Pointer to external array storing double data	
	std::vector<size_t> dataExtents; ///< External dataset dimensions
};

};

#endif
