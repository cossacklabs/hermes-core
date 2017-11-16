#!/usr/bin/env bash
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

TEST_FILE=/tmp/testfile
echo "some content" > $TEST_FILE
USER_ID=user1
PRIVATE_KEY1=/tmp/user1.priv
PRIVATE_KEY_BASE64=UkVDMgAAAC0Tj5tGAPfpgfYMBACxX6onvlWvcc2Gb9ZylBlJdjebTpV3OCIx
echo -n $PRIVATE_KEY_BASE64 | base64 -d > $PRIVATE_KEY1

USER_ID2=user2
PRIVATE_KEY2=/tmp/user2.priv
PRIVATE_KEY2_BASE64=UkVDMgAAAC00lzw7ABmvKHvjOqWW8i+dxwHTU8RzuaATkZNBcLmCm8TBxRn2
echo -n $PRIVATE_KEY2_BASE64 | base64 -d > $PRIVATE_KEY2
CONFIG=docs/examples/python/config.json
BIN=docs/examples/python/hermes_client.py

SUCCESS=0
FAIL=1

# add block
echo "add block"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --meta "metadata" --config=$CONFIG --add
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# read block
echo "read block"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --read
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# update block
echo "update block"
echo "some new content" > $TEST_FILE
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --meta "new metadata" --config=$CONFIG --update
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# rotate block
echo "rotate block"
md5sum db/data_store/`echo -n ${TEST_FILE}|base64`/data > ${TEST_FILE}.1.md5
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --rotate
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

md5sum db/data_store/`echo -n ${TEST_FILE}|base64`/data > ${TEST_FILE}.2.md5

diff ${TEST_FILE}.1.md5 ${TEST_FILE}.2.md5 > /dev/null
diff_result=$?
rm ${TEST_FILE}.1.md5 ${TEST_FILE}.2.md5

# check that md5 of data before rotate != md5 of data after rotate
# result == 1 if not equal
if [ ! $diff_result -eq 1 ]; then
    echo "md5 hash sum not different"
    exit 1
fi

# read block
echo "read block"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --read
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --config=$CONFIG --read
if [ $? -eq $SUCCESS ]; then
    exit 1
fi


# grant access
echo "grant read access"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --grant_read --for_user=$USER_ID2
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --config=$CONFIG --read
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --meta "new metadata" --config=$CONFIG --update
if [ $? -eq $SUCCESS ]; then
    exit 1
fi


echo "grant update"
# grant update to user 2
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --grant_update --for_user=$USER_ID2
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --meta "new metadata 2" --config=$CONFIG --update
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi


# revoke update from user 2
echo "revoke update"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --revoke_update --for_user=$USER_ID2
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --meta "new metadata 2" --config=$CONFIG --update
if [ $? -eq $SUCCESS ]; then
    exit 1
fi

# revoke read from user 2
echo "revoke read"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --revoke_read --for_user=$USER_ID2
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
python $BIN --id $USER_ID2 --private_key $PRIVATE_KEY2 --doc $TEST_FILE --config=$CONFIG --read
if [ $? -eq $SUCCESS ]; then
    exit 1
fi

# delete block
echo "delete block"
python $BIN --id $USER_ID --private_key $PRIVATE_KEY1 --doc $TEST_FILE --config=$CONFIG --delete
if [ ! $? -eq $SUCCESS ]; then
    exit 1
fi
