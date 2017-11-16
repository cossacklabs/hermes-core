cmake_minimum_required(VERSION 3.6)
project(hermes_core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wstrict-aliasing -fstrict-aliasing -pedantic -fno-common -Wno-unused-result -fno-common -fwrapv -Wstrict-prototypes")

# use clang as compiler explicitly
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang)

add_executable(hermes_client_example docs/examples/c/mid_hermes/client/hermes_client.c docs/examples/c/mid_hermes/common/transport.c docs/examples/c/utils/utils.c docs/examples/c/utils/base64.c src/secure_transport/utils.c src/secure_transport/utils.h docs/examples/c/mid_hermes/common/credential_store_client.c src/secure_transport/session_callback.c include/hermes/secure_transport/session_callback.h)
TARGET_LINK_LIBRARIES(hermes_client_example hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)


FILE(GLOB CREDENTIAL_STORE_SOURCE_FILES docs/examples/c/mid_hermes/credential_store_service/*.c docs/examples/c/mid_hermes/common/transport.c docs/examples/c/utils/*.c docs/examples/c/mid_hermes/common/*.c)
add_executable(credential_store_server_c ${CREDENTIAL_STORE_SOURCE_FILES} )
TARGET_INCLUDE_DIRECTORIES(credential_store_server_c PRIVATE docs/examples/c/mid_hermes/credential_store_service docs/examples/c/mid_hermes/utils docs/examples/c/mid_hermes/common)
TARGET_LINK_LIBRARIES(credential_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)

FILE(GLOB KEY_STORE_SOURCE_FILES docs/examples/c/mid_hermes/key_store_service/*.c docs/examples/c/mid_hermes/common/*.c docs/examples/c/utils/*.c)
add_executable(key_store_server_c ${KEY_STORE_SOURCE_FILES})
TARGET_LINK_LIBRARIES(key_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
TARGET_INCLUDE_DIRECTORIES(key_store_server_c PRIVATE docs/examples/c/mid_hermes/key_store_service docs/examples/c/mid_hermes/utils)

FILE(GLOB DATA_STORE_SOURCE_FILES docs/examples/c/mid_hermes/data_store_service/*.c docs/examples/c/mid_hermes/common/*.c docs/examples/c/utils/*.c)
add_executable(data_store_server_c ${DATA_STORE_SOURCE_FILES})
TARGET_LINK_LIBRARIES(data_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
TARGET_INCLUDE_DIRECTORIES(data_store_server_c PRIVATE docs/examples/c/mid_hermes/data_store_service docs/examples/c/mid_hermes/utils)

FILE(GLOB MID_HERMES_TEST_SOURCE_FILES tests/mid_hermes/*c tests/common/*.c)
add_executable(mid_hermes_test ${MID_HERMES_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(mid_hermes_test PRIVATE tests)
TARGET_LINK_LIBRARIES(mid_hermes_test hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)

FILE(GLOB CREDENTIAL_STORE_TEST_SOURCE_FILES tests/credential_store/*c tests/common/*.c)
add_executable(credential_store_test ${CREDENTIAL_STORE_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(credential_store_test PRIVATE tests)
TARGET_LINK_LIBRARIES(credential_store_test hermes_credential_store pthread hermes_common themis soter)

FILE(GLOB DATA_STORE_TEST_SOURCE_FILES tests/data_store/*c tests/common/*.c)
add_executable(data_store_test ${DATA_STORE_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(data_store_test PRIVATE tests)
TARGET_LINK_LIBRARIES(data_store_test hermes_data_store pthread hermes_common themis soter)

FILE(GLOB KEY_STORE_TEST_SOURCE_FILES tests/key_store/*c tests/common/*.c)
add_executable(key_store_test ${KEY_STORE_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(key_store_test PRIVATE tests)
TARGET_LINK_LIBRARIES(key_store_test hermes_key_store pthread hermes_common themis soter)

FILE(GLOB MID_HERMES_LL_TEST_SOURCE_FILES tests/mid_hermes_ll/*c tests/common/*.c)
add_executable(mid_hermes_ll_test ${MID_HERMES_LL_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(mid_hermes_ll_test PRIVATE tests)
TARGET_LINK_LIBRARIES(mid_hermes_ll_test hermes_mid_hermes_ll pthread hermes_common themis soter)

FILE(GLOB RPC_TEST_SOURCE_FILES tests/rpc/*c tests/common/*.c)
add_executable(rpc_test ${RPC_TEST_SOURCE_FILES})
TARGET_INCLUDE_DIRECTORIES(rpc_test PRIVATE tests)
TARGET_LINK_LIBRARIES(rpc_test hermes_common pthread hermes_rpc hermes_mid_hermes_ll themis soter)

FILE(GLOB MIDHERMES_LL_SOURCES docs/examples/c/mid_hermes_low_level/*.c docs/examples/c/utils/*.c)
add_executable(midhermes_ll ${MIDHERMES_LL_SOURCES})
TARGET_LINK_LIBRARIES(midhermes_ll hermes_mid_hermes_ll themis soter)