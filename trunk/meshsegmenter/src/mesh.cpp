
#include "mesh.h"
 
// void checkTopology( TMesh& mesh ) {}
// void subdivide( TMesh& mesh, double length){}
// void edgeMelt( TMesh& mesh, double length)
// {
// 	cerr << "Melting edges" << endl;
// 	// Check length of edges. Collapse edges that are too small
// 	TMesh::HalfedgeIter h_it, h_end = mesh.halfedges_end();
// 	bool done = false;
// 	while( !done )
// 	{
// 		done = true;
// 	for( h_it = mesh.halfedges_begin(); h_it != h_end; ++h_it )
// 	{
// 		OpenMesh::Vec3d p1 = mesh.point( mesh.to_vertex_handle( h_it.handle() ));
// 		OpenMesh::Vec3d p2 = mesh.point( mesh.from_vertex_handle( h_it.handle() ) );
// 		p1 -= p2;
// 		if ( p1.length() < length && mesh.is_collapse_ok( h_it.handle() ) )
// 		{
// 			cerr << p1.length() << " < " << length << endl;
// 			TMesh::HalfedgeHandle h = h_it.handle();
// 			cerr << h.idx() << endl;
// 			cerr << mesh.has_halfedge_status() << endl;
// 			cerr << mesh.n_halfedges() << endl;
// 			mesh.collapse( h );
// 			done = false;
// 			mesh.garbage_collection();
// 			break;
// 		}
// 	}
// 	}
// }

//void computeBins( TMesh& mesh, bool odd, double minDist ){}
  
#ifdef VERBOSE
#define PR(s) cerr << s;
#else
#define PR(s)
#endif

bool edgeLengthComp( SEdge* a, SEdge* b )
{
	SVertex* vv3 = a->endPointPtr;
	SVertex* vv1 = a->nextEdgePtr->nextEdgePtr->endPointPtr;
	double lengthA = norm( vv3->thePosition - vv1->thePosition );
	vv3 = b->endPointPtr;
	vv1 = b->nextEdgePtr->nextEdgePtr->endPointPtr;
	double lengthB = norm( vv3->thePosition - vv1->thePosition );
	return ( lengthA < lengthB );
}

bool veq( const TVector3D& a, const TVector3D& b, const double small = 0.00001 )
{
	bool equal = (fabs(b[0] - a[0])<=small) 
	&& (fabs(b[1] - a[1])<=small)
	&& (fabs(b[2] - a[2])<=small);
	return ( equal );
}
	
void progress()
{
	static int ctn = 0;
	switch( ctn )
	{
		case 0: fprintf( stderr,"-\b"); break;
		case 1: fprintf( stderr,"/\b"); break;
		case 2: fprintf( stderr,"|\b"); break;
		case 3: fprintf( stderr,"\\\b"); break;
	}						
	ctn++; if (ctn == 4) ctn = 0;	
}


CMesh::~CMesh()
{
	reset();
}

void CMesh::reset()
{
	for( list<SFace*>::iterator it = fList.begin(); it != fList.end(); ++it )
	{
		delete(*it);
	}
	for( list<SVertex*>::iterator it = vList.begin(); it != vList.end(); ++it )
	{
		delete(*it);
	}
	for( list<SEdge*>::iterator it = eList.begin(); it != eList.end(); ++it )
	{
		delete(*it);
	}
	for( deque<SFace*>::iterator it = facePool.begin(); it != facePool.end(); ++it )
	{
		delete(*it);
	}
	for( deque<SVertex*>::iterator it = vertexPool.begin(); it != vertexPool.end(); ++it )
	{
		delete(*it);
	}
	for( deque<SEdge*>::iterator it = edgePool.begin(); it != edgePool.end(); ++it )
	{
		delete(*it);
	}
	fList.clear();
	vList.clear();
	eList.clear();
	vertexPool.clear();
	facePool.clear();
	edgePool.clear();
}

void CMesh::printFace( SFace* face )
{
	printEdge( face->anEdgePtr );
	printEdge( face->anEdgePtr->nextEdgePtr );
	printEdge( face->anEdgePtr->nextEdgePtr->nextEdgePtr );
}

void CMesh::printVertex( SVertex* vertex )
{
	cerr << vertex->thePosition << endl;
}

