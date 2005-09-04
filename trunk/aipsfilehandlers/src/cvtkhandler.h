/************************************************************************
 * File: cvtkhandler.h                                                  *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A file header for the vtk image file format             *
 *                                                                      *
 * Author: Patrick Musmann (p.musmann@fz-juelich.de)                    *
 *         Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status:  Alpha                                                       *
 * Created: 2004-05-21 (PM)                                             *
 * Changed: 2004-10-16 Corrected an error in method save (HB)           *
 *          2004-12-22 save() rewritten (HB)                            *
 *          2004-12-23 Implemented load(), documented source code (HB)  *
 * TODO:    Support optional writing of ascii and xml vtk files         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CVTKHANDLER_H
#define CVTKHANDLER_H

#include <aipsfilehandlers_config.h>

#ifdef HAVE_VTK
// VTK includes
#include "vtkPointData.h"
#include "vtkShortArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkStructuredPointsReader.h"

// AIPS includes
#include <cbinaryfilehandler.h>

namespace aips {

/**
 * A file header for the vtk image file format. Currently only saving of 2D/3D unsigned short and double 
 * and loading of 3D ushort, float and double vtkStructuredPoints is supported. Origin and Spacing given
 * in the vtk files are currently ignored
 */
class CVtkHandler : public CBinaryFileHandler
{
private:
    /// Copy constructor
    CVtkHandler( CVtkHandler& );
    /// Assignment operator
    CVtkHandler& operator=( CVtkHandler& );
public:
    /// Constructor
    CVtkHandler() throw();
    /// Destructor
    virtual ~CVtkHandler() throw();
    /// Loads a data set
    virtual TDataFile load( const std::string &sFilename )
        const throw( FileException );
    /// Saves a data set
    virtual void save( const std::string & sFilename, const TDataFile &theData )
        const throw( FileException );
private:
		/// Converts a given vtkDataArray into an AIPS image volume
		template<typename TVar, typename TSet, typename TArray> 
			boost::shared_ptr<TSet> convertVTKImage( TArray* aVtkArray, std::vector<size_t> dim ) const;
};

} // End namespace aips

#endif // HAVE_VTK
#endif
