# -*- coding: utf-8 -*-
# Home made test
import xc_base
import geom
import xc

from materials.ehe import EHE_materials
from materials import concrete_base

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  feProblem.getPreprocessor
# Define materials
errMax= concrete_base.testReinfSteelCharacteristicDiagram(preprocessor, EHE_materials.B500S)


# print "errMax= ",(errMax)
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if errMax<1e-10:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
