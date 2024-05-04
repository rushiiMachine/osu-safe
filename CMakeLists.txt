cmake_minimum_required(VERSION 3.25)
project(osu_safe C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/FindWDK/cmake")
find_package(WDK REQUIRED)

set(CMAKE_C_STANDARD 23)
string(APPEND CMAKE_C_FLAGS " /Wall /wd5045")

wdk_add_driver(osu-safe
        KMDF 1.15 # Windows 10 WDK
        WINVER 0x0A00 # Windows 10
        NTDDI_VERSION 0x0A000000 # Windows 10 1507 "Threshold"

        driver.c
        filter.c
        util.c
)

target_link_libraries(osu-safe WDK::FLTMGR)
