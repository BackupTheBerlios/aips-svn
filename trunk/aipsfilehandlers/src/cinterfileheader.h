/***************************************************************************
 *   Copyright (C) 2004 by Patrick Musmann                                 *
 *   p.musmann@fz-juelich.de                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CINTERFILEHEADER_H
#define CINTERFILEHEADER_H

#include <cimageheader.h>

// Standard includes
#include <string>
#include <vector>
#include <sstream>
//#include <algorithm>

using namespace aips;

/**
@author Patrick Musmann
*/
/** A file header for the interfile file format */
class CInterfileHeader : public CImageHeader
{
private:
    /// Copy constructor
    CInterfileHeader( CInterfileHeader& );
    /// Assignment operator
    CInterfileHeader& operator=( CInterfileHeader& );

public:
/* Structors */
    /// Constructor
    CInterfileHeader()
      throw();
    /// Constructor with default pairs
    CInterfileHeader( const std::vector<std::pair<std::string, std::string> > &aPairVector )
      throw();
    /// Destructor
    ~CInterfileHeader()
       throw();

/* Other methods */
    /// Loads the header from an open stream
    virtual void loadHeader( std::istream& theFile )
        throw( FileException );
    /// Saves the header to an open stream
    virtual void saveHeader( std::ostream& theFile )
        throw( FileException );

private:
/* Properties */
    /// ordered header words
//     std::vector<std::string> _theHwVector;

/* static methods */
    /// Scan line for key pair value separted by separator
    static const bool scanLine( const std::string &sLine, const std::string &sSeparator, std::string &sKey, std::string &sValue );
    /// Remove leading and trailing white spaces
    static const std::string rmSurroundingChars( const std::string &aString, const std::string &sChars = " \f\n\r\t\v" );

    /// Returns Converts a string to T_numtype value
    template<class T_numtype>
    static const T_numtype string2numtype( const std::string &stringValue )
    {
        T_numtype numtypeValue(0);
        std::istringstream iss( stringValue );
        iss >> numtypeValue;
        return numtypeValue;
    }
};

#endif
