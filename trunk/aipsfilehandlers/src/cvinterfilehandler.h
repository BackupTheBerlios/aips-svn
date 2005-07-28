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
#ifndef AIPSCVINTERFILEHANDLER_H
#define AIPSCVINTERFILEHANDLER_H

// AIPS includes
#include "cbinaryfilehandler.h"

#include "cinterfileheader.h"

namespace aips {

/**
@author Patrick Musmann
*/

/**
 * A handler for the volume interfile file format
 * Files are build the following way:
 * <.hv> ascii header file
 * <.v> Data in raw format (float)
 */

class CVInterfileHandler : public CBinaryFileHandler
{
private:
  /// Copy constructor
  CVInterfileHandler( CVInterfileHandler& );
  /// Assignment operator
  CVInterfileHandler& operator=( CVInterfileHandler& );

public:
    /// Constructor
    CVInterfileHandler()
      throw();
    /// Destructor
    virtual ~CVInterfileHandler()
      throw();
    /// Loads a data set
    virtual TDataFile load( const std::string &sFilename )
      const throw( FileException );
    /// Saves a data set
    virtual void save( const std::string & sFilename, const TDataFile &theData )
      const throw( FileException );
private:
    /// Returns vector of default pairs
    const std::vector< std::pair<std::string, std::string> > initHV() const;
};

} // End namespace aips

#endif