void CMesh::printEdge( SEdge* edge )
{
	//cerr << "ID: " << edge->lID << ", nextEdgePtr: " << edge->nextEdgePtr->lID << ", face: " << edge->theFacePtr->anEdgePtr->lID;
	cerr << ",opposingEdgePtr: ";
/*	if ( edge->opposingEdgePtr )
		cerr << edge->opposingEdgePtr->lID;
	else
		cerr << "NULL";*/
	cerr << endl << ", endpoint: "; printVertex( edge->endPointPtr );
}

void CMesh::printMesh()
{
	cout << "Faces" << endl << "--------" << endl;
 	for( list<SFace*>::iterator it = fList.begin(); it != fList.end(); ++it )
	{ 
		printFace( *it ); 
		cerr << "---" << endl; 
	}
 	cout << "Vertices" << endl << "---------------" << endl;
 	for( list<SVertex*>::iterator it = vList.begin(); it != vList.end(); ++it )
		printVertex( *it );
	cout << "Edges" << endl << "--------" << endl;
	for( list<SEdge*>::iterator it = eList.begin(); it != eList.end(); ++it )
		printEdge( *it );
}

struct RegPair
{
	SEdge* anEdgePtr; // The half edge that should be splitted
	SVertex* nV; //the Vertex to insert
};

struct Rebuild
{
	SEdge* oldEdge; // The half edge that should be splitted
	SEdge* newEdge1; //the Vertex to insert
	SEdge* newEdge2; //the Vertex to insert
	bool operator==( const Rebuild& r )
	{
		return( oldEdge == r.oldEdge && newEdge1 == r.newEdge1 && newEdge2 == r.newEdge2 );
	}
};

void CMesh::triangleMelt( double minLength )
{
	for( list<SFace*>::iterator fit = fList.begin(); fit != fList.end(); ++fit )
	{
		TVector3D a( (*fit)->anEdgePtr->endPointPtr->thePosition[0], 
			(*fit)->anEdgePtr->endPointPtr->thePosition[1], (*fit)->anEdgePtr->endPointPtr->thePosition[2] );
		TVector3D b( (*fit)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition[0], 
			(*fit)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition[1], (*fit)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition[2] );
		TVector3D c( (*fit)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition[0], 
			(*fit)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition[1], (*fit)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition[2] );
		TVector3D v0 = a - b;
		TVector3D v1 = b - c;
		TVector3D v2 = c - a;
		if ( norm(v0) < minLength && norm(v1) < minLength && norm(v2) < minLength )
		{
			cerr << "Need to delete face " << endl;
		}
	}
}

