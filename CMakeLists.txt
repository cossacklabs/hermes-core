cmake_minimum_required(VERSION 3.6)
project(hermes_core)

set(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE SOURCE_FILES *.h *.c)

add_executable(client docs/examples/c/mid_hermes/client/hermes_client.c docs/examples/c/mid_hermes/common/transport.c docs/examples/c/utils/utils.c docs/examples/c/utils/base64.c src/secure_transport/utils.c src/secure_transport/utils.h)
LINK_LIBRARIES(/usr/lib)
TARGET_LINK_LIBRARIES(client hermes_mid_hermes hermes_mid_hermes_ll hermes_credential_store hermes_data_store hermes_key_store hermes_rpc pthread hermes_common themis soter hermes_secure_transport)
