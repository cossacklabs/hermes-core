#
# Copyright (c) 2017 Cossack Labs Limited
#
# This file is a part of Hermes-core.
#
# Hermes-core is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes-core is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
#
#

#CC = clang
INCLUDE_PATH = include
SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

CFLAGS += -I$(INCLUDE_PATH) -fPIC
LDFLAGS += -Lbuild -Llibs/themis/build

ifeq ($(PREFIX),)
PREFIX = /usr
endif

SHARED_EXT = so

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

CFLAGS += -Werror -Wno-switch

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

AWK_CMD = awk '{ printf "%-30s %-10s\n",$$1, $$2; }'
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_OK_ = printf "$(OK_STRING)\n" | $(AWK_CMD)
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;

ifndef ERROR
include src/common/common.mk
include src/rpc/rpc.mk
include src/credential_store/credential_store.mk
include src/data_store/data_store.mk
include src/key_store/key_store.mk
include src/mid_hermes_ll/mid_hermes_ll.mk
include src/mid_hermes/mid_hermes.mk
endif


all: err core

test_all: err test

core: rpc_shared credential_store_shared data_store_shared key_store_shared mid_hermes_shared

static_core: rpc_static credential_store_static data_store_static key_store_static mid_hermes_static

common_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(COMMON_BIN).a $(COMMON_OBJ)