void CMesh::edgeMelt( double minLength )
{
PR("Edge melting... ")
eList.sort( edgeLengthComp );	
	list<SEdge*>::iterator global = eList.begin();
	while( global != eList.end() )
	{
		//cerr << ".";
		// Check if actual edge is too large
		SVertex* vv3 = (*global)->endPointPtr;
		SVertex* vv1 = (*global)->nextEdgePtr->nextEdgePtr->endPointPtr;		
		double length = norm( vv3->thePosition - vv1->thePosition );
		if ( length < minLength )
		{
			progress();	
			SEdge* e1 = *global;
			SEdge* e2 = e1->nextEdgePtr;
			SEdge* e0 = e2->nextEdgePtr;
			SEdge* e5 = e1->opposingEdgePtr;
			SEdge* e3 = e5->nextEdgePtr;
			SEdge* e4 = e3->nextEdgePtr;
			SVertex* v1 = e0->endPointPtr;
			SVertex* v2 = e1->endPointPtr;
			if ( e4->nextEdgePtr != e5 || e0->nextEdgePtr != e1 )
				cerr << "Triangle is corrupt" << endl;
			if ( v1 == e5->endPointPtr && v2 == e4->endPointPtr )
			{					
				SVertex* v0 = e2->endPointPtr;
				SVertex* v3 = e3->endPointPtr;
				SFace* f0 = e1->theFacePtr;
				SFace* f1 = e5->theFacePtr;
				// Find all edges which end up in our vertex v2 and update them to use v1
				SEdge* startEdge = e4->opposingEdgePtr;
				SEdge* actEdge = startEdge;
				int i = 0;
				do
				{
					if ( actEdge->endPointPtr == v2 )
						actEdge->endPointPtr = v1;
					else if ( actEdge->opposingEdgePtr->endPointPtr == v2 )
						actEdge->opposingEdgePtr->endPointPtr = v1;
					actEdge = actEdge->opposingEdgePtr->nextEdgePtr;
//					cerr << actEdge << endl;
					++i; if ( i > 50 ) { cerr << "Error" << endl; break;}
				}
				while( actEdge != startEdge );
				//cerr << endl;
				// Melt the edge and reset				
				e0->opposingEdgePtr->opposingEdgePtr = e2->opposingEdgePtr;				
				e2->opposingEdgePtr->opposingEdgePtr = e0->opposingEdgePtr;
				e3->opposingEdgePtr->opposingEdgePtr = e4->opposingEdgePtr;
				e4->opposingEdgePtr->opposingEdgePtr = e3->opposingEdgePtr;
				v0->anEdgePtr = e2->opposingEdgePtr;
				v1->anEdgePtr = e0->opposingEdgePtr;
				v3->anEdgePtr = e3->opposingEdgePtr;
				v1->thePosition = v1->thePosition + 0.5 * ( v2->thePosition - v1->thePosition );
/*				cerr << e0->opposingEdgePtr->theFacePtr << endl;
				cerr << e2->opposingEdgePtr->theFacePtr << endl;
				cerr << e3->opposingEdgePtr->theFacePtr << endl;
				cerr << e4->opposingEdgePtr->theFacePtr << endl;
				cerr << endl;*/
				eList.remove(e0); delete e0;//edgePool.push_back( e0 );
				eList.remove(e1); delete e1;//edgePool.push_back( e1 );
				eList.remove(e2); delete e2;//edgePool.push_back( e2 );
				eList.remove(e3); delete e3;//edgePool.push_back( e3 );
				eList.remove(e4); delete e4;//edgePool.push_back( e4 );
				eList.remove(e5); delete e5;//edgePool.push_back( e5 );
				vList.remove(v2);
				v2->clear();
				vertexPool.push_back( v2 );
				fList.remove(f0); delete f0;//facePool.push_back( f0 );
				fList.remove(f1); delete f1;//facePool.push_back( f1 );
				eList.sort( edgeLengthComp );
				global = eList.begin();
			}
			else
			{
				if ( v1 == e4->endPointPtr ) cerr << "V1->e4" << endl;
				if ( v1 == e5->endPointPtr ) cerr << "V1->e5" << endl;
				if ( v1 == e3->endPointPtr ) cerr << "V1->e3" << endl;
				if ( v2 == e4->endPointPtr ) cerr << "V2->e4" << endl;
				if ( v2 == e5->endPointPtr ) cerr << "V2->e5" << endl;
				if ( v2 == e3->endPointPtr ) cerr << "V2->e3" << endl;
				++global;
			}
		}
		else
		{
			++global;
			progress();
 		}
	}
	//cerr << endl;
PR("done\nMesh consists of " << vList.size() << " vertices and " << fList.size() << " faces." << endl)
}

void CMesh::edgeFlip( double maxLength )
{
	for( list<SEdge*>::iterator global = eList.begin(); global != eList.end(); ++global )
	{
		SVertex* v3 = (*global)->endPointPtr;
		SVertex* v1 = (*global)->nextEdgePtr->nextEdgePtr->endPointPtr;		
		double length = sqrt( (v3->thePosition[0]-v1->thePosition[0]) * (v3->thePosition[0]-v1->thePosition[0]) +
			(v3->thePosition[1]-v1->thePosition[1]) * (v3->thePosition[1]-v1->thePosition[1]) +
			(v3->thePosition[2]-v1->thePosition[2]) * (v3->thePosition[2]-v1->thePosition[2]) );
		if ( length > maxLength )
		{
			// Test			
		}
	}
}

