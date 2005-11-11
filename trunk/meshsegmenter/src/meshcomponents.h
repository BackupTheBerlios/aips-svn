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

#ifndef MESHCOMPONENTS_H
#define MESHCOMPONENTS_H
#include <aipsnumeric.h>
#include <deque>

/*#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>*/

using namespace aips;
using namespace std;

// struct TMeshTraits : public OpenMesh::DefaultTraits
// {
// 	typedef OpenMesh::Vec3d Point;
// 	typedef OpenMesh::Vec3d Normal;
// 	typedef OpenMesh::Vec3d Force;
// 	VertexAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Status );
// 	FaceAttributes( OpenMesh::Attributes::Normal | 
// 	OpenMesh::Attributes::Status );
// 	HalfedgeAttributes( OpenMesh::Attributes::PrevHalfedge |
// 	OpenMesh::Attributes::Status );
// 	EdgeAttributes( OpenMesh::Attributes::Status );
// 	VertexTraits
// 	{
// 		public:
// 		ulong ulID; ///< ID number (for backconversion into external formats)
// 		TVector3D theForce; ///< Actual force on the vertex
// 		deque<TVector3D> lastPositions;
// 		uint stability;
// 		bool isStable;
// 	};
// 	FaceTraits
// 	{
// 		int iTag; ///< A tag for various purposes
// 	};
// 	EdgeTraits
// 	{
// 		bool bDelete; // Mark for deletion
// 		bool bOld; // Mark that this is an old edge
// 	};
// };
// 
// typedef OpenMesh::TriMesh_ArrayKernelT<TMeshTraits> TMesh;

struct SVertex;
struct SFace;

struct SEdge
{
	SVertex* endPointPtr; // Vertex at the end of the half edge
	SEdge* opposingEdgePtr; // Adjacent half-edge
	SFace* theFacePtr; // Face the half-edge borders
	SEdge* nextEdgePtr; // Next half-edge around the face
	bool bDelete; // Mark for deletion
	bool bOld; // Mark that this is an old edge
	SEdge() 
		: endPointPtr( NULL ), opposingEdgePtr( NULL ), theFacePtr( NULL ), nextEdgePtr( NULL ),
			bDelete( false ), bOld( false )
	{
	}
	SEdge( SVertex* endPointPtr_, SFace* theFacePtr_, SEdge* nextEdgePtr_ = NULL, SEdge* opposingEdgePtr_ = NULL ) 
		: endPointPtr( endPointPtr_ ), opposingEdgePtr( opposingEdgePtr_ ), theFacePtr( theFacePtr_ ), 
			nextEdgePtr( nextEdgePtr_ ), bDelete( false ), bOld( false )
	{
	}
	void set( SVertex* endPointPtr_, SFace* theFacePtr_, SEdge* nextEdgePtr_, SEdge* opposingEdgePtr_ = NULL )
	{
		endPointPtr = endPointPtr_; 
		theFacePtr = theFacePtr_; 
		nextEdgePtr = nextEdgePtr_; 
		opposingEdgePtr = opposingEdgePtr_;
	}
	void update( SFace* theFacePtr_, SEdge* nextEdgePtr_ )
	{
		theFacePtr = theFacePtr_; 
		nextEdgePtr = nextEdgePtr_;
	}
};

/** A structure to save a surface vertex into */
struct SVertex
{
	TVector3D thePosition; ///< Spatial position of vertex
	SEdge* anEdgePtr;      ///< One of the half-edges emantating from the vertex
	ulong ulID; ///< ID number (for backconversion into external formats)
	TVector3D theForce; ///< Actual force on the vertex
	TVector3D theNormal; ///< Vertex normal (calculated out of the face normals of adjacent faces)
	deque<TVector3D> lastPositions;
	uint stability;
	bool isStable;
	/// Constructor to set the vertex explicitly
  explicit SVertex( const TVector3D& thePosition_ = 0.0 );
  void clear()
  {
  	isStable = false;
  	stability = 0;
  	lastPositions.clear();
  	theForce = 0.0;
  	theNormal = 0.0;
  	anEdgePtr = NULL;
  	thePosition = 0.0;
  }
};

struct SFace
{
	SEdge* anEdgePtr; ///< One of the half-edges bordering the face
	int iTag; ///< A tag for various purposes
	TVector3D theNormal; ///< Face normal vector
	/// Constructor to set an half-edge of the face explicitly
	explicit SFace( SEdge* anEdgePtr_ = NULL );
};

/* Implementation of inline functions */

/* SVertex */

/** \param position_ starting position of the vertex */
inline SVertex::SVertex( const TVector3D& thePosition_ ) 
		: thePosition( thePosition_ ), anEdgePtr( NULL ), ulID( 0 ), theForce ( 0.0 ), theNormal ( 0.0 ), stability(0), isStable(false)
{
}

/* SFace */

/** \param anEdge_ one of the half-edges bordering the face */
inline SFace::SFace( SEdge* anEdgePtr_ ) 
		: anEdgePtr( anEdgePtr_ ), iTag ( 0 ), theNormal( 0.0 )
{
}
#endif
