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

pyChermes = Extension(
    'hermes', sources = ['pyhermes.c', 'transport.c'], include_dirs=['../include'], library_dirs=['../build'],
    libraries=['hermes_mid_hermes', 'hermes_mid_hermes_ll', 'hermes_credential_store', 'hermes_data_store',
               'hermes_key_store', 'hermes_rpc', 'hermes_common', 'themis', 'soter'])

setup(
    name='hermes',
    version='0.5',
    description='python bindings for Hermes',
    ext_modules=[pyChermes])