ulong CMesh::subdivide( double maxLength )
{
PR("Starting sd with ml " << maxLength << endl)
	int sd=0;
	uint last = vList.size();
PR("Looking for edges to be subdivided ... ")
	bool rego = true;
	while( rego )
	{
		list<RegPair> local;
		list<SFace*> newFaces;
		rego = false;
		for( list<SEdge*>::iterator global = eList.begin(); global != eList.end(); ++global )
		{		
			progress();
			if ( (*global)->bDelete == false )
			{
			// Look at local
			// Determine edge length
			SVertex* v3 = (*global)->endPointPtr;
			SVertex* v2 = (*global)->nextEdgePtr->endPointPtr;
			SVertex* v1 = (*global)->nextEdgePtr->nextEdgePtr->endPointPtr;
			TVector3D diff = v3->thePosition - v1->thePosition;
			TVector3D diff2 = v2->thePosition - v1->thePosition;
			double length = norm(diff);
			// This triangle is too large, we need to subdivide all edges
/*			if ( !rego && (length*length2)>(maxLength*maxLength) )
			{
				rego = true; // cerr << "~";
			}*/
			if ( length >= maxLength )
			{
				if (!rego) {rego=true;}//cerr << "~";}
				++sd;
				(*global)->opposingEdgePtr->bDelete = true;
				(*global)->bDelete= true;
				RegPair rp;
				SVertex* v;
/*				if ( !vertexPool.empty() )
				{
					v = vertexPool.front();
					vertexPool.pop_front();
				}
				else*/
					v = new SVertex;
				//v->bOld = false;
				//v->id = vid; ++vid;
				v->thePosition = v1->thePosition + ( v3->thePosition - v1->thePosition ) * 0.5;
				rp.anEdgePtr = (*global);
				list<SVertex*>::iterator vit = vList.begin();
				while( vit != vList.end() )
				{
					if ( veq( (*vit)->thePosition, v->thePosition ) )
					{
						cerr << "A " << (*vit)->thePosition[0] << " " << (*vit)->thePosition[1] << " " << (*vit)->thePosition[2] << " " << endl;
						cerr << "B " << v->thePosition[0] << " " << v->thePosition[1] << " " << v->thePosition[2] << " " << endl;
						rp.nV = (*vit);
						break;
						//exit(1);
					}
					++vit;
				}
				if ( vit == vList.end() )
				{
					rp.nV = v;
					vList.push_back( v );
				}
				else
				{
					delete v;
					//v->clear();
					//vertexPool.push_back( v );
				}
				local.push_back( rp );
				rp.anEdgePtr = (*global)->opposingEdgePtr;
				local.push_back( rp );
			}
		}		
	}
PR("done" << endl << "Marking all faces to be subdivided... ")
	list<Rebuild> reb;
	
	for( list<SFace*>::iterator fit = fList.begin(); fit != fList.end(); ++fit )
	{
		progress();
		SEdge* e0 = (*fit)->anEdgePtr;
		SEdge* e1 = (*fit)->anEdgePtr->nextEdgePtr;
		SEdge* e2 = (*fit)->anEdgePtr->nextEdgePtr->nextEdgePtr;
		int found = 0;
		RegPair reg[3];
		for( list<RegPair>::iterator rit = local.begin(); rit != local.end(); ++rit )
		{			
			if ( ((*rit).anEdgePtr == e0 ) || ((*rit).anEdgePtr == e1 ) || ((*rit).anEdgePtr == e2 ))
			{
				reg[found] = (*rit); found++;
			}
			if ( found == 3 ) break;
		}
		if ( found > 0 )
		{
			if ( found == 3 && ( reg[1].anEdgePtr->nextEdgePtr == reg[0].anEdgePtr || reg[2].anEdgePtr->nextEdgePtr == reg[1].anEdgePtr 
						|| reg[0].anEdgePtr->nextEdgePtr == reg[2].anEdgePtr ) )
				swap( reg[2], reg[0] );
			else if ( found == 2 && reg[1].anEdgePtr->nextEdgePtr == reg[0].anEdgePtr )
				swap( reg[1], reg[0] );						
// 			cout << "Need to subdivide " << found << " edges on face " << (*fit)->anEdgePtr->id << endl;
			if ( found == 1 )
			{
//  				cerr << "Found 1 " << endl;
				SVertex* w = reg[0].nV; 
				SVertex* v1 = reg[0].anEdgePtr->endPointPtr;
				SVertex* v2 = reg[0].anEdgePtr->nextEdgePtr->endPointPtr;				
				SVertex* v0 = reg[0].anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr;
				SEdge* e0 = reg[0].anEdgePtr;
				SEdge* e1 = reg[0].anEdgePtr->nextEdgePtr;
				SEdge* e2 = reg[0].anEdgePtr->nextEdgePtr->nextEdgePtr;
				SFace* f = reg[0].anEdgePtr->nextEdgePtr->theFacePtr;
// 				cerr << "Old face is "; printFace(f);
				SFace* fa;
/*				if ( !facePool.empty() )
				{
					fa = facePool.front();
					facePool.pop_front();
				}
				else*/
					fa = new SFace;
				fa->iTag=0;
				SEdge* d[4];
				for ( int i = 0; i < 4; ++i )
				{
/*					if ( !edgePool.empty() )
					{
						d[i] = edgePool.front();
						edgePool.pop_front();
					}
					else*/
						d[i] = new SEdge;
				}
				f->anEdgePtr = d[0];
				fa->anEdgePtr = d[1];
				d[0]->set( v1, f, e1 );
				w->anEdgePtr = d[0];
				e1->update( f, d[3] );
				d[3]->set(  w, f, d[0], d[2] );
				
				
				d[1]->set(  w, fa, d[2] );
				v0->anEdgePtr = d[1];
				d[2]->set( v2, fa, e2, d[3] );
				e2->update( fa, d[1] );
								
				newFaces.push_back( fa );
				eList.remove( e0 );
				e0->bOld = true;
				Rebuild r;
				r.oldEdge = e0; r.newEdge1 = d[1]; r.newEdge2 = d[0]; 
				reb.push_back( r );
				for( int i = 0; i < 4; ++i )
					eList.push_back( d[i] );
			}
			else if ( found == 2 )
			{
				SVertex* w = reg[0].nV; 
				SVertex* x = reg[1].nV; 
				SVertex* v1 = reg[0].anEdgePtr->endPointPtr;
				SVertex* v2 = reg[0].anEdgePtr->nextEdgePtr->endPointPtr;				
				SVertex* v0 = reg[0].anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr;
				SFace* f = reg[0].anEdgePtr->theFacePtr;
				SEdge* e0 = reg[0].anEdgePtr;
				SEdge* e1 = reg[0].anEdgePtr->nextEdgePtr;
				SEdge* e2 = reg[0].anEdgePtr->nextEdgePtr->nextEdgePtr;
				
				SFace* fa;
				SFace* fb;
/*				if ( !facePool.empty() )
				{
					fa = facePool.front();
					facePool.pop_front();
				}
				else*/
					fa = new SFace;
/*				if ( !facePool.empty() )
				{
					fb = facePool.front();
					facePool.pop_front();
				}
				else*/
					fb = new SFace;
				fa->iTag = 0;
				fb->iTag = 0;

				SEdge* d[8];
				for ( int i = 0; i < 8; ++i )
				{
/*					if ( !edgePool.empty() )
					{
						d[i] = edgePool.front();
						edgePool.pop_front();
					}
					else*/
						d[i] = new SEdge;
				}
				
				f->anEdgePtr = d[0];
				fa->anEdgePtr = d[3];
				fb->anEdgePtr = d[1];
				
				d[0]->set(  w, f, d[4] );
				v0->anEdgePtr = d[0] ;
				d[4]->set( v2, f, e2, d[5] );
				e2->update( f, d[0] );

				d[1]->set( v1, fb, d[2] );
				d[2]->set(  x, fb, d[7] );
				d[7]->set(  w, fb, d[1], d[6] );
				w->anEdgePtr=d[1];
				x->anEdgePtr=d[7];
				v1->anEdgePtr=d[2];

				d[3]->set( v2, fa, d[5] );
				d[5]->set(  w, fa, d[6], d[4] );
				d[6]->set(  x, fa, d[3], d[7] );
								
 				newFaces.push_back( fa );
 				newFaces.push_back( fb );
				eList.remove( e0 );
				eList.remove( e1 );
				e0->bOld = true;
				e1->bOld = true;
				Rebuild r;
				r.oldEdge = e0; r.newEdge1 = d[0]; r.newEdge2 = d[1]; reb.push_back( r );
				r.oldEdge = e1; r.newEdge1 = d[2]; r.newEdge2 = d[3]; reb.push_back( r );
				for( int i = 0; i < 8; ++i )
					eList.push_back( d[i] );
			}
		  if ( found == 3 )
			{
// 				cerr << "Found 3 " << endl;
				SVertex* w = reg[2].nV; 
				SVertex* y = reg[1].nV; 
				SVertex* x = reg[0].nV; 
				SVertex* v2 = reg[1].anEdgePtr->endPointPtr;
				SVertex* v1 = reg[0].anEdgePtr->endPointPtr;
				SVertex* v0 = reg[2].anEdgePtr->endPointPtr;												
				SEdge* e0 = reg[0].anEdgePtr;
				SEdge* e1 = reg[1].anEdgePtr;
				SEdge* e2 = reg[2].anEdgePtr;
				
				if( e0->nextEdgePtr == e2 )
				{
					cerr << "SWAP" << endl;					
					exit(1);
				}						
						
				SFace* f = reg[0].anEdgePtr->theFacePtr;
				SFace* fa;
				SFace* fb;
				SFace* fc;
/*				if ( !facePool.empty() )
				{
					fa = facePool.front();
					facePool.pop_front();
				}
				else*/
					fa = new SFace;
/*				if ( !facePool.empty() )
				{
					fb = facePool.front();
					facePool.pop_front();
				}
				else*/
					fb = new SFace;
/*				if ( !facePool.empty() )
				{
					fc = facePool.front();
					facePool.pop_front();
				}
				else*/
					fc = new SFace;
				SEdge* d[12];
				for ( int i = 0; i < 12; ++i )
				{
/*					if ( !edgePool.empty() )
					{
						d[i] = edgePool.front();
						edgePool.pop_front();
					}
					else*/
						d[i] = new SEdge;
				}
								
				f->anEdgePtr = d[0];
				fa->anEdgePtr = d[1];
				fb->anEdgePtr = d[3];
				fc->anEdgePtr = d[7];
				newFaces.push_back( fa );
				newFaces.push_back( fb );
				newFaces.push_back( fc );
				
				d[0]->set(  x, f, d[6] );
				v0->anEdgePtr= d[0] ;
				d[6]->set(  w, f, d[5], d[7] );
				x->anEdgePtr= d[6] ;
				d[5]->set( v0, f, d[0] );
				w->anEdgePtr=d[5];

				d[1]->set( v1, fa, d[2] );				
				d[2]->set(  y, fa, d[8] );
				v1->anEdgePtr= d[2] ;				
				d[8]->set(  x, fa, d[1], d[9] );								
				y->anEdgePtr=d[8] ;

				d[3]->set( v2, fb, d[4] );
				d[4]->set(  w, fb, d[10] );
				v2->anEdgePtr=d[4];
				d[10]->set( y, fb, d[3], d[11] );				

				d[7]->set( x, fc, d[9], d[6] );
				d[9]->set( y, fc, d[11], d[8] );
				d[11]->set( w, fc, d[7], d[10] );				
					
				eList.remove( e0 );
				eList.remove( e1 );
				eList.remove( e2 );
				e0->bOld = true;
				e1->bOld = true;
				e2->bOld = true;
				Rebuild r;
				r.oldEdge = e0; r.newEdge1 = d[0]; r.newEdge2 = d[1]; reb.push_back( r );
				r.oldEdge = e1; r.newEdge1 = d[2]; r.newEdge2 = d[3]; reb.push_back( r );
				r.oldEdge = e2; r.newEdge1 = d[4]; r.newEdge2 = d[5]; reb.push_back( r );
				for( int i = 0; i < 12; ++i )
					eList.push_back( d[i] );
			}
		}
	}
	// Rebuild topology
PR("Done" << endl << "Rebuilding topology ... ")
	bool changed = true;
	while( !reb.empty() && changed )
	{
		progress();
		changed = false;
		Rebuild r = *(reb.begin());
		for( list<Rebuild>::iterator rit = reb.begin(); rit != reb.end(); ++rit )
		{			
			if ( rit->oldEdge->opposingEdgePtr == r.oldEdge )
			{
				r.newEdge1->opposingEdgePtr = rit->newEdge2;
				r.newEdge2->opposingEdgePtr = rit->newEdge1;
				rit->newEdge1->opposingEdgePtr = r.newEdge2;
				rit->newEdge2->opposingEdgePtr = r.newEdge1;
/*				cout << "Reb " << rit->oldEdge->id << " / " << r.oldEdge->id 
					<< " to " << rit->newEdge2->id << " / " << r.newEdge1->id << " and " << rit->newEdge1->id
					<< " / " << r.newEdge2->id << endl;				*/
				SEdge* a = rit->oldEdge;
				SEdge* b = r.oldEdge;					
				reb.remove( *rit );
				reb.remove( r );
				//edgePool.push_back( a );
				//edgePool.push_back( b );
				delete a;
				delete b;
				changed = true;
				break;
			}
		}
	}
	fList.insert( fList.end(), newFaces.begin(), newFaces.end() );
PR("done" << endl << "Mesh size is " << fList.size() << " faces" << endl)
	if ( vList.size() == last ) rego = false;
	}
	return sd;
}

