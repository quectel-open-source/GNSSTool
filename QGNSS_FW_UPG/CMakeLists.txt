
cmake_minimum_required (VERSION 3.24)
project (QGNSS_FW_UPG VERSION 1.0.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#add debug info
add_compile_definitions(QG_OUTPUT_DEBUG)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/QGNSS_FW_UPG/uart
	${CMAKE_CURRENT_SOURCE_DIR}/QGNSS_FW_UPG/st
	${CMAKE_CURRENT_SOURCE_DIR}/QGNSS_FW_UPG/st/teseo5
)
message("uart path ${CMAKE_CURRENT_SOURCE_DIR}")
#config version
include(Version.cmake)
set(inipath "${CMAKE_CURRENT_BINARY_DIR}/version.h.in")
set(rcpath "${CMAKE_CURRENT_BINARY_DIR}/version.rc.in")
if(EXISTS ${inipath} AND EXISTS ${rcpath})
message("path exists ${inipath}")
message("path exists ${rcpath}")
set(COMPANY_NAME "Quectel" )
set(PRODUCT_NAME    ${PROJECT_NAME})
set(LegalCopyRight  "Copyright(C)  2023")
CMake_Version(${inipath} ${rcpath})
set(RCFile ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
else()
message("path not exists ${inipath}")
endif()

#qgnss_fw_upg cmake
add_subdirectory (QGNSS_FW_UPG)


get_target_property(subdir ${PROJECT_NAME}  SUBDIRECTORIES)
message("SUBDIRECTORIES ${subdir}")
get_property(cmakesubdir DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY SUBDIRECTORIES)
message("cmakesubdir ${cmakesubdir}")