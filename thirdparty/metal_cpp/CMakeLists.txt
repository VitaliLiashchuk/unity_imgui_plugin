project(MetalCpp)

add_library(MetalCpp OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/definition.cpp)

target_include_directories(MetalCpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

find_library(METAL_LIBRARY Metal REQUIRED)
target_link_libraries(MetalCpp PUBLIC ${METAL_LIBRARY})

if (CMAKE_SYSTEM_NAME STREQUAL Darwin)
    find_library(APPKIT_LIBRARY AppKit REQUIRED)
    target_link_libraries(MetalCpp PUBLIC ${APPKIT_LIBRARY})
endif ()
