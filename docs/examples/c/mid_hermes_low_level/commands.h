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



#ifndef COMMANDS_H
#define COMMANDS_H

int upd_block(const char* user_id, const char* user_sk, const char* block_file_name, const char* block_meta_data);
int add_block(const char* user_id, const char* user_sk, const char* block_file_name, const char* block_meta_data);
int get_block(const char* user_id, const char* user_sk, const char* block_file_name);
int del_block(const char* user_id, const char* user_sk, const char* block_file_name);
int grant_read(const char* user_id, const char* user_sk, const char* block_file_name, const char* for_user_id);
int grant_update(const char* user_id, const char* user_sk, const char* block_file_name, const char* for_user_id);
int deny_read(const char* user_id, const char* user_sk, const char* block_file_name, const char* for_user_id);
int deny_update(const char* user_id, const char* user_sk, const char* block_file_name, const char* for_user_id);
int rotate_block(const char* user_id, const char* user_sk, const char* block_file_name);



#endif //COMMANDS_H
