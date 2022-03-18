CloudAPI
========

Defined in ``dfx/api/CloudAPI.hpp``

The ``CloudAPI`` will construct a transport based on what is available in terms of both
this client library and what the server is capable of supporting. It attempts to create
based on the priority of gRPC, WebSocket, REST but it can be forced using the CloudConfig
transport value.

Almost all calls will result in a CloudStatus result code which should be verified to be
``OK``.

The API follows a standard CRUD model for operations leveraging Create, Retrieve,
Update, Delete style operations.

.. code::

   // Populate the CloudConfig structure
   CloudConfig config;
   auto status = dfx::api::loadCloudConfig(config);
   if ( status.OK() ) {

        // Create the instance
        std::shared_ptr<CloudAPI> cloud;
        status = CloudAPI::createInstance(config, cloud);
        if ( status.OK() ) {

            // Use the cloud instance to obtain services
            // ..
        }
   }

..
   doxygenclass:: dfx::api::CloudAPI
   :members:


.. toctree::
   :maxdepth: 2

   cloudapi-device
   cloudapi-measurement
   cloudapi-measurementstream
   cloudapi-organization
   cloudapi-profile
   cloudapi-signal
   cloudapi-study
   cloudapi-user
