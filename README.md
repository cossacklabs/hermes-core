<h1 align="center">
  <a href="https://www.cossacklabs.com/hermes/"><img src="https://github.com/cossacklabs/hermes-core/wiki/images/hermes_logo.png" alt="End-to-end secure data storage, processing, and sharing framework with zero trust to storage/exchange infrastructure." width="420"></a>
</h1>
<h3 align="center">
  <br>
  End-to-end secure data storage, processing, and sharing framework with zero trust to storage/exchange infrastructure.
  <br>
</h3>

---

<p align="center">
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/github/release/cossacklabs/hermes-core.svg" alt="GitHub release"></a>
  <a href="https://circleci.com/gh/cossacklabs/hermes-core"><img src="https://circleci.com/gh/cossacklabs/hermes-core/tree/master.svg?style=shield&circle-token=75820c008f79dd9751880eff37673be5ca34aa13" alt="Circle CI"></a>
  <a href="https://github.com/cossacklabs/hermes-core/releases/latest"><img src="https://img.shields.io/badge/platform-Debian%20%7C%20Ubuntu%20%7C%20CentOS%20%7C%20macOS-green.svg" alt="Platforms"></a>
</p>
<br>

# What is Hermes

**Hermes** — cryptographic framework for building multi-user end-to-end encrypted data storage and sharing/processing with zero leakage risks from storage and transport infrastructure (so called **end-to-end encrypted zero knowledge architectures**).

Hermes acts as a **protected data circulation layer** with cryptographic access control for your distributed application, with zero security risk of data exposure from servers and storage.

Hermes allows deploying end-to-end encrypted data exchange, sharing, and collaboration in your apps. Hermes is platform agnostic: works for mobile, web, or server applications storing data in any database/datastore.

## What is Hermes-core

[Hermes is a proprietary framework](https://www.cossacklabs.com/hermes/) licensed by Cossack Labs. 

**Hermes-core** is an open source (AGPL 3.0) repository for developers and security community that illustrates proof of concept of Hermes, which should be used for studying and verification of the methodology and cryptographic backend. Hermes-core is not a production version of Hermes, more a sneak peek of its core layer. 

Drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com) if you are interested in commercial license or support.

## Data model

Hermes operates with data that is subdivided into records, which represent the hierarchy of recordsets and groups of recordsets. Each blob of data is encrypted using a symmetric key, from which a set of hashes is generated. Possession of a symmetric key by a user allows reading and carrying out other processes on hashes (including with writing data).

In Hermes-core `a document` equals `a block` and is not subdivided further as Hermes-core is a basic building block for the hierarchic infrastructure of Hermes.

## Hermes entities

There are 3 storage entities in Hermes (and, consequently, in Hermes-core) that constitute the **Server side**: 
- **Data store** contains the hierarchy of encrypted objects.
- **Credential store** stores keys and hashes, asymmetrically encrypted in such a way that can only be decrypted by authorised user’s private key. Those can contain access control key which grants READ access and Update Tag which allows performing WRITE operations.
- **Keystore** contains the symmetric keys (for READ and UPDATE), with as many copies of these keys as there are users authorised to access to the record, where every copy is wrapped (asymmetrically encrypted) with a public credential of the respective authorised user. If the permissions to READ and to WRITE extend to not just blocks, but to the list of blocks, they turn into permissions to DELETE/ADD elements.  

The 4th entity of Hermes is **Client**:
- **Client** (or clients) is the active entity in the Hermes architecture, the one that actually produces or consumes the data. The Client only possesses the keypair that allows decrypting the asymmetrically encrypted data from the Server. The READ permissions are always checked on Client. The absence of the key for performing READ operations will not allow the Client to decrypt the downloaded piece of data.
The WRITE permissions are checked both on Client and Server so they cannot “fool” each other.

## Use cases and industries

TODO:

Healthcare
Share FHIR and other medical records safely and distribute granular access to personnel in a secure way. Cut HIPAA costs by pushing many security controls to the encryption layer.	Finance
Finance
Store and process customer payment data securely, minimise insider threats and enable secure, accountable cross-organisation data exchange.
Enterprise
Enterprise
Protect commercially sensitive data and enforce access control, integrate with existing PKI and IAM stack, enforce group policies and efficient key/storage management – all the while keeping the data end-to-end encrypted.	B2C: Customer apps
B2C: Customer apps
Instill greater trust in your product by implementing end-to-end encryption of customer data. It’s not only E2EE messengers that deserve the right to use user trust as competitive advantage. Implement it in your products or infrastructure, hassle-free.

