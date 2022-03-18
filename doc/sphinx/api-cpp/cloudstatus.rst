CloudStatus
===========

Defined in ``dfx/api/CloudStatus.hpp``

Every operation returns a ``CloudStatus`` to indicate if it
was successful ``CLOUD_OK`` or if it failed for some other
reason.

.. code:: cpp

   class CloudStatus {
   public:
     // ... omitted ..
     dfx_status_code code;
     std::string message;
     int protocolErrorCode;
     std::string protocolErrorMessage;
   };

The CloudStatus contains both an enumerated code along with
a textual message. It may also contain a protocolErrorCode
and associated message which may provide more details
about the underlying error if available.

The typical use is:

.. code:: cpp

   auto status = service->somecall();
   if ( !status.OK() ) {
      std::cerr << "Error: " << dfx::api::ToString(status.code) << std::endl;
      std::cerr << "Message: " << status.message << std::endl;
   }

Please refer to the ``dfx/api/CloudStatus.hpp`` for details
on the various enumerated return codes.