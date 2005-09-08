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
#include "cdatasetcolors.h"

using namespace std;

CDataSetColors::CDataSetColors()
{	
	theLookupTable = vtkLookupTable::New();
	theLookupTable->SetTableRange( 0.0, 255.0 );
	theLookupTable->SetRampToLinear();
	theLookupTable->SetSaturationRange( 0.0, 0.0 );
	theLookupTable->SetValueRange( 0.0, 1.0 );
	theLookupTable->SetHueRange( 0.0, 0.0);
	theLookupTable->Build();
	theLookupTable->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 );
	theColorBar = vtkScalarBarActor::New();
	theColorBar->SetLookupTable( theLookupTable );
	theColorBar->SetOrientationToVertical();
	theColorBar->SetWidth( 0.05 );
	theColorBar->SetHeight( 0.9 );
	dClampValues[0] = 0.0;
	dClampValues[1] = 255.0;
}

CDataSetColors::~CDataSetColors()
{
	theLookupTable->Delete();
	theColorBar->Delete();
}

vtkLookupTable* CDataSetColors::getLookupTable()
{
	return theLookupTable;
}

vtkScalarBarActor* CDataSetColors::getColorBar()
{
	return theColorBar;
}

void CDataSetColors::loadLookupTable( std::string sFilename )
{
	theLookupTable->SetNumberOfTableValues( 256 );
	ifstream file ( sFilename.c_str() );
	unsigned char r[256],g[256],b[256];
	double color[4];
	color[3] = 1.0;
	file.read( (char*)r, 256 );
	file.read( (char*)g, 256 );
	file.read( (char*)b, 256 );
	file.close();
	for( int i = 0; i < 256; ++i )		
	{
		color[0] = static_cast<double>( r[i] ) / 255.0;
		color[1] = static_cast<double>( g[i] ) / 255.0;
		color[2] = static_cast<double>( b[i] ) / 255.0;		
		theLookupTable->SetTableValue( i, color );
	}	
	theLookupTable->GetTableValue( 0, color );
	color[3] = 0.0;
	theLookupTable->SetTableValue( 0, color );
}

void CDataSetColors::setUpperClamp( double dValue )
{
	dClampValues[1] = dValue;
	theLookupTable->SetTableRange( dClampValues );
}

void CDataSetColors::setLowerClamp( double dValue )
{
	dClampValues[0] = dValue;
	theLookupTable->SetTableRange( dClampValues );
}