void CMesh::computeNormals()
{
// 	cerr << "Computing face normals... ";
	for( list<SFace*>::iterator it = fList.begin(); it != fList.end(); ++it )
	{
		TVector3D p1 = (*it)->anEdgePtr->endPointPtr->thePosition;
		TVector3D p2 = (*it)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition;
		TVector3D p3 = (*it)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition;
		TVector3D d1 = p2 - p1;
		TVector3D d2 = p3 - p1; 
		if ( norm(d1) > 0.00001 && norm(d2) > 0.00001 )
		{
			d1 = d1 * (1.0/norm(d1));			
			d2 = d2 * (1.0/norm(d2));
			TVector3D normal = cross(d1,d2);		
			(*it)->theNormal = normal;
		}
		else
		{
			(*it)->theNormal = (*it)->anEdgePtr->endPointPtr->thePosition;
			(*it)->theNormal /= norm( (*it)->theNormal );
		}
	}
// 	cerr << "done" << endl << "Computing vertex normals... ";
	for( list<SVertex*>::iterator vit = vList.begin(); vit != vList.end(); ++vit )
	{
		if ((*vit) != NULL)
		{
			SEdge* startEdge = (*vit)->anEdgePtr;
			SEdge* actEdge = startEdge;
			TVector3D normal = 0.0;
			uint uiNoOfFaces = 0;
			do
			{
					normal += actEdge->theFacePtr->theNormal;
				uiNoOfFaces++;
				actEdge = actEdge->opposingEdgePtr->nextEdgePtr;
			}
			while( actEdge != startEdge );
			if ( uiNoOfFaces == 0 )
				exit(-1);
			normal /= uiNoOfFaces;
			(*vit)->theNormal = normal;
		 }
		 else
		 {
		 	cerr << "Current vertex is NULL" << endl;
		 }
	}
// 	cerr << "done" << endl;
}

