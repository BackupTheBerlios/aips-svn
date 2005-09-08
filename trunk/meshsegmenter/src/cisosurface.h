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
#ifndef CISOSURFACE_H
#define CISOSURFACE_H

#include <aipsdatatraits.h>

using namespace aips;

/**
@author Hendrik Belitz
*/

struct TriangleKey
{
public:
    TriangleKey (int iV0 = -1, int iV1 = -1, int iV2 = -1);
    bool operator< (const TriangleKey& rkKey) const;
    int V[3];
};

inline TriangleKey::TriangleKey (int iV0, int iV1, int iV2)
{
    if ( iV0 < iV1 )
    {
        if ( iV0 < iV2 )
        {
            // v0 is minimum
            V[0] = iV0;
            V[1] = iV1;
            V[2] = iV2;
        }
        else
        {
            // v2 is minimum
            V[0] = iV2;
            V[1] = iV0;
            V[2] = iV1;
        }
    }
    else
    {
        if ( iV1 < iV2 )
        {
            // v1 is minimum
            V[0] = iV1;
            V[1] = iV2;
            V[2] = iV0;
        }
        else
        {
            // v2 is minimum
            V[0] = iV2;
            V[1] = iV0;
            V[2] = iV1;
        }
    }
}
//----------------------------------------------------------------------------
inline bool TriangleKey::operator< (const TriangleKey& rkKey) const
{
    if ( V[2] < rkKey.V[2] )
        return true;
    if ( V[2] > rkKey.V[2] )
        return false;
    if ( V[1] < rkKey.V[1] )
        return true;
    if ( V[1] > rkKey.V[1] )
        return false;
    return V[0] < rkKey.V[0];
}

class CIsoSurface{
public:
    // The input is a 3D image with lexicographically ordered voxels in
    // (x,y,z) stored in a linear array.  Voxel (x,y,z) is stored in the
    // array at location x+xbound*(y+ybound*z).  The caller is responsible
    // for deleting aiData if it was dynamically allocated.
    CIsoSurface (int iXBound, int iYBound, int iZBound, int* aiData);

    // The level value *must* not be exactly an integer.  This simplifies the
    // level surface construction immensely.
    void ExtractContour (float fLevel, std::vector<TVector3D>& rkVA,
        std::vector<TriangleKey>& rkTA);

    // The extraction has duplicate vertices on edges shared by voxels.  This
    // function will eliminate the duplication.
    void MakeUnique (std::vector<TVector3D>& rkVA,
        std::vector<TriangleKey>& rkTA);

    // The extraction does not use any topological information about the level
    // surface.  The triangles can be a mixture of clockwise-ordered and
    // counterclockwise-ordered.  This function is an attempt to give the
    // triangles a consistent ordering by selecting a normal in approximately
    // the same direction as the average gradient at the vertices (when
    // bSameDir is true), or in the opposite direction (when bSameDir is
    // false).  This might not always produce a consistent order, but is
    // fast.  A consistent order can be computed if you build a table of
    // vertex, edge, and face adjacencies, but the resulting data structure
    // is very expensive to process to reorient triangles.
    void OrientTriangles (std::vector<TVector3D>& rkVA,
        std::vector<TriangleKey>& rkTA, bool bSameDir);

protected:
    enum
    {
        EI_XMIN_YMIN =  0,
        EI_XMIN_YMAX =  1,
        EI_XMAX_YMIN =  2,
        EI_XMAX_YMAX =  3,
        EI_XMIN_ZMIN =  4,
        EI_XMIN_ZMAX =  5,
        EI_XMAX_ZMIN =  6,
        EI_XMAX_ZMAX =  7,
        EI_YMIN_ZMIN =  8,
        EI_YMIN_ZMAX =  9,
        EI_YMAX_ZMIN = 10,
        EI_YMAX_ZMAX = 11,
        FI_XMIN      = 12,
        FI_XMAX      = 13,
        FI_YMIN      = 14,
        FI_YMAX      = 15,
        FI_ZMIN      = 16,
        FI_ZMAX      = 17,

        EB_XMIN_YMIN = 1 << EI_XMIN_YMIN,
        EB_XMIN_YMAX = 1 << EI_XMIN_YMAX,
        EB_XMAX_YMIN = 1 << EI_XMAX_YMIN,
        EB_XMAX_YMAX = 1 << EI_XMAX_YMAX,
        EB_XMIN_ZMIN = 1 << EI_XMIN_ZMIN,
        EB_XMIN_ZMAX = 1 << EI_XMIN_ZMAX,
        EB_XMAX_ZMIN = 1 << EI_XMAX_ZMIN,
        EB_XMAX_ZMAX = 1 << EI_XMAX_ZMAX,
        EB_YMIN_ZMIN = 1 << EI_YMIN_ZMIN,
        EB_YMIN_ZMAX = 1 << EI_YMIN_ZMAX,
        EB_YMAX_ZMIN = 1 << EI_YMAX_ZMIN,
        EB_YMAX_ZMAX = 1 << EI_YMAX_ZMAX,
        FB_XMIN      = 1 << FI_XMIN,
        FB_XMAX      = 1 << FI_XMAX,
        FB_YMIN      = 1 << FI_YMIN,
        FB_YMAX      = 1 << FI_YMAX,
        FB_ZMIN      = 1 << FI_ZMIN,
        FB_ZMAX      = 1 << FI_ZMAX
    };

    // vertex-edge-triangle table to support mesh topology
    class VETable
    {
    public:
        VETable ();

        bool IsValidVertex (int i) const;
        float GetX (int i) const;
        float GetY (int i) const;
        float GetZ (int i) const;
        void Insert (int i, const TVector3D& rkP);
        void Insert (int i0, int i1);
        void RemoveTriangles (std::vector<TVector3D>& rkVA,
            std::vector<TriangleKey>& rkTA);

    protected:
        void RemoveVertex (int i);
        bool Remove (TriangleKey& rkT);

        class Vertex
        {
        public:
            Vertex ();

            TVector3D P;
            int AdjQuantity;
            int Adj[4];
            bool Valid;
        };

        Vertex m_akVertex[18];
    };

    int GetVertices (float fLevel, int iX, int iY, int iZ, VETable& rkTable);

    void GetXMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetXMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetYMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetYMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetZMinEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);
    void GetZMaxEdges (int iX, int iY, int iZ, int iType, VETable& rkTable);

    TVector3D GetGradient (TVector3D kP);

    int m_iXBound, m_iYBound, m_iZBound, m_iXYBound;
    int* m_aiData;
};

//----------------------------------------------------------------------------
inline bool CIsoSurface::VETable::IsValidVertex (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].Valid;
}
//----------------------------------------------------------------------------
inline float CIsoSurface::VETable::GetX (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].P[0];
}
//----------------------------------------------------------------------------
inline float CIsoSurface::VETable::GetY (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].P[1];
}
//----------------------------------------------------------------------------
inline float CIsoSurface::VETable::GetZ (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].P[2];
}
//----------------------------------------------------------------------------
inline CIsoSurface::VETable::Vertex::Vertex ()
{
    AdjQuantity = 0;
    Valid = false;
}
#endif
