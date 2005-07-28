/************************************************************************
 * File: csliceselector.h                                               *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Takes a single orthogonal slice from a 3D volume and    *
 *              converts it to a 2D image                               *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.6                                                         *
 * Status : Alpha                                                       *
 * Created: 10.10.03                                                    *
 * Changed: 28.01.04 Class now inherits from CConverter                 *
 *                   Added documentation, clarified code                *
 *                   Module now also accepts 2D images as input         *
 *          05.02.04 Corrected an error regarding the slice parameter   *
 *          22.04.04 Slicer now uses Blitz++ array slicing if available *
 *          05.05.04 Current slice and ori are now local variables      *
 *                   Slice selector now also works on multichannel data *
 *                   Slice selector now also works on vector fields     * 
 *          21.12.04 Updated class for new aips version. Introduced     *
 *                   member function template slice<> to enable slicing *
 *                   of all four dataset types                          * 
 *        2005-01-26 Fixed a bug which sliced the z-axis in a wrong     *
 *                    manner                                            *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CSLICESELECTOR_H
#define CSLICESELECTOR_H

// AIPS includes
#include <cconverter.h>
#include <aipsdatatraits.h>

// Library includes
#include "libid.h"
// Dialog includes
#include "cslicerdialog.h"

using namespace aips;

/**
 * Takes a single orthogonal slice from a 3D volume and converts it to a 2D image
 */
class CSliceSelector : public CConverter, CObserver
{
private:
  /// Standard constructor
  CSliceSelector();
  /// Copy constructor
  CSliceSelector( CSliceSelector& );
  /// Assignment operator
  CSliceSelector& operator=( CSliceSelector& );
public:
/* Structors */
  /// Constructor
  CSliceSelector( ulong ulID )
    throw();
  /// Destructor
  virtual ~CSliceSelector()
    throw();
/* Functions from CConverter implemented here */
  virtual void apply()
    throw();
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual const std::string dump() const
    throw(); 
	/// Corresponding slot for functor
  void updateSlice( int iSlice );
	/// Corresponding slot for functor
  void updateOrientation( int iOrientation );
	virtual void execute( boost::shared_ptr<CEvent> anEvent );
private:
	/// Internal method template for the actual slicing process
	template<typename T> bool slice() throw();
	template<unsigned int index> void call() throw();
private:	
	boost::shared_ptr<CSlicerDialog> myDialog; ///< Associated dialog	
};

#endif
