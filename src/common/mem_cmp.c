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


#include <hermes/common/mem_cmp.h>

// https://github.com/chmike/cst_time_memcmp#fastest-implementation-using-subscipt

int cst_time_memcmp(const void *m1, const void *m2, size_t n) {
    const unsigned char *pm1 = (unsigned char *)m1; 
    const unsigned char *pm2 = (unsigned char *)m2; 
    int res = 0, diff;
    if (n > 0) {
        do {
            --n;
            diff = pm1[n] - pm2[n];
            res = (res & -!diff) | diff;
        } while (n != 0);
    }
    return (res > 0) - (res < 0);
}
