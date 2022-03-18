=======
Studies
=======

Defined in ``dfx/api/StudyAPI.hpp``

Studies are organized segments of analyses that guide the measurement process. Studies consist of Types,
Templates and Assets. A study type is a general, high-level grouping for studies. A template is a
pre-built study standard createdEpochSeconds and made available by the Nuralogix team. Templates are version
controlled and named for use with a study.

..
   doxygenstruct:: dfx::api::Study

------
Create
------

Creates a new study within an organization.

.. doxygenfunction:: dfx::api::StudyAPI::create

--------
Retrieve
--------

.. doxygenfunction:: dfx::api::StudyAPI::list

.. doxygenfunction:: dfx::api::StudyAPI::retrieve

.. doxygenfunction:: dfx::api::StudyAPI::retrieveMultiple

------
Update
------

.. doxygenfunction:: dfx::api::StudyAPI::update

------
Delete
------

.. doxygenfunction:: dfx::api::StudyAPI::remove

