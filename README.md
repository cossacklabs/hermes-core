<h1 align="center">
  <br>
  HERMES-CORE
  <br>
  0.5 PoC
  <br>
</h1>

<p align="center">
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/github/release/cossacklabs/hermes-core.svg" alt="GitHub release"></a>
  <a href="https://circleci.com/gh/cossacklabs/hermes-core"><img src="https://circleci.com/gh/cossacklabs/hermes-core/tree/master.svg?style=shield&circle-token=75820c008f79dd9751880eff37673be5ca34aa13" alt="Circle CI"></a>
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/badge/platform-Debian%20%7C%20Ubuntu%20%7C%20CentOS%20%7C%20OSX-green.svg" alt="Platforms"></a>
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

The [scientific paper](https://github.com/cossacklabs/hermes-core/wiki/Hermes-Scientific-Paper-Redirect#hermes-scientific-paper) explains the concept behind Hermes, math model, risk & threats analysis and provides implementation details.

Amazing tutorials available for Hermes-core with Client side written in [C](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial), [Python](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial), and [Go](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial).

We also advise you to check out the ever-evolving [Implementing Hermes-based Security Systems](https://github.com/cossacklabs/hermes-core/wiki/Hermes-Scientific-Paper-Redirect#implementing-hermes-based-security-systems) document to find out more about Hermes-core and implementing Hermes-based systems in the real world.


# Quick start

## Installing from repository
## Building from source

# Languages

Hermes-core is available on C, however, clients are implemented on C, Python and Go: 

| Platform | Tutorial | Code example |
| :----- | :----- | :------ |
| C core / C client | [Local CLI tutorial](https://github.com/cossacklabs/hermes-core/wiki/Local-CLI-example) | [docs/examples/c/mid_hermes_low_level](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c/mid_hermes_low_level) |
| C core / C client | [C tutorial](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial) | [docs/examples/c](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c) |
| C core / Python client | [Python tutorial](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial) | [docs/examples/python](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/python) |
| C core / Go client | [Go tutorial](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial) | [docs/examples/go](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/go) |

 
# Repository status    
This repository holds public proof-of-concept version of **Hermes** - **Hermes-core**, which should be used for studying and verification of the methodology and cryptographic backend. 


# License
There is a separate, commercial licensed Hermes version for industrial use (its core crypto code is similar to this repository, yet it holds additional convenience interfaces and services). 

# Contributing to us

If you're looking for something to contribute to and gain eternal respect, just pick the things in the [list of issues](https://github.com/cossacklabs/themis/issues). 

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/themis/issues) or write an [email](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an [email](mailto:info@cossacklabs.com).


# Contacts
For all questions and inquieries, use [info@cossacklabs.com](mailto:info@cossacklabs.com) or raise an Issue.      
[![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/)
