//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDVertexInfo

#include "MEDVertexInfo.h"



#include "domain/mesh/Mesh.h"
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! Constructor.
XC::MEDVertexInfo::MEDVertexInfo(void)
  : spaceDimension(3), tipoCoordenadas("CARTESIAN") {}

//! Constructor.
XC::MEDVertexInfo::MEDVertexInfo(const Mesh &mesh)
  : spaceDimension(3), tipoCoordenadas("CARTESIAN"), nombresCoordenadas(mesh.getNombresCoordenadas())
  {
    nombresUnidades= std::vector<std::string>(spaceDimension, mesh.getNombreUnidades());
    const Node *theNode= nullptr;
    Mesh &mesh_no_const= const_cast<Mesh &>(mesh);
    NodeIter &theNodes = mesh_no_const.getNodes();
    while((theNode = theNodes()) != nullptr)
      {
        const int tag= theNode->getTag();
        const Pos3d pos= theNode->getInitialPosition3d();
        std::vector<double> coo(3);
        coo[0]= pos.x();coo[1]= pos.y();coo[2]= pos.z();
        new_vertice(tag,coo);
      }
  }

//! Deletes mesh.
void XC::MEDVertexInfo::clear(void)
  {
    coordenadas.clear();
    MEDBaseInfo::clear();
  }

//! @brief Creates the i-th vertex.
void XC::MEDVertexInfo::new_vertice(size_t i,const std::vector<double> &coo)
  {
    const size_t sz= coo.size();
    if(sz>=spaceDimension)
      {
        for(size_t j= 0;j<spaceDimension;j++)
          coordenadas.push_back(coo[j]);
        new_tag(i);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; " << spaceDimension << " components expected "
                << sz << " obtained." << std::endl;
  }

//! @brief Appends a new vertex.
//! @param i index of the vertex in the container.
//! @paral l coordinates of the vertex.
void XC::MEDVertexInfo::newVertex(const size_t &i,const boost::python::list &l)
  {
    const size_t sz= len(l);
    std::vector<double> coo(sz,0.0);
    for(size_t j=0; j<sz; j++)
      coo[j]= boost::python::extract<double>(l[j]);
    new_vertice(i,coo);
  }

//! @brief Acceso al vector de coordenadas.
const std::vector<double> &XC::MEDVertexInfo::getCoordenadas(void) const
  { return coordenadas; }

//! @brief Assigns coordinate names.
void XC::MEDVertexInfo::setCoordinateNames(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      nombresCoordenadas.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Assigns coordinate units.
void XC::MEDVertexInfo::setUnitNames(const boost::python::list &l)
  {
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      nombresUnidades.push_back(boost::python::extract<std::string>(l[i]));
  }

//! @brief Dumps cells definition on MED mesh
void XC::MEDVertexInfo::to_med(MEDMEM::MESHING &mesh) const
  {
    const int numNodes= getNumVertices();
    const std::vector<double> &coo= getCoordenadas();
    const size_t &sd= getSpaceDimension();
    if(sd>0)
      {
        mesh.setCoordinates(getSpaceDimension(),numNodes,&coo[0],getTipoCoordenadas(),::MED_FULL_INTERLACE);
        if(nombresCoordenadas.size()<sd)
          std::cerr << "No se han especificado los nombres de las coordenadas."
                    << std::endl;
        else 
          mesh.setCoordinatesNames(&nombresCoordenadas[0]);
        if(nombresCoordenadas.size()<sd)
          std::cerr << "No se han especificado las unidades de las coordenadas."
                    << std::endl;
        mesh.setCoordinatesUnits(&nombresUnidades[0]);
      }
    else
      std::cerr << "Space of dimension zero." << std::endl;
  }
