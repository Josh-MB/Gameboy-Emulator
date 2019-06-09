cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(gb_emu VERSION 0.1 LANGUAGES CXX)

# Prevent in source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)

include(GNUInstallDirs)

# Function for unrolling directory structure into filters in VS
function(gm_emu_getDirList curDir result)
	file(GLOB children RELATIVE ${curDir} ${curDir}/*)
	set(dirList "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curDir}/${child})
			list(APPEND dirList ${child})
		endif()
	endforeach()
	set(${result} ${dirList} PARENT_SCOPE)
endfunction()

# Set up output binary directory
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)
if(UNIX)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin/Debug_linux)
	else()
		set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin/Release_linux)
	endif()
endif()
message(STATUS "Output dir: ${OUTPUT_DIR}")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

# Preprocessor definitions
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32 -D_CRT_SECURE_NO_WARNINGS")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_UNIX")
endif()


# Find libraries (use modules to help find)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

#set(SFML_STATIC_LIBRARIES TRUE)
#if(WIN32)
#	find_package(Opengl)
#	set(SFML_ROOT F:/Programming/Libraries/SFML-2.3.2)
#elseif(UNIX)
	#FindOpenGL()
	#set(SFML_ROOT ${HOME}/Libraries/SFML-2.3.2)
#endif()

#find_package(SFML COMPONENTS graphics window system)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

SET(GSL_INCLUDE_DIR "" CACHE PATH "Path to gsl")
if(NOT EXISTS "${GSL_INCLUDE_DIR}/gsl")
	message(SEND_ERROR "Can't find gsl in ${GSL_INCLUDE_DIR}")
else()
	message("-- Found gsl")
endif()
INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})

enable_testing()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(src_suffix "src")
set(include_suffix "include")

set(suffixes "${src_suffix};${include_suffix}")

set(fileList "")
foreach(suffix ${suffixes})
	set(EMUDIR_${suffix} "${CMAKE_CURRENT_SOURCE_DIR}/${suffix}")
	gm_emu_getDirList(${EMUDIR_${suffix}} "${suffix}Dirs")

	foreach(dir ${${suffix}Dirs})
		file(GLOB_RECURSE "${suffix}_${dir}_files"
			"${EMUDIR_${suffix}}/${dir}/*")
		list(APPEND fileList ${${suffix}_${dir}_files})
		source_group(${suffix}\\${dir} FILES ${${suffix}_${dir}_files})
	endforeach()
endforeach()

file(GLOB HPP_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB SRC_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/*cpp")

add_executable(gb_emu
	${SRC_MAIN}
	${HPP_MAIN}
	${fileList}
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT gb_emu)

source_group("src" FILES ${SRC_MAIN})
source_group("include" FILES ${HPP_MAIN})

target_include_directories(gb_emu
	PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty_include)
#	PRIVATE ${SFML_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/thirdparty_include)

target_link_libraries(gb_emu ${SDL2_LIBRARIES})

if(WIN32)
	set_target_properties(gb_emu PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/\$(Configuration)")
endif()