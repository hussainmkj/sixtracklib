import os
import numpy as np

modulepath=os.path.dirname(os.path.abspath(__file__)) + '/..'

try:
  import pyopencl as cl
  os.environ['PYOPENCL_COMPILER_OUTPUT']='1'
  srcpath = ['-I%s'%modulepath,'-I%s/../common'%modulepath]
  src=open(os.path.join(modulepath,'block.c')).read()
  ctx = cl.create_some_context(interactive=False)
  prg=cl.Program(ctx,src).build(options=srcpath)
  queue = cl.CommandQueue(ctx)
  mf = cl.mem_flags
  rw=mf.READ_WRITE | mf.COPY_HOST_PTR
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
  data_g=cl.Buffer(ctx, rw, hostbuf=block.data)
  part_g=cl.Buffer(ctx, rw, hostbuf=beam.particles)
  blockid=np.uint64(block.blockid)
  nturn=np.uint64(nturn)
  npart=np.uint64(beam.npart)
  elembyelemid=np.uint64(elembyelemid)
  turnbyturnid=np.uint64(turnbyturnid)
  prg.Block_track(queue,[beam.npart],None,
                  data_g, part_g,
                  blockid, nturn, npart,
                  elembyelemid, turnbyturnid)
  cl.enqueue_copy(queue,block.data,data_g)
  cl.enqueue_copy(queue,beam.particles,part_g)
  if elembyelem:
    block.elembyelem=_elembyelem.get_beam()
  if turnbyturn:
    block.turnbyturn=_turnbyturn.get_beam()

