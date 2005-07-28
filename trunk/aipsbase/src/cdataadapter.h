/************************************************************************
 * File: cdataadapter.h                                                 *
 * Project: AIPS                                                        *
 * Description: An abstract base class for adapters to external libs    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 22.01.04                                                    *
 * Changed: 26.01.04 saveData now throws an exception if given          *
 *                   voxel size is greater than 2                       *
 *          27.01.04 Corrected wrong endianess on saving and loading    *
 *          24.04.04 Deleted unneccessary includes                      *
 *                   Added writing of big and little endianess files    *
 *          27.04.04 Updated docs and corrected minor errors            *
 *        2004-11-22 load and save now handle multiple data types       *
 *        2004-11-24 Class now uses boost::shared_ptr                   *
 *        2004-11-25 Corrected some errors in load() and save()         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef CDATAADAPTER_H
#define CDATAADAPTER_H

// Standard includes
#include <string> // std::string

// AIPS includes
#include "cbase.h"

namespace aips { 

/**
 * CDataAdapter is an abstract interface to exchange data between the aips data structures 
 * (like CDataSet and CMesh) and other libraries/applications (e.g. AVS, ITK, VTK). 
 * For specific data types and external libraries this class will be specialized.
 * 
 * This class provides a bidirectional interface. 
 * Through this it can be used to convert internal structures into external structures and vice versa.
 */
class CDataAdapter : public CBase
{
private:
	/// Standard constructor
	CDataAdapter();
	/// Copy constructor
	CDataAdapter( const CDataAdapter& );
	/// Assignment operator
	//CDataAdapter operator=( const CDataAdapter& ); //icc returning abtract class is not allowed
public:
/* Structors */
	/// Constructor
  CDataAdapter( const std::string &sClassName_ = "CDataAdapter", const std::string &sClassVersion_ = "0.1", 
		const std::string &sDerivedFrom_ = "CBase" ) throw();	
	/// Destructor
  virtual ~CDataAdapter() throw() = 0;
};

} 
#endif //CDATAADAPTER_H

