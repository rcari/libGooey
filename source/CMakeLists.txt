CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

INCLUDE( build/ModuleInfos.cmake )

PROJECT ( Gooey C CXX )

INCLUDE_DIRECTORIES( ${GOOEY_INCLUDES} )

### ------------- Gooey -------------
### Source files
SET( Gooey_SRCS
	src/GooeyApplication.cpp
	src/GooeyEngine.cpp
)
INCLUDE( src/commands/sources.cmake )
INCLUDE( src/common/sources.cmake )
INCLUDE( src/layout/sources.cmake )
INCLUDE( src/views/sources.cmake )
INCLUDE( src/widgets/sources.cmake )
INCLUDE( src/windows/sources.cmake )

### Header files
SET( Gooey_HDRS
	inc/GooeyApplication.hpp
	inc/GooeyExport.hpp
)
### Qt mocable header files
SET( Gooey_MOC_HDRS
	inc/GooeyEngine.hpp
)
INCLUDE( inc/commands/headers.cmake )
INCLUDE( inc/common/headers.cmake )
INCLUDE( inc/layout/headers.cmake )
INCLUDE( inc/views/headers.cmake )
INCLUDE( inc/widgets/headers.cmake )
INCLUDE( inc/windows/headers.cmake )

### ------------- MOC --------------
QT4_WRAP_CPP( Gooey_MOC_SRCS ${Gooey_MOC_HDRS} )

# Target library
ADD_LIBRARY( ${GOOEY_LIBRARY} SHARED ${Gooey_SRCS} ${Gooey_MOC_SRCS} ${Gooey_HDRS} ${Gooey_MOC_HDRS} )
TARGET_LINK_LIBRARIES( ${GOOEY_LIBRARY} ${KORE_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

# Mac OS specifics
IF ( APPLE )
	SET_TARGET_PROPERTIES ( 
		${GOOEY_LIBRARY} PROPERTIES
		COMPILE_DEFINITIONS "_K_BUILD_GOOEY;_GOOEY_VERSION=\"${GOOEY_VERSION_STRING}\";_K_MACX;${DEBUG_DEFINES}"
		COMPILE_FLAGS -fno-common
		VERSION ${GOOEY_VERSION_STRING}
	)
# Windows specifics
ELSEIF ( WIN32 )
	IF ( MSVC )
		SOURCE_GROUP ( "Header Files" FILES ${Gooey_HDRS} ${Gooey_MOC_HDRS} )
		SOURCE_GROUP ( "Source Files" FILES ${Gooey_SRCS} )
		SOURCE_GROUP ( "Moc Files" FILES ${Gooey_MOC_SRCS} )
	ENDIF ( MSVC )
	SET_TARGET_PROPERTIES ( 
		Gooey PROPERTIES
		COMPILE_DEFINITIONS "_K_BUILD_GOOEY;_GOOEY_VERSION=\"${Gooey_Version}\";_K_WIN32;${DEBUG_DEFINES}"
	)
# Linux specifics
ELSEIF ( UNIX )
	SET_TARGET_PROPERTIES ( 
		Gooey PROPERTIES
		COMPILE_DEFINITIONS "_K_BUILD_GOOEY;_GOOEY_VERSION=\"${Gooey_Version}\";_K_UNIX;${DEBUG_DEFINES}"
		VERSION ${Gooey_Version}
	)
ENDIF ( APPLE )

# Documentation
IF ( DOXYGEN_FOUND )
	SET ( DOXYGEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../doc/html )
	ADD_CUSTOM_COMMAND (
		OUTPUT ${DOXYGEN_OUTPUT}
		COMMAND ${CMAKE_COMMAND} -E echo_append "Building Pantin Documentation..."
		COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile
		COMMAND ${CMAKE_COMMAND} -E echo "Done."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../doc
	 )
	ADD_CUSTOM_TARGET ( gooey-doc DEPENDS ${DOXYGEN_OUTPUT} )
ENDIF ( DOXYGEN_FOUND )
