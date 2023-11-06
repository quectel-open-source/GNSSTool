file(GLOB src
		 ${CMAKE_SOURCE_DIR}/*.cpp
		 *.cpp
		 *.c
		 uart/*cpp
		 st/*.cpp
		 st/teseo5/*.cpp
)

if(RCFile AND MSVC)
list(APPEND src ${RCFile})
endif()

add_executable ("${PROJECT_NAME}-exe" ${src})
add_compile_definitions(BUILDSHARED)

add_library(${PROJECT_NAME} SHARED ${src})
