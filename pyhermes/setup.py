#
# Copyright (c) 2017 Cossack Labs Limited
#
# This file is a part of Hermes-core.
#
# Hermes-core is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes-core is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
#
#

from distutils.core import setup, Extension

py_c_hermes = Extension(
    'pyhermes',
    sources=['pyhermes.c', 'transport.c', 'py_secure_transport.c', 'py_midhermes.c', 'py_transport_wrapper.c',
             'py_transport.c'
             ],
    libraries=['hermes_mid_hermes', 'hermes_mid_hermes_ll', 'hermes_credential_store', 'hermes_data_store',
               'hermes_key_store', 'hermes_rpc', 'hermes_common', 'themis', 'soter', 'hermes_secure_transport'])

setup(
    name='pyhermes',
    version='0.5',
    description='python bindings for Hermes',
    ext_modules=[py_c_hermes],

    url='https://cossacklabs.com',
    author='CossackLabs',
    author_email='dev@cossacklabs.com',
    classifiers=[
        "Intended Audience :: Developers",
        "Natural Language :: English",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: POSIX",
        #"Operating System :: POSIX :: BSD",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python",
        "Programming Language :: Python :: 2",
        #"Programming Language :: Python :: 2.6",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.2",
        "Programming Language :: Python :: 3.3",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: Implementation :: CPython",
    ],
)
