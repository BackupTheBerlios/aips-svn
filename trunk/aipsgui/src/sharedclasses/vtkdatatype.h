//
// C++ Interface: vtkdatatype
//
// Description: 
//
//
// Author:  <HB>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

/* VTK 4.4 defines vtkFloatingPointyType == double. VTK 4.2 uses float as
   a standard floating point type, so we need to define vtkFloatingPointyType
   == float. By this the code should get compiled with any VTK 4.x version */
#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 
