.. include:: <isonum.txt>

About
=====

**DFX Cloud** provides a library for communicating with the DeepAffex\ |trade| Affective
Intelligence Cloud Engine.

Communication is handled using the DeepAffex\ |trade| json/protobufs payloads which are used for the REST,
WebSocket and gRPC transport layers. This library wraps all three into one library enabling easy migration
between the various backend transports. The *primary goal* is to facilitate Measurement capabilities,
secondary is to provide some of the additional ancillary endpoints.

.. warning::
   1. Not all transports implement all features
   2. Not all platforms which the library exists can leverage all transports

**DFX Cloud** is written as independent transports to facility mobile and embedded/constrained
devices which do not wish to include the baggage for unused transports. Using different compilation options,
the library built can be reduced to the transport of interest while maintaining the flexibility of
later swapping libraries to get additional transports.

It is also recognized that this library will be leveraged by two types of applications:

   1. A basic measurement only application which is only concerned with performing a measurement
      and getting a result back from the DeepAffex\ |trade| cloud servers.
   2. A more feature oriented client which wishes to access all of the rich features that
      the DeepAffex\ |trade| cloud API offers.