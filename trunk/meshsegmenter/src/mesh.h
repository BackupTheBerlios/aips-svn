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
#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <list>
#include <deque>
#include <meshcomponents.h>

using namespace std;

typedef list<SFace*> TFList;
typedef list<SEdge*> TEList;
typedef list<SVertex*> TVList;

class CMesh
{
public:
	list<SFace*> fList;
	list<SVertex*> vList;
	list<SEdge*> eList;
	vector<list<SVertex*> > bins;
	CMesh() : bins(8+8*36+8*256) {}
	~CMesh();
	ulong subdivide(double maxLength );
	void edgeMelt( double minLength );
	void edgeFlip( double maxLength );
	void triangleMelt( double minLength );
	void printFace( SFace* face );
	void printVertex( SVertex* vertex );
	void printEdge( SEdge* edge );
	void printMesh();
	void computeNormals();
	void checkTopology();
	void reset();
	void computeBins( bool odd = false, double minDist = 1.0 );
private:	
	CMesh( const CMesh& aMesh );
	CMesh& operator=( const CMesh& aMesh );
	deque<SFace*> facePool;
	deque<SVertex*> vertexPool;
	deque<SEdge*> edgePool;
};

#endif
