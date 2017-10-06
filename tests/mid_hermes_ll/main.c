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

#include <common/test_utils.h>
#include <common/common.h>
#include "block_tests.h"

void mid_hermes_ll_tests(){
    testsuite_fail_if(mid_hermes_ll_block_tests(), "mid hermes ll block");
}

int main(int argc, char *argv[]){
    fprintf(stderr, "here");
    testsuite_start_testing();
    testsuite_enter_suite("mid hermes ll test");

    testsuite_run_test(mid_hermes_ll_tests);

    testsuite_finish_testing();
    return testsuite_get_return_value();
}
