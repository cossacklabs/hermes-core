
# Hermes-core library 0.5 PoC

**Hermes-core** is a proof of concept for **Hermes**.

**Hermes** is a cryptography-based method of providing protected data storage and sharing that allows enforcing cryptographically checked CRUD permissions to data blocks and doesn't let server that's running Hermes do anything worse than DoS. In other words, Hermes enables collaboration and distributed data sharing through enforcing access control with the help of cryptographic methods.

**Hermes** operates with data that is subdivided into records, which represent the hierarchy of recordsets and groups of recordsets. Each blob of data is encrypted using a symmetric key, from which a set of hashes is generated. Possession of a symmetric key by a user allows reading and carrying out other processes on hashes (including with writing data).

In **Hermes-core** a document equals a block and is not subdivided further as Hermes-core is a basic building block for the hierarchic infrastructure of Hermes.

There are 3 storage entities in **Hermes** (and, consequently, in **Hermes-core**) that constitute the **Server** side: 
- **Data store** contains the hierarchy of encrypted objects.
- **Credential store** stores keys and hashes, asymmetrically encrypted in such a way that can only be decrypted by authorised user’s private key. Those can contain access control key which grants READ access and Update Tag which allows performing WRITE operations.
- **Keystore** contains the symmetric keys (for READ and UPDATE), with as many copies of these keys as there are users authorised to access to the record, where every copy is wrapped (asymmetrically encrypted) with a public credential of the respective authorised user. If the permissions to READ and to WRITE extend to not just blocks, but to the list of blocks, they turn into permissions to DELETE/ADD elements.  

The 4th entity of Hermes is **Client**:
- **Client** (or clients) is the active entity in the Hermes architecture, the one that actually produces or consumes the data. The Client only possesses the keypair that allows decrypting the asymmetrically encrypted data from the Server. The READ permissions are always checked on Client. The absence of the key for performing READ operations will not allow the Client to decrypt the downloaded piece of data.
The WRITE permissions are checked both on Client and Server so they cannot “fool” each other.

