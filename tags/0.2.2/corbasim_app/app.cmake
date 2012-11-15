


set(app_GENERATED
	${CMAKE_CURRENT_SOURCE_DIR}/appC.h
	${CMAKE_CURRENT_SOURCE_DIR}/appC.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/appS.h
	${CMAKE_CURRENT_SOURCE_DIR}/appS.cpp)

add_custom_command(
	OUTPUT ${app_GENERATED}
	DEPENDS app.idl
	COMMAND ${CORBASIM_ORB_IDL_COMPILER} ${CORBASIM_ORB_IDL_COMPILER_OPTIONS} app.idl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${CORBASIM_PREFIX}app_idl SHARED ${app_GENERATED} app_adapted.cpp)
install(TARGETS ${CORBASIM_PREFIX}app_idl DESTINATION lib)

	