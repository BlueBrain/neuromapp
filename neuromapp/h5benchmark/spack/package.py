# Copyright 2013-2018 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack import *

class H5benchmark(CMakePackage):
    """Mini-application to evaluate morphology files I/O performance"""

    homepage = "https://github.com/BlueBrain/neuromapp/tree/sergiorg_h5bmark/neuromapp/h5benchmark"
    url      = "https://github.com/BlueBrain/neuromapp.git"
    git      = "https://github.com/BlueBrain/neuromapp.git"

    version('develop', branch='sergiorg_h5bmark', submodules=False)
    version('1.0.0', branch='6627a1', submodules=False)

    depends_on('cmake@3.2:', type='build')
    depends_on('hdf5+mpi')
    depends_on('morphio@sergiorg_h5bmark')
    depends_on('morpho-kit@blargh')
    depends_on('mpi')

    root_cmakelists_dir = 'neuromapp/h5benchmark'

    def cmake_args(self):
        return [
            '-DINTERNAL_PACKAGES:BOOL=OFF',
            '-DCMAKE_CXX_COMPILER={0}'.format(self.spec['mpi'].mpicxx)
        ]
