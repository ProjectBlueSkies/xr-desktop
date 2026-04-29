cmake_minimum_required(VERSION 3.16)
project(xr-imu-daemon VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-libusb)
pkg_check_modules(UDEV   REQUIRED libudev)
pkg_check_modules(EVDEV  REQUIRED libevdev)

add_executable(xr-imu-daemon
    src/main.c
    src/imu.c
    src/shm.c
)

target_include_directories(xr-imu-daemon PRIVATE
    ${HIDAPI_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
    ${EVDEV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/../shared
)

target_link_libraries(xr-imu-daemon
    ${HIDAPI_LIBRARIES}
    ${UDEV_LIBRARIES}
    ${EVDEV_LIBRARIES}
    m
)

install(TARGETS xr-imu-daemon DESTINATION bin)
install(FILES udev/60-xr-glasses.rules DESTINATION /etc/udev/rules.d)
