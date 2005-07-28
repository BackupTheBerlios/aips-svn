/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
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

#include <qapplication.h>

#define PROG_VAL( value ) 
#define PROG_MAX( value ) 
#define PROG_RESET() 
#define APP_PROC() 

/*
#define PROG_VAL( value ) notify( boost::shared_ptr<CProgressEvent>( new CProgressEvent( this, value ) ) );
#define PROG_MAX( value ) notify( boost::shared_ptr<CProgressStartEvent>( new CProgressStartEvent( this, value ) ) )
#define PROG_RESET() notify( boost::shared_ptr<CProgressResetEvent>( new CProgressResetEvent( this ) ) )
#define APP_PROC() qApp->processEvents( 10 )
*/
