#!/bin/bash
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

@Echo off

mkdir -p ./db/credential_store
mkdir -p ./db/key_store
mkdir -p ./db/data_store

#generate 3 users
for i in {1..3}
do
    ../key_gen/key_pair_gen user$i.priv ./db/credential_store/`echo -ne 'user'$i | base64`
    for j in {1..3}
    do
	#generate random block
	dd bs=1024 count=1024 </dev/urandom >user$i-file$j.data
	#add block to hermes
	./hermes_client_ll add_block user$i `cat user$i.priv | base64` user$i-file$j.data "$j file of user$i"
	if [ $? -ne 0 ]; then
	    echo user$i-file$j.data adding error
	fi
	#read block from hermes
	./hermes_client_ll read_block user$i `cat user$i.priv | base64` user$i-file$j.data
	if [ $? -ne 0 ]; then
	    echo user$i-file$j.data getting error
	fi
    done
done

rm -rf *.priv
rm -rf *.data
rm -rf *.data_block
rm -rf *.data_meta
rm -rf ./db