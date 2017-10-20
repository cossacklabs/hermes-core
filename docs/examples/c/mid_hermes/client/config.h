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



#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

const uint8_t credential_store_id[]="credential_store";
const uint8_t credential_store_pk[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xd5, 0x3e, 0xf0, 0x5c, 0x03, 0x04, 0x7d, 0xf9, 0x5d, 0x2b, 0x62, 0x3b, 0xd6, 0x90, 0x1c, 0x72, 0x07, 0xad, 0xcc, 0x0c, 0x7e, 0xdf, 0xb7, 0x0b, 0x97, 0xb4, 0x0a, 0x73, 0x10, 0x99, 0x60, 0x0d, 0x0a, 0xc1, 0x56, 0x3d, 0xbc};
const uint8_t credential_store_id[]="data_store";
const uint8_t credential_store_pk[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xbc, 0x32, 0x3a, 0xdc, 0x03, 0xeb, 0x9f, 0x59, 0x13, 0xca, 0x14, 0x74, 0xbc, 0x6e, 0xd8, 0x88, 0xcb, 0x67, 0x66, 0x6f, 0x3c, 0xed, 0x63, 0x1a, 0x84, 0xd7, 0x8a, 0x13, 0x0f, 0x9a, 0x0b, 0x72, 0xcd, 0x2d, 0xf6, 0x9c, 0x59};
const uint8_t credential_store_id[]="key_store";
const uint8_t credential_store_pk[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xd0, 0x86, 0xbe, 0xba, 0x03, 0xc7, 0xc0, 0xce, 0xac, 0xad, 0xe3, 0xcb, 0x7a, 0x37, 0x67, 0x90, 0xb1, 0xb1, 0x67, 0xa3, 0x81, 0xee, 0xfb, 0xff, 0x41, 0x66, 0x9e, 0xd4, 0x7b, 0x06, 0x7e, 0x85, 0xa2, 0xe2, 0x3a, 0x28, 0x6b};

#endif //CONFIG_H