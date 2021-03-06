#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | ETAGI                                                                     |
#  | Software description here                                                 |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ 

project(Galileo)

# --------------------------------------
#  set variables for compilation 
# --------------------------------------
# select all headers in the include dir
FILE(GLOB myProject_HEADERS "${PROJECT_SOURCE_DIR}/*.h")   
# select all sources in the include dir
FILE(GLOB myProject_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")  
# select all files for the graphical interface
FILE(GLOB myProject_FORMS "${PROJECT_SOURCE_DIR}/*.ui")  
# select all files for the resources
FILE(GLOB myProject_UI_RESOURCES "${PROJECT_SOURCE_DIR}/*.qrc")

SET (ENABLE_WINDEPLOYQT 0 CACHE BOOL "Set to 1 to activate deployment of QT apps, 0 by default" )


# resource file for icon
IF(WIN32)
	FILE(GLOB myProject_RESOURCES "${PROJECT_SOURCE_DIR}/*.rc")
ENDIF()
	
# this is necessary to find in the code the ui____.h file built in the build folder
include_directories(${PROJECT_BINARY_DIR}) 


# copy icons and configuration files to the building folder
if(MSVC)
		

  FILE(GLOB myProject_SETTINGS "${PROJECT_SOURCE_DIR}/settings/*.ini") 
  FOREACH(F ${myProject_SETTINGS})
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/settings/)
	file(COPY "${F}" DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/settings/)
  ENDFOREACH(F)	
  
endif()


# --------------------------------------
#  add definitions
# --------------------------------------
add_definitions (${QT_DEFINITIONS})   #required to use QT and gui
add_definitions (-DSOFTWARE_VERSION=${SOFTWARE_VERSION})   #required to use QT and gui

# Add options to build specific targets
option(BUILD_QT5 "Build support with QT 5" "ON") #this will be removed as qt4 will be no longer supported
if(NOT BUILD_QT5)

  message (FATAL_ERROR  "QT 4 does not support serial port communication - cannot build ${PROJECT_NAME}")

elseif(  BUILD_QT5)

	# allows automatic linking of QT - see http://www.cmake.org/cmake/help/v3.0/policy/CMP0020.html
	cmake_policy(SET CMP0020 NEW)
	
	# Find the QtWidgets library ---- really weird way of find libraries ---TODO solve this ! 
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Core REQUIRED )
	find_package( Qt5Gui REQUIRED ) 
	find_package( Qt5SerialPort REQUIRED )
	find_package( Qt5OpenGL REQUIRED )
	find_package( Qt5Charts REQUIRED )
	find_package( Qt5PrintSupport REQUIRED )
	
	
	#	find_package (Qt5Concurrent REQUIRED)

	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: QT5_BINARY_DIR ---- ${QT5_BINARY_DIR}")
		message (STATUS "${PROJECT_NAME} MESSAGE: QT5_LIBRARIES_DIR ---- ${QT5_LIBRARIES_DIR}")
		message (STATUS "${PROJECT_NAME} MESSAGE: Qt5Widgets ---- ${Qt5Widgets_LIBRARIES}")
		message (STATUS "${PROJECT_NAME} MESSAGE: Qt5Charts ---- ${Qt5Charts_LIBRARIES}")
	endif ( ENABLE_verbose )
	if (NOT QT5_LIBRARIES_DIR)
                message(WARNING "${PROJECT_NAME} MESSAGE: QT5_LIBRARIES_DIR not found --- set namually")
	endif()
	link_directories(${QT_LIBRARY_DIR})
	link_directories(${QT5_LIBRARIES_DIR})

	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)
	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	 
	QT5_WRAP_UI(myProject_FORMS_HEADERS ${myProject_FORMS})
	
	#this allows the executable to encode all the icons 
    QT5_ADD_RESOURCES(UI_RESOURCES_RCC ${myProject_UI_RESOURCES})
	
	if ( ENABLE_verbose )
		message (STATUS "${PROJECT_NAME} MESSAGE: myProject_FORMS_HEADERS  :: ${myProject_FORMS_HEADERS} ")
	endif ( ENABLE_verbose )

	add_executable (${PROJECT_NAME} ${myProject_SOURCES} ${myProject_HEADERS}
									${myProject_FORMS_HEADERS} ${UI_RESOURCES_RCC} ${myProject_RESOURCES})
									#${myProject_HEADERS_MOC})#${myProject_FORMS} )
	qt5_use_modules(${PROJECT_NAME} Widgets Charts PrintSupport)

	target_link_libraries ( ${PROJECT_NAME}  
							${QT_LIBRARIES} ${Qt5SerialPort_LIBRARIES} Qt5::PrintSupport) 


	# differently than others, QT has the windeployqt file that takes care of dlls and other stuff
	# see this post: http://stackoverflow.com/questions/36981942/how-to-use-add-custom-command-to-copy-dlls-using-generator-expressions
	# Windows specific build steps
	if(ENABLE_WINDEPLOYQT AND WIN32)
			# Run winddeployqt if it can be found
		find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT5_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
		if ( ENABLE_verbose )
			message (STATUS "${PROJECT_NAME} MESSAGE: WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
			message (STATUS "${PROJECT_NAME} MESSAGE: Current file target is : ${F} \n")
		endif ( ENABLE_verbose )
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> --no-translations COMMENT "Preparing Qt runtime dependencies")
	endif()
  
	# allows folders for MSVC
	if (MSVC AND ENABLE_SOLUTION_FOLDERS) 
		set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Apps")
	endif (MSVC AND ENABLE_SOLUTION_FOLDERS)

endif()

