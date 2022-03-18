============
Measurements
============

Defined in ``dfx/api/MeasurementAPI.hpp``

Used to manage the DeepAffex Measurements endpoint.

.. note::
   Measurements are to manage existing measurements to create a new measurement, use *MeasurementStream* service.

.. code:: cpp

   CloudConfig config;
   auto status = dfx::api::loadCloudConfig(config);
   if ( status.OK() ) {
        std::shared_ptr<CloudAPI> cloud;
        status = CloudAPI::createInstance(config, cloud);
        if ( status.OK() ) {
            auto service = cloud->measurements(config);

            // List out all measurements accessible to your user token
            std::vector<Measurement> measurements;
            uint16_t totalCount;
            status = service->list(config, {}, 0, measurements, totalCount);
            if ( status.OK() ) {
               for (const auto& measurement : measurements) {
                   std::cout << measurement.id << std::endl;
               }
            }
        }
   }

--------
Retrieve
--------

.. doxygenfunction:: dfx::api::MeasurementAPI::list
   :project: CloudAPI

.. doxygenfunction:: dfx::api::MeasurementAPI::retrieve
   :project: CloudAPI

.. doxygenfunction:: dfx::api::MeasurementAPI::retrieveMultiple
   :project: CloudAPI
