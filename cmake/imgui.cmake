set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_android.cpp
        ${IMGUI_DIR}/backends/imgui_impl_android.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL iOS)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_metal.mm
        ${IMGUI_DIR}/backends/imgui_impl_metal.h
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_osx.mm
        ${IMGUI_DIR}/backends/imgui_impl_osx.h
        ${IMGUI_DIR}/backends/imgui_impl_metal.mm
        ${IMGUI_DIR}/backends/imgui_impl_metal.h
    )
elseif(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_win32.h
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx11.h
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx12.h
        ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
    )
endif()

add_library(imgui OBJECT ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_compile_definitions(imgui PUBLIC
        IMGUI_IMPL_VULKAN_NO_PROTOTYPES
        IMGUI_IMPL_OPENGL_ES3
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL iOS)
    target_compile_definitions(imgui PUBLIC
        IMGUI_IMPL_METAL_CPP
        IMGUI_IMPL_METAL_CPP_EXTENSIONS
    )
    target_link_libraries(imgui PUBLIC MetalCpp)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_compile_definitions(imgui PUBLIC
        IMGUI_IMPL_METAL_CPP
        IMGUI_IMPL_METAL_CPP_EXTENSIONS
    )
    find_library(GAME_CONTROLLER_FW GameController REQUIRED)
    target_link_libraries(imgui PUBLIC ${GAME_CONTROLLER_FW} MetalCpp)
endif()
