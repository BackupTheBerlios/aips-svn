//
// C++ Interface: citkhandler
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CITKHANDLER_H
#define CITKHANDLER_H

#include <aipsbase_config.h>
#include <aipsfilehandlers_config.h>
#ifdef HAVE_ITK
#include <cbinaryfilehandler.h>
#include <citkadapter.h>

namespace aips {

/**
 *  
 */
class CITKHandler : public CBinaryFileHandler
{
private:
    /// Copy constructor
    CITKHandler( CITKHandler& );
    /// Assignment operator
    CITKHandler& operator=( CITKHandler& );
public:
    /// Constructor
    CITKHandler() throw();
    /// Destructor
    virtual ~CITKHandler() throw();
    /// Loads a data set
    virtual TDataFile load( const std::string &sFilename )
        const throw( FileException );
    /// Saves a data set
    virtual void save( const std::string & sFilename, const TDataFile &theData )
        const throw( FileException );
};

} // End namespace aips

#endif
#endif
