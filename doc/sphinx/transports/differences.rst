
====================
Protocol Differences
====================

The primary differences between working with the web
(REST and WebSocket) and gRPC protocol implements.

1. REST simple command line usages are possible (with curl), however it's lack
   of streaming prevents the ability to create a new measurement. Communication is
   performed using JSON serialization.
2. WebSocket are more complicated but is required to create a measurement
   stream. Outside of the measurement stream and performance, WebSockets are roughly
   the same as the REST API but data is serialized over Protobuf.
3. gRPC is a limited support API utilized by *standalone devices* which unlike
   the cloud are intended strictly for processing without storage retention.
