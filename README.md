# Hermes core library 0.5 PoC

Hermes is a cryptography-based method of providing protected data storage and sharing that allows enforcing cryptographically checked CRUD permissions to data blocks and doesn't let server that's running Hermes do anything worse than DoS. 

Scientific paper and a number of descriptive documents are in progress. Meanwhile, you can read [wiki](https://github.com/cossacklabs/hermes-core/wiki) for a simplistic description.

This repository holds public proof-of-concept version, which should be used to study and verify the methodology and cryptographic backend. 

There is a separate, commercial licensed Hermes version for industrial use (its core crypto code is similar to this repository, yet it holds additional convenience interfaces and services). 

Hermes is a part of backend protection suite called [Toughbase](https://www.cossacklabs.com/toughbase/).

# Building

Requirements:

To build Hermes, you only need to install the standard build environment (in Debian you need to install ‘build-essential’ package) and our crypto SDK [Themis](https://www.github.com/cossacklabs/themis/). This open-source version of Hermes is only checked to work on Linux. 

To build and install Hermes:

```
make
make install
```

Hermes is ready to use.

# Using Hermes:

Hermes sits right between frontend app and data storage so you'll need something to fill these roles to use Hermes.

We've included simple file storage primitives - file_db - to help you study Hermes in action. They're not present in the deafult Hermes build, but can easily be included through: 

```
make WITH_FILE_DB
```

After that, Hermes libraries can use files to...

To learn more about the ways of using Hermes with(in) other applications, please see C tests (examples coming soon!). 

For Python users there is a simple example with usage of ctypes wrappers of Hermes procedures. 

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
