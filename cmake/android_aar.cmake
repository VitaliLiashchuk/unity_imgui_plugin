# cmake -P cmake/android_aar.cmake
# Run from repo root after installing all Android ABIs.
# Expects: package/Plugins/Android/jni/<abi>/libUnityImGuiPlugin.so

cmake_minimum_required(VERSION 3.20)

set(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(STAGING_DIR "${REPO_ROOT}/build/android_aar")
set(OUT_DIR "${REPO_ROOT}/package/Plugins/Android")
set(AAR_NAME "UnityImGuiPlugin.aar")
set(MANIFEST_SRC "${REPO_ROOT}/android/AndroidManifest.xml")

find_program(ZIP zip REQUIRED)

if(NOT EXISTS "${STAGING_DIR}/jni")
    message(FATAL_ERROR "No jni/ found in ${STAGING_DIR}. Build and install at least one Android ABI first.")
endif()

# Copy manifest into staging dir
file(COPY "${MANIFEST_SRC}" DESTINATION "${STAGING_DIR}")

# Ensure output dir exists
file(MAKE_DIRECTORY "${OUT_DIR}")

# Remove old AAR
file(REMOVE "${OUT_DIR}/${AAR_NAME}")

# Pack AAR from staging dir, output directly to package/Plugins/Android/
execute_process(
    COMMAND ${ZIP} -r "${OUT_DIR}/${AAR_NAME}" "AndroidManifest.xml" "jni/"
    WORKING_DIRECTORY "${STAGING_DIR}"
    RESULT_VARIABLE ZIP_RESULT
)

if(NOT ZIP_RESULT EQUAL 0)
    message(FATAL_ERROR "zip failed with exit code ${ZIP_RESULT}")
endif()

message(STATUS "AAR created: ${OUT_DIR}/${AAR_NAME}")