void CMesh::checkTopology()
{
	ulong cw = 0;
	ulong ccw = 0;
	computeNormals();
PR("Checking triangle orientation... ")
	for( list<SFace*>::iterator it = fList.begin(); it != fList.end(); ++it )
	{		
		progress();
		TVector3D v0 = (*it)->anEdgePtr->endPointPtr->thePosition;
		TVector3D v1 = (*it)->anEdgePtr->endPointPtr->thePosition;
		TVector3D v2 = (*it)->anEdgePtr->endPointPtr->thePosition;
		TVector3D n0 = cross( v0, v1 );
		TVector3D n1 = cross( v1, v2 );
		TVector3D n2 = cross( v2, v0 );
		double area = dot( (*it)->theNormal, n0+n1+n2);
		if ( area > 0 ) ccw++;
		else cw++;
	}
PR(ccw << " CCW and " << cw << " CW triangles" << endl << "Checking for doubled vertices... ")
	list<SVertex*> localVList = vList;
	localVList.sort();
	ulong dv = 0;
	for( list<SVertex*>::iterator it = localVList.begin(); it != localVList.end(); ++it )
	{		
		list<SVertex*>::iterator su = it; ++su;
		if ( su != localVList.end() && veq( (*su)->thePosition,(*it)->thePosition, 0.1 ) )
			dv++;
	}
PR("found " << dv << " illegal pairs" << endl << "Correcting doubled vertices... ")
	
	localVList = vList;
	localVList.sort();
	dv = 0;
	for( list<SVertex*>::iterator it = localVList.begin(); it != localVList.end(); ++it )
	{		
		progress();
		list<SVertex*>::iterator su = it; ++su;
		if ( su != localVList.end() && veq( (*su)->thePosition, (*it)->thePosition, 0.1) )
		{
			// Find these points in the original list and update all edges which contain these points
			// Find first occurence
			list<SVertex*>::iterator fo = vList.begin();
			while( fo != vList.end() ) 
			{
				if ( veq((*fo)->thePosition , (*it)->thePosition, 0.1 ) )
					break;
				else
					++fo;
			}
			// Find second occurence
			list<SVertex*>::iterator so = fo; ++so;
			while( so != vList.end() ) 
			{
				if ( veq((*so)->thePosition , (*it)->thePosition, 0.1 ) )
					break;
				else
					++so;
			}
			SVertex* tod = *so;
			// Update edges
			for ( list<SEdge*>::iterator ei = eList.begin(); ei != eList.end(); ++ei )
				if ( (*ei)->endPointPtr == (*so) )
					(*ei)->endPointPtr = (*fo);
			
			// Remove second occurence
			vList.remove( tod );			
			delete tod;
/*			tod->clear();
			vertexPool.push_back( tod );*/
			// Reset iterators
			localVList = vList;
			localVList.sort();
			it = localVList.begin();
		}
	}
PR("done" << endl << "Checking for doubled vertices... ")
	localVList = vList;
	localVList.sort();
	dv = 0;
	for( list<SVertex*>::iterator it = localVList.begin(); it != localVList.end(); ++it )
	{		
		list<SVertex*>::iterator su = it; ++su;
		if ( su != localVList.end() && veq( (*su)->thePosition , (*it)->thePosition, 0.01 ))
			dv++;
	}
PR("found " << dv << " illegal pairs" << endl << "Checking for doubled triangles... ")
	ulong df = 0; 
	for( list<SFace*>::iterator it = fList.begin(); it != fList.end(); ++it )
	{		
		for( list<SFace*>::iterator ut = it; ut != fList.end(); ++ut )
		{
			if ( ut != it )
			{
				TVector3D si[3];
				TVector3D ti[3];
				si[0] = (*ut)->anEdgePtr->endPointPtr->thePosition;
				si[1] = (*ut)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition;
				si[2] = (*ut)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition;
				ti[0] = (*it)->anEdgePtr->endPointPtr->thePosition;
				ti[1] = (*it)->anEdgePtr->nextEdgePtr->endPointPtr->thePosition;
				ti[2] = (*it)->anEdgePtr->nextEdgePtr->nextEdgePtr->endPointPtr->thePosition;
				if ( ( veq(si[0], ti[0]) && veq(si[1] ,ti[1]) && veq(si[2], ti[2]) )
					|| ( veq(si[0], ti[1]) && veq(si[1] ,ti[2]) && veq(si[2], ti[0]) )
					|| ( veq(si[0], ti[2]) && veq(si[1] ,ti[0]) && veq(si[2], ti[1]) ) )
				{		
					// We should erase the actual triangle			
					SFace* f = (*ut); ++ut;
					SFace* g = (*it);
					for ( list<SEdge*>::iterator ei = eList.begin(); ei != eList.end(); ++ei )
						if ( (*ei)->theFacePtr == f )
							(*ei)->theFacePtr = g;
					fList.remove(f);
					//facePool.push_back( f );
					delete f;
					df++; --ut;
					cerr << fList.size() << endl;
				}					
			}
		}			
	}
PR("found " << df << " illegal triangles" << endl)
}

