# Hermes core library 0.5 PoC

Hermes is novel way to store and protect the data cryptographically, which enables you to add CRUD permissions to data blocks via cryptographic process, where server running Hermes can't do anything worse than DoS. 

Scientific paper and a number of descriptive documents are in progress. In the meanwhile, you can read [wiki](https://github.com/cossacklabs/hermes-core/wiki) for simplistic description.

This repository holds public proof-of-concept version, which should be used to study and verify the methodology and cryptographic backend. 

There is a separate, commercial licensed Hermes version for industrial use (it's core crypto code is equal to this repository, yet it holds additional convenience interfaces and services). 

Hermes is a part of backend protection suite called [Toughbase](https://www.cossacklabs.com/toughbase/).

# Building

Requirements:

To build Hermes you need only standard build environment (in Debian you need to install ‘build-essential’ package) and our crypto SDK [Themis](https://www.github.com/cossacklabs/themis/) installed. This open-source version of Hermes is checked to work only on Linux. 

To build and install Hermes:

```
make
make install
```

Hermes is ready to use.

# Using Hermes:

Hermes sits right between frontend app and data storage. Because of that, to use Hermes you need something to act these roles.

To study Hermes better, we've included simple file storage primitives, file_db. They're not included in deafult Hermes build, and, if requires, can be included by: 

```
make WITH_FILE_DB
```

After that, Hermes libraries can use files to 

To learn more, how Hermes can be used within other applications, please see C tests (examples coming soon!). 

For Python users there is a simple example with usage of ctypes wrappers of hermes procedures. 

Pyhermes.py is a simple console application to demonstrate main hermes abilities:

```
usage: 
pyhermes.py -[u|p|c|d|b|cc|uu|r] <parameters>
	-u – user id, used for retrieve user public key from credential store
	-p – user public key (in base64 format)
	-c – command:
		‘cd’ - create document (require to set -cc parameter)
		‘rd’ - read document (require to set -d parameter)
		‘ud’- update document (require to set -d and -cc parameters)
		‘dd’- delete document (require to set -d parameter)
		‘cb’ -create private document block (require to set -d and -cc parameters)
		‘rb’- read private document block (require to set -d and -b parameters)
		‘ub’- update private document block (require to set -d, -b and -cc parameters)
		‘db’- delete private document block (require to set -d and -b parameter)
		‘ge’ -grand access to another user to determined document (block) (require to set -d(and/or -b), -uu and -r parameters)
		‘de’ - deny access to another user to determined document (block) (require to set -d(and/or -b), -uu )
	-d – document id to manipulate
	-b – document block id to manipulate
	-cc – context of document/block to insert/update
	-uu – user id to grant/deny access
	-r – rights mask (1 for read-only access, 2 for read-write access)
```