## Documentation and papers

[Project's GitHub Wiki](https://www.github.com/cossacklabs/hermes-core/wiki) contains the ever-evolving official documentation, which contains everything from deployment guidelines to use-cases, including charts and tutorials you might find useful. 

The scientific paper ["Hermes – a framework for cryptographically assured access control and data security"](https://www.cossacklabs.com/files/hermes-theory-paper-rev1.pdf) explains the concept behind Hermes, math model, risk & threats analysis and provides implementation details.

We also advise you to check out the ever-evolving [Implementing Hermes-based Security Systems](https://www.cossacklabs.com/hermes/implementing-hermes-based-systems/) document to find out more about Hermes-core and implementing Hermes-based systems in the real world.


# Installation

You can build it manually from source, or install from the available package manager.

- If you are running Ubuntu, Debian or CentOS, check [Installing from repository](https://github.com/cossacklabs/hermes-core/wiki/Installing-Hermes-core) page.

- If you want to have the latest version of Hermes-core, you can build it from sources: [Building Hermes core](https://github.com/cossacklabs/hermes-core/wiki/Building-Hermes-core).

# Languages

Hermes-core is available on C, however, client side applications are implemented on C, Python and Go: 

| Platform | Tutorial | Code example |
| :----- | :----- | :------ |
| C core / C client | [Local CLI tutorial](https://github.com/cossacklabs/hermes-core/wiki/Local-CLI-example) | [docs/examples/c/mid_hermes_low_level](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c/mid_hermes_low_level) |
| C core / C client | [C tutorial](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial) | [docs/examples/c](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/c) |
| C core / Python client | [Python tutorial](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial) | [docs/examples/python](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/python) |
| C core / Go client | [Go tutorial](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial) | [docs/examples/go](https://github.com/cossacklabs/hermes-core/tree/master/docs/examples/go) |

TODO: write about other languages


# Availability

[Hermes itself](https://www.cossacklabs.com/hermes/) supports the following architectures: x86/x64, armv*, various Android architectures. 

Hermes supports, but not limited to:

* Debian (8, 9), CentOS 7, Ubuntu (14.04, 16.04, 18.04),
* macOS (10.12, 10.13, 10.14),
* Android (4 - 9) / CyanogenMod 11+,
* iOS (9 - 12),
* Docker-containers, VMs.

Hermes-core has limited support, only x86/x64 platforms.

# Examples and tutorials

Consider checking full tutorials to understand how to add and update blocks, grant READ and UPDATE access right for users, revoke access rights.

- [Usage examples](https://github.com/cossacklabs/hermes-core/wiki/Usage-examples) describe how examples work and what are possible usages for Hermes-core.
- [C tutorial](https://github.com/cossacklabs/hermes-core/wiki/C-tutorial), where both Hermes and client app are written in C. 
- [Python tutorial](https://github.com/cossacklabs/hermes-core/wiki/Python-tutorial), where Hermes app is C-based, but client code runs on Python. 
- [Go tutorial](https://github.com/cossacklabs/hermes-core/wiki/Go-tutorial), where Hermes app is C-based, but client code runs on Go. 

# Licensing and commercial support

Hermes-core license is GNU Affero General Public License v3.0.

There is a separate, commercial licensed Hermes version for industrial use (its core crypto code is similar to this repository, yet it holds additional convenience interfaces and services). Commercial license can include custom cryptographic engineering (building cryptographic scheme based on Hermes for your use-case) and engineering support.

Drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com) if you are interested.

# Contacts

If you want to ask a technical question, feel free to raise an [issue](https://github.com/cossacklabs/hermes-core/issues) or write to [dev@cossacklabs.com](mailto:dev@cossacklabs.com).

To talk to the business wing of Cossack Labs Limited, drop us an email to [info@cossacklabs.com](mailto:info@cossacklabs.com).
   
[![Blog](https://img.shields.io/badge/blog-cossacklabs.com-7a7c98.svg)](https://cossacklabs.com/) [![Twitter CossackLabs](https://img.shields.io/badge/twitter-cossacklabs-fbb03b.svg)](http://twitter.com/cossacklabs) [![Medium CossackLabs](https://img.shields.io/badge/medium-%40cossacklabs-orange.svg)](https://medium.com/@cossacklabs/)
