#!/bin/bash

@Echo off

mkdir -p ./db/credential_store
mkdir -p ./db/key_store
mkdir -p ./db/data_store

#generate 3 users
for i in {1..3}
do
    ./key_pair_gen user$i.priv ./db/credential_store/`echo -ne 'user'$i'\0' | base64`
    for j in {1..3}
    do
	#generate random block
	dd bs=1024 count=1024 </dev/urandom >user$i-file$j.data
	#add block to hermes
	./hermes_client_ll ba user$i `cat user$i.priv | base64` user$i-file$j.data "$j file of user$i"
	if [ $? -ne 0 ]; then
	    echo user$i-file$j.data adding error
	fi
	#read block from hermes
	./hermes_client_ll br user$i `cat user$i.priv | base64` user$i-file$j.data
	if [ $? -ne 0 ]; then
	    echo user$i-file$j.data getting error
	fi
    done
done

#generate 3 files for each user
#dd bs=1024 count=1024 </dev/urandom >user1_file1

#add 2 files for user1

#./hermes_client_ll ba user1 `echo user1.priv | base64` data_store.c "1-st file of user1"
#./hermes_client_ll ba user1 `echo user1.priv | base64` key_store.c "2-st file of user1"

rm -rf *.priv
rm -rf *.data
rm -rf *.data_block
rm -rf *.data_meta
rm -rf ./db