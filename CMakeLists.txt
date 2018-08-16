#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |                                                                           |
#  | Software description here                                                 |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(Didacti-coop)   #      

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

option(ENABLE_verbose "Verbose cmake cofiguration " "ON")
if ( ENABLE_verbose )
	message(WARNING " Verbose configuration active ! You may get a lot of messages now ... " )
endif ( ENABLE_verbose )

#cmake tweaks
if(COMMAND cmake_policy) #return true if the COMMAND cmake_policy exist - this allows compatibility with older versions
	# We don't want to mix relative and absolute paths in linker lib lists.
	cmake_policy(SET CMP0003 NEW) # Required by CMake 2.7+
	
	# avoid link warnings - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0015.html
	cmake_policy(SET CMP0015 NEW)
	
	#Ignore COMPILE_DEFINITIONS_<Config> properties - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0043.html
	cmake_policy(SET CMP0043 NEW) 
	
	#The target in the TARGET signature of add_custom_command() must exist - see https://cmake.org/cmake/help/v3.0/policy/CMP0040.html
	cmake_policy(SET CMP0040 NEW)
	
	# Allow using the LOCATION target property.
	if( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)
		cmake_policy(SET CMP0026 NEW)
	#else (NOT "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)
	#	cmake_policy(SET CMP0026 OLD)
	endif( "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 3.0)

	# pass to the installer the project name
	cmake_policy(SET CMP0053 OLD) 

endif(COMMAND cmake_policy)

# Our own CMake configurations
include(${PROJECT_SOURCE_DIR}/CMakeModules/script_init_cmake.cmake REQUIRED)		

include(${CMAKE_MODULE_PATH}/UtilsMacros.cmake REQUIRED)		        # CMake macros

set (Fluicell_SOURCE_DIR ${PROJECT_SOURCE_DIR})



#  Add option enable visual studio to create folders
option(ENABLE_SOLUTION_FOLDERS "Enable folders for MSVC - ON by default if MSVC - OFF otherwise" ON)
if (MSVC)
  set (ENABLE_SOLUTION_FOLDERS 1)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else (MSVC)
  set (ENABLE_SOLUTION_FOLDERS 0)
endif (MSVC) 


# -------------------- apps --------------------------------
OPTION(ENABLE_BUILD_apps "Option for building apps - ON by default" ON)
if (ENABLE_BUILD_apps)
	add_subdirectory(apps)
endif(ENABLE_BUILD_apps)

# --------------------------------------
#  Copy useful resources to the build folder
# --------------------------------------					
# file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)


# --------------------------------------
#  settings for install and package --- not fully supported yet
# --------------------------------------					
OPTION(ENABLE_BUILD_install "Option to add the install target - ON by default" ON)
if (ENABLE_BUILD_install)
	include(${PROJECT_SOURCE_DIR}/CMakeModules/script_BuildInstall.cmake REQUIRED)		# CMake configuration for the install target
endif(ENABLE_BUILD_install)

OPTION(ENABLE_BUILD_package "Option for building the package - ON by default" ON)
if (ENABLE_BUILD_package)
	include(${PROJECT_SOURCE_DIR}/CMakeModules/script_BuildPackage.cmake REQUIRED)		# CMake configuration for package						
endif(ENABLE_BUILD_package)
