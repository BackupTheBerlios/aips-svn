/************************************************************************
 * File: cdataadapter.h                                                 *
 * Project: AIPS                                                        *
 * Description: An abstract base class for adapters to external libs    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.3                                                         *
 * Status : Beta                                                        *
 * Created: 2004-01-22                                                  *
 * Changed:                                                             *
 *        2004-01-26 saveData now throws an exception if given          *
 *                   voxel size is greater than 2                       *
 *        2004-01-27 Corrected wrong endianess on saving and loading    *
 *        2004-04-24 Deleted unneccessary includes                      *
 *                   Added writing of big and little endianess files    *
 *        2004-04-27 Updated docs and corrected minor errors            *
 *        2004-11-22 load and save now handle multiple data types       *
 *        2004-11-24 Class now uses boost::shared_ptr                   *
 *        2004-11-25 Corrected some errors in load() and save()         *
 *        2005-11-20 Updated documentation                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef CDATAADAPTER_H
#define CDATAADAPTER_H

#define CDATAADAPTER_VERSION "0.3"

// Standard includes
#include <string> // std::string

// AIPS includes
#include "cbase.h"

namespace aips { 

/**
 * CDataAdapter is an abstract interface to exchange data between the aips data structures 
 * (like CDataSet and CMesh) and other libraries/applications (e.g. ITK, VTK, ImageMagick).
 * For specific data types and external libraries this class will be specialized.
 * 
 * This class provides a bidirectional interface which can be used to convert internal
 * structures into external structures and vice versa.
 */
class CDataAdapter : public CBase
{
private:

	/// Standard constructor
	CDataAdapter();
	/// Copy constructor
	CDataAdapter( const CDataAdapter& );
	/// Assignment operator
	CDataAdapter& operator=( const CDataAdapter& );
public:
/** \name Structors */
  //@{
	/// Constructor
  CDataAdapter( const std::string &sClassName_ = "CDataAdapter",
    const std::string &sClassVersion_ = CDATAADAPTER_VERSION, const std::string &sDerivedFrom_ = "CBase" )
    throw();
	/// Destructor
  virtual ~CDataAdapter()
    throw() = 0;
  //@}  
};

} 
#endif //CDATAADAPTER_H

