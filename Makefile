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

LDFLAGS += -Lbuild
CFLAGS += -I$(INCLUDE_PATH) -fPIC


UNAME=$(shell uname)

ifeq ($(UNAME),Darwin)
	IS_MACOS := true
endif

ifeq ($(PREFIX),)
PREFIX = /usr

	# MacOS
	ifdef IS_MACOS
		PREFIX = /usr/local
	endif

endif

ifdef IS_MACOS
	CFLAGS += -I/$(PREFIX)/include
else
	LDFLAGS += -Llibs/themis/build
endif


SHARED_EXT = so
ifdef IS_MACOS
	SHARED_EXT = dylib
endif

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
include src/secure_transport/secure_transport.mk
endif


all: err core

core: rpc_shared credential_store_shared data_store_shared key_store_shared mid_hermes_shared secure_transport_shared

static_core: rpc_static credential_store_static data_store_static key_store_static mid_hermes_static secure_transport_static

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

mid_hermes_ll_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(MID_HERMES_LL_BIN).$(SHARED_EXT) $(MID_HERMES_LL_OBJ) $(MID_HERMES_OBJ) $(KEY_STORE_OBJ) $(DATA_STORE_OBJ) $(CREDENTIAL_STORE_OBJ) $(RPC_OBJ) $(LDFLAGS) -l$(COMMON_BIN) -lthemis -lsoter