/** Do a binning for all vertices and compute repulsion forces */
void CMesh::computeBins( bool odd, double minDist )
{
	// Fill the bins
	if ( !odd )
	{
		for( list<SVertex*>::iterator it = vList.begin(); it != vList.end(); ++it )
		{
			int x = static_cast<int>( round( (*it)->thePosition[0] ) ) / 16;
			int y = static_cast<int>( round( (*it)->thePosition[1] ) ) / 16;
			int z = static_cast<int>( round( (*it)->thePosition[2] ) ) / 16;
			if ( x > 8 ) x = 8;
			if ( y > 8 ) y = 8;
			if ( z > 8 ) z = 8;
			bins[x+y*8+z*256].push_back( *it );
		}
	}
	else
	{
		for( list<SVertex*>::iterator it = vList.begin(); it != vList.end(); ++it )
		{
			int x = static_cast<int>( round( (*it)->thePosition[0] + 8.0 ) ) / 16;
			int y = static_cast<int>( round( (*it)->thePosition[1] + 8.0 ) ) / 16;
			int z = static_cast<int>( round( (*it)->thePosition[2] + 8.0 ) ) / 16;
			if ( x > 8 ) x = 8;
			if ( y > 8 ) y = 8;
			if ( z > 8 ) z = 8;
			bins[x+y*8+z*256].push_back( *it );
		}
	}
	// Check all canditates of each bin against each other
	for( int i = 0; i < (8+36*8+256*8); ++i )
	{
		//cerr << bins[i].size() << " vertices in bin " << i << endl;
		if ( !bins[i].empty() )
		for( list<SVertex*>::iterator it = bins[i].begin(); it != bins[i].end(); ++it )
		{
			list<SVertex*>::iterator successor = it; ++successor;
			for( list<SVertex*>::iterator jt = successor; jt != bins[i].end(); ++jt )
			{
				TVector3D conn = (*it)->thePosition - (*jt)->thePosition;
				double dDistance = norm( conn );
				if ( minDist > numeric_limits<double>::epsilon() && dDistance < minDist )
				{
					double dFactor = ( minDist - dDistance ) / dDistance;
					(*it)->theForce += conn * dFactor;
					(*jt)->theForce += -conn * dFactor;
				}
			}
			
		}
		bins[i].clear();
	}
}
