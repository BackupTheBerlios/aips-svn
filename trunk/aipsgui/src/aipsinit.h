/************************************************************************
 * File: aipsinit.h                                                     *
 * Project: AIPS                                                        *
 * Description: AIPS initialization functions                           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Beta                                                        *
 * Created: 20.01.04                                                    *
 * Changed: 21.01.04 Added terminate-handler                            *
 *          23.04.04 Changed output of terminate handler                *
 *          27.04.04 Added precise output for possible CExceptions      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSINIT_H
#define AIPSINIT_H

// Standard includes
#include <iostream>
#include <new>

// AIPS includes
#include <clog.h>
#include "cmainwin.h"

// Qt includes
#include <qapplication.h>

namespace aips {
  /// Initializes and runs AIPS
  void init( int argc, char *argv[] );
  
  namespace internal {
    /// The aips handler for unexpected exceptions
    void aipsUnexpectedExceptionHandler();
    /// The aips handler for failed calls of new()
    void aipsNewHandler();
    /// The aips termination routine
    void aipsTerminate();
  }
}

#endif
