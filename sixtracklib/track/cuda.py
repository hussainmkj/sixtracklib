import os
import numpy as np

modulepath=os.path.dirname(os.path.abspath(__file__)) + '/..'

try:
  import pycuda.driver as cuda
  import pycuda.autoinit
  from pycuda.compiler import SourceModule
  srcpath = ['-I%s'%modulepath,'-I%s/../common'%modulepath]
  src=open(os.path.join(modulepath,'block.c')).read()
  prg = SourceModule(src, keep=True, options=srcpath)
except ImportError:
  raise ImportError

def track(block,beam,nturn=1,elembyelem=False,turnbyturn=False):
  elembyelemid=0;turnbyturnid=0;
  if elembyelem:
      _elembyelem=block.set_elembyelem(beam,nturn)
      elembyelemid=_elembyelem.offset
  if turnbyturn:
      _turnbyturn=block.set_turnbyturn(beam,nturn)
      turnbyturnid=_turnbyturn.offset
  data_g = cuda.mem_alloc(block.data.nbytes)
  part_g = cuda.mem_alloc(beam.particles.nbytes)
  cuda.memcpy_htod(data_g, block.data.data)
  cuda.memcpy_htod(part_g, beam.particles)
  blockid = np.uint64(block.blockid)
  nturn = np.uint64(nturn)
  npart = np.uint64(beam.npart)
  elembyelemid = np.uint64(elembyelemid)
  turnbyturnid = np.uint64(turnbyturnid)
  kern = prg.get_function("Block_track")
  signiture = (np.intp, )*2 + (np.uint64, )*5
  kern.prepare(signiture)
  block = (beam.npart, 1, 1)
  grid = (1, 1)
  kern.prepared_call(grid, block,
                  data_g, part_g,
                  blockid, nturn, npart,
                  elembyelemid, turnbyturnid)
  cuda.memcpy_dtoh(block.data.data, data_g)
  cuda.memcpy_dtoh(beam.particles, part_g)
  if elembyelem:
    block.elembyelem=_elembyelem.get_beam()
  if turnbyturn:
    block.turnbyturn=_turnbyturn.get_beam()
