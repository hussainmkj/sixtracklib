import os
import ctypes
import numpy as np

modulepath=os.path.dirname(os.path.abspath(__file__)) + '/..'

blocklibpath=os.path.join(modulepath, 'block.so')
blocklib=ctypes.CDLL(blocklibpath)
blocklib.Block_track.argtypes=[ctypes.c_void_p, # *data
                               ctypes.c_void_p, # *particles
                               ctypes.c_uint64, # blockid
                               ctypes.c_uint64, # nturn
                               ctypes.c_uint64, # elembyelemid
                               ctypes.c_uint64 ] # turnbyturnid

def track(block,beam,nturn=1,elembyelem=False,turnbyturn=False):
  elembyelemid=0;turnbyturnid=0;
  if elembyelem:
      _elembyelem=block.set_elembyelem(beam,nturn)
      elembyelemid=_elembyelem.offset
  if turnbyturn:
      _turnbyturn=block.set_turnbyturn(beam,nturn)
      turnbyturnid=_turnbyturn.offset
  blocklib.Block_track(block.data.ctypes.data, beam.ctypes(),
                       block.blockid, nturn,
                       elembyelemid,turnbyturnid)
  if elembyelem:
    block.elembyelem=_elembyelem.get_beam()
  if turnbyturn:
    block.turnbyturn=_turnbyturn.get_beam()
