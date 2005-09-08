#ifndef CVECTOR_H
#define CVECTOR_H

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
#include <limits>
//#include <SDLcore.h>

class CVector
{
public:
  // Konstruktoren
  CVector ();
  CVector ( const CVector& aVector );
  CVector ( const double dX, const double dY, const double dZ );
  CVector ( const double dArray[3]);

  // Destruktor
  ~CVector ( );

  // Memberfunktionen
  CVector operator+ ( const CVector& aVector );   // Vektoraddition
  CVector operator- ( const CVector& aVector );   // Vektorsubtraktion
  CVector &operator= ( const CVector& aVector );  // Zuweisungsoperator
  CVector operator* ( const double dSkalar );     // Multiplikation mit Skalar
  double &operator[] ( const int ubIndex );     // Auf CVector kann wie auf ein double-Array zugegriffen werden
  operator double*() const;                       // Cast-Operator für Double-Arrays
  operator float*() const;                        // Cast-Operator für Float-Arrays

  double get (const unsigned char ubIndex ) const;        // Explizit eine Komponente erhalten
  void set (const unsigned char ubIndex, const double dNewVal);          // Explizit eine Komponente setzen

  CVector vec ( const CVector &a ) const;         // Vektorprodukt
  double skal ( const CVector &a ) const;         // Skalarprodukt
  double norm () const;                     // Vektornorm
	bool operator<( const CVector& a ) const
	{
		return ( dVal[0] <= a.dVal[0] && dVal[1] <= a.dVal[1] && dVal[2] <= a.dVal[2] );
	}
	bool operator==( const CVector& a ) const
	{
		bool equal = (fabs(dVal[0] - a.dVal[0])<=0.00001) 
		&& (fabs(dVal[1] - a.dVal[1])<=0.00001)
		&& (fabs(dVal[2] - a.dVal[2])<=0.00001);
		return ( equal );
	}
  // I/O-Streaming zur Ausgabe eines Vektor
  //friend ostream& operator<< (ostream& ostr, const CVector &aVector);

private:
  double dVal[3];
};

// ostream& operator<< (ostream& aostream, const CVector &aVector)
// {
// 	aostream << "[" << aVector.dVal[0] << ";" << aVector.dVal[1] << ";" << aVector.dVal[2] << "]";
// 	return aostream;
// }

#endif

