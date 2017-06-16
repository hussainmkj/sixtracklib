import os
import ctypes
import numpy as np

modulepath=os.path.dirname(os.path.abspath(__file__)) + '/..'

blocklibpath=os.path.join(modulepath, 'block.so')
blocklib=ctypes.CDLL(blocklibpath)
blocklib.Block_track.argtypes=[ctypes.c_void_p, # *data
                               ctypes.c_void_p, # *offsets
                               ctypes.c_void_p, # *particles
                               ctypes.c_uint32, # nturn
                               ctypes.c_uint32, # npart
                               ctypes.c_void_p, # *turnbyturn
                               ctypes.c_void_p ] # *elembyelem

def track(block, beam, nturn=1, track_by_turn=False, track_by_element=False):
  track_by_turn_c = None
  track_by_element_c = None
  if track_by_turn:
    _turnbyturn=block.set_turnbyturn(beam,nturn)
    track_by_turn_c = _turnbyturn.view.ctypes.data
  if track_by_element:
    _elembyelem=block.set_elembyelem(beam,nturn)
    track_by_element_c = _elembyelem.view.ctypes.data
  offsets_arg = np.array([len(block.offsets)] + block.offsets, dtype=np.uint32)
  blocklib.Block_track(
                       block.data.ctypes.data, offsets_arg.ctypes.data, beam.ctypes(),
                       nturn, beam.npart,
                       track_by_turn_c, track_by_element_c)
  if track_by_turn:
    block.turnbyturn=_turnbyturn.get_beam()
  if track_by_element:
    block.elembyelem=_elembyelem.get_beam()

