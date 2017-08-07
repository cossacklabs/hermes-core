/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/



#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <hermes/common/buffer.h>
#include <hermes/common/errors.h>


#define BUFFER_BLOCK_CAPACITY_DEFAULT 1024*10

struct buffer_t_ {
    size_t capacity_;
    size_t length_;
    union {
        uint8_t *data;
        const uint8_t *cdata;
    } data_;
    const uint8_t *currpos_;
    int read_only_;
};

int buffer_realloc_(buffer_t *buffer, const size_t size) {
    if (!buffer) {
        return BUFFER_INVALID_PARAM;
    }
    if (buffer->read_only_) {
        return BUFFER_WRITE_READ_ONLY;
    }
    if (size > (buffer->capacity_ * BUFFER_BLOCK_CAPACITY_DEFAULT)) {
        size_t new_capacity_ = ((size) / BUFFER_BLOCK_CAPACITY_DEFAULT);
        new_capacity_ += (size % BUFFER_BLOCK_CAPACITY_DEFAULT) ? 1 : 0;
        uint8_t *a = realloc(buffer->data_.data, new_capacity_ * BUFFER_BLOCK_CAPACITY_DEFAULT);
        assert(a);
        buffer->data_.data = a;
        buffer->capacity_ = new_capacity_;
    }
    return BUFFER_SUCCESS;
}

buffer_t *buffer_create() {
    buffer_t *buffer = malloc(sizeof *buffer);
    assert(buffer);
    buffer->capacity_ = 1;
    buffer->length_ = 0;
    buffer->data_.data = malloc(buffer->capacity_ * BUFFER_BLOCK_CAPACITY_DEFAULT);
    if (!(buffer->data_.data)) {
        free(buffer);
        return NULL;
    }
    buffer->currpos_ = buffer->data_.cdata;
    buffer->read_only_ = 0;
    return buffer;
}

buffer_t *buffer_create_with_data(const uint8_t *data, const size_t data_len) {
    buffer_t *res = buffer_create();
    assert(res);
    if (BUFFER_SUCCESS != buffer_realloc_(res, data_len)) {
        buffer_destroy(&res);
        return NULL;
    }
    res->currpos_ = res->data_.cdata;
    memcpy(res->data_.data, data, data_len);
    res->length_ = data_len;
    return res;
}

buffer_t *buffer_create_with_(void *unused, ...) {
    buffer_t *res = buffer_create();
    assert(res);
    va_list va;
    va_start(va, unused);
    char *mark = va_arg(va, char*);
    while (mark != NULL) {
        if (HERMES_BUFFER_MAGIC != (int64_t) mark) {
            buffer_destroy(&res);
            va_end(va);
            return NULL;
        }
        int64_t type = va_arg(va, int64_t);
        char *str;
        uint8_t *data;
        size_t data_len;
        switch (type) {
            case HERMES_BUFFER_NODE_TYPE_STR:
                str = va_arg(va, char*);
                if (BUFFER_SUCCESS != buffer_push_string(res, str)) {
                    buffer_destroy(&res);
                    va_end(va);
                    return NULL;
                }
                break;
            case HERMES_BUFFER_NODE_TYPE_BIN:
                data = va_arg(va, uint8_t*);
                data_len = va_arg(va, size_t);
                if (BUFFER_SUCCESS != buffer_push_data(res, data, data_len)) {
                    buffer_destroy(&res);
                    va_end(va);
                    return NULL;
                }
                break;
            default:
                buffer_destroy(&res);
                va_end(va);
                return NULL;
        }
        mark = va_arg(va, char*);
    }
    va_end(va);
    return res;
}

int buffer_extract(buffer_t *buffer, ...) {
    va_list va;
    va_start(va, buffer);
    char *mark = va_arg(va, char*);
    for (; mark != NULL; mark = va_arg(va, char*)) {
        if ((int64_t) mark != HERMES_BUFFER_MAGIC) {
            va_end(va);
            return BUFFER_INVALID_PARAM;
        }
        int type = va_arg(va, int);
        switch (type) {
            case HERMES_BUFFER_NODE_TYPE_STR: {
                const char **str = va_arg(va, const char**);
                if (BUFFER_SUCCESS != buffer_pop_string(buffer, str)) {
                    va_end(va);
                    return BUFFER_INVALID_PARAM;
                }
                break;
            }
            case HERMES_BUFFER_NODE_TYPE_BIN: {
                const uint8_t **data = va_arg(va, const uint8_t**);
                size_t *data_len = va_arg(va, size_t*);
                if (BUFFER_SUCCESS != buffer_pop_data(buffer, data, data_len)) {
                    va_end(va);
                    return BUFFER_INVALID_PARAM;
                }
                break;
            }
            default:
                va_end(va);
                return BUFFER_INVALID_PARAM;
        }
    }
    va_end(va);
    return BUFFER_SUCCESS;
}

int buffer_init_with_data(buffer_t *buffer, const uint8_t *data, const size_t data_len) {
    if (!buffer || !data || !data_len) {
        return BUFFER_INVALID_PARAM;
    }
    buffer->capacity_ = 1;
    buffer->length_ = data_len;
    buffer->data_.cdata = data;
    buffer->currpos_ = buffer->data_.cdata;
    buffer->read_only_ = 1;
    return BUFFER_SUCCESS;
}

int buffer_destroy(buffer_t **buffer) {
    if ((!buffer) || !(*buffer)) {
        return BUFFER_INVALID_PARAM;
    }
    if (!((*buffer)->read_only_) && (*buffer)->data_.data) {
        free((*buffer)->data_.data);
        (*buffer)->data_.data = NULL;
    }
    free(*buffer);
    return BUFFER_SUCCESS;
}

