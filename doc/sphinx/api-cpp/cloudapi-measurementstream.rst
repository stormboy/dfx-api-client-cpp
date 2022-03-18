Measurement Creation
====================

Defined in ``dfx/api/MeasurementStreamAPI.hpp``

Measurements are created using a MeasurementStream supported by both the WebSocket and gRPC transports.

-------------
Sending Chunk
-------------

With a valid Measurement obtained from ``dfx::api::MeasurementStreamAPI::setupStream`` payload chunks can
be processed by the DFX Server.

.. code:: cpp

   std::shared_ptr<MeasurementStreamAPI> stream = cloud->measurementStream(config);
   auto status = stream->setupStream(config, config.studyID);
    if ( status.OK() ) {
       stream->sendChunk(config, payloadBytes, isLast);
   }

.. doxygenfunction:: dfx::api::MeasurementStreamAPI::sendChunk

----------------------
Measurement Completion
----------------------

The proper way to complete a measurement is by identifying the last payload chunk in the
``sendChunk`` as true which will properly notify the server that it is processing the last
chunk of a measurement.

After the final chunk has been sent though there will be a brief period while the server
receives and finishes processing this final chunk. The best way to handle is to use the
``waitForCompletion`` method.

.. code:: cpp

   bool isLast = true;
   auto status = stream->sendChunk(config, payloadBytes, isLast);

   status = stream->waitForCompletion(config, 30*1000);    // Wait for up to 30 seconds
   if ( status == CLOUD_TIMEOUT ) {
       stream->cancel(config);                             // Force the connection down
   }

.. doxygenfunction:: dfx::api::MeasurementStreamAPI::waitForCompletion

----------------------
Measurement Cancelling
----------------------

It is possible to cancel a measurement which will notify the server and shutdown the
internal asynchronous thread processing providing a proper cleanup of measurement
resources.

.. code:: cpp

   stream->cancel(config);

.. doxygenfunction:: dfx::api::MeasurementStreamAPI::cancel

..
   doxygenclass:: dfx::api::MeasurementStreamAPI
