#include "themis/secure_session.h"
#include "themis/secure_session_utils.h"
#include <string.h>
#include <hermes/common/errors.h>
#include "hermes/secure_transport/transport.h"
#include "utils.h"

typedef struct secure_transport_type {
    // transport that will be wrapped
    hm_rpc_transport_t* user_transport;
    // id of host to which is connected
    uint8_t* public_key_id;
    size_t public_key_id_length;
    // public key of host to which is connected
    uint8_t* public_key;
    size_t public_key_length;
    // secure session for this connection
    secure_session_t* session;
    secure_session_user_callbacks_t* session_callbacks;
} secure_transport_t;


// get_public_key_for_id_wrapper return public key linked with this connection if id is equals to id of public key
int get_public_key_for_id_wrapper(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data){
    secure_transport_t* transport = (secure_transport_t*)(user_data);
    if(transport->public_key_id_length == id_length && memcmp(id, transport->public_key_id, id_length) == 0){
        if (key_buffer_length < transport->public_key_length){
            return THEMIS_FAIL;
        }
        memcpy(key_buffer, transport->public_key, transport->public_key_length);
        return THEMIS_SUCCESS;
    }
    return THEMIS_FAIL;
};


// hermes_transport_send encrypt buffer with secure session and send data with <send_data> function
// implement hermes transport interface
uint32_t hermes_transport_send(void *transport_, const uint8_t *buffer, const size_t buffer_length){
    secure_transport_t* transport = (secure_transport_t*)(transport_);
    size_t wrapped_buffer_size;
    uint8_t* wrapped_buffer;
    if (secure_session_wrap(transport->session, buffer, buffer_length, NULL, &wrapped_buffer_size) != THEMIS_BUFFER_TOO_SMALL){
        return HM_FAIL;
    }
    wrapped_buffer = malloc(wrapped_buffer_size);
    if (!wrapped_buffer){
        return HM_FAIL;
    }
    if (secure_session_wrap(transport->session, buffer, buffer_length, wrapped_buffer, &wrapped_buffer_size) != THEMIS_SUCCESS){
        free(wrapped_buffer);
        return HM_FAIL;
    }
    if(send_data(wrapped_buffer, wrapped_buffer_size, transport->user_transport) == HM_FAIL){
        free(wrapped_buffer);
        return HM_FAIL;
    };
    free(wrapped_buffer);
    return HM_SUCCESS;
};

// hermes_transport_receive read data from transport with <read_data_size> and <read_data> functions,
// unwrap with secure_session and return when read size == buffer_length
uint32_t hermes_transport_receive(void *transport_, uint8_t *buffer, size_t buffer_length){
    secure_transport_t* transport = (secure_transport_t*)(transport_);
    size_t total_read = 0;
    do{
        size_t temp_buffer_size;
        uint8_t* temp_buffer;
        temp_buffer_size = read_data_size(transport->user_transport);
        if (temp_buffer_size == HM_FAIL){
            return HM_FAIL;
        }
        temp_buffer = malloc(temp_buffer_size);
        if(!temp_buffer){
            return HM_FAIL;
        }
        if (read_data(temp_buffer, temp_buffer_size, transport->user_transport) == HM_FAIL){
            free(temp_buffer);
            temp_buffer = NULL;
            return HM_FAIL;
        };
        size_t unwrapped_size = 0;
        if (secure_session_unwrap(
                transport->session, temp_buffer, temp_buffer_size, NULL, &unwrapped_size) != THEMIS_BUFFER_TOO_SMALL){
            free(temp_buffer);
            temp_buffer = NULL;
            return HM_FAIL;
        }
        if (unwrapped_size + total_read > buffer_length){
            free(temp_buffer);
            temp_buffer = NULL;
            return HM_FAIL;
        }
        themis_status_t status = secure_session_unwrap(
                transport->session, temp_buffer, temp_buffer_size, buffer+total_read, &unwrapped_size);
        // TODO: maybe rewrite with reusing buffer in loop
        free(temp_buffer);
        temp_buffer = NULL;
        if (status != THEMIS_SUCCESS){
            return HM_FAIL;
        }
        total_read += unwrapped_size;
    }while(total_read < buffer_length);
    return HM_SUCCESS;
};

