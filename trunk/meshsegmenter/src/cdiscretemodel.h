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
#ifndef CDISCRETEMODEL_H
#define CDISCRETEMODEL_H

#include <mesh.h>
#include <boost/progress.hpp>
#include <aipsnumeric.h>
#include <csubject.h>

using namespace aips;
/**
@author Hendrik Belitz
*/
class CDiscreteModel : public CSubject
{
public:
  CDiscreteModel() throw();
  ~CDiscreteModel() throw();
	void setExternalForceField( TField3DPtr field_ );
	void setMesh( CMesh* mesh_ );
	//void setMesh( TMesh* mesh_ );
	void iterate();
private:
	CMesh* mesh;
	//TMesh* mesh;
  TField3DPtr field;
};

#endif
