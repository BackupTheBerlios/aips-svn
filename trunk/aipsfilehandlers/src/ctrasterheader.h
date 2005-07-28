//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: P. Musmann <p.musmann@fz-juelich.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CTRASTERHEADER_H
#define CTRASTERHEADER_H


//base class
#include <cimageheader.h>

using namespace aips;


/**
@author Patrick Musmann
*/

/** A file header for the bin file format */
class CTRasterHeader : public CImageHeader
{
private:
    /// Copy constructor
    CTRasterHeader( CTRasterHeader& );
    /// Assignment operator
    CTRasterHeader& operator=( CTRasterHeader& );

public:
/* Structors */
    /// Constructor
  CTRasterHeader()
        throw();
    /// Destructor
  ~CTRasterHeader()
        throw();

/* Other methods */
    /// Loads the header from an open stream
    virtual void loadHeader( std::istream& theFile )
        throw( FileException );
    /// Saves the header to an open stream
    virtual void saveHeader( std::ostream& theFile )
        throw( FileException );
};

#endif
