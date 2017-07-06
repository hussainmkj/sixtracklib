"""
SixTrackLib

Authors: R. De Maria, G. Iadarola, D. Pellegrini, H. Jasim

Copyright 2017 CERN. This software is distributed under the terms of the GNU
Lesser General Public License version 2.1, copied verbatim in the file
`COPYING''.

In applying this licence, CERN does not waive the privileges and immunities
granted to it by virtue of its status as an Intergovernmental Organization or
submit itself to any jurisdiction.
"""


#from legacy_cblock import cBlock
from cblock import Block
from cbeam import cBeam

try:
    import track.cuda as default_track
except ImportError:
    try:
        import track.cl as default_track
    except ImportError:
        #import track.cpu as default_track
        pass

