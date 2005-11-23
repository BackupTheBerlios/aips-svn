/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: $DATE                                                       *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef DIFFUSIONDEFS_H
#define DIFFUSIONDEFS_H

#include <aipsnumeric.h>
#include <aipsbase_config.h>

namespace aips 
{

#ifdef USE_BLITZ
typedef blitz::TinyVector<double,26> TVectorKE;
#else
typedef CVector<double,26> TVectorKE;
#endif

}

#endif
