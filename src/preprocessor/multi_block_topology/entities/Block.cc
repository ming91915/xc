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
//Block.cc

#include "Block.h"
#include "Pnt.h"
#include "QuadSurface.h"
#include "xc_basic/src/matrices/m_int.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "Face.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "vtkCellType.h"

// Numbering of vertices:
//
//       5        8
//       +--------+
//      /        /|
//     /      7 / |
//  6 +--------+  |
//    |        |  |
//    |        |  + 4
//    |        | /
//    |        |/
//    +--------+
//    2        3
//
// Numbering of faces:
//
//                
//       +--------+         0: Bottom face; vertices 1,2,3,4.
//      /   5    /|         1: Left-side face; vertices 1,2,6,5.
//     /        / |         2: Front face; vertices 2,3,7,6.
//    +--------+  |         3: Right-side face; vertices 3,4,8,7.
//    |        | 3|         4: Dorsal face; vertices 1,4,8,5.
//    |        |  +         5: Top face; vertices 5,6,7,8.
//    |   2    | /
//    |        |/
//    +--------+
//

//! @brief Constructor.
XC::Block::Block(Preprocessor *m,const std::string &nombre)
  : Body(m,nombre) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::Block::getCopy(void) const
  { return new Block(*this); }


//! @brief Interface with VTK.
int XC::Block::getVtkCellType(void) const
  { return VTK_HEXAHEDRON; }

//! @brief Interface with MED format of Salome.
int XC::Block::getMEDCellType(void) const
  { return MED_HEXA8; }

//! @brief Check that number of divisions of the lines are compatible.
bool XC::Block::checkNDivs(void) const
  {
    return (sups[0].checkNDivs() && sups[1].checkNDivs() && sups[2].checkNDivs() && sups[3].checkNDivs() && sups[4].checkNDivs() && sups[5].checkNDivs());
  }

//! @brief Return the number of edges.
size_t XC::Block::getNumberOfLines(void) const
  { return 12; }

//! @brief Return the number of vertices.
size_t XC::Block::getNumberOfVertices(void) const
  { return 8; }

//! @brief Return the number of faces.
size_t XC::Block::getNumberOfFaces(void) const
  { return 6; }

//! @brief Return the face with the index passed as parameter.
const XC::Block::BodyFace *XC::Block::GetFace(const size_t &i) const
  { return &sups[i-1]; }

//! @brief Return the face with the index passed as parameter.
XC::Block::BodyFace *XC::Block::GetFace(const size_t &i)
  { return &sups[i-1]; }

//! @brief Return the i-th edge of the solid.
const XC::CmbEdge::Lado *XC::Block::GetArista(const size_t &i) const
  {
    const CmbEdge::Lado *retval(nullptr);
    switch(i)
      {
        case 1:
        case 2:
        case 3:
        case 4:
          retval=sups[0].GetLado(i);
          break;
        case 5:
        case 6:
        case 7:
        case 8:
          retval= sups[i-4].GetLado(4);
          break;
        case 9:
        case 10:
        case 11:
        case 12:
          retval= sups[5].GetLado(i-8);
          break;
        default:
          retval= nullptr;
          break;
      }
    return retval;
  }

//! @brief Return the i-th vertex of the solid.
const XC::Pnt *XC::Block::GetVertice(const size_t &i) const
  {
    if(i<=4)
      return sups[0].GetVertice(i);
    else if(i<=8)
      return sups[5].GetVertice(i-4);
    else
      return nullptr;
  }

//! @brief Return the i-th vertex of the solid.
XC::Pnt *XC::Block::GetVertice(const size_t &i)
  {
    if(i<=4)
      return sups[0].GetVertice(i);
    else if(i<=8)
      return sups[5].GetVertice(i-4);
    else
      return nullptr;
  }

//! @brief Return the surfaces that close the solid.
std::set<const XC::Face *> XC::Block::getSurfaces(void)
  {
    std::set<const Face *> retval;
    if(!sups[0].Vacia()) retval.insert(sups[0].Surface());
    if(!sups[1].Vacia()) retval.insert(sups[1].Surface());
    if(!sups[2].Vacia()) retval.insert(sups[2].Surface());
    if(!sups[3].Vacia()) retval.insert(sups[3].Surface());
    if(!sups[4].Vacia()) retval.insert(sups[4].Surface());
    if(!sups[5].Vacia()) retval.insert(sups[5].Surface());
    return retval;
  }

