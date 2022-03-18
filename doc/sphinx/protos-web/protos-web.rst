Web Protobufs
=============

The ``CloudAPI`` will construct a transport based on what is available in terms of both
this client library and what the server is capable of supporting. It attempts to create
based on the priority of gRPC, WebSocket, REST but it can be forced using the CloudConfig
transport value.

Almost all calls will result in a CloudStatus result code which should be verified to be
``OK``.

.. toctree::
   :maxdepth: 2

   protos-web-device
