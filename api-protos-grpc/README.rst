===============
api-protos-grpc
===============

This folder will produce an object library with the compiled protobuf definitions.

.. important::
   The \*.protos come from the Conan package *dfxpublicprotos* which is done to avoid embedding the
   external sources within this repository. It also enables accurately tracking exactly which version
   of the protobufs are being used and simplifies updating.

-------
Outputs
-------

Depending upon what options are enabled on the build, the object library will include different features:

+-------------------+--------------------------------------------------+
| Option            |        Description                               |
+===================+==================================================+
|                   | Protobufs are Always generated                   |
+-------------------+--------------------------------------------------+
| WITH_GRPC         | Generate gRPC sources                            |
+-------------------+--------------------------------------------------+
| WITH_TESTS        | Generate gRPC gMock stubs                        |
+-------------------+--------------------------------------------------+


- protobufs for ALL services will be built
- if WITH_GRPC option enabled, gRPC protobufs will be built
- if WITH_TESTS option enabled, gRPC gMock stubs will be built for testing