//! @brief Update topology of the enclosing surface (neighbors).
void XC::Block::actualiza_topologia(void)
  {
    if(!sups[0].Vacia()) set_surf(sups[0].Surface());
    if(!sups[1].Vacia()) set_surf(sups[1].Surface());
    if(!sups[2].Vacia()) set_surf(sups[2].Surface());
    if(!sups[3].Vacia()) set_surf(sups[3].Surface());
    if(!sups[4].Vacia()) set_surf(sups[4].Surface());
    if(!sups[5].Vacia()) set_surf(sups[5].Surface());
  }

//! Return the index of the surface passed as parameter
//! as follows:
//! - If the surface is the first one that defines the solid,
//! then that one is the base.
//! - If one or more of the faces are already defined, we search for the index
//!  on the base of the line in common with it.
//! If this line exists, that index corresponds to the surface in the solid.
size_t XC::Block::indice(Face *s) const
  {
    size_t retval= 0;
    if(sups[0].Vacia())
      retval= 0; //It's the base.
    else //Base is already set.
      {
        const Face *base= sups[0].Surface();
        size_t primero= base->CommonEdge(*s); //Line shared by "s" and the base.
        if(primero)
          retval= primero; //Is a side face.
        else
          retval= 5; //Is the top face.
      }
    return retval;
  }

//! @brief Set the surface as solid limit.
void XC::Block::coloca(const size_t &i,Face *s)
  {
    size_t primero= 1;
    int sentido= 1;
    if( (i>0) && (i<5)) //Is a side face.
      {
        const Face *base= sups[0].Surface();
        primero= s->CommonEdge(*base); //Index of the line in common with the base.
        const Edge *linea= base->GetLado(i)->getEdge();
        sentido= base->SenseOfEdge(linea,*s);
      }
    if(i == 5) //Is the top face
      {
        //Seek for an assigned face.
        size_t icara= 1;
        Face *cara= sups[1].Surface();
        if(!cara) { icara=2; cara= sups[icara].Surface(); }
        if(!cara) { icara=3; cara= sups[icara].Surface(); }
        if(!cara) { icara=4; cara= sups[icara].Surface(); }
        if(!cara)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error before introducing face 5 you must introduce"
	            << " either the 1 , 2 , 3 or 4 faces." << std::endl;
        else
          {
            primero= cara->CommonEdge(*s); //Index of the line in common of s with the face.
            if(primero) //They have a common edge.
              {
                const Edge *linea= cara->GetLado(primero)->getEdge();
                sentido= -cara->SenseOfEdge(linea,*s);
              }
            else //They don't share a common edge.
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error: Block(" << getName() << "); the face "
			  << s->getName() 
                          << " does not share a common edge with face "
			  << cara->getName() << '.' << std::endl;
              }
          }
      }
    bool directo= true;
    if(sentido==1)
      directo= true;
    else
      if(sentido==-1)
        directo= false;
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the surfaces do not share a common edge."
		  << std::endl;
    sups[i]= BodyFace(s,primero,directo);
  }

//! @brief Insert the surface with the identifier passed as parameter
//! (if found).
void XC::Block::inserta(const size_t &i)
  {
    Face *s= BuscaFace(i);
    if(s)
      coloca(indice(s),s);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; surface: " << i
		<< " not found." << std::endl;
  }

//! @brief Create and insert the faces from the indices passed
//! as parameter.
void XC::Block::add_caras(const std::vector<size_t> &indices_caras)
  {
    const size_t nc= indices_caras.size(); //Number of indices.
    for(size_t i= 0;i<nc;i++)
      inserta(indices_caras[i]);
  }

//! @brief Trigger the creation of nodes on faces.
void XC::Block::create_nodes_caras(void)
  {
    sups[0].create_nodes();
    sups[1].create_nodes();
    sups[2].create_nodes();
    sups[3].create_nodes();
    sups[4].create_nodes();
    sups[5].create_nodes();
  }

