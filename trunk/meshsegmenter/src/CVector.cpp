/***********************************************************************
 * Datei: CVector.h                                                    *
 * Projekt: SDL++                                                      *
 * Beschreibung: Eine Klasse zur Darstellung von Vektoren im R^3       *
 *                                                                     *
 * Autor: Hendrik Belitz                                               *
 *                                                                     *
 * Version: 0.2                                                        *
 * Created: 19.09.02                                                   *
 * Changed:                                                            *
 ***********************************************************************/

#include <iostream>
#include <cmath>
#include "CVector.h"

CVector::CVector ()
{
  for (int i = 0; i < 3; i++ )
    this->dVal[i] = 0.0;
}

CVector::CVector ( const CVector& aVector )
{
  for (int i = 0; i < 3; i++ )
    dVal[i] = aVector.dVal[i];
}

CVector::CVector ( const double dX, const double dY, const double dZ )
{
  this->dVal[0] = dX;
  this->dVal[1] = dY;
  this->dVal[2] = dZ;
}

CVector::CVector ( const double dArray[3] )
{
  for (int i=0; i<3; i++)
    this->dVal[i] = dArray[i];
}

CVector::~CVector ()
{
}

double CVector::get(const unsigned char ubIndex) const
{
  return dVal[ubIndex];
}

void CVector::set(const unsigned char ubIndex, const double dNewVal)
{
  dVal[ubIndex] = dNewVal;
}

CVector CVector::operator+ ( const CVector &aVector )
{
  CVector tmp;
  for (int i = 0; i < 3; i++ )
    tmp.dVal[i] = aVector.get(i) + dVal[i];
  return tmp;
}

CVector CVector::operator- ( const CVector &aVector )
{
  CVector tmp;
  for ( int i = 0; i < 3; i++ )
    tmp.dVal[i] = dVal[i] - aVector.get( i );
  return tmp;
}

CVector &CVector::operator= ( const CVector &aVector )
{
  for ( int i = 0; i < 3; i++ )
    dVal[i] = aVector.get( i );
  return ( *this );
}

CVector CVector::operator* ( const double dSkalar)
{
  CVector tmp( *this );
  for ( unsigned char i = 0; i < 3; i++ )
    tmp[i] *= dSkalar;
  return tmp;
}

double &CVector::operator[] ( const int i )
{
  return ( dVal[i] );
}

CVector CVector::vec ( const CVector& aVector ) const
{
  CVector tmp;
  tmp[0] = this->dVal[1] * aVector.dVal[2] - this->dVal[2] * aVector.dVal[1];
  tmp[1] = this->dVal[2] * aVector.dVal[0] - this->dVal[0] * aVector.dVal[2];
  tmp[2] = this->dVal[0] * aVector.dVal[1] - this->dVal[1] * aVector.dVal[0];
  return tmp;
}

double CVector::skal ( const CVector& aVector ) const
{
  double s;
  s = aVector.get( 0 ) * dVal[0] + aVector.get( 1 ) * dVal[1] + aVector.get( 2 ) * dVal[2];
  return s;
}

double CVector::norm () const
{
  return sqrt( this->skal(*this));
}

CVector::operator double*() const
{
  double *ret = new double[3];
  ret[0] = dVal[0];
  ret[1] = dVal[1];
  ret[2] = dVal[2];
  return ret;
}

CVector::operator float*() const
{
  float *ret = new float[3];
  ret[0] = static_cast<float>( dVal[0] );
  ret[1] = static_cast<float>( dVal[1] );
  ret[2] = static_cast<float>( dVal[2] );
  return ret;
}

// ostream& operator<< (ostream& aostream, const CVector& aVector)
// {
//   aostream << "( " << aVector.get(0) << " ; " << aVector.get(1) << " ; " << aVector.get(2) << " )";
//   return aostream;
// }
