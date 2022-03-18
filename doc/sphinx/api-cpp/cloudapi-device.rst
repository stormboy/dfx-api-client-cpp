=======
Devices
=======

Defined in ``dfx/api/DeviceAPI.hpp``

Used to manage the DeepAffex Devices endpoint.

.. code:: cpp

   CloudConfig config;
   auto status = dfx::api::loadCloudConfig(config);
   if ( status.OK() ) {
        std::shared_ptr<CloudAPI> cloud;
        status = CloudAPI::createInstance(config, cloud);
        if ( status.OK() ) {
            auto service = cloud->device(config);

            // List out all devices accessible to your user token
            std::vector<Device> devices;
            uint16_t totalCount;
            status = service->list(config, {}, 0, devices, totalCount);
            if ( status.OK() ) {
               for (const auto& device : devices) {
                   std::cout << device.id << std::endl;
               }
            }

        }
   }

------
Create
------

.. doxygenfunction:: dfx::api::DeviceAPI::create
   :project: CloudAPI

--------
Retrieve
--------

.. doxygenfunction:: dfx::api::DeviceAPI::list
   :project: CloudAPI

.. doxygenfunction:: dfx::api::DeviceAPI::retrieve
   :project: CloudAPI

.. doxygenfunction:: dfx::api::DeviceAPI::retrieveMultiple
   :project: CloudAPI

------
Update
------

.. doxygenfunction:: dfx::api::DeviceAPI::update
   :project: CloudAPI

------
Delete
------

.. doxygenfunction:: dfx::api::DeviceAPI::remove
   :project: CloudAPI

