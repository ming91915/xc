# -*- coding: utf-8 -*-
from __future__ import division

'''Routines for cantilever retaining walls design.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from postprocess.reports import common_formats as fmt
from postprocess.reports import draw_schema_armature_mur as draw_schema
import math
import scipy.interpolate
import matplotlib
#matplotlib.use('PS')
import matplotlib.pyplot as plt
from materials.sia262 import SIA262_materials
from model.geometry import retaining_wall_geometry
from rough_calculations import ng_rebar_def
from rough_calculations import ng_rc_section
import os
from miscUtils import LogMessages as lmsg

def filterRepeatedValues(yList,mList,vList):
  sz= len(yList)

  mapM={}
  mapV= {}
  for i in range(0,sz):
    y= abs(yList[i])
    mapM[y]= mList[i]
    mapV[y]= vList[i]

  retY= list()
  retM= list()
  retV= list()
  for y in mapM:
    retY.append(y)
  retY.sort()
  for y in retY:
    retM.append(mapM[y])
    retV.append(mapV[y])
  return retY, retM, retV

class InternalForces(object):
  '''Internal forces for a retaining wall obtained from
     Laryx (Cubus suite) software.'''
  def __init__(self,y,mdMax,vdMax,MdSemelle,VdSemelle):
    self.y, self.mdMax, self.vdMax= filterRepeatedValues(y,mdMax,vdMax)
    self.interpolate()
    self.stemHeight= self.y[-1]
    self.MdSemelle= MdSemelle
    self.VdSemelle= VdSemelle
  def interpolate(self):
    self.mdMaxVoile= scipy.interpolate.interp1d(self.y,self.mdMax)
    self.vdMaxVoile= scipy.interpolate.interp1d(self.y,self.vdMax)    
  def __imul__(self,f):
    for m in self.mdMax:
      m*=f
    for v in self.vdMax:
      v*=f
    self.interpolate()
    self.MdSemelle*=f
    self.VdSemelle*=f
    return self
  def clone(self):
    return InternalForces(self.y, self.mdMax, self.vdMax,self.MdSemelle,self.VdSemelle)
  def __mul__(self, f):
    retval= self.clone()
    retval*= f
    return retval
  def __rmul__(self,f):
    return self*f
  def MdEncastrement(self,footingThickness):
    '''Bending moment (envelope) at stem base.'''
    yEncastrement= self.stemHeight-footingThickness/2.0
    return self.Md(yEncastrement)
  def VdEncastrement(self,epaisseurEncastrement):
    '''Shear force (envelope) at stem base.'''
    yV= self.stemHeight-epaisseurEncastrement
    return abs(self.vdMaxVoile(yV))
  def Vd(self, yCoupe):
    '''Shear (envelope) at height yCoupe.'''
    return abs(self.vdMaxVoile(yCoupe))
  def Md(self, yCoupe):
    '''Bending moment (envelope) at height yCoupe.'''
    return abs(self.mdMaxVoile(yCoupe))
  def getYVoile(self,hCoupe):
    return self.stemHeight-hCoupe
  def writeGraphic(self,fileName):
    '''Draws a graphic of internal forces (envelopes) in
       the wall stem.'''
    z= []
    for yi in self.y:
      z.append(self.stemHeight-yi)
    m= []
    for mi in self.mdMax:
      m.append(mi/1e3)
    v= []
    for vi in self.vdMax:
      v.append(vi/1e3)
    plt.plot(m,z,'-', v, z,'--')
    plt.legend(['Md (kN m/m)', 'Vd (kN/m)'], loc='best')
    plt.title("Efforts internes.")
    plt.savefig(fileName)
    plt.close()

class RetainingWallReinforcement(dict):
  ''' Simplified reinforcement for a cantilever retaining wall.'''
  def __init__(self,concreteCover=40e-3, steel= SIA262_materials.B500B):
    '''Constructor '''
    super(RetainingWallReinforcement, self).__init__()
    self.concreteCover= concreteCover
    #Materials.
    self.steel= steel
    #Default reinforcement
    AdefA= ng_rebar_def.RebarFamily(self.steel,8e-3,0.15,concreteCover)
    Adef= AdefA
    for i in range(1,15):
      self[i]= Adef
    # #Armature de peau semelle
    # R= self.footingThickness-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[10]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    # #Armature couronnement.
    # R= self.stemTopWidth-2*self.concreteCover-8e-3
    # n= math.ceil(R/0.15)+1
    # ecart= R/(n-1)
    # self[13]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
    
  def setArmature(self,index,armature):
    '''Assigns armature.'''
    self[index]= armature

  def getArmature(self,index):
    '''Return armature at index.'''
    return self[index]
    

class RetainingWall(retaining_wall_geometry.CantileverRetainingWallGeometry):
  '''Cantilever retaining wall.'''
  b= 1.0

  def __init__(self,name= 'prb',concreteCover=40e-3,stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25):
    '''Constructor '''
    super(RetainingWall,self).__init__(name,stemBottomWidth,stemTopWidth,footingThickness)
    #Materials.
    self.concrete= SIA262_materials.c25_30
    self.reinforcement= RetainingWallReinforcement(concreteCover)
    
  def getBasicAnchorageLength(self,index):
    '''Returns basic anchorage length for the reinforcement at "index".''' 
    return self.reinforcement.getArmature(index).getBasicAnchorageLength(self.concrete)

  def getSection1(self):
    '''Returns RC section for armature in position 1.'''
    return ng_rc_section.RCSection(self.reinforcement[1],self.concrete,self.b,self.stemBottomWidth)
  def getSection2(self,y):
    '''Returns RC section for armature in position 2.'''
    c= self.getDepth(y)
    return ng_rc_section.RCSection(self.reinforcement[2],self.concrete,self.b,c)
  def getSection3(self):
    '''Returns RC section for armature in position 3.'''
    return ng_rc_section.RCSection(self.reinforcement[3],self.concrete,self.b,self.footingThickness)
  def getSection4(self):
    '''Returns RC section for armature in position 4.'''
    return ng_rc_section.RCSection(self.reinforcement[4],self.concrete,self.b,self.stemBottomWidth)
  def getSection6(self):
    '''Returns RC section for armature in position 6.'''
    return ng_rc_section.RCSection(self.reinforcement[6],self.concrete,self.b,self.stemTopWidth)
  def getSection7(self):
    '''Returns RC section for armature in position 7.'''
    return ng_rc_section.RCSection(self.reinforcement[7],self.concrete,self.b,self.footingThickness)
  def getSection8(self):
    '''Returns RC section for armature in position 8.'''
    return ng_rc_section.RCSection(self.reinforcement[8],self.concrete,self.b,self.footingThickness)
  def getSection11(self):
    '''Returns RC section for armature in position 11.'''
    return ng_rc_section.RCSection(self.reinforcement[11],self.concrete,self.b,(self.stemTopWidth+self.stemBottomWidth)/2.0)

  def setULSInternalForcesEnvelope(self,wallInternalForces):
    '''Assigns the ultimate limit state infernal forces envelope for the stem.'''
    if(hasattr(self,'stemHeight')):
      if(self.stemHeight!=wallInternalForces.stemHeight):
        lmsg.warning('stem height (' + str(self.stemHeight) + ' m) different from length of internal forces envelope law('+ str(wallInternalForces.stemHeight)+ ' m') 
    else:
      self.stemHeight= wallInternalForces.stemHeight
    self.internalForcesULS= wallInternalForces

  def setSLSInternalForcesEnvelope(self,wallInternalForces):
    '''Assigns the serviceability limit state infernal forces envelope for the stem.'''
    if(hasattr(self,'stemHeight')):
      if(self.stemHeight!=wallInternalForces.stemHeight):
        lmsg.warning('stem height (' + str(self.stemHeight) + ' m) different from length of internal forces envelope law('+ str(wallInternalForces.stemHeight)+ ' m') 
    else:
      self.stemHeight= wallInternalForces.stemHeight
    self.internalForcesSLS= wallInternalForces
    
  def writeDef(self,pth,outputFile):
    '''Write wall definition in LaTeX format.'''
    pathFiguraEPS= pth+self.name+".eps"
    pathFiguraPDF= pth+self.name+".pdf"
    self.internalForcesULS.writeGraphic(pathFiguraEPS)
    os.system("convert "+pathFiguraEPS+" "+pathFiguraPDF)
    outputFile.write("\\begin{table}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{tabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{1}{|c|}{\\textsc{"+self.name+"}}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\begin{tabular}{c|l}\n")
    outputFile.write("\\begin{minipage}{85mm}\n")
    outputFile.write("\\vspace{2mm}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\includegraphics[width=80mm]{"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\vspace{1pt}\n")
    outputFile.write("\\end{minipage} & \n")
    self.writeGeometry(outputFile)
    outputFile.write("\\end{tabular} \\\\\n")

    outputFile.write("\\hline\n")
    outputFile.write("\\begin{tabular}{llll}\n")
    outputFile.write("\\multicolumn{3}{c}{\\textsc{Matériels}}\\\\\n")
    outputFile.write("  Béton: " + self.concrete.materialName +" & ")
    outputFile.write("  Acier: " + self.reinforcement.steel.materialName +" & ")
    outputFile.write("  ConcreteCover: "+ fmt.Diam.format(self.reinforcement.concreteCover*1e3)+ " mm\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\end{tabular}\n")
    outputFile.write("\\caption{Matériels et dimensions mur "+ self.name +"} \\label{tb_def_"+self.name+"}\n")
    outputFile.write("\\end{center}\n")
    outputFile.write("\\end{table}\n")


  def writeResult(self,pth):
    '''Write reinforcement verification results in LaTeX format.'''
    outputFile= open(pth+self.name+".tex","w")
    self.writeDef(pth,outputFile)
    outputFile.write("\\bottomcaption{Calcul armatures mur "+ self.name +"} \\label{tb_"+self.name+"}\n")
    outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{Armatures mur "+self.name+"}}\\\\\\hline\n}\n")
    outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.name+" (suite)}}\\\\\\hline\n}\n")
    outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{supertabular}[H]{|l|}\n")
    outputFile.write("\\hline\n")

    #Coupe 1. Béton armé. Encastrement.
    C1= self.getSection1()
    VdEncastrement= self.internalForcesULS.VdEncastrement(self.stemBottomWidth)
    MdEncastrement= self.internalForcesULS.MdEncastrement(self.footingThickness)
    outputFile.write("\\textbf{Armature 1 (armature extérieure en attente) :} \\\\\n")
    NdEncastrement= 0.0 #we neglect axial force
    C1.writeResultFlexion(outputFile,NdEncastrement, MdEncastrement,VdEncastrement)
    C1.writeResultStress(outputFile,self.internalForcesSLS.MdEncastrement(self.footingThickness))

    #Coupe 2. Béton armé. Voile
    yCoupe2= self.internalForcesULS.getYVoile(self.getBasicAnchorageLength(1))
    C2= self.getSection2(yCoupe2)
    Nd2= 0.0 #we neglect axial force
    Vd2= self.internalForcesULS.Vd(yCoupe2)
    Md2= self.internalForcesULS.Md(yCoupe2)
    outputFile.write("\\textbf{Armature 2 (armature extériéure voile):}\\\\\n")
    C2.writeResultFlexion(outputFile,Nd2,Md2,Vd2)
    C2.writeResultStress(outputFile,self.internalForcesSLS.Md(yCoupe2))

    #Coupe 3. Béton armé. Semelle
    C3= self.getSection3()
    Nd3= 0.0 #we neglect axial force
    Vd3= self.internalForcesULS.VdSemelle
    Md3= self.internalForcesULS.MdSemelle
    outputFile.write("\\textbf{Armature 3 (armature supérieure semelle):}\\\\\n")
    C3.writeResultFlexion(outputFile,Nd3,Md3,Vd3)
    C3.writeResultStress(outputFile,self.internalForcesSLS.MdSemelle)

    C4= self.getSection4()
    C5= C4
    C6= self.getSection6()
    C7= self.getSection7()
    C8= self.getSection8()
    C9= C8
    C11= self.getSection11()
    C12= C11

    #Coupe 4. armature intérieure en attente. Encastrement voile 
    outputFile.write("\\textbf{Armature 4 (armature intérieure en attente):}\\\\\n")
    C4.writeResultCompression(outputFile,0.0,C12.tensionRebars.getAs())

    #Coupe 5. armature intérieure en voile.
    outputFile.write("\\textbf{Armature 5 (armature intérieure en voile):}\\\\\n")
    C5.writeResultCompression(outputFile,0.0,C12.tensionRebars.getAs())

    #Coupe 6. armature couronnement.
    outputFile.write("\\textbf{Armature 6 (armature couronnement):}\\\\\n")
    C6.writeResultFlexion(outputFile,0.0,0.0,0.0)

    #Coupe 7. armature inférieure semelle.
    outputFile.write("\\textbf{Armature 7 (armature trsv. inférieure semelle):}\\\\\n")
    C7.writeResultCompression(outputFile,0.0,C8.tensionRebars.getAs())

    #Coupe 8. armature long. inférieure semelle.
    outputFile.write("\\textbf{Armature 8 (armature long. inférieure semelle):}\\\\\n")
    C8.writeResultTraction(outputFile,0.0)

    #Coupe 9. armature long. supérieure semelle.
    outputFile.write("\\textbf{Armature 9 (armature long. supérieure semelle):}\\\\\n")
    C9.writeResultTraction(outputFile,0.0)

    #Armature 10. armature de peau semelle.
    outputFile.write("\\textbf{Armature 10 (armature de peau semelle):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.concrete,self.reinforcement[10],1e-5)

    #Coupe 11. armature long. extérieure voile.
    outputFile.write("\\textbf{Armature 11 (armature long. extérieure voile):}\\\\\n")
    C11.writeResultTraction(outputFile,0.0)

    #Coupe 12. armature long. intérieure voile.
    outputFile.write("\\textbf{Armature 12 (armature long. intérieure voile):}\\\\\n")
    C12.writeResultTraction(outputFile,0.0)

    #Armature 13. armature long. couronnement.
    outputFile.write("\\textbf{Armature 13 (armature long. couronnement):}\\\\\n")
    outputFile.write("  --\\\\\n")
    #writeRebars(outputFile,self.concrete,self.reinforcement[13],1e-5)
    outputFile.write("\\hline\n")
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{center}\n")
    outputFile.close()

  def drawSchema(self,pth):
    '''Retaining wall scheme drawing in LaTeX format.'''
    outputFile= open(pth+'schema_'+self.name+".tex","w")
    outputFile.write("\\begin{figure}\n")
    outputFile.write("\\begin{center}\n")
    outputFile.write(draw_schema.hdr)
    for l in draw_schema.lines:
      outputFile.write(l)

    defStrings= {}
    defStrings[1]= self.getSection1().tensionRebars.getDefStrings()
    yCoupe2= self.internalForcesULS.getYVoile(self.getBasicAnchorageLength(1))
    defStrings[2]= self.getSection2(yCoupe2).tensionRebars.getDefStrings()
    defStrings[3]= self.getSection3().tensionRebars.getDefStrings()
    defStrings[4]= self.getSection4().tensionRebars.getDefStrings()
    defStrings[5]= self.getSection4().tensionRebars.getDefStrings() #C5==C4
    defStrings[6]= self.getSection6().tensionRebars.getDefStrings()
    defStrings[7]= self.getSection7().tensionRebars.getDefStrings()
    defStrings[8]= self.getSection8().tensionRebars.getDefStrings()
    defStrings[9]= self.getSection8().tensionRebars.getDefStrings() #C9==C8
    #defStrings[10]= self.getSection10().tensionRebars.getDefStrings()
    defStrings[11]= self.getSection11().tensionRebars.getDefStrings()
    defStrings[12]= self.getSection11().tensionRebars.getDefStrings() #C12==C11

    rebarAnno= draw_schema.getRebarAnnotationLines(defStrings)
    for l in rebarAnno:
      outputFile.write(l)
    outputFile.write(draw_schema.tail)
    outputFile.write("\\end{center}\n")
    outputFile.write("\\caption{Schéma armatures mur "+ self.name +"} \\label{fg_"+self.name+"}\n")
    outputFile.write("\\end{figure}\n")
