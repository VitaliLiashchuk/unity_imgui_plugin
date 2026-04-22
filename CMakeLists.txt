cmake_minimum_required(VERSION 3.20)
project(UnityImGuiPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------
# Thirdparty
# --------------------------------------------------------------------------
if(APPLE)
    add_subdirectory(thirdparty/metal_cpp)
endif()

include(cmake/imgui.cmake)

if(ANDROID)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_ANDROID_KHR)
    set(VOLK_PULL_IN_VULKAN OFF)        # we supply Vulkan headers ourselves
    add_subdirectory(thirdparty/volk)

    add_subdirectory(thirdparty/vma)    # creates VulkanMemoryAllocator (interface)
    add_library(vma ALIAS VulkanMemoryAllocator)
endif()

# --------------------------------------------------------------------------
# Plugin sources
# --------------------------------------------------------------------------
if(APPLE)
    file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Apple/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Apple/*.mm"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_SYSTEM_NAME}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_SYSTEM_NAME}/*.mm"
    )
    file(GLOB_RECURSE HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Apple/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${CMAKE_SYSTEM_NAME}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${CMAKE_SYSTEM_NAME}/*.h"
    )
else()
    file(GLOB_RECURSE SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${CMAKE_SYSTEM_NAME}/*.cpp"
    )
    file(GLOB_RECURSE HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${CMAKE_SYSTEM_NAME}/*.hpp"
    )
endif()

# iOS → STATIC (linked into Unity app bundle); everything else → SHARED
if(CMAKE_SYSTEM_NAME STREQUAL iOS)
    add_library(UnityImGuiPlugin STATIC ${SOURCES} ${HEADERS})
else()
    add_library(UnityImGuiPlugin SHARED ${SOURCES} ${HEADERS})
endif()

# --------------------------------------------------------------------------
# Common includes & links
# --------------------------------------------------------------------------
target_include_directories(UnityImGuiPlugin PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/common>
)

target_link_libraries(UnityImGuiPlugin PRIVATE imgui)

# --------------------------------------------------------------------------
# Platform-specific
# --------------------------------------------------------------------------
if(APPLE)
    target_include_directories(UnityImGuiPlugin PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Apple>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${CMAKE_SYSTEM_NAME}>
    )
    target_compile_definitions(UnityImGuiPlugin PRIVATE IMGUI_IMPL_METAL_CPP IMGUI_IMPL_METAL_CPP_EXTENSIONS)
    target_link_libraries(UnityImGuiPlugin PRIVATE MetalCpp)
    find_library(METAL_FW Metal REQUIRED)
    find_library(QUARTZCORE_FW QuartzCore REQUIRED)
    target_link_libraries(UnityImGuiPlugin PRIVATE ${METAL_FW} ${QUARTZCORE_FW})
    if(CMAKE_SYSTEM_NAME STREQUAL iOS)
        find_library(UIKIT_FW UIKit REQUIRED)
        target_link_libraries(UnityImGuiPlugin PRIVATE ${UIKIT_FW})
        set_target_properties(UnityImGuiPlugin PROPERTIES PREFIX "")
    else()
        find_library(APPKIT_FW AppKit REQUIRED)
        target_link_libraries(UnityImGuiPlugin PRIVATE ${APPKIT_FW})
        set_target_properties(UnityImGuiPlugin PROPERTIES PREFIX "")
    endif()

elseif(ANDROID)
    target_include_directories(UnityImGuiPlugin PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Android>
    )
    target_compile_definitions(UnityImGuiPlugin PRIVATE VK_NO_PROTOTYPES)
    target_link_libraries(UnityImGuiPlugin PRIVATE volk VulkanMemoryAllocator android log GLESv3 EGL)

elseif(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_include_directories(UnityImGuiPlugin PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Windows>
    )
    target_compile_definitions(UnityImGuiPlugin PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(UnityImGuiPlugin PRIVATE d3d11 d3d12 dxgi)
    set_target_properties(UnityImGuiPlugin PROPERTIES DEBUG_POSTFIX "")
endif()

# --------------------------------------------------------------------------
# Install → package/Plugins/<platform>/  (UPM package)
# --------------------------------------------------------------------------
set(UNITY_PACKAGE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/package")

if(CMAKE_SYSTEM_NAME STREQUAL iOS)
    set(UNITY_PLUGIN_DEST "${UNITY_PACKAGE_ROOT}/Plugins/iOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(UNITY_PLUGIN_DEST "${UNITY_PACKAGE_ROOT}/Plugins/Darwin")
elseif(ANDROID)
    set(UNITY_PLUGIN_DEST "${CMAKE_CURRENT_SOURCE_DIR}/build/android_aar/jni/${ANDROID_ABI}")
elseif(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(UNITY_PLUGIN_DEST "${UNITY_PACKAGE_ROOT}/Plugins/Windows/x86_64")
endif()

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    install(TARGETS UnityImGuiPlugin
        RUNTIME DESTINATION "${UNITY_PLUGIN_DEST}"
    )
else()
    install(TARGETS UnityImGuiPlugin
        LIBRARY DESTINATION "${UNITY_PLUGIN_DEST}"
        ARCHIVE DESTINATION "${UNITY_PLUGIN_DEST}"
        RUNTIME DESTINATION "${UNITY_PLUGIN_DEST}"
    )
endif()