mid_hermes_ll_shared: common_static $(MID_HERMES_OBJ) $(MID_HERMES_LL_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


secure_transport_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(SECURE_TRANSPORT_BIN).a $(SECURE_TRANSPORT_OBJ)

secure_transport_static: common_static $(SECURE_TRANSPORT_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

secure_transport_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(SECURE_TRANSPORT_BIN).$(SHARED_EXT) $(SECURE_TRANSPORT_OBJ) $(CREDENTIAL_STORE_OBJ) $(RPC_OBJ) $(LDFLAGS) -lthemis -lsoter -l$(COMMON_BIN)

secure_transport_shared: common_static $(SECURE_TRANSPORT_OBJ)
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

# alias for now, will install hermes-libs later
install_all: install

mid_hermes_ll_example: CMD = cd docs/examples/c/mid_hermes_low_level && make

mid_hermes_ll_example: static_core
	@echo -n "make midHermes lowLevel example"
	@$(BUILD_CMD_)

mid_hermes_ll_example_clean: CMD = cd docs/examples/c/mid_hermes_low_level && make clean

mid_hermes_ll_example_clean:
	@echo -n "make midHermes lowLevel example clean"
	@$(BUILD_CMD_)

mid_hermes_example_client: CMD = cd docs/examples/c/mid_hermes/client && make

mid_hermes_example_client:
	@echo -n "make midHermes example client"
	@$(BUILD_CMD_)


mid_hermes_example_client_clean: CMD  = cd docs/examples/c/mid_hermes/client && make clean;

mid_hermes_example_client_clean:
	@echo -n "make midHermes example client clean"
	@$(BUILD_CMD_)

mid_hermes_example_credential_store: CMD = cd docs/examples/c/mid_hermes/credential_store_service && make;

mid_hermes_example_credential_store:
	@echo -n "make midHermes example credential store"
	@$(BUILD_CMD_)

mid_hermes_example_credential_store_clean: CMD = cd docs/examples/c/mid_hermes/credential_store_service && make clean;

mid_hermes_example_credential_store_clean:
	@echo -n "make midHermes example credential store clean"
	@$(BUILD_CMD_)

mid_hermes_example_key_store: CMD = cd docs/examples/c/mid_hermes/key_store_service && make;

mid_hermes_example_key_store:
	@echo -n "make midHermes example key store"
	@$(BUILD_CMD_)

mid_hermes_example_key_store_clean: CMD = cd docs/examples/c/mid_hermes/key_store_service && make clean;

mid_hermes_example_key_store_clean:
	@echo -n "make midHermes example key store clean"
	@$(BUILD_CMD_)

mid_hermes_example_data_store: CMD = cd docs/examples/c/mid_hermes/data_store_service && make;

mid_hermes_example_data_store:
	@echo -n "make midHermes example data store"
	@$(BUILD_CMD_)

mid_hermes_example_data_store_clean: CMD = cd docs/examples/c/mid_hermes/data_store_service && make clean;

mid_hermes_example_data_store_clean:
	@echo -n "make midHermes example data store"
	@$(BUILD_CMD_)

ckeygen: CMD = cd docs/examples/c/key_gen && make

ckeygen:
	@echo -n "make C keygen"
	@$(BUILD_CMD_)

ckeygen_clean: CMD = cd docs/examples/c/key_gen && make clean;

ckeygen_clean:
	@echo -n "make C keygen clean"
	@$(BUILD_CMD_)

examples: static_core ckeygen mid_hermes_ll_example mid_hermes_example_client mid_hermes_example_credential_store mid_hermes_example_key_store mid_hermes_example_data_store

examples_clean: ckeygen_clean mid_hermes_ll_example_clean mid_hermes_example_client_clean mid_hermes_example_credential_store_clean mid_hermes_example_key_store_clean mid_hermes_example_data_store_clean

examples_clean:
	@$(BUILD_CMD_)

uninstall: CMD = rm -rf $(PREFIX)/include/hermes && rm -f $(PREFIX)/lib/libhermes*.a && rm -f $(PREFIX)/lib/libhermes*.$(SHARED_EXT)

uninstall:
	@echo -n "hermes uninstall "
	@$(BUILD_CMD_)

PACKAGE_NAME = hermes-core
# check that it's git repo
GIT_VERSION := $(shell if [ -d ".git" ]; then git version; fi 2>/dev/null)
# check that repo has any tag
GIT_TAG_STATUS := $(shell git describe --tags HEAD 2>/dev/null)
GIT_TAG_STATUS := $(.SHELLSTATUS)

ifdef GIT_VERSION
# if has tag then use it
        ifeq ($(GIT_TAG_STATUS),0)
# <tag>-<commit_count_after_tag>-<last_commit-hash>
                VERSION = $(shell git describe --tags HEAD | cut -b 1-)
        else
# <base_version>-<total_commit_count>-<last_commit_hash>
                VERSION = 0.5.0-$(shell git rev-list --all --count)-$(shell git describe --always HEAD)
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
	@cp -r include/hermes src/credential_store src/data_store src/key_store src/mid_hermes src/mid_hermes_ll src/secure_transport  pyhermes $(HEADERS_FOLDER)/
# delete non-header files
	@find build/include ! -name *.h -type f -exec rm {} \;
# delete empty folders that may copied before
	@find build/include ! -name *.h -type d | sort -r | xargs rmdir --ignore-fail-on-non-empty

COSSACKLABS_URL = https://www.cossacklabs.com
MAINTAINER = "Cossack Labs Limited <dev@cossacklabs.com>"
LICENSE_NAME = "GNU Affero General Public License v3.0"
SHORT_DESCRIPTION = Hermes is a cryptography-based method of providing protected data storage and sharing
RPM_SUMMARY = "Hermes is a cryptography-based method of providing protected data \
	storage and sharing that allows enforcing cryptographically checked CRUD \
	permissions to data blocks and doesn't let server that's running Hermes \
	do anything worse than DoS."
PACKAGE_CATEGORY = security

LIBRARY_SO_VERSION := $(shell echo $(VERSION) | sed 's/^\([0-9.]*\)\(.*\)*$$/\1/')
ifeq ($(LIBRARY_SO_VERSION),)
	LIBRARY_SO_VERSION := 0.5.0
endif
DEBIAN_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEBIAN_STRETCH_VERSION := libssl1.0.2
DEBIAN_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
ifeq ($(DEBIAN_CODENAME),stretch)
        DEBIAN_DEPENDENCIES := --depends $(DEBIAN_STRETCH_VERSION) --depends libthemis
else
        DEBIAN_DEPENDENCIES := --depends openssl --depends libthemis
endif
RPM_DEPENDENCIES = --depends openssl --depends libthemis
RPM_RELEASE_NUM = 1

ifeq ($(shell lsb_release -is 2> /dev/null),Debian)
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else ifeq ($(shell lsb_release -is 2> /dev/null),Ubuntu)
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else
# centos/rpm
	OS_NAME = $(shell cat /etc/os-release | grep -e "^ID=\".*\"" | cut -d'"' -f2)
	OS_VERSION = $(shell cat /etc/os-release | grep -i version_id|cut -d'"' -f2)
	ARCHITECTURE = $(shell arch)
	RPM_VERSION = $(shell echo -n "$(VERSION)"|sed s/-/_/g)
	NAME_SUFFIX = $(RPM_VERSION).$(OS_NAME)$(OS_VERSION).$(ARCHITECTURE).rpm
endif

HEADERS_FOLDER = $(BIN_PATH)/include
HEADER_DIRS = $(shell ls $(HEADERS_FOLDER))
# HEADER_FILES_MAP is path pairs of header files for fpm
HEADER_FILES_MAP = $(foreach dir,$(HEADER_DIRS), $(BIN_PATH)/include/$(dir)/=$(PREFIX)/include/$(dir))

STATIC_LIBRARY_FILES = $(shell ls $(BIN_PATH)/ | egrep *\.a$$)
# STATIC_BINARY_LIBRARY_MAP is path pairs of *.a files for fpm
STATIC_BINARY_LIBRARY_MAP = $(foreach file,$(STATIC_LIBRARY_FILES),$(strip $(BIN_PATH)/$(file)=$(PREFIX)/lib/$(file) $(BIN_PATH)/$(file).$(LIBRARY_SO_VERSION)=$(PREFIX)/lib/$(file).$(LIBRARY_SO_VERSION)))

SHARED_LIBRARY_FILES = $(shell ls $(BIN_PATH)/ | egrep *\.$(SHARED_EXT)$$)
# SHARED_BINARY_LIBRARY_MAP is path pairs of *.so files for fpm
SHARED_BINARY_LIBRARY_MAP = $(foreach file,$(SHARED_LIBRARY_FILES),$(strip $(BIN_PATH)/$(file).$(LIBRARY_SO_VERSION)=$(PREFIX)/lib/$(file).$(LIBRARY_SO_VERSION) $(BIN_PATH)/$(file)=$(PREFIX)/lib/$(file)))


BINARY_LIBRARY_MAP = $(STATIC_BINARY_LIBRARY_MAP) $(SHARED_BINARY_LIBRARY_MAP)

POST_INSTALL_SCRIPT := $(BIN_PATH)/post_install.sh
POST_UNINSTALL_SCRIPT := $(BIN_PATH)/post_uninstall.sh

install_shell_scripts:
# run ldconfig to update ld.so cache
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@cp $(POST_INSTALL_SCRIPT) $(POST_UNINSTALL_SCRIPT)

symlink_realname_to_soname:
	# add version to filename and create symlink with realname to full name of library
	@for f in `ls $(BIN_PATH) | egrep ".*\.(so|a)(\..*)?$$" | tr '\n' ' '`; do \
		mv $(BIN_PATH)/$$f $(BIN_PATH)/$$f.$(LIBRARY_SO_VERSION); \
		ln -s $(PREFIX)/lib/$$f.$(LIBRARY_SO_VERSION) $(BIN_PATH)/$$f; \
	done

strip: 
	@find . -name \*.so\.* -exec strip -o {} {} \;

deb: test core static_core collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/deb
#libhermes-dev
	@fpm --input-type dir \
		 --output-type deb \
		 --name lib$(PACKAGE_NAME)-dev \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/lib$(PACKAGE_NAME)-dev_$(NAME_SUFFIX) \
		 --architecture $(DEBIAN_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEBIAN_DEPENDENCIES) --depends "lib$(PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)" \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(HEADER_FILES_MAP)

#libhermes
	@fpm --input-type dir \
		 --output-type deb \
		 --name lib$(PACKAGE_NAME) \
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
		 $(BINARY_LIBRARY_MAP)

# it's just for printing .deb files
	@find $(BIN_PATH) -name \*.deb


rpm: test core static_core collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/rpm
#libhermes-devel
	@fpm --input-type dir \
         --output-type rpm \
         --name lib$(PACKAGE_NAME)-devel \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary $(RPM_SUMMARY) \
         $(RPM_DEPENDENCIES) --depends "lib$(PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)"\
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-devel-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --rpm-auto-add-directories \
         --category $(PACKAGE_CATEGORY) \
		 $(HEADER_FILES_MAP)
#libhermes
	@fpm --input-type dir \
         --output-type rpm \
         --name lib$(PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary $(RPM_SUMMARY) \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --rpm-auto-add-directories \
         --category $(PACKAGE_CATEGORY) \
         $(BINARY_LIBRARY_MAP)
# it's just for printing .rpm files
	@find $(BIN_PATH) -name \*.rpm
