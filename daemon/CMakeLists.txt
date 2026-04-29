cmake_minimum_required(VERSION 3.16)
project(xr-imu-daemon VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_path(UDEV_INCLUDE_DIR libudev.h)
find_library(UDEV_LIBRARY udev)
if(NOT UDEV_INCLUDE_DIR OR NOT UDEV_LIBRARY)
    message(FATAL_ERROR "libudev not found — install libudev-dev")
endif()

set(VITURE_SDK_DIR     ${CMAKE_SOURCE_DIR}/upstream/lib/x86_64/viture)
set(VITURE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/upstream/include/sdks)

# ── daemon ────────────────────────────────────────────────────────────────────

add_executable(xr-imu-daemon
    src/main.c
    src/imu.c
    src/shm.c
)

target_include_directories(xr-imu-daemon PRIVATE
    ${UDEV_INCLUDE_DIR}
    ${VITURE_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/../shared
)

target_link_libraries(xr-imu-daemon PRIVATE
    ${UDEV_LIBRARY}
    ${VITURE_SDK_DIR}/libglasses.so
    rt
    m
)

# Embed RPATH so the binary can find libglasses.so at both build and install time
set_target_properties(xr-imu-daemon PROPERTIES
    BUILD_RPATH  "${VITURE_SDK_DIR}"
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/xr-desktop"
)

# ── imu-test ──────────────────────────────────────────────────────────────────

add_executable(imu-test tools/imu-test.c)
target_include_directories(imu-test PRIVATE ${CMAKE_SOURCE_DIR}/../shared)
target_link_libraries(imu-test PRIVATE rt)

# ── install ───────────────────────────────────────────────────────────────────

install(TARGETS xr-imu-daemon DESTINATION bin)
install(FILES ${VITURE_SDK_DIR}/libglasses.so
        DESTINATION lib/xr-desktop)
install(FILES udev/60-xr-glasses.rules
        DESTINATION /etc/udev/rules.d)
