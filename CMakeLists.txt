cmake_minimum_required(VERSION 3.8)
project(hermes_core_fork)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS -L$(HOME)/projects/themis-test/mainfork/build)
set(CMAKE_EXE_LINKER_FLAGS -lthemis)
set(CMAKE_EXE_LINKER_FLAGS -lsoter)
#
#set(CMAKE_PREFIX_PATH $(HOME)/projects/themis-test/mainfork/build)
#find_package(themis)

#set(CMAKE_MODULE_PATH $(HOME)/projects/themis-test/mainfork/build)
#
#find_package(themis $(HOME)/projects/themis-test/mainfork/build/libthemis.dylib)

find_library(themis PATHS $(HOME)/projects/themis-test/mainfork/build)

#link_directories($(HOME)/projects/themis-test/mainfork/build)

link_directories($(HOME)/projects/themis-test/mainfork/build)

set(SOURCE_FILES
        include/hermes/common/buffer.h
        include/hermes/common/errors.h
        include/hermes/common/hash_table.h
        include/hermes/common/utils.h
        include/hermes/credential_store/client.h
        include/hermes/credential_store/db.h
        include/hermes/credential_store/server.h
        include/hermes/credential_store/service.h
        include/hermes/data_store/client.h
        include/hermes/data_store/db.h
        include/hermes/data_store/server.h
        include/hermes/data_store/service.h
        include/hermes/key_store/client.h
        include/hermes/key_store/db.h
        include/hermes/key_store/server.h
        include/hermes/key_store/service.h
        include/hermes/mid_hermes/mid_hermes.h
        include/hermes/mid_hermes_ll/interfaces/credential_store.h
        include/hermes/mid_hermes_ll/interfaces/data_store.h
        include/hermes/mid_hermes_ll/interfaces/key_store.h
        include/hermes/mid_hermes_ll/mid_hermes_ll.h
        include/hermes/mid_hermes_ll/mid_hermes_ll_block.h
        include/hermes/mid_hermes_ll/mid_hermes_ll_buffer.h
        include/hermes/mid_hermes_ll/mid_hermes_ll_rights_list.h
        include/hermes/mid_hermes_ll/mid_hermes_ll_token.h
        include/hermes/mid_hermes_ll/mid_hermes_ll_user.h
        include/hermes/rpc/buffers_list.h
        include/hermes/rpc/client.h
        include/hermes/rpc/param_pack.h
        include/hermes/rpc/server.h
        include/hermes/rpc/transport.h
        src/common/buffer.c
        src/common/hm_hash_table.c
        src/credential_store/client.c
        src/credential_store/functions.c
        src/credential_store/functions.h
        src/credential_store/server.c
        src/credential_store/service.c
        src/data_store/client.c
        src/data_store/functions.c
        src/data_store/functions.h
        src/data_store/server.c
        src/data_store/service.c
        src/key_store/client.c
        src/key_store/functions.c
        src/key_store/functions.h
        src/key_store/server.c
        src/key_store/service.c
        src/mid_hermes/credential_store_impl.c
        src/mid_hermes/data_store_impl.c
        src/mid_hermes/key_store_impl.c
        src/mid_hermes/mid_hermes.c
        src/mid_hermes_ll/mid_hermes_ll_block.c
        src/mid_hermes_ll/mid_hermes_ll_buffer.c
        src/mid_hermes_ll/mid_hermes_ll_rights_list.c
        src/mid_hermes_ll/mid_hermes_ll_token.c
        src/mid_hermes_ll/mid_hermes_ll_user.c
        src/mid_hermes_ll/utils.c
        src/mid_hermes_ll/utils.h
        src/rpc/buffers_list.c
        src/rpc/client.c
        src/rpc/param_pack.c
        src/rpc/server.c
        tests/common/common.c
        tests/common/common.h
        tests/common/sput.h
        tests/common/test_credential_store_db.c
        tests/common/test_credential_store_db.h
        tests/common/test_data_store_db.c
        tests/common/test_data_store_db.h
        tests/common/test_key_store_db.c
        tests/common/test_key_store_db.h
        tests/common/test_transport.c
        tests/common/test_transport.h
        tests/common/test_utils.c
        tests/common/test_utils.h
        tests/credential_store/credential_store_tests.c
        tests/data_store/data_store_tests.c
        tests/key_store/key_store_tests.c
        tests/mid_hermes/mid_hermes.c
        tests/rpc/param_pack_test.c
        tests/rpc/rpc_test.c
        tests/rpc/server_test.c
        CMakeLists.txt
        Makefile)

include_directories(include)
include_directories(include/hermes)

add_library(hermes_core_fork SHARED ${SOURCE_FILES})
target_link_libraries(hermes_core_fork libthemis.a)