#! /bin/sh
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

# before running these script run credential/key/data stores from docs/examples/c/mid_hermes/*
# and update paths in this script to yours

TEST_FILE=/tmp/testfile
echo "some content" > $TEST_FILE
USER_ID=user1
# keypair of user 1
PRIVATE_KEY=UkVDMgAAAC0Tj5tGAPfpgfYMBACxX6onvlWvcc2Gb9ZylBlJdjebTpV3OCIx
PUBLIC_KEY=VUVDMgAAAC2ISqIZApjrN6BCVmWoJdbhjN2EmBAfLq3VKEdRnz0gVdYcIDQp

USER_ID2=user2
# keypair of user 2
PRIVATE_KEY2=UkVDMgAAAC00lzw7ABmvKHvjOqWW8i+dxwHTU8RzuaATkZNBcLmCm8TBxRn2
PUBLIC_KEY2=VUVDMgAAAC0lAFeZAqw+nKzco1l2qtXELqVr7fmpsMf4hSrOa/TkAu5OQ6Cy

# path to dir where are compiled client binary
BIN=cmake-build-debug
SUCCESS=0
FAIL=1
DB_DIR=db/credential_store
KEYSTORE_PUBLIC=VUVDMgAAAC3QMLOAAoms9u5nTh1Ir3AnTPt5RkMJY9leIfF6uMIxms/Bkywp
DATASTORE_PUBLIC=VUVDMgAAAC0VCQ/fAt88d2N8vDFVAKbDJHsXew8HgB55PIrVfhELXrEf1N89

echo -n "$PUBLIC_KEY" | base64 --decode > $DB_DIR/`echo -n $USER_ID|base64`
echo -n "$PUBLIC_KEY2" | base64 --decode > $DB_DIR/`echo -n $USER_ID2|base64`
echo -n $KEYSTORE_PUBLIC | base64 --decode > $DB_DIR/`echo -n "key_store_server" | base64`
echo -n $DATASTORE_PUBLIC | base64 --decode > $DB_DIR/`echo -n "data_store_server" | base64`

# add block
echo "add block"
$BIN/client add_block $USER_ID $PRIVATE_KEY $TEST_FILE "metadata" 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# read block
echo "read block"
$BIN/client read_block $USER_ID $PRIVATE_KEY $TEST_FILE 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# update block
echo "update block"
echo "some new content" > $TEST_FILE
$BIN/client update_block $USER_ID $PRIVATE_KEY $TEST_FILE "new metadata" 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
$BIN/client read_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE 1>/dev/null
if [ $? -eq $SUCCESS ]; then
    exit 1
fi


# grant access
echo "grant read access"
$BIN/client grant_read $USER_ID $PRIVATE_KEY $TEST_FILE $USER_ID2 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
$BIN/client read_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
$BIN/client update_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE "new metadata" 1>/dev/null
if [ $? -eq $SUCCESS ]; then
    exit 1
fi


echo "grant update"
# grant update to user 2
$BIN/client grant_update $USER_ID $PRIVATE_KEY $TEST_FILE $USER_ID2 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
$BIN/client update_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE "new metadata" 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi


# revoke update from user 2
echo "revoke update"
$BIN/client revoke_update $USER_ID $PRIVATE_KEY $TEST_FILE $USER_ID2 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# update block
echo "update block by user 2"
echo "some new content by user2" > $TEST_FILE
$BIN/client update_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE "new metadata" 1>/dev/null
if [ $? -eq $SUCCESS ]; then
    exit 1
fi

# revoke read from user 2
echo "revoke read"
$BIN/client revoke_read $USER_ID $PRIVATE_KEY $TEST_FILE $USER_ID2 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi

# read by user 2
echo "read by user 2"
$BIN/client read_block $USER_ID2 $PRIVATE_KEY2 $TEST_FILE 1>/dev/null
if [ $? -eq $SUCCESS ]; then
    exit 1
fi

# delete block
echo "delete block"
$BIN/client delete_block $USER_ID $PRIVATE_KEY $TEST_FILE 1>/dev/null
if [ $? -eq $FAIL ]; then
    exit 1
fi