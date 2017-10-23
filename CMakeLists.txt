cmake_minimum_required(VERSION 3.6)
project(hermes_core)

set(CMAKE_CXX_STANDARD 11)

add_executable(hermes_client_example docs/examples/c/mid_hermes/client/hermes_client.c docs/examples/c/mid_hermes/common/transport.c docs/examples/c/utils/utils.c docs/examples/c/utils/base64.c src/secure_transport/utils.c src/secure_transport/utils.h docs/examples/c/mid_hermes/common/credential_store_client.c docs/examples/c/mid_hermes/common/session_callback.c docs/examples/c/mid_hermes/common/session_callback.h)
TARGET_LINK_LIBRARIES(hermes_client_example hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)


FILE(GLOB CREDENTIAL_STORE_SOURCE_FILES docs/examples/c/mid_hermes/credential_store_service/*.c docs/examples/c/mid_hermes/common/transport.c docs/examples/c/utils/*.c)
add_executable(credential_store_server_c ${CREDENTIAL_STORE_SOURCE_FILES})
TARGET_LINK_LIBRARIES(credential_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
TARGET_INCLUDE_DIRECTORIES(credential_store_server_c PRIVATE docs/examples/c/mid_hermes/credential_store_service docs/examples/c/mid_hermes/utils)

FILE(GLOB KEY_STORE_SOURCE_FILES docs/examples/c/mid_hermes/key_store_service/*.c docs/examples/c/mid_hermes/common/*.c docs/examples/c/utils/*.c)
add_executable(key_store_server_c ${KEY_STORE_SOURCE_FILES})
TARGET_LINK_LIBRARIES(key_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
TARGET_INCLUDE_DIRECTORIES(key_store_server_c PRIVATE docs/examples/c/mid_hermes/key_store_service docs/examples/c/mid_hermes/utils)

FILE(GLOB DATA_STORE_SOURCE_FILES docs/examples/c/mid_hermes/data_store_service/*.c docs/examples/c/mid_hermes/common/*.c docs/examples/c/utils/*.c)
add_executable(data_store_server_c ${DATA_STORE_SOURCE_FILES})
TARGET_LINK_LIBRARIES(data_store_server_c hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
TARGET_INCLUDE_DIRECTORIES(data_store_server_c PRIVATE docs/examples/c/mid_hermes/data_store_service docs/examples/c/mid_hermes/utils)