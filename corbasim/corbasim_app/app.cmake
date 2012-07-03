

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(app_GENERATED
	${CMAKE_CURRENT_SOURCE_DIR}/appC.h
	${CMAKE_CURRENT_SOURCE_DIR}/appC.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/appC.inl
	${CMAKE_CURRENT_SOURCE_DIR}/appS.h
	${CMAKE_CURRENT_SOURCE_DIR}/appS.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/appS.inl)

add_custom_command(
	OUTPUT ${app_GENERATED}
	DEPENDS app.idl
	COMMAND tao_idl ${TAO_IDL_OPTIONS} app.idl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${CORBASIM_PREFIX}app_idl SHARED ${app_GENERATED} app_adapted.cpp)

	
	
	










	
