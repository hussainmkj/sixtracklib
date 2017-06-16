import os
import numpy as np

modulepath=os.path.dirname(os.path.abspath(__file__)) + '/..'

try:
  import pyopencl as cl
  os.environ['PYOPENCL_COMPILER_OUTPUT']='1'
  srcpath = ['-I%s'%modulepath,'-I%s/../common'%modulepath]
  src=open(os.path.join(modulepath,'block.c')).read()
  ctx = cl.create_some_context(interactive=False)
  prg = cl.Program(ctx,src).build(options=srcpath)
  prg_track_by_turn = cl.Program(ctx,src).build(options=srcpath+['-DTRACK_BY_TURN'])
  prg_track_by_element = cl.Program(ctx,src).build(options=srcpath+['-DTRACK_BY_ELEMENT'])
  prg_track_by_turn_and_element = cl.Program(ctx,src).build(options=srcpath+['-DTRACK_BY_TURN', '-DTRACK_BY_ELEMENT'])
  queue = cl.CommandQueue(ctx)
  mf = cl.mem_flags
  rw=mf.READ_WRITE | mf.COPY_HOST_PTR
  ro=mf.READ_ONLY | mf.COPY_HOST_PTR
  wo = mf.WRITE_ONLY | mf.COPY_HOST_PTR
except ImportError:
  raise ImportError

def track(block,beam, nturn=1, track_by_turn=False, track_by_element=False):
  track_by_args = []
  use_prg = prg
  if track_by_turn:
    _turnbyturn=block.set_turnbyturn(beam,nturn)
    track_by_turn_g = cl.Buffer(ctx, wo, hostbuf=_turnbyturn.view)
    track_by_args.append(track_by_turn_g)
    if not track_by_element:
      use_prg = prg_track_by_turn
  if track_by_element:
    _elembyelem = block.set_elembyelem(beam,nturn)
    track_by_elem_g = cl.Buffer(ctx, wo, hostbuf=_elembyelem.view)
    track_by_args.append(track_by_elem_g)
    if not track_by_turn:
      use_prg = prg_track_by_element
    else:
      use_prg = prg_track_by_turn_and_element
  data_g = cl.Buffer(ctx, ro, hostbuf=block.data)
  offsets_g = cl.Buffer(ctx, ro, hostbuf=np.array([len(block.offsets)] + block.offsets, dtype=np.uint32))
  part_g = cl.Buffer(ctx, rw, hostbuf=beam.particles)
  nturn=np.uint32(nturn)
  npart=np.uint32(beam.npart)
  use_prg.Block_track(queue,[beam.npart],None,
                  data_g, offsets_g, part_g,
                  nturn, npart,
                  *track_by_args)
  cl.enqueue_copy(queue,block.data,data_g)
  cl.enqueue_copy(queue,beam.particles,part_g)
  if track_by_turn:
    cl.enqueue_copy(queue, _turnbyturn.view, track_by_turn_g)
    block.turnbyturn=_turnbyturn.get_beam()
  if track_by_element:
    cl.enqueue_copy(queue, _elembyelem.view, track_by_elem_g)
    block.elembyelem=_elembyelem.get_beam()

