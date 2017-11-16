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


#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <themis/themis_error.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

#define UNUSED(expr) (void)(expr)

typedef int test_utils_status_t;

test_utils_status_t string_to_bytes(const char *str, uint8_t *bytes, size_t size);
size_t rand_int(size_t max_val);

/* These are wrappers for sput test framework. Since sput uses a static state variable, when called from different files, it resets test number count (since for each file different test context is used). */
void testsuite_start_testing(void);
void testsuite_enter_suite(const char *suite_name);
void testsuite_run_test_in_file(void (*test_func)(void), const char *test_func_name, const char *test_file_name);
void testsuite_finish_testing(void);
void testsuite_fail_if_on_line(bool condition, const char *name, unsigned long line);
void testsuite_fail_unless_on_line(bool condition, const char *name, unsigned long line);
int testsuite_get_return_value(void);

#define testsuite_fail_if(_COND_, _NAME_) testsuite_fail_if_on_line(_COND_, _NAME_, __LINE__)
#define testsuite_fail_unless(_COND_, _NAME_) testsuite_fail_unless_on_line(_COND_, _NAME_, __LINE__)
#define testsuite_run_test(_FUNC_) testsuite_run_test_in_file(_FUNC_, #_FUNC_, __FILE__)

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_H */
