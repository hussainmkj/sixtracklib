from cblock import Block

import track.cpu as cpu

try:
  import track.cl as cl
except ImportError:
  print("Warning: error import OpenCL: track_cuda not available")
  cl = None

try:
  import track.cuda as cuda
except ImportError:
  print("Warning: error import Cuda: track_cuda not available")
  cuda = None

class cBlock(Block):
  def __init__(self, size=512):
    Block.__init__(self, size)
  def track(self,beam,nturn=1,elembyelem=False,turnbyturn=False):
    cpu.track(self, beam, nturn, elembyelem, turnbyturn)
  if cl:
    def track_cl(self,beam,nturn=1,elembyelem=False,turnbyturn=False):
      cl.track(self, beam, nturn, elembyelem, turnbyturn)
  if cuda:
    def track_cuda(self,beam,nturn=1,elembyelem=False,turnbyturn=False):
      cuda.track(self, beam, nturn, elembyelem, turnbyturn)
