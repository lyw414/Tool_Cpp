cmake_minimum_required(VERSION 3.4)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
endif()
 
#set(project_headers ./../json.hpp ./nlohmann/json.hpp) 
file(GLOB_RECURSE project_headers ./../*.hpp)
file(GLOB_RECURSE project_cpps ./../*.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group(${project_headers})
source_group(${project_cpps})
#source_group(${all_files})
message("print header files: ${project_headers}") 
message("print resource files: ${project_cpps}") 
message("print all files:  ${all_files}") 
add_executable(json
		 "${project_headers}"
		 "${project_cpps}")


