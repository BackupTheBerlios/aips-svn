/***********************************************************************
 * File: cinvert.h                                                     *
 * Project: AIPS image filters plugin library                          *
 * Description: Inverts the whole image                                *
 *                                                                     *
 * Author: Hendrik Belitz                                              *
 *                                                                     *
 * Version: 0.4                                                        *
 * Created: 28.08.03                                                   *
 * Changed: 09.09.03 Standardized variable names                       *
 *                   Exceptions added, attributes are now private      *
 *                   Constructor initializes constant parameters       *
 *          23.09.03 Filter now works on "short" data                  *
 *          29.09.03 Adapted filter for arbitrary intensity ranges     *
 *          06.05.04 Filter now works on 3D and on multichannel images *
 *                   Added documentation                               *
 ***********************************************************************/
#ifndef CINVERT_H
#define CINVERT_H

#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/* Inverts the intensity of the input */
class CInvert : public CFilter
{
private:
	/// Standard constructor 
	CInvert();
	/// Copy constructor
	CInvert( CInvert& );
	/// Assignment operator
	CInvert& operator= ( CInvert& );
public:
/* Structors */
	/// Constructor
  CInvert( ulong ulID )
    throw();
	/// Destructor
  virtual ~CInvert()
    throw();
/* Other methods */	
	/// Reimplemented from CPipelineItem 
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();    
	/// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
};

#endif