// init_secure_session establish secure session using transport
hermes_status_t init_secure_session(hm_rpc_transport_t* transport, secure_session_t* session){
    size_t buffer_size;
    // size of tempBuffer took from secure_session code and examples
    uint8_t tempBuffer[2048];
    if (secure_session_generate_connect_request(session, NULL, &buffer_size) != THEMIS_BUFFER_TOO_SMALL){
        return HM_FAIL;
    }
    if (secure_session_generate_connect_request(session, tempBuffer, &buffer_size) != THEMIS_SUCCESS){
        return HM_FAIL;
    }

    if(send_data(tempBuffer, buffer_size, transport) != HM_SUCCESS){
        return HM_FAIL;
    }

    themis_status_t status;
    uint32_t data_size;
    uint32_t bytes_read;
    while(!secure_session_is_established(session)){
        data_size = read_data_size(transport);
        if (data_size == HM_FAIL){
            return HM_FAIL;
        }
        // receive response
        bytes_read = read_data(tempBuffer, data_size, transport);
        if(bytes_read == HM_FAIL){
            return HM_FAIL;
        };
        // calculate unwrapped size
        status = secure_session_unwrap(session, tempBuffer, data_size, tempBuffer, &buffer_size);
        if (status == THEMIS_SSESSION_SEND_OUTPUT_TO_PEER){
            if (send_data(tempBuffer, buffer_size, transport) != HM_SUCCESS){
                return HM_FAIL;
            }
        } else if (status == THEMIS_SUCCESS){
            break;
        } else {
            return HM_FAIL;
        }
    }
    return HM_SUCCESS;
}

// create_secure_transport establish secure session as user with <user_id> using <private_key> with host with id
// <public_key_id> that will be authenticated via <public_key> and return wrapepd <user_transport> with established
// secure session
hm_rpc_transport_t* create_secure_transport(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *private_key, const size_t private_key_length,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *public_key_id, const size_t public_key_id_length,
        hm_rpc_transport_t* user_transport){
    secure_transport_t* transport = calloc(1, sizeof(secure_transport_t));
    transport->user_transport = user_transport;
    transport->public_key = malloc(public_key_length);
    memcpy(transport->public_key, public_key, public_key_length);
    transport->public_key_length = public_key_length;

    transport->public_key_id = malloc(public_key_id_length);
    memcpy(transport->public_key_id, public_key_id, public_key_id_length);
    transport->public_key_id_length = public_key_id_length;

    transport->session_callbacks = malloc(sizeof(secure_session_user_callbacks_t));
    transport->session_callbacks->user_data = transport;
    transport->session_callbacks->get_public_key_for_id = get_public_key_for_id_wrapper;
    transport->session_callbacks->send_data = NULL;
    transport->session_callbacks->receive_data = NULL;
    transport->session_callbacks->state_changed = NULL;


    transport->session = secure_session_create(user_id, user_id_length, private_key, private_key_length, transport->session_callbacks);
    if (!transport->session)
    {
        destroy_secure_transport(&transport);
        return NULL;
    }
    if(init_secure_session(user_transport, transport->session)!=HM_SUCCESS){
        destroy_secure_transport(&transport);
        return NULL;
    }

    hm_rpc_transport_t* rpc_transport = calloc(1, sizeof(hm_rpc_transport_t));
    rpc_transport->user_data = transport;
    rpc_transport->recv = hermes_transport_receive;
    rpc_transport->send = hermes_transport_send;
    return rpc_transport;
}

uint32_t destroy_rpc_secure_transport(hm_rpc_transport_t** transport_){
    if(!transport_ || !(*transport_) || !((*transport_))){
        return HM_FAIL;
    }
    secure_transport_t* transport = (secure_transport_t*) ((*transport_)->user_data);
    destroy_secure_transport(&transport);
}

// destroy_secure_transport free memory allocated for secure transport
uint32_t destroy_secure_transport(secure_transport_t** transport_){
    if(!transport_ || !(*transport_) || !((*transport_))){
        return HM_FAIL;
    }
    secure_transport_t* transport = *transport_;

    free(transport->session_callbacks);
    transport->session_callbacks = NULL;

    secure_session_destroy(transport->session);
    transport->session = NULL;

    free(transport->public_key);
    transport->public_key = NULL;

    free(transport->public_key_id);
    transport->public_key_id = NULL;

    free(transport);
    *transport_ = NULL;

    return HM_SUCCESS;
}