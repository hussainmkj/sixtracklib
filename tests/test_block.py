import numpy as np
import sixtracklib

from sixtracklib.track import cpu
try:
  from sixtracklib.track import cl as cl
except ImportError:
  cl = None
try:
  from sixtracklib.track import cuda as cuda
except ImportError:
  cuda = None


block = sixtracklib.Block(2)
block.Multipole([1.,3.,5.],[2.,4.,6.],0,0,0,)
block.Drift(56.)
block.Drift(5.)
block.Block()

def test_track():
  beam=sixtracklib.cBeam(50)
  cpu.track(block, beam)

  assert beam.particles[2]['s']  ==61.0
  assert beam.particles[2]['px'] ==-1.0
  assert beam.particles[2]['py'] ==2.0


def test_track_cl():
  if cl:
    beam=sixtracklib.cBeam(50)
    cl.track(block, beam)

    assert beam.particles[2]['s'] ==61.0
    assert beam.particles[2]['px']==-1.0
    assert beam.particles[2]['py']==2.0

def test_track_cuda():
  if cuda:
    beam=sixtracklib.cBeam(50)
    cuda.track(block, beam)

    assert beam.particles[2]['s'] ==61.0
    assert beam.particles[2]['px']==-1.0
    assert beam.particles[2]['py']==2.0