# For better understanding of the concepts behind Hermes and Hermes-core:
- Read the [scientific paper](https://github.com/cossacklabs/hermes-core/wiki/Hermes-Scientific-Paper-Redirect#hermes-scientific-paper) to understand Hermes better.
- Read the [documentation that accompanies Hermes-core](https://github.com/cossacklabs/hermes-core/wiki).
- Fast forward to hands-on tutorials available for Hermes-core with Client side written in [C](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial), [Python](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial), and [Go](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial).
- We also advise you to check out the ever-evolving [Implementing Hermes-based Security Systems](https://github.com/cossacklabs/hermes-core/wiki/Hermes-Scientific-Paper-Redirect#implementing-hermes-based-security-systems) document to find out more about Hermes-core and implementing Hermes-based systems in the real world.

*** 
### Repository status    
This repository holds public proof-of-concept version of **Hermes** - **Hermes-core**, which should be used for studying and verification of the methodology and cryptographic backend. 
***
### License
There is a separate, commercial licensed Hermes version for industrial use (its core crypto code is similar to this repository, yet it holds additional convenience interfaces and services). 

*** 

# Installing

## Debian / Ubuntu

> Note: Depending on your permissions, adding `sudo` might be necessary!

**1. Import the public key used by Cossack Labs to sign packages:**
```console
wget -qO - https://pkgs.cossacklabs.com/gpg | sudo apt-key add -
```
> Note: If you wish to validate key fingerprint, it is: `29CF C579 AD90 8838 3E37 A8FA CE53 BCCA C8FF FACB`.

**2. You may need to install the apt-transport-https package before proceeding:**
```console
sudo apt-get install apt-transport-https
```
**3. Add Cossack Labs repository to your `sources.list`.**
You should add a line that specifies your OS name and the release name:
```console
deb https://pkgs.cossacklabs.com/stable/$OS $RELEASE main
```
* `$OS` should be `debian` or `ubuntu`.
* `$RELEASE` should be one of Debian or Ubuntu release names. You can determine this by running `lsb_release -cs`, if you have `lsb_release` installed.
* We currently build packages for the following OS and RELEASE combinations:
*Debian "Wheezy" (Debian 7)*
*Debian "Jessie" (Debian 8)*
*Debian "Stretch" (Debian 9)*
*Ubuntu Precise Pangolin (Ubuntu 12.04)*
*Ubuntu Trusty Tahr (Ubuntu 14.04)*
*Ubuntu Xenial Xerus (Ubuntu 16.04)*
*Ubuntu Yakkety Yak (Ubuntu 16.10)*
*Ubuntu Zesty Zapus (Ubuntu 17.04)*

* For example, if you are running *Debian 9 "Stretch"*, run:
```console
echo "deb https://pkgs.cossacklabs.com/stable/debian stretch main" | \
  sudo tee /etc/apt/sources.list.d/cossacklabs.list
```
**4. Reload local package database:**
```console
sudo apt-get update
```
**5. Install the package**
```console
sudo apt-get install libhermes-core
```

## CentOS / RHEL / OEL
> Note, we only build RPM packages for x86_64.

**1. Import the public key used by Cossack Labs to sign packages:**
```
rpm --import https://pkgs.cossacklabs.com/gpg
```
>Note: If you wish to validate key fingerprint, it is: `29CF C579 AD90 8838 3E37 A8FA CE53 BCCA C8FF FACB`.

**2. Create a Yum repository file for Cossack Labs package repository:**
```console
wget -qO - https://pkgs.cossacklabs.com/stable/centos/cossacklabs.repo | \
  sudo tee /etc/yum.repos.d/cossacklabs.repo
```
**3. Install the package:**
sudo yum install libhermes-core

## Running a local example

You can try Hermes-core using a local example that contains all the storage entities Hermes-core needs for correct work.
This local example can be found in `docs/examples/c/mid_hermes_low_level` folder of the repository.
* Before using this local example you need to register some "users" in Hermes.   
To register a user with ID "USER", type this into the command line:
```console
cd docs/examples/c/mid_hermes_low_level
mkdir -p ./db/credential_store
../key_gen/key_pair_gen USER.priv ./db/credential_store/$(echo -ne USER | base64)
```
* Create first file file.1block:
```console
echo "smth" > file1.block
```
* Add the block `file1.block` with meta "first block in HERMES" to Hermes-core as a USER, using the following command:

```console
./hermes_client_ll add_block USER $(cat USER.priv | base64) file1.block "first block in Hermes"
```
* Now you can see some new files in the Hermes-core storages:
```console
./db/data_store/ZmlsZTEuYmxvY2sK/data                - encrypted block
./db/data_store/ZmlsZTEuYmxvY2sK/mac                 - Update TAG
./db/data_store/ZmlsZTEuYmxvY2sK/meta                - block meta
./db/key_store/ZmlsZTEuYmxvY2sK/VVNFUgo=/r/token     - read token
./db/key_store/ZmlsZTEuYmxvY2sK/VVNFUgo=/r/owner     - read token owner
./db/key_store/ZmlsZTEuYmxvY2sK/VVNFUgo=/w/token     - update token
./db/key_store/ZmlsZTEuYmxvY2sK/VVNFUgo=/w/owner     - update token owner
```
> ZmlsZTEuYmxvY2sK - base64 encoded string "file1.block"

> VVNFUgo=         - base64 encoded string "USER"

* Find the list of all the other commands supported by the example using the following command:
```console
docs/examples/c/mid_hermes_low_level/hermes_client_ll --help
usage: hermes_client_ll <command> <user id> <base64 encoded user private key>  <name of file for proceed> <meta> <for user>.
           <command>                         - executes the command to be performed by the client, see below;
           <user id>                         - user identifier (user needs to be registered in Credential store);
           <base64 encoded user private key> - base64 encoded private key of the user;
           <name of file to be processed>    - filename of the file to be processed (file name is used as block ID in Hermes);
           <meta>                            - some data associated with a file that is stored in the database in plaintext;
           <for user>                        - identifier of the user for which permissions are provided/revoked from (this information is needed by some commands).

commands:
           add_block -  add <name of file to be processed> block with <meta> to Hermes system
           read_block -  read <name of file to be processed> block with <meta> from Hermes system
           update_block -  update <name of file to be processed> block with <meta> in Hermes system
           delete_block -  delete <name of file to be processed> block from Hermes system
           rotate -   rotate <name of file to be processed> block from Hermes system
           grant_read - grant read access for <for user> to <name of file to be processed> block in Hermes system
           grant_update - grant update access for <for user> to <name of file to be processed> block in Hermes system
           revoke_read - deny read access for <for user> to <name of file to be processed> block in Hermes system
           revoke_update - deny update access for <for user> to <name of file to be processed> block in Hermes system
```
***
# Building from source

> Note: This open-source version of Hermes-core is only checked to work on Linux.     

To build Hermes-core, some dependencies need to be satisfied. You need to install:
* The standard build environment (for Debian-like systems you need to install `build-essential` development libraries and header files through `$ apt-get install build-essential openssl-dev`),
* Our crypto SDK [Themis](https://github.com/cossacklabs/themis/wiki/Building-and-installing).  

## Download and install   

To build Hermes-core, open the terminal and type in the following command:
```console
$ git clone https://github.com/cossacklabs/hermes-core
$ cd hermes-core
$ make && sudo make install
``` 
Hermes-core is ready to use.

For more detailed instructions on building and installing Hermes-core and for tests, see [Building Hermes-core](https://github.com/cossacklabs/hermes-core/wiki/Building-Hermes-core/).
***
# Contacts
For all questions and inquieries, use info@cossacklabs.com or raise an Issue.      
[![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/)