//! @brief Return (ndivI+1)*(ndivJ+1)*(ndivK+1) positions for the nodes.
//!
//! The returned 3D-matrix of points is organized as follows:
//! - The point (0,1,1) (layer,row,column) corresponds to vertex  1.
//! - The layer index ranges from 0 for face 1 to ncapas for face 6.
//! - The row index ranges from 1 for face 5 to nfilas for face 3.
//! - The column index rangess from 1 for face 2 to ncol for face 4.
//!
//! So:
//! - Edge 1 has indices (0,j=1..nfil,1) 
//! - Edge 2 has indices (0,nfil,k=1..ncol) 
//! - Edge 3 has indices (0,j=1..nfil,ncol)
//! - Edge 4 has indices (0,1,k=1..ncol)

//! - Edge 5 has indices (i=0..ncapas,1,1) 
//! - Edge 6 has indices (i=0..ncapas,nfil,1) 
//! - Edge 7 has indices (i=0..ncapas,nfil,ncol) 
//! - Edge 8 has indices (i=0..ncapas,1,ncol) 

//! - Edge 9 has indices (ncapas,j=1..nfil,1) 
//! - Edge 10 has indices (ncapas,nfil,k=1..ncol) 
//! - Edge 11 has indices (ncapas,j=1..nfil,ncol) 
//! - Edge 12 has indices (ncapas,1,k=1..ncol) 
TritrizPos3d XC::Block::get_posiciones(void) const
  {
    const size_t ndiv_12= NDivI();
    const size_t ndiv_23= NDivJ();
    const size_t ndiv_15= NDivK();
    const Pos3d p1= GetVertice(1)->GetPos();
    const Pos3d p2= GetVertice(2)->GetPos();
    const Pos3d p3= GetVertice(3)->GetPos();
    const Pos3d p4= GetVertice(4)->GetPos();
    const Pos3d p5= GetVertice(5)->GetPos();
    const Pos3d p6= GetVertice(6)->GetPos();
    const Pos3d p7= GetVertice(7)->GetPos();
    const Pos3d p8= GetVertice(8)->GetPos();
    const MatrizPos3d ptos_l5= MatrizPos3d(p1,p5,ndiv_15);
    const MatrizPos3d ptos_l6= MatrizPos3d(p2,p6,ndiv_15);
    const MatrizPos3d ptos_l7= MatrizPos3d(p3,p7,ndiv_15);
    const MatrizPos3d ptos_l8= MatrizPos3d(p4,p8,ndiv_15);
    TritrizPos3d retval(ptos_l5,ptos_l6,ptos_l7,ptos_l8,ndiv_12,ndiv_23);
    return retval;
  }

//! @brief Return the number of divisions along the edge 1->2.
size_t XC::Block::NDivI(void) const
  { return GetArista(1)->NDiv(); }

//! @brief Return the number of divisions along the edge 2->3.
size_t XC::Block::NDivJ(void) const
  { return GetArista(2)->NDiv(); }

//! @brief Return the number of divisions along the edge 1->5.
size_t XC::Block::NDivK(void) const
  { return GetArista(5)->NDiv(); }

