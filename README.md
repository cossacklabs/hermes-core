<h1 align="center">


  HERMES-CORE 0.5.1 PoC
</h1>
<p align="center">
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/github/release/cossacklabs/hermes-core.svg" alt="GitHub release"></a>
  <a href="https://circleci.com/gh/cossacklabs/hermes-core"><img src="https://circleci.com/gh/cossacklabs/hermes-core/tree/master.svg?style=shield&circle-token=75820c008f79dd9751880eff37673be5ca34aa13" alt="Circle CI"></a>
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/badge/platform-Debian%20%7C%20Ubuntu%20%7C%20CentOS%20%7C%20macOS-green.svg" alt="Platforms"></a>
</p>
<br>



#### Hermes-core is a security tool for distributed data sharing and collaboration that enforces access control cryptographically.
<br>

# What is Hermes

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

## Documentation

[Project's GitHub Wiki](https://www.github.com/cossacklabs/hermes-core/wiki) contains the ever-evolving official documentation, which contains everything from deployment guidelines to use-cases, including charts and tutorials you might find useful. 

The scientific paper ["Hermes – a framework for cryptographically assured access control and data security"](https://www.cossacklabs.com/files/hermes-theory-paper-rev1.pdf) explains the concept behind Hermes, math model, risk & threats analysis and provides implementation details.

Amazing tutorials available for Hermes-core with Client side written in [C](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial), [Python](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial), and [Go](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial).

We also advise you to check out the ever-evolving [Implementing Hermes-based Security Systems](https://www.cossacklabs.com/hermes/implementing-hermes-based-systems/) document to find out more about Hermes-core and implementing Hermes-based systems in the real world.


# Languages and tutorials

Hermes-core is available on C, however, clients are implemented on C, Python and Go: 

| Platform | Tutorial | Code example |
| :----- | :----- | :------ |
| C core / C client | [Local CLI tutorial](https://github.com/cossacklabs/hermes-core/wiki/Local-CLI-example) | [docs/examples/c/mid_hermes_low_level](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c/mid_hermes_low_level) |
| C core / C client | [C tutorial](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial) | [docs/examples/c](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c) |
| C core / Python client | [Python tutorial](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial) | [docs/examples/python](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/python) |
| C core / Go client | [Go tutorial](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial) | [docs/examples/go](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/go) |


# Quick start

You can get Hermes-core library from repository or build it from sources.

## Installing from repository

> Please refer to the [Installing from repository](https://github.com/cossacklabs/hermes-core/wiki/Installing-Hermes-core) page if you want to install dev packages or to catch more details.

Hermes-core is available for the following versions of operating systems:       
`*.deb:`
- Debian: Wheezy, Jessie, Stretch;
- Ubuntu: Trusty Tahr, Xenial Xerus, Yakkety Yak, Zesty Zapus.

`*.rpm:`
- CentOS: 7.


### Installing for Debian / Ubuntu

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

We currently build packages for the following OS and RELEASE combinations:

- *Debian "Wheezy" (Debian 7)*
- *Debian "Jessie" (Debian 8)*
- *Debian "Stretch" (Debian 9)*
- *Ubuntu Trusty Tahr (Ubuntu 14.04)*
- *Ubuntu Xenial Xerus (Ubuntu 16.04)*
- *Ubuntu Yakkety Yak (Ubuntu 16.10)*
- *Ubuntu Zesty Zapus (Ubuntu 17.04)*

For example, if you are running *Debian 8 "Jessie"*, run:
```console
echo "deb https://pkgs.cossacklabs.com/stable/debian jessie main" | \
  sudo tee /etc/apt/sources.list.d/cossacklabs.list
```
**Note:** If you want to install Hermes-core on a machine running Debian 9 "Stretch", the package management system will ask you to delete the newer version of libssl and replace it for the libssl version supported by [Themis](https://github.com/cossacklabs/themis) (libssl1.0-dev). Please check the list of the files to be removed to avoid breaking the dependencies on your machine.

**4. Reload local package database:**
```console
sudo apt-get update
```
**5. Install the package**
```console
sudo apt-get install libhermes-core
```

### Installing for CentOS / RHEL / OEL
> Note, we only build RPM packages for x86_64.

**1. Import the public key used by Cossack Labs to sign packages:**
```console
sudo rpm --import https://pkgs.cossacklabs.com/gpg
```
>Note: If you wish to validate key fingerprint, it is: `29CF C579 AD90 8838 3E37 A8FA CE53 BCCA C8FF FACB`.

**2. Create a Yum repository file for Cossack Labs package repository:**
```console
wget -qO - https://pkgs.cossacklabs.com/stable/centos/cossacklabs.repo | \
  sudo tee /etc/yum.repos.d/cossacklabs.repo
```
**3. Install the package:**
```console
sudo yum install libhermes-core
```

That's all! Hermes-core is ready to use. The easiest way is to follow one of the tutorials provided above.

## Building from source

> You can check the [Building Hermes-core](https://github.com/cossacklabs/hermes-core/wiki/Building-Hermes-core) page to see how to run tests.

Let's install the libraries and utilities that we're going to need.
For Debian 9 "Stretch" the command will be:
```console
sudo apt-get update && sudo apt-get install build-essential libssl1.0-dev git
```

For all other Debian versions, use:
```console
sudo apt-get update && sudo apt-get install build-essential libssl-dev git 
```

We need `build-essential` for building binary libraries and `libssl` as backend for [Themis](https://github.com/cossacklabs/themis).

Let's download and install Themis into your system:

```console
git clone https://github.com/cossacklabs/themis
cd themis
make && sudo make install
cd ..
```

Now you should download and install Hermes-core:
```console
git clone https://github.com/cossacklabs/hermes-core
cd hermes-core
make && sudo make install
```

That's all! Hermes-core is ready to use. The easiest way is to follow one of the tutorials provided above.
 
# Repository status    
This repository holds public proof-of-concept version of **Hermes** - **Hermes-core**, which should be used for studying and verification of the methodology and cryptographic backend. 


# License
Hermes-core license is GNU Affero General Public License v3.0.
There is a separate, commercial licensed Hermes version for industrial use (its core crypto code is similar to this repository, yet it holds additional convenience interfaces and services).

# Contributing to us

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/hermes-core/issues). 

Check [Contributing guide](https://github.com/cossacklabs/hermes-core/wiki/contributing) for more details.

# Contacts

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/hermes-core/issues) or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).
   
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/)
