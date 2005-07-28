/************************************************************************
 * File: aipsinit.cpp                                                   *
 * Project: AIPS                                                        *
 * Description: AIPS initialization functions                           *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 20.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "aipsinit.h"

using namespace std;

/**
 * Initializes and runs AIPS
 * \param argc Number of arguments the program was called with
 * \param argv Arguments the program was called with
 */
void aips::init( int argc, char *argv[] )
{
  string filename = "screen";
  if ( argc > 1 ) filename = argv[1];
  initLogServer( filename, true );
  std::set_unexpected( aips::internal::aipsUnexpectedExceptionHandler );
  std::set_new_handler( aips::internal::aipsNewHandler );
  std::set_terminate( aips::internal::aipsTerminate );
  // Construct a new QApplication instance
  QApplication anApp( argc, argv );
  // Generate new main window
  CMainWin* mainWin = new CMainWin ( NULL );
  // Run application
  QObject::connect( &anApp, SIGNAL( lastWindowClosed() ),
                      &anApp, SLOT( quit()) );
  anApp.setMainWidget( mainWin );
  mainWin->show();
  int iAppStatus;
  try
  {
    iAppStatus = anApp.exec();
  }
	catch ( CException &e )
	{
		std::cerr << "<" << e.getErrorNumber() << "- ";
		switch( e.getExceptionType() )
		{
			case CException::RECOVER:
				std::cerr << "RECOVER"; break;
			case CException::WARNING:
				std::cerr << "WARNING"; break;		
			case CException::FATAL:
				std::cerr << "FATAL"; break;
			case CException::UNKNOWN:
				std::cerr << "UNKNOWN"; break;
		}
		std::cerr << " >" << e.what() << std::endl;
		std::terminate();
	}
  catch ( std::exception &e )
  {
    std::cerr << e.what() << endl; std::terminate();
  }
  if ( iAppStatus != 0 ) exit ( iAppStatus );  
}

/** The aips handler for unexpected exceptions */
void aips::internal::aipsUnexpectedExceptionHandler()
{
  std::cerr << "** Catched some unexpected exception." << endl;
  terminate();
}

/** The aips handler for failed calls of new() */
void aips::internal::aipsNewHandler()
{
  std::cerr << "** Memory allocation failed." << endl;
  terminate();
}

/** The aips termination routine */
void aips::internal::aipsTerminate()
{
  std::cerr << "* A fatal error has occured. AIPS is now terminating.\n"
            << "* Please send a mail describing the situation (and \n"
						<< "* containing any other useful information, like the \n"
						<< "* call stack and output of aips) where this happened to \n"
            << "* aips-devel@lists.sourceforge.net" << endl;
  exit( EXIT_FAILURE );
}
