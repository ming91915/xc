# -*- coding: utf-8 -*-

'''Eurocode 3 checks for a beam or column.'''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model.geometry import geom_utils as lu
from materials.ec3 import EC3_limit_state_checking as EC3lsc

class EC3Beam(lu.LineWrapper):
  def __init__(self,line,ec3Shape):
    super(EC3Beam,self).__init__(line)
    self.ec3Shape= ec3Shape
  def getLateralBucklingReductionFactor(self,sectionClass,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0,typo= 'rolled'):
    ''' Returns lateral torsional buckling reduction factor value
        for the elements inside the line.'''
    xi= self.getNodeAbcissae()
    Mi= self.getElementFunctionOrdinates(-1,'getMz1',1,'getMz2')
    return self.ec3Shape.getLateralBucklingReductionFactor(sectionClass,xi,Mi,ky,kw, k1, k2,typo)

  def updateLateralBucklingReductionFactor(self,sectionClass,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0,typo= 'rolled'):
    chiLT= self.getLateralBucklingReductionFactor(sectionClass,ky,kw, k1, k2,typo)
    elems= self.getElements()
    for e in elems:
       e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.


  def installULSControlRecorder(self,recorderType,sectionClass= 1, chiLT=1.0):
    '''Installs recorder for verification of ULS criterion.'''
    preprocessor= self.line.getPreprocessor
    nodes= preprocessor.getNodeHandler
    domain= preprocessor.getDomain
    recorder= domain.newRecorder(recorderType,None)
    elems= self.getElements()
    eleTags= list()
    for e in elems:
      eleTags.append(e.tag)
      e.setProp('ULSControlRecorder',recorder)
    idEleTags= xc.ID(eleTags)
    recorder.setElements(idEleTags)
    self.ec3Shape.setupULSControlVars(elems,sectionClass,chiLT)
    if(nodes.numDOFs==3):
      recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
    else:
      recorder.callbackRecord= EC3lsc.controlULSCriterion()

    recorder.callbackRestart= "print \"Restart method called.\""
    return recorder

