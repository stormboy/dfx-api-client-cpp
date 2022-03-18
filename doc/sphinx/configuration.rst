.. include:: <isonum.txt>

=============
Configuration
=============

To communicate with the DeepAffex\ |trade| Cloud services requires a `<CloudConfig>`_ when constructing
the `<CloudAPI>`_ instance.

There are a couple helpers for managing the `<CloudConfig>`_ but it is just a structure of properties,
some of which are optional or only used in certain contexts so can be populated manually and these facilities
are provided to ease adoption.

.. important::

   The password and license fields should be managed in a secure fashion and while these examples show
   a convenient way to manage for development, a production usage would provide better security around
   the credentials.

The YAML format of the `dfxcloud.yaml` file is designed so it can be used in both simple single server
configuration scenarios and more complex scenarios involving multiple servers and credentials.

.. code:: bash

   # Sample simple ~/.dfxcloud.yaml also available with: dfxcli config sample
   auth-email=joe@somewhere.com       # Provided during sign-up
   auth-password=ODsuB0vsHFM70zi      # Provided during sign-up
   auth-org=somewhere                 # Provided during sign-up
   license=k1qURX13BH66LAk            # Provided during sign-up
   host=https://api.deepaffex.ai
   port=443
   study-id=1a815f20-44cb-4f05-b04c-edea380ab0c1
   transport-type=WEBSOCKET

This YAML file can be loaded with the following code:

.. code:: cpp

   CloudConfig config;
   auto result = dfx::api::loadCloudConfig(config, "~/.dfxcloud.yaml")
   if ( result != CloudStatus::OK ) {
         // Failure for some reason
   }

Alternatively, this could be manually constructed in source:

.. code:: cpp

   CloudConfig config;
   config.authEmail="joe@somewhere.com";
   config.authPassword="ODsuB0vsHFM70zi";
   config.authOrg="somewhere";
   config.license="k1qURX13BH66LAk";
   config.serverHost="https://api.deepaffex.ai";
   config.serverPort=443;
   config.studyID="1a815f20-44cb-4f05-b04c-edea380ab0c1";
   config.transportType=CloudAPI::TRANSPORT_TYPE_WEBSOCKET;

In more advanced scenarios, the `dfxcloud.yaml` may contain multiple server and user credential
configurations which are identified by the `context` key. It achieves this by processing the file
content in multiple passes. For instance, a configuration file can manage separate `REST` or
`WebSocket` contexts which are specified prior to loading. This is how the `dfxcli` tool handles
the `-c` context option to switch targets.

.. code:: bash

   # Show example of advanced multi-context YAML format
   dfxcli config sample -a

.. code:: cpp

   CloudConfig config;
   config.context = "joe-rest";    // Identify a context key to load
   auto result = dfx::api::loadCloudConfig(config, "~/.dfxcloud.yaml")
   if ( result != CloudStatus::OK ) {
         // Failure for some reason
   }