int buffer_push_status(buffer_t *buffer, const int status) {
    if (!buffer || 0 != buffer_realloc_(buffer, buffer->length_ + sizeof(int32_t)+1)) {//int field if buffer get 1(HERMES_BUFFER_NODE_TYPE_INT)+4(value) bytes
        return BUFFER_BAD_ALLOC;
    }
    if (buffer->read_only_) {
        return BUFFER_WRITE_READ_ONLY;
    }
    buffer->data_.data[buffer->length_] = HERMES_BUFFER_NODE_TYPE_INT;
    memcpy((buffer->data_.data) + (buffer->length_) + 1, (uint8_t *) &status, sizeof(int32_t));
    buffer->length_ += 5;
    return BUFFER_SUCCESS;
}

int buffer_push_data(buffer_t *buffer, const uint8_t *data, const size_t data_len) {
    if (!buffer || !data || !data_len || 0 != buffer_realloc_(buffer, buffer->length_ + data_len + sizeof(uint32_t) + 1)) {//buffer field if buffer get 1(HERMES_BUFFER_NODE_TYPE_BIN + 4(size) + data_len(value) bytes
        return BUFFER_BAD_ALLOC;
    }
    if (buffer->read_only_) {
        return BUFFER_WRITE_READ_ONLY;
    }
    buffer->data_.data[buffer->length_] = HERMES_BUFFER_NODE_TYPE_BIN;
    memcpy((buffer->data_.data) + (buffer->length_) + 1, (uint8_t *) &data_len, sizeof(uint32_t));
    memcpy((buffer->data_.data) + (buffer->length_) + 1 + sizeof(uint32_t), data, data_len);
    buffer->length_ += data_len + 1 + sizeof(uint32_t);
    return BUFFER_SUCCESS;
}

int buffer_push_string(buffer_t *buffer, const char *string) {
    if (!buffer || !string || strlen(string) == 0 ||
        0 != buffer_realloc_(buffer, buffer->length_ + strlen(string) + 6)) { //buffer field if buffer get 1(HERMES_BUFFER_NODE_TYPE_STR + 4(size) + (strlen+1)(value) bytes
        return BUFFER_BAD_ALLOC;
    }
    if (buffer->read_only_) {
        return BUFFER_WRITE_READ_ONLY;
    }
    buffer->data_.data[buffer->length_] = HERMES_BUFFER_NODE_TYPE_STR;
    size_t str_len = strlen(string);
    memcpy((buffer->data_.data) + (buffer->length_) + 1, (uint8_t *) &str_len, sizeof(uint32_t));
    memcpy((buffer->data_.data) + (buffer->length_) + 1 + sizeof(uint32_t), string, strlen(string) + 1);
    buffer->length_ += strlen(string) + 2 + sizeof(uint32_t);
    return BUFFER_SUCCESS;
}

int buffer_pop_status(buffer_t *buffer, int *status) {
    if (!buffer || !status) {
        return BUFFER_INVALID_PARAM;
    }
    if (buffer->currpos_ >= (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_AT_END;
    }
    if (buffer->currpos_[0] != HERMES_BUFFER_NODE_TYPE_INT) {
        return BUFFER_INCORRECT_BLOCK_TYPE;
    }
    if ((buffer->currpos_ + 5) > (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_CORRUPTED;
    }
    memcpy(status, buffer->currpos_ + 1, 4);
    buffer->currpos_ += 5;
    return BUFFER_SUCCESS;
}

int buffer_pop_data(buffer_t *buffer, const uint8_t **data, size_t *data_length) {
    if (!buffer || !data || !data_length) {
        return BUFFER_INVALID_PARAM;
    }
    if (buffer->currpos_ >= (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_AT_END;
    }
    if (buffer->currpos_[0] == HERMES_BUFFER_NODE_TYPE_BIN) {
        return BUFFER_INCORRECT_BLOCK_TYPE;
    }
    *data_length = 0;
    memcpy(data_length, buffer->currpos_ + 1, 4);
    if ((buffer->currpos_ + 5 + *(data_length)) > (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_CORRUPTED;
    }
    *data = buffer->currpos_ + 5;
    buffer->currpos_ += *data_length + 5;
    return BUFFER_SUCCESS;
}

int buffer_pop_string(buffer_t *buffer, const char **str) {
    if (!buffer || !str) {
        return BUFFER_INVALID_PARAM;
    }
    if (buffer->currpos_ >= (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_AT_END;
    }
    if (buffer->currpos_[0] == HERMES_BUFFER_NODE_TYPE_STR) {
        return BUFFER_INCORRECT_BLOCK_TYPE;
    }
    size_t str_len = 0;
    memcpy(&str_len, buffer->currpos_ + 1, 4);
    if (strlen((const char *) (buffer->currpos_) + 5) != str_len &&
        (buffer->currpos_ + 6 + str_len) <= (buffer->data_.cdata + buffer->length_)) {
        return BUFFER_CORRUPTED;
    }
    *str = (char *) (buffer->currpos_) + 5;
    buffer->currpos_ += str_len + 6;
    return BUFFER_SUCCESS;
}

size_t buffer_get_size(buffer_t *buffer) {
    if (!buffer) {
        return 0;
    }
    return buffer->length_;
}

const uint8_t *buffer_get_data(buffer_t *buffer) {
    if (!buffer) {
        return NULL;
    }
    return buffer->data_.cdata;
}
