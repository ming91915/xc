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
//python_interface.tcc

const double &(XC::CrossSectionProperties2d::*getE)(void) const= &XC::CrossSectionProperties2d::E;
const double &(XC::CrossSectionProperties2d::*getG)(void) const= &XC::CrossSectionProperties2d::G;
const double &(XC::CrossSectionProperties2d::*getA)(void) const= &XC::CrossSectionProperties2d::A;
const double &(XC::CrossSectionProperties2d::*getAlpha)(void) const= &XC::CrossSectionProperties2d::Alpha;
const double &(XC::CrossSectionProperties2d::*getI)(void) const= &XC::CrossSectionProperties2d::I;
class_<XC::CrossSectionProperties2d, bases<EntCmd> >("CrossSectionProperties2d")
  .add_property("E", make_function( getE, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setE,"Elastic modulus.")
  .add_property("G", make_function( getG, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setG,"Shear modulus.")
  .add_property("A", make_function( getA, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setA,"Area.")
  .add_property("Alpha", make_function( getAlpha, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setAlpha," Shear reduction factor.")
  .add_property("I", make_function( getI, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setI, "Moment of inertia.")
  .def(self_ns::str(self_ns::self))
  ;

const double &(XC::CrossSectionProperties3d::*getIz)(void) const= &XC::CrossSectionProperties3d::Iz;
const double &(XC::CrossSectionProperties3d::*getIy)(void) const= &XC::CrossSectionProperties3d::Iy;
const double &(XC::CrossSectionProperties3d::*getIyz)(void) const= &XC::CrossSectionProperties3d::Iyz;
const double &(XC::CrossSectionProperties3d::*getJ)(void) const= &XC::CrossSectionProperties3d::J;
class_<XC::CrossSectionProperties3d, bases<XC::CrossSectionProperties2d> >("CrossSectionProperties3d")
  .add_property("Iz", make_function( getIz, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIz)
  .add_property("Iy", make_function( getIy, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIy)
  .add_property("Iyz", make_function( getIyz, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIyz)
  .add_property("J", make_function( getJ, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setJ)
  .def("rotate",&XC::CrossSectionProperties3d::rotate,"Rotate section (deprecated).")
  .def("getI1",&XC::CrossSectionProperties3d::getI1)
  .def("getI2",&XC::CrossSectionProperties3d::getI2)
  .def("getVDirEje1",&XC::CrossSectionProperties3d::getVDirEje1)
  .def("getVDirEje2",&XC::CrossSectionProperties3d::getVDirEje2)
  .def("getVDirStrongAxis",&XC::CrossSectionProperties3d::getVDirStrongAxis)
  .def("getVDirWeakAxis",&XC::CrossSectionProperties3d::getVDirWeakAxis)
  ;

XC::Material *(XC::DiscretBase::*getMaterialPtr)(void) const= &XC::DiscretBase::getMaterialPtr;
class_<XC::DiscretBase, bases<EntConNmb>, boost::noncopyable >("DiscretBase", no_init)
  .def("getMaxY",&XC::DiscretBase::getMaxY,"Returns y coordinate maximum value.")
  .def("getMaxZ",&XC::DiscretBase::getMaxZ,"Returns z coordinate maximum value.")
  .def("getMinY",&XC::DiscretBase::getMinY,"Returns y coordinate minimum value.")
  .def("getMinZ",&XC::DiscretBase::getMinZ,"Returns z coordinate minimum value.")
  .def("getMaterial",make_function(getMaterialPtr,return_internal_reference<>()), "returns a pointer to the material.")
  ;

class_<XC::SectionMassProperties, bases<EntCmd>, boost::noncopyable >("SectionMassProperties", no_init)
  .def("getAreaGrossSection",&XC::SectionMassProperties::getAreaGrossSection,"Returns region's gross section.")
  .def("getCdgGrossSection",&XC::SectionMassProperties::getCdgGrossSection,"Returns center of gravity of gross section.")
  .def("getIyGrossSection",&XC::SectionMassProperties::getIyGrossSection,"Inertia of the gross section about an axis parallel to y through his center of gravity.")
  .def("getIzGrossSection",&XC::SectionMassProperties::getIzGrossSection,"Inertia of the gross section about an axis parallel to z through his center of gravity.")
  .def("getPyzGrossSection",&XC::SectionMassProperties::getPyzGrossSection,"Product of inertia of the gross section about y and z axis through his center of gravity.")

  .def("getIxGrossSection",&XC::SectionMassProperties::getIxGrossSection,"Inertia of the gross section about an axis parallel to x through his center of gravity.")
  .def("getTheta_pGrossSection",&XC::SectionMassProperties::getTheta_pGrossSection,"Agle of the principal axis of inertia of the gross section with respect of current axis.")
.def("getDirEjeI_aGrossSection",&XC::SectionMassProperties::getDirEjeI_aGrossSection,"Direction of a principal axis of inertia of the gross section with respect of current axis.")
  .def("getDirEjeI_bGrossSection",&XC::SectionMassProperties::getDirEjeI_bGrossSection,"Direction of the other principal axis of inertia of the gross section with respect of current axis.")
  .def("getI1GrossSection",&XC::SectionMassProperties::getI1GrossSection,"Major principal moment of inertia of the gross section.")
  .def("getI2GrossSection",&XC::SectionMassProperties::getI2GrossSection,"Minor principal moment of inertia of the gross section.")
  .def("getEjesInerciaGrossSection",&XC::SectionMassProperties::getEjesInerciaGrossSection,"Principal axis of inertia of the gross section.")
  .def("getVDirEje1GrossSection",&XC::SectionMassProperties::getVDirEje1GrossSection,"Direction of the major principal axis of inertia of the gross section.")
  .def("getVDirStrongAxisGrossSection",&XC::SectionMassProperties::getVDirStrongAxisGrossSection,"Direction of the major principal axis of inertia of the gross section.")
.def("getVDirEje2GrossSection",&XC::SectionMassProperties::getVDirEje2GrossSection,"Direction of the minor principal axis of inertia of the gross section.")
  .def("getVDirWeakAxisGrossSection",&XC::SectionMassProperties::getVDirWeakAxisGrossSection,"Direction of the minor principal axis of inertia of the gross section.")
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)
  .def("getIOGrossSection",&XC::SectionMassProperties::getIOGrossSection)
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)
//.def("getIGrossSection",&XC::SectionMassProperties::getIGrossSection)

  .def("getAreaHomogenizedSection",&XC::SectionMassProperties::getAreaHomogenizedSection,"Returns region's homogenized section.")
  .def("getCdgHomogenizedSection",&XC::SectionMassProperties::getCdgHomogenizedSection,"Returns center of gravity of homogenized section.")
  .def("getIyHomogenizedSection",&XC::SectionMassProperties::getIyHomogenizedSection,"Inertia of the homogenized section about an axis parallel to y through his center of gravity.")
  .def("getIzHomogenizedSection",&XC::SectionMassProperties::getIzHomogenizedSection,"Inertia of the homogenized section about an axis parallel to z through his center of gravity.")
  .def("getPyzHomogenizedSection",&XC::SectionMassProperties::getPyzHomogenizedSection,"Product of inertia of the homogenized section about y and z axis through his center of gravity.")

  .def("getIxHomogenizedSection",&XC::SectionMassProperties::getIxHomogenizedSection,"Inertia of the homogenized section about an axis parallel to x through his center of gravity.")
  .def("getTheta_pHomogenizedSection",&XC::SectionMassProperties::getTheta_pHomogenizedSection,"Agle of the principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getDirEjeI_aHomogenizedSection",&XC::SectionMassProperties::getDirEjeI_aHomogenizedSection,"Direction of a principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getDirEjeI_bHomogenizedSection",&XC::SectionMassProperties::getDirEjeI_bHomogenizedSection,"Direction of the other principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getI1HomogenizedSection",&XC::SectionMassProperties::getI1HomogenizedSection,"Major principal moment of inertia of the homogenized section.")
  .def("getI2HomogenizedSection",&XC::SectionMassProperties::getI2HomogenizedSection,"Minor principal moment of inertia of the homogenized section.")
  .def("getEjesInerciaHomogenizedSection",&XC::SectionMassProperties::getEjesInerciaHomogenizedSection,"Principal axis of inertia of the homogenized section.")
  .def("getVDirEje1HomogenizedSection",&XC::SectionMassProperties::getVDirEje1HomogenizedSection,"Direction of the major principal axis of inertia of the homogenized section.")
  .def("getVDirStrongAxisHomogenizedSection",&XC::SectionMassProperties::getVDirStrongAxisHomogenizedSection,"Direction of the major principal axis of inertia of the homogenized section.")
  .def("getVDirEje2HomogenizedSection",&XC::SectionMassProperties::getVDirEje2HomogenizedSection,"Direction of the minor principal axis of inertia of the homogenized section.")
  .def("getVDirWeakAxisHomogenizedSection",&XC::SectionMassProperties::getVDirWeakAxisHomogenizedSection,"Direction of the minor principal axis of inertia of the homogenized section.")
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)
  .def("getIOHomogenizedSection",&XC::SectionMassProperties::getIOHomogenizedSection)
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)
//.def("getIHomogenizedSection",&XC::SectionMassProperties::getIHomogenizedSection)

  .def("getCrossSectionProperties3d",&XC::SectionMassProperties::getCrossSectionProperties3d)
  .def("getCrossSectionProperties2d",&XC::SectionMassProperties::getCrossSectionProperties2d)
  ;

#include "cell/python_interface.tcc"
#include "geom_section/python_interface.tcc"
#include "section/python_interface.tcc"