common_static: $(COMMON_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


rpc_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(RPC_BIN).a $(RPC_OBJ)

rpc_static: common_static $(RPC_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

rpc_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(RPC_BIN).$(SHARED_EXT) $(RPC_OBJ) $(LDFLAGS) -l$(COMMON_BIN)

rpc_shared: common_static $(RPC_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(CREDENTIAL_STORE_BIN).a $(CREDENTIAL_STORE_OBJ)

credential_store_static: common_static rpc_static $(CREDENTIAL_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(CREDENTIAL_STORE_BIN).$(SHARED_EXT) $(CREDENTIAL_STORE_OBJ) $(LDFLAGS) -l$(COMMON_BIN) -l$(RPC_BIN)

credential_store_shared: common_static rpc_shared $(CREDENTIAL_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

data_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(DATA_STORE_BIN).a $(DATA_STORE_OBJ)

data_store_static: common_static rpc_static $(DATA_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

data_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(DATA_STORE_BIN).$(SHARED_EXT) $(DATA_STORE_OBJ) $(LDFLAGS) -l$(COMMON_BIN) -l$(RPC_BIN)

data_store_shared: common_static rpc_shared $(DATA_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

key_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(KEY_STORE_BIN).a $(KEY_STORE_OBJ)

key_store_static: common_static rpc_static $(KEY_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

key_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(KEY_STORE_BIN).$(SHARED_EXT) $(KEY_STORE_OBJ) $(LDFLAGS) -l$(COMMON_BIN) -l$(RPC_BIN)

key_store_shared: common_static rpc_shared $(KEY_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(MID_HERMES_BIN).a $(MID_HERMES_OBJ)

mid_hermes_static: mid_hermes_ll_static rpc_static $(MID_HERMES_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(MID_HERMES_BIN).$(SHARED_EXT) $(MID_HERMES_OBJ) $(LDFLAGS) -l$(MID_HERMES_LL_BIN) -l$(RPC_BIN)

mid_hermes_shared: mid_hermes_ll_shared rpc_shared $(MID_HERMES_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_ll_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(MID_HERMES_LL_BIN).a $(MID_HERMES_LL_OBJ)

mid_hermes_ll_static: common_static $(MID_HERMES_LL_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_ll_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(MID_HERMES_LL_BIN).$(SHARED_EXT) $(MID_HERMES_LL_OBJ) $(LDFLAGS) -l$(COMMON_BIN)

mid_hermes_ll_shared: common_static $(MID_HERMES_LL_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


$(OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

include tests/test.mk

err: ; $(ERROR)

clean: CMD = rm -rf $(BIN_PATH)

clean:
	@$(BUILD_CMD)

make_install_dirs: CMD = mkdir -p $(PREFIX)/include/hermes $(PREFIX)/lib

make_install_dirs:
	@echo -n "making dirs for install "
	@$(BUILD_CMD_)

install_headers: CMD = cp -r include/hermes $(PREFIX)/include

install_headers: err all make_install_dirs
	@echo -n "install soter headers "
	@$(BUILD_CMD_)

install_static_libs: CMD = install $(BIN_PATH)/*.a $(PREFIX)/lib

install_static_libs: err all make_install_dirs
	@echo -n "install static libraries "
	@$(BUILD_CMD_)

install_shared_libs: CMD = install $(BIN_PATH)/*.$(SHARED_EXT) $(PREFIX)/lib

install_shared_libs: err all make_install_dirs
	@echo -n "install shared libraries "
	@$(BUILD_CMD_)

install: install_headers install_static_libs install_shared_libs

ll_example: CMD = make docs/examples/c/mid_hermes_low_level/Makefile

ll_example: static_core
	@echo -n "make midHermes lowLevel example"
	@$(BUILD_CMD_)


examples: static_core ll_example hermes_example
	

uninstall: CMD = rm -rf $(PREFIX)/include/hermes && rm -f $(PREFIX)/lib/libhermes*.a && rm -f $(PREFIX)/lib/libhermes*.so

uninstall:
	@echo -n "hermes uninstall "
	@$(BUILD_CMD_)

PACKAGE_NAME = hermes
# check that it's git repo
GIT_VERSION := $(shell if [ -d ".git" ]; then git version; fi 2>/dev/null)
# check that repo has any tag
GIT_TAG_STATUS := $(shell git describe --tags HEAD 2>/dev/null)
GIT_TAG_STATUS := $(.SHELLSTATUS)

ifdef GIT_VERSION
# if has tag then use it
        ifeq ($(GIT_TAG_STATUS),0)
                VERSION = $(shell git describe --tags HEAD | cut -b 1-)
        else
# otherwise use last commit hash
                VERSION = $(shell git describe --always HEAD)
        endif
else
# if it's not git repo then use date as version
        VERSION = $(shell date -I | sed s/-/_/g)
endif


get_version:
	@echo $(VERSION)

DIST_FILENAME = $(VERSION).tar.gz

dist:
	mkdir -p $(VERSION)
	rsync -avz src $(VERSION)
	rsync -avz docs $(VERSION)
	rsync -avz tests $(VERSION)
	rsync -avz LICENSE.md $(VERSION)
	rsync -avz Makefile $(VERSION)
	rsync -avz README.md $(VERSION)
	rsync -avz include $(VERSION)
	rsync -avz pyhermes $(VERSION)
	tar -zcvf $(DIST_FILENAME) $(VERSION)
	rm -rf $(VERSION)

unpack_dist:
	@tar -xf $(DIST_FILENAME)

collect_headers:
	@mkdir -p $(HEADERS_FOLDER) $(HEADERS_FOLDER)/pyhermes
	@cp -r include/hermes src/credential_store src/data_store src/key_store src/mid_hermes src/mid_hermes_ll pyhermes $(HEADERS_FOLDER)/
# delete non-header files
	@find build/include ! -name *.h -type f -exec rm {} \;
# delete empty folders that may copied before
	@find build/include ! -name *.h -type d | sort -r | xargs rmdir --ignore-fail-on-non-empty


COSSACKLABS_URL = https://www.cossacklabs.com
MAINTAINER = "Cossack Labs Limited <dev@cossacklabs.com>"
LICENSE_NAME = "Apache License Version 2.0"
SHORT_DESCRIPTION = Hermes is a cryptography-based method of providing protected data storage and sharing
RPM_SUMMARY = "Hermes is a cryptography-based method of providing protected data \
	storage and sharing that allows enforcing cryptographically checked CRUD \
	permissions to data blocks and doesn't let server that's running Hermes \
	do anything worse than DoS."
PACKAGE_CATEGORY = security

LIBRARY_SO_VERSION := $(shell echo $(VERSION) | sed 's/^\([0-9.]*\)\(.*\)*$$/\1/')
DEBIAN_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEBIAN_STRETCH_VERSION := libssl1.0.2
DEBIAN_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
ifeq ($(DEBIAN_CODENAME),stretch)
        DEBIAN_DEPENDENCIES := $(DEBIAN_STRETCH_VERSION)
else
        DEBIAN_DEPENDENCIES := openssl
endif
RPM_DEPENDENCIES = --depends openssl --depends libthemis

ifeq ($(shell lsb_release -is 2> /dev/null),Debian)
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else ifeq ($(shell lsb_release -is 2> /dev/null),Ubuntu)
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else
	OS_NAME = $(shell cat /etc/os-release | grep -e "^ID=\".*\"" | cut -d'"' -f2)
	OS_VERSION = $(shell cat /etc/os-release | grep -i version_id|cut -d'"' -f2)
	ARCHITECTURE = $(shell arch)
	NAME_SUFFIX = $(shell echo -n "$(VERSION)"|sed s/-/_/g).$(OS_NAME)$(OS_VERSION).$(ARCHITECTURE).rpm
endif


HEADER_FILES_MAP = $(BIN_PATH)/include/hermes/=$(PREFIX)/include/hermes \
	$(BIN_PATH)/include/pyhermes/=$(PREFIX)/include/pyhermes \
	$(BIN_PATH)/include/credential_store/=$(PREFIX)/include/credential_store \
	$(BIN_PATH)/include/key_store/=$(PREFIX)/include/key_store \
	$(BIN_PATH)/include/data_store/=$(PREFIX)/include/data_store \
	$(BIN_PATH)/include/mid_hermes/=$(PREFIX)/include/mid_hermes \
	$(BIN_PATH)/include/mid_hermes_ll/=$(PREFIX)/include/mid_hermes_ll

STATIC_BINARY_LIBRARY_MAP = $(BIN_PATH)/libhermes_common.a=$(PREFIX)/lib/libhermes_common.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_data_store.a=$(PREFIX)/lib/libhermes_data_store.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_mid_hermes.a=$(PREFIX)/lib/libhermes_mid_hermes.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_rpc.a=$(PREFIX)/lib/libhermes_rpc.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_credential_store.a=$(PREFIX)/lib/libhermes_credential_store.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_key_store.a=$(PREFIX)/lib/libhermes_key_store.a.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_mid_hermes_ll.a=$(PREFIX)/lib/libhermes_mid_hermes_ll.a.$(LIBRARY_SO_VERSION)

SHARED_BINARY_LIBRARY_MAP = $(BIN_PATH)/libhermes_data_store.so=$(PREFIX)/lib/libhermes_data_store.so.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_mid_hermes.so=$(PREFIX)/lib/libhermes_mid_hermes.so.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_rpc.so=$(PREFIX)/lib/libhermes_rpc.so.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_credential_store.so=$(PREFIX)/lib/libhermes_credential_store.so.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_key_store.so=$(PREFIX)/lib/libhermes_key_store.so.$(LIBRARY_SO_VERSION) \
		 $(BIN_PATH)/libhermes_mid_hermes_ll.so=$(PREFIX)/lib/libhermes_mid_hermes_ll.so.$(LIBRARY_SO_VERSION)


BINARY_LIBRARY_MAP = $(STATIC_BINARY_LIBRARY_MAP) $(SHARED_BINARY_LIBRARY_MAP)

POST_INSTALL_SCRIPT := $(BIN_PATH)/post_install.sh
POST_UNINSTALL_SCRIPT := $(BIN_PATH)/post_uninstall.sh

install_shell_scripts:
	@printf "ln -s $(PREFIX)/lib/libhermes_common.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_common.so \n \
		ln -s $(PREFIX)/lib/libhermes_data_store.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_data_store.so \n \
		ln -s $(PREFIX)/lib/libhermes_mid_hermes.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_mid_hermes.so \n \
		ln -s $(PREFIX)/lib/libhermes_rpc.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_rpc.so \n \
		ln -s $(PREFIX)/lib/libhermes_credential_store.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_credential_store.so \n \
		ln -s $(PREFIX)/lib/libhermes_key_store.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_key_store.so \n \
		ln -s $(PREFIX)/lib/libhermes_mid_hermes_ll.so.$(LIBRARY_SO_VERSION) $(PREFIX)/lib/libhermes_mid_hermes_ll.so" > $(POST_INSTALL_SCRIPT)

	@printf "unlink  $(PREFIX)/lib/libhermes_common.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_data_store.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_mid_hermes.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_rpc.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_credential_store.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_key_store.so 2>/dev/null \n \
		unlink  $(PREFIX)/lib/libhermes_mid_hermes_ll.so 2>/dev/null" > $(POST_UNINSTALL_SCRIPT)

HEADERS_FOLDER = $(BIN_PATH)/include

deb: core static_core collect_headers install_shell_scripts #test
	@find . -name \*.so -exec strip -o {} {} \;
	@mkdir -p $(BIN_PATH)/deb
#libhermes-dev
	@fpm --input-type dir \
		 --output-type deb \
		 --name libhermes-dev \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/lib$(PACKAGE_NAME)-dev_$(NAME_SUFFIX) \
		 --architecture $(DEBIAN_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEBIAN_DEPENDENCIES) \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(BINARY_LIBRARY_MAP) \
		 $(HEADER_FILES_MAP) 1>/dev/null

#libhermes
	@fpm --input-type dir \
		 --output-type deb \
		 --name libhermes \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/lib$(PACKAGE_NAME)_$(NAME_SUFFIX) \
		 $(DEBIAN_DEPENDENCIES) \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --architecture $(DEBIAN_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 --deb-priority optional \
		 --category $(PACKAGE_CATEGORY) \
		 $(BINARY_LIBRARY_MAP) 1>/dev/null

# it's just for printing .deb files
	@find $(BIN_PATH) -name \*.deb


rpm: core static_core collect_headers install_shell_scripts #test
	@find . -name \*.so -exec strip -o {} {} \;
	@mkdir -p $(BIN_PATH)/rpm
#libhermes-devel
	@fpm --input-type dir \
         --output-type rpm \
         --name libhermes-devel \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary $(RPM_SUMMARY) \
         $(RPM_DEPENDENCIES) \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-devel-$(NAME_SUFFIX) \
         --version $(VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(BINARY_LIBRARY_MAP) \
		 $(HEADER_FILES_MAP) 1>/dev/null
#libhermes
	@fpm --input-type dir \
         --output-type rpm \
         --name libhermes \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary $(RPM_SUMMARY) \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(BINARY_LIBRARY_MAP) 1>/dev/null
# it's just for printing .rpm files
	@find $(BIN_PATH) -name \*.rpm
