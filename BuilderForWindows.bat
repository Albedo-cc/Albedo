::cmake --help
::cmake --version
::cmake -C <initial-cache-script> <path-to-source>
::cmake -D <var>[:<type>]=<value> <path-to-source> (Tips>> for build-in varibles, do not specify the type.)

cmake -S ./ -B ./build -D CMAKE_BUILD_TYPE=Debug