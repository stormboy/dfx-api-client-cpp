Testing
=======

Testing is done by the test/CMakeLists.txt using Google Test.

Due to the nature of the API function available on different transports, the tests
are written to GTEST_SKIP() when something is unavailable on a particular transport
and the message return should indicate why a test was skipped.

.. code:: bash

   just build           # Build the executable build/test/test-cloud-api
   just test            # Run test-cloud-api using ~/.dfxcloud.yaml config
   just test joe-rest   # Test using ~/.dfxcloud.yaml with context=joe-rest

For even more options like selective running with standard Google Test
filters, invoke the executable directly.

.. code:: bash

   just build
   cd build/test                              # Folder where built test executable resides
   ./test-cloud-api --help                    # Provide all help available
   ./test-cloud-api --gtest_list_tests        # List all available Google Tests
   ./test-cloud-api --gtest_filter=DeviceTests.\*    # Run all device tests
   ./test-cloud-api -c joe-rest --gtest_filter=DeviceTests.\*






