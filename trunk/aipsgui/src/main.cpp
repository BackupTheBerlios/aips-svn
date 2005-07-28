/************************************************************************
 * File: main.cpp                                                       *
 * Project: AIPS                                                        *
 * Description: main program                                            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status:  Stable                                                      *
 * Created: 21.08.03                                                    *
 * Changed: 07.01.04 Moved the initialisation stuff to aips::init       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

/**
 * \mainpage This is the online documentation of the AIPS project sources.
 * Use this online reference as a base for the development of your own plugin-Libraries
 * or extensions to the main program.
 *
 * Please read the ChangeLog before overwriting any old version of AIPS
 *
 * Send your error reports (preferably with a screen dump of AIPS' output and 
 * the call stack output of your debugger) to aips-devel@lists.sourceforge.net
 */

#include <aipsinit.h>

int main ( int argc, char *argv[] )
{
	aips::init ( argc, argv );
  return EXIT_SUCCESS;
}


