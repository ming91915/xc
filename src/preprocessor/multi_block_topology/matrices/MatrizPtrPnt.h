//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MatrizPtrPnt.h
//Matriz de puntos

#ifndef MATRIZPTRPNT_H
#define MATRIZPTRPNT_H

#include "MatrizPtrBase.h"
#include "xc_basic/src/matrices/m_int.h"
#include <vector>

class Pos3d;
class RangoMatriz;
class Vector3d;

namespace XC{
class Pnt;
class SFreedom_Constraint;
class MultiBlockTopology;

//! \ingroup MultiBlockTopologyMR
//! 
//! @brief Matriz de pointers to puntos.
class MatrizPtrPnt: public MatrizPtrBase<Pnt>
  {
  protected:


    friend class MultiBlockTopology;
  public:
    //! @brief Constructor.
    MatrizPtrPnt(const size_t &f=0,const size_t &c=0)
      : MatrizPtrBase<Pnt>(f,c) {}
    m_int getTags(void) const;

    const MultiBlockTopology *getMultiBlockTopology(void) const;
    MultiBlockTopology *getMultiBlockTopology(void);

    void setPnt(const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &);

    Pnt *buscaPunto(const size_t &);
    const Pnt *buscaPunto(const size_t &) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;
    std::deque<size_t> CopiaPuntos(const RangoMatriz &,const std::vector<size_t> &,const Vector3d &);

    Pos3d getCentroide(void) const;


  };

} //end of XC namespace

#endif