//! @brief Create nodes for the block.
void XC::Block::create_nodes(void)
  {
    std::cout << "Entra Block::create_nodes" << std::endl;
    checkNDivs();
    if(ttzNodes.Null())
      {
        create_nodes_caras();
        BodyFace &base= sups[0];
        BodyFace &tapa= sups[5];
        BodyFace &latIzdo= sups[1];
        BodyFace &latDcho= sups[3];
        BodyFace &caraFrontal= sups[2];
        BodyFace &caraDorsal= sups[4];

        const size_t capas= NDivK()+1;
        const size_t filas= NDivJ()+1;
        const size_t cols= NDivI()+1;
        ttzNodes = TritrizPtrNod(capas,filas,cols); //Pointers to node.
        TritrizPos3d pos_nodes= get_posiciones(); //Posiciones of the nodes.

        //Vertices.
	ttzNodes(1,1,1)= GetVertice(1)->getNode();
        ttzNodes(1,filas,1)= GetVertice(2)->getNode();
	ttzNodes(1,filas,cols)= GetVertice(3)->getNode();
        ttzNodes(1,1,cols)= GetVertice(4)->getNode();
	ttzNodes(capas,1,1)= GetVertice(5)->getNode();
        ttzNodes(capas,filas,1)= GetVertice(6)->getNode();
	ttzNodes(capas,filas,cols)= GetVertice(7)->getNode();
        ttzNodes(capas,1,cols)= GetVertice(8)->getNode();

        const Node *n1= ttzNodes(1,1,1);
        const Node *n2= ttzNodes(1,filas,1);
        const Node *n3= ttzNodes(1,filas,cols);
        const Node *n4= ttzNodes(1,1,cols);
        const Node *n5= ttzNodes(capas,1,1);
        const Node *n6= ttzNodes(capas,filas,1);
        const Node *n7= ttzNodes(capas,filas,cols);
        const Node *n8= ttzNodes(capas,1,cols);

    std::cout << "capas= " << capas << std::endl;
    std::cout << "filas= " << filas << std::endl;
    std::cout << "cols= " << cols << std::endl;

    std::cout << "base" << std::endl;
        //Linking with the nodes of the base i=1
        ID IJK1= base.Surface()->getNodeIndices(n1);
        ID IJK2= base.Surface()->getNodeIndices(n2);
        ID IJK4= base.Surface()->getNodeIndices(n4);
        size_t ind_i= 0, ind_j= 0;
        if((IJK2[1]-IJK1[1])>0)
          { ind_i= 1; ind_j= 2; }
        else if((IJK2[2]-IJK1[2])>0)
          { ind_j= 1; ind_i= 2; }
        const size_t nf= abs(IJK2[ind_i]-IJK1[ind_i])+1;
        const size_t nc= abs(IJK4[ind_j]-IJK1[ind_j])+1;
	std::cout << "IJK1= " << IJK1 << std::endl;
	std::cout << "IJK2= " << IJK2 << std::endl;
	std::cout << "IJK4= " << IJK4 << std::endl;
	std::cout << "ind_i= " << ind_i << " nf= " << nf << std::endl;
	std::cout << "ind_j= " << ind_j << " nc= " << nc << std::endl;
        double d2= 0;
        for(size_t i=1;i<=nf;i++)
          for(size_t j=1;j<=nc;j++)
            {
              size_t J= (IJK2[ind_i]-IJK1[ind_i])/(nf-1)*(i-1)+IJK1[ind_i];
              size_t K= (IJK4[ind_j]-IJK1[ind_j])/(nc-1)*(j-1)+IJK1[ind_j];
              if(ind_i<ind_j)
                ttzNodes(1,J,K)= base.getNode(i,j);
              else
                ttzNodes(1,J,K)= base.getNode(j,i);
              d2= dist2(ttzNodes(1,J,K)->getInitialPosition3d(),pos_nodes(1,J,K));
              if(d2>1e-4)
		std::cerr << "Block::create_nodes; error while linking node: ("
                          << i << "," << j << ") in face." << std::endl;
	      std::cout << "i= " << i << " j= " << j << " J= " << J << " K= " << K 
                        << " dist2= " << d2 << std::endl;
            }
	/*
    std::cout << "tapa" << std::endl;
        //Tapa i=capas
        IJK1= tapa.Surface()->getNodeIndices(n5);
        IJK2= tapa.Surface()->getNodeIndices(n7);
        for(size_t i=1;i<=filas;i++)
          for(size_t j=1;j<=cols;j++)
            {
              size_t J= (IJK2[1]-IJK1[1])/(filas-1)*(i-1)+IJK1[1];
              size_t K= (IJK2[2]-IJK1[2])/(cols-1)*(j-1)+IJK1[2];
              ttzNodes(capas,J,K)= tapa.getNode(i,j);
            }

        //Lateral izquierdo j=1.
        IJK1= latIzdo.Surface()->getNodeIndices(n1);
        IJK2= latIzdo.Surface()->getNodeIndices(n6);
        for(size_t i=1;i<=filas;i++)
          for(size_t j=1;j<=cols;j++)
            {
              size_t J= (IJK2[1]-IJK1[1])/(filas-1)*(j-1)+IJK1[1];
              size_t K= (IJK2[2]-IJK1[2])/(filas-1)*(j-1)+IJK1[2];
              ttzNodes(capas,J,K)= tapa.getNode(i,j);
            }
	*/

        for(size_t k= 2;k<capas;k++) //Capas interiores.
          for(size_t j= 2;j<filas;j++) //Filas interiores.
            for(size_t i= 2;i<cols;i++) //Columnas interiores.
              create_node(pos_nodes(i,j,k),i,j,k);
      }
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
  }

//! @brief Trigger mesh generation.
void XC::Block::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing Block...(" << getName() << ")...";
    create_nodes();
    if(ttzElements.Null())
      create_elements(dm);
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }
