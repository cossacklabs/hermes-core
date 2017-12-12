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



#include <hermes/common/hash_table.h>
#include <hermes/common/errors.h>

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define HM_HASH_TABLE_CAP 1024  //2^10

typedef struct hm_hash_table_entry_type hm_hash_table_entry_t;

struct hm_hash_table_entry_type {
    uint8_t *key;
    size_t key_length;
    uint8_t *val;
    size_t val_length;
    hm_hash_table_entry_t *next;
};


hm_hash_table_entry_t *hm_hash_table_entry_create(
        const uint8_t *key, const size_t key_length, const uint8_t *val, const size_t val_length) {
    hm_hash_table_entry_t *entry = calloc(sizeof(hm_hash_table_entry_t), 1);
    assert(entry);
    entry->key = malloc(key_length);
    assert(entry->key);
    memcpy(entry->key, key, key_length);
    entry->key_length = key_length;
    entry->val = malloc(val_length);
    assert(entry->val);
    memcpy(entry->val, val, val_length);
    entry->val_length = val_length;
    return entry;
}

uint32_t hm_hash_table_entry_destroy(hm_hash_table_entry_t *entry) {
    if (!entry) {
        return HM_INVALID_PARAMETER;
    }
    if (entry->next) {
        hm_hash_table_entry_destroy(entry->next);
    }
    free(entry->key);
    free(entry->val);
    free(entry);
    return HM_SUCCESS;
}

int hm_hash_(const uint8_t *key, const size_t key_length) {
    //djb2 function.
    unsigned long hash = 5381;
    size_t i = 0;
    for (; i < key_length; ++i) {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash % HM_HASH_TABLE_CAP;
}

struct hm_hash_table_type {
    hm_hash_table_entry_t *t[HM_HASH_TABLE_CAP];
};

hm_hash_table_t *hm_hash_table_create(void) {
    hm_hash_table_t *table = calloc(sizeof(hm_hash_table_t), 1);
    assert(table);
    return table;
}

uint32_t hm_hash_table_destroy(hm_hash_table_t **table) {
    if (!table || !(*table)) {
        return HM_INVALID_PARAMETER;
    }
    uint32_t i = 0;
    for (; i < HM_HASH_TABLE_CAP; ++i) {
        hm_hash_table_entry_destroy((*table)->t[i]);
    }
    free(*table);
    *table = NULL;
    return HM_SUCCESS;
}

uint32_t hm_hash_table_set(
        hm_hash_table_t *table, const uint8_t *key, const size_t key_length,
        const uint8_t *val, const size_t val_length) {
    if (!table || !key || !key_length || !val || !val_length) {
        return HM_INVALID_PARAMETER;
    }
    int hash = hm_hash_(key, key_length);
    hm_hash_table_entry_t *last = NULL, *next = table->t[hash];
    while (next && next->key && memcmp(key, next->key, key_length) > 0) {
        last = next;
        next = next->next;
    }

    if (next && next->key && 0 == memcmp(key, next->key, key_length)) {
        return HM_FAIL; //key already exist
    } else {
        hm_hash_table_entry_t *entry = hm_hash_table_entry_create(key, key_length, val, val_length);
        if (next == table->t[hash]) {
            entry->next = next;
            table->t[hash] = entry;
        } else if (next == NULL) {
            last->next = entry;
        } else {
            entry->next = next;
            last->next = entry;
        }
    }
    return HM_SUCCESS;
}

uint32_t hm_hash_table_get(
        hm_hash_table_t *table, const uint8_t *key, const size_t key_length, uint8_t **val, size_t *val_length) {
    if (!table || !key || !key_length || !val || !val_length) {
        return HM_INVALID_PARAMETER;
    }
    int hash = hm_hash_(key, key_length);
    hm_hash_table_entry_t *entry = table->t[hash];
    while (entry && entry->key && memcmp(key, entry->key, key_length) > 0) {
        entry = entry->next;
    }
    if (!entry || !(entry->key) || 0 != memcmp(key, entry->key, key_length)) {
        return HM_FAIL;
    }
    *val = entry->val;
    *val_length = entry->val_length;
    return HM_SUCCESS;
}

