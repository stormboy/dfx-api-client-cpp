# DFX API Client C++ Library

This is a C++ client library for the DeepAffex Cloud API. It contains basic
functionality, including:

* Registering a device
* User login
* Creating a measurement
* Subscribing to results
* Adding measurement data

This library requires a C++17 compiler and the Python-based
[Conan dependency](https://conan.io) tool along with some additional Python 3
packages for creating documentation. A
[Python Virtual Environment](https://packaging.python.org/guides/installing-using-pip-and-virtual-environments)
is recommended for managing the required Python dependencies.

There are different transports REST, WebSocket, gRPC which are supported based
on the type of server being connected to. The feature sets of those transports
are controlled based on user credentials and version, but as a minimum basic
measurement functionality is supported across all variants.

For instructions on building, refer to [BUILDING.md](BUILDING.md).
