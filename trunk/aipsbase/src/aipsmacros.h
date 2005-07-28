/************************************************************************
 * File: aipsmacros.h                                                   *
 * Project: AIPS                                                        *
 * Description: Common macro definitions for debugging and error        *
 *              reporting purposes                                      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 1.0                                                         *
 * Status : Final                                                       *
 * Created: 21.01.04                                                    *
 * Changed: 26.01.04 Macro SERROR moved to file aipserror.h             *
 *          28.04.04 Added macro DS                                     *
 *          2004-09-14 Benchmarking now uses boost::timer. The aips     *
 *                     class CTimer is obsolete and was removed         *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
 
#ifndef AIPSMACROS_H
#define AIPSMACROS_H
 
#ifdef DL3
#define DL2
#endif
#ifdef DL2
#define DEBUG
#endif

/**
 * Easy debugging macros ( with three debug levels )
 * The following macros will be defined in respect of the debug level:
 * DBG-Level   Macro
 *   DEBUG     DBG(), DBG1()
 *    DL2      DBG(), DBG2()
 *    DL3      DBG(), DBG2(), DBG3(), FBEGIN, FEND
 */
#ifdef DEBUG
/* DS puts out something without function and line number */
#define DS(s) alog << LDEBUG << "[1] " << s << std::endl;
#define DBG(s) alog << LDEBUG << "[1]\n " << __PRETTY_FUNCTION__ << " line " << __LINE__ \
  << "\n " << s << std::endl
/* DBG1 is only an abbreviation of DBG */
#define DBG1(s) DBG(s)
#else
#define DS(s)
#define DBG(s)
#define DBG1(s)
#endif /* DEBUG */
#ifdef DL2
/// Debugging macro for level 2
#define DBG2(s) alog << LDEBUG << "[2]\n " << __PRETTY_FUNCTION__ << " line " << __LINE__ \
  << "\n " << s << std::endl
#else
#define DBG2(s)
#endif /* DL2 */
// Debugging level 3: Shows function start and begin
#ifdef DL3
/// Debugging macro for level 3
#define DBG3(s) alog << LDEBUG << "[3]\n " << __PRETTY_FUNCTION__ << " line " << __LINE__ \
  << "\n " << s << std::endl
/// Pretty print at function start
#define FBEGIN alog << "(+++++)\n" << __PRETTY_FUNCTION__ << "\n(+++++)" << std::endl
/// Pretty print at function end
#define FEND alog << "(-----)\n" << __PRETTY_FUNCTION__ << "\n(-----)" << std::endl
#else
#define DBG3(s)
#define FBEGIN
#define FEND
#endif /* DL3 */

/* Benchmarking macros */
#ifdef BENCHMARK
/** Use this macro to start a stop watch */
#define BENCHSTART boost::timer theStopWatch
/** Use this macro to restart the stop watch */
#define BENCHRESTART theStopWatch.restart()
/** Use this macro to print out the measured time */
#define BENCHSTOP alog << LINFO << "\n Elapsed time " << theStopWatch.elapsed() << endl
#else
#define BENCHSTART 
#define BENCHRESTART 
#define BENCHSTOP 
#endif

#endif /* AIPSMACROS_H */
