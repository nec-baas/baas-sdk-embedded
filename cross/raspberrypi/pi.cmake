set(RASPBERRYPI_ROOT $ENV{HOME}/raspberrypi)
set(RASPBERRYPI_TOOLCHAIN ${RASPBERRYPI_ROOT}/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER ${RASPBERRYPI_TOOLCHAIN}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${RASPBERRYPI_TOOLCHAIN}/bin/arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH ${RASPBERRYPI_ROOT}/rootfs)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
include_directories(${CMAKE_FIND_ROOT_PATH}/usr/include/) 

