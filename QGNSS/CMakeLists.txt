cmake_minimum_required (VERSION 3.8)
#项目名QGNSS-Box 版本xxxxx 语言c cxx
project (QGNSS2  VERSION 2.0.11 LANGUAGES C CXX)
#c++ 版本17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
#include 当前cmake根目录下所有头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# include cmake   配置.cmake 全局变量
set(cmakedir ${CMAKE_SOURCE_DIR}/cmake)
file(GLOB cmake_files ${CMAKE_SOURCE_DIR}/cmake/*.cmake )
foreach(citem ${cmake_files})
    include(${citem})
endforeach()

#Qt
option(QC_Build "Use Qt Creater " OFF)
if(QC_Build)
find_package( Qt5  REQUIRED COMPONENTS  Widgets Gui Core LinguistTools AxContainer WebEngineWidgets SerialPort  )
else()

set(CMAKE_PREFIX_PATH ${Qt5Dir}/lib/cmake)
find_package( Qt5  REQUIRED COMPONENTS  Widgets Gui Core LinguistTools AxContainer WebEngineWidgets SerialPort   )
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS Svg REQUIRED)
endif()

if(Qt5_FOUND)
message("Qt found ${Qt5_FOUND} ${Qt5_VERSION_MAJOR}")
endif()

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

# CMakeLists.txt
if(MSVC)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

#QGNNS cmake
add_subdirectory (submodule/qexcell)        #qt excell 封装
add_subdirectory (submodule/breakpad)   #异常闪退处理
add_subdirectory (submodule/QGNSS_LIB)      #核心库项目 rtcm nmea io 等库

add_subdirectory (submodule/ql_ntrip_server)

add_subdirectory (QGNSS)                    #QGNSS 主项目
