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
#include "cdiscretemodel.h"

CDiscreteModel::CDiscreteModel()throw()
{
}


CDiscreteModel::~CDiscreteModel()throw()
{
}

void CDiscreteModel::setExternalForceField( TField3DPtr field_ )
{
	field = field_;
}

void CDiscreteModel::setMesh( TMesh* mesh_ )
{
	mesh = mesh_;
}

void CDiscreteModel::iterate()
{
	double disc = 4.0;
	double internal = 0.025;
	boost::timer t;
	std::vector<size_t> extents = field->getExtents();
	edgeMelt( *mesh, 0.5*disc );
/*	for( list<SVertex*>::iterator vit = mesh->vList.begin(); vit != mesh->vList.end(); ++vit )
	{
		swap( (*vit)->position[2], (*vit)->position[1] );
	}*/
		
	while( disc >= 4.0 )
	{
		double divisor = 0.2;
		double threshold = 0.75;
/*		while( mesh->subdivide( 1.25*disc ) > 0 )
			cerr << ".";*/
		boost::progress_display show( 1000 );
		ulong numv = 0;
		for(int i = 0; i < 1000; ++i )
		{
			ulong stableNodes = 0;
			if (i%(static_cast<long>(disc))==0)
			{
				subdivide( *mesh,1.5*disc );
				edgeMelt( *mesh,0.5*disc );
 			}
 			numv = 0;
 			TMesh::VertexIter v_it, v_end = mesh->vertices_end();
			for( v_it = mesh->vertices_begin(); v_it != v_end; ++v_it, ++numv )
				v_it->theForce = 0.0;
			computeBins( *mesh, !((i%2)==0), disc*0.5 );
			mesh->update_normals();
			for( v_it = mesh->vertices_begin(); v_it != v_end; ++v_it )
			{
				if (!v_it->isStable)
				{					
					// Bending force, Find neighbors
					TVector3D cog = 0.0;
					uint uiNeighbours = 0;
					TMesh::VertexVertexIter vv_it;
					for( vv_it = mesh->vv_iter( v_it ); vv_it; ++vv_it )
					{
						OpenMesh::Vec3d p = mesh->point( vv_it );
						TVector3D t( p[0], p[1], p[2] );
						cog += t;
						uiNeighbours++;						
					}
					cog /= static_cast<double>( uiNeighbours );
					OpenMesh::Vec3d p = mesh->point( v_it );
					TVector3D thePosition( p[0], p[1], p[2] );
					p = mesh->normal( v_it );					
					TVector3D theNormal( p[0], p[1], p[2] );
					// Mark transition and update force
					TVector3D curvature = cog - thePosition;
					double dForceStrength = dot( curvature, theNormal ); 
					TVector3D force1 = curvature * dForceStrength * 1.0 / norm(curvature);
					curvature = cog - ( thePosition + force1 );
					dForceStrength = dot( curvature, theNormal ); 
					TVector3D force2 = curvature * dForceStrength * -1.1 / norm(curvature);
					TVector3D inner = ( force1 + force2 ) * internal;
					if ( norm(inner) > 1.0 )
						inner /=	norm(inner);
					v_it->theForce += inner;
					// Add balloon force
					//(*vit)->theForce += (*vit)->theNormal * -0.001;
					// Add external force		
					if ( thePosition[0] > 0.0 && thePosition[0] < static_cast<double>(extents[0])
						&& thePosition[1] > 0.0 && thePosition[1] < static_cast<double>(extents[1])
						&& thePosition[2] > 0.0 && thePosition[2] < static_cast<double>(extents[2]) )
					{
						const TVector3D& pos = thePosition;
/*						TVector3D nb[8];
  					double vals[8];
	  				nb[0][0] = static_cast<int>( ceil( pos[0] ) );  nb[0][1] = static_cast<int>( floor( pos[1] ) ); nb[0][2] = static_cast<int>( floor( pos[2] ) );
  					nb[1][0] = static_cast<int>( ceil( pos[0] ) );  nb[1][1] = static_cast<int>( ceil( pos[1] ) ); nb[1][2] = static_cast<int>( floor( pos[2] ) );
	  				nb[2][0] = static_cast<int>( floor( pos[0] ) ); nb[2][1] = static_cast<int>( floor( pos[1] ) ); nb[2][2] = static_cast<int>( floor( pos[2] ) );
	  				nb[3][0] = static_cast<int>( floor( pos[0] ) ); nb[3][1] = static_cast<int>( ceil( pos[1] ) ); nb[3][2] = static_cast<int>( floor( pos[2] ) );
		  			nb[4][0] = static_cast<int>( ceil( pos[0] ) );  nb[4][1] = static_cast<int>( floor( pos[1] ) ); nb[4][2] = static_cast<int>( ceil( pos[2] ) );
  					nb[5][0] = static_cast<int>( ceil( pos[0] ) );  nb[5][1] = static_cast<int>( ceil( pos[1] ) ); nb[5][2] = static_cast<int>( ceil( pos[2] ) );
	  				nb[6][0] = static_cast<int>( floor( pos[0] ) ); nb[6][1] = static_cast<int>( floor( pos[1] ) ); nb[6][2] = static_cast<int>( ceil( pos[2] ) );
  					nb[7][0] = static_cast<int>( floor( pos[0] ) ); nb[7][1] = static_cast<int>( ceil( pos[1] ) ); nb[7][2] = static_cast<int>( ceil( pos[2] ) );
  					double sum = 0.0;
	  				for( int i = 0; i < 8; ++i )
  					{
  						vals[i] = sqrt(3.0) - norm( nb[i]-pos );
  						sum +=vals[i];
  					}
  					TVector3D eforce = 0.0;
	  				for( int i = 0; i < 8; ++i )
  						eforce += ( (vals[i]/sum) * (*field)( static_cast<ushort>( nb[i][0] ),
  							static_cast<ushort>( nb[i][1] ), static_cast<ushort>( nb[i][2] ) ) );*/
  					TVector3D eforce = (*field)( static_cast<ushort>( round( pos[0] ) ),
  							static_cast<ushort>( round( pos[1] ) ), static_cast<ushort>( round( pos[2] ) ) ) ;
						eforce = dot(theNormal,eforce) * theNormal;
//cerr << norm(eforce) << endl;
						if ( norm(eforce) < 0.1 && norm(eforce) > 0.0 )
							eforce = eforce / norm(eforce) * 0.1;
						v_it->theForce += eforce;
					}
else
{
	cerr << thePosition << " <> " << static_cast<double>(extents[0]) << "," << static_cast<double>(extents[1])
		<< "," << static_cast<double>(extents[2]) << endl ;
}
					double forceNorm = norm ( v_it->theForce );
					if ( forceNorm > 0.01 )
					{
						if ( forceNorm > 1.0 )
							v_it->theForce /= forceNorm;					
						v_it->lastPositions.push_back(thePosition);
						thePosition += (0.5*v_it->theForce);
						p = mesh->point( v_it );
// 						cerr << p[0] << " " << p[1] << " " << p[2] << endl;
						p[0] = thePosition[0]; p[1] = thePosition[1]; p[2] = thePosition[2];
						mesh->set_point( v_it, p );
// 						cerr << p[0] << " " << p[1] << " " << p[2] << endl << "-" << endl;
						if( i > 8 ) 
						{
							size_t size = v_it->lastPositions.size();
							TVector3D mean = 0.0;						
							for( deque<TVector3D>::iterator vi = v_it->lastPositions.begin(); vi != v_it->lastPositions.end(); ++vi )
								mean += (*vi);
							mean /= static_cast<double>(size);
							if ( i > 50 && norm( thePosition - mean ) < divisor ) v_it->stability++;
							if ( size > 10 ) v_it->lastPositions.pop_front();
						}
					}
					else
					{
						v_it->stability++;
					}
					if( v_it->stability > 100 )
						v_it->isStable = true;					
				}
				else
				{
					stableNodes++;
					v_it->theForce[0] = 1.0;
				}
			}
			if ( static_cast<double>(stableNodes) > ( threshold * static_cast<double>(numv)) )
			{
				threshold += 0.05;
				divisor += 0.025;
			}
			if ( i%5 == 0 )
			{
				boost::shared_ptr<CEvent> e( new CEvent( this, 0 ) );
				notify( e );
			}
			else
			{
				boost::shared_ptr<CEvent> e( new CEvent( this, 1 ) );
				notify( e );
			}
			++show;
			if ( static_cast<double>(stableNodes) > ( 0.95 * static_cast<double>(numv)) )
			{
				cerr << "Stability reached after " << i << " iterations" << endl;
				i=1000;
			}			
		} // FOR iterations
		cerr << "Mesh consists of " << numv << " vertices" << endl;
		disc /= 2.0;
		internal *= 10.0;
		cerr << "Reinitialize mesh .. ";
		TMesh::VertexIter v_it, v_end = mesh->vertices_end();
		for( v_it = mesh->vertices_begin(); v_it != v_end; ++v_it )
		{
			v_it->isStable = false;
			v_it->stability = 0;
			v_it->lastPositions.clear();
		}
		cerr << "done" << endl;
		double timetaken = t.elapsed();		
		cerr << "Iteration with dr = " << disc*2.0 << " took " << timetaken << " secs ( " << timetaken/500.0 << " pI ) " << endl;
		t.restart();
	} // WHILE multiscale
	cerr << endl;
	// Now save the mesh to a file
	cerr << "Saving mesh to mousebrain.mesh" << endl;
	ulong id = 0;
	TMesh::VertexIter v_it, v_end = mesh->vertices_end();
	for( v_it = mesh->vertices_begin(); v_it != v_end; ++v_it, ++id )
	{
		v_it->ulID = id;
	}
/*	ofstream file( "mousebrain.mesh" );
	file << id << endl;
	for( list<SVertex*>::iterator vit = mesh->vList.begin(); vit != mesh->vList.end(); ++vit, ++id )
	{
		file << (*vit)->thePosition[0] << " " << (*vit)->thePosition[1] << " " << (*vit)->thePosition[2] << endl;
	}
	file << mesh->fList.size() << endl;
	for( list<SFace*>::iterator fit = mesh->fList.begin(); fit != mesh->fList.end(); ++fit )
	{
		file << (*fit)->anEdgePtr->endPointPtr->ulID << " ";
		file << (*fit)->anEdgePtr->nextEdgePtr->endPointPtr->ulID << " ";
		file << (*fit)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->ulID << endl;
	}
	file.close();*/
}

