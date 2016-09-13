/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef PROTOCOLS_STUBS_GENERATOR_H_
#define PROTOCOLS_STUBS_GENERATOR_H_

typedef struct{
  void* handler_pointer_;
  void* call_cxt_;
} call_handler_ctx_t;

typedef void (*hermes__service_call_handler_t)(void* ctx, const uint8_t* res_buf, const size_t res_buf_length);

#define HERMES_FUNCTION_DEFINITIONS(object_ctx_type, name)			\
  int name(void* ctx, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length); \
  protocol_status_t object_ctx_type##_##name##_proxy(object_ctx_type##_t* ctx, const uint8_t* param_buf, const size_t param_buf_length, hermes__service_call_handler_t handler, void* call_ctx); \
  protocol_status_t object_ctx_type##_##name##_sync_proxy(object_ctx_type##_t* ctx, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res, size_t* res_length); \
  functions_collection_status_t object_ctx_type##_##name##_stub(const char* id, const uint8_t* read_key_id_buf, const size_t read_key_id_buf_len, uint8_t** read_key_buf, size_t* read_key_buf_len, void* ctx)


#define HERMES_STUB_FUNC(object_ctx_type, name)				\
  functions_collection_status_t object_ctx_type##_##name##_stub(const char* id, const uint8_t* param_buf, const size_t param_buf_len, uint8_t** result_buf, size_t* result_buf_len, void* ctx){ \
    HERMES_CHECK(param_buf && param_buf_len !=0, return FUNCTIONS_COLLECTION_INVALID_PARAM); \
    HERMES_CHECK(!object_ctx_type##_##name(ctx, id, param_buf, param_buf_len, result_buf, result_buf_len), return FUNCTIONS_COLLECTION_FAIL); \
    return FUNCTIONS_COLLECTION_SUCCESS;				\
  }

#define HERMES_PROXY_FUNC(object_ctx_type, name)					\
  protocol_status_t object_ctx_type##_##name##_proxy(object_ctx_type##_t* ctx, const uint8_t* param_buf, const size_t param_buf_length, hermes__service_call_handler_t handler, void* call_ctx){ \
    HERMES_CHECK(ctx && param_buf && param_buf_length >0, return PROTOCOL_INVALID_PARAM);						\
    HERMES_CHECK(PROTOCOL_SUCCESS==object_ctx_type##_call(ctx, #name, param_buf, param_buf_length, handler, call_ctx), handler(call_ctx, NULL, 0);return PROTOCOL_FAIL); \
    return PROTOCOL_SUCCESS;				\
  }\
  protocol_status_t object_ctx_type##_##name##_sync_proxy(object_ctx_type##_t* ctx, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res, size_t* res_length){ \
    HERMES_CHECK(ctx && param_buf && param_buf_length >0, return PROTOCOL_INVALID_PARAM);						\
    HERMES_CHECK(PROTOCOL_SUCCESS==object_ctx_type##_sync_call(ctx, #name, param_buf, param_buf_length, res, res_length), return PROTOCOL_FAIL); \
    return PROTOCOL_SUCCESS;				\
  }

#define HERMES_FUNC(object_ctx_type, name) \
  HERMES_STUB_FUNC(object_ctx_type, name) \
  HERMES_PROXY_FUNC(object_ctx_type, name)															     


#endif /* PROTOCOLS_STUBS_GENERATOR_H_ */
