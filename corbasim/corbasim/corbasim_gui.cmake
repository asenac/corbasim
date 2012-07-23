set(corbasim_gui_EXPORTED_HEADERS
    gui/export.hpp
    gui/json.hpp
    gui/utils.hpp
    gui/qvariant.hpp
    gui/InputRequestProcessor.hpp
    gui/ModelNode.hpp
    gui/OperationSequence.hpp
    gui/SimpleScriptEditor.hpp
    gui/RequestDialog.hpp
    gui/LogModel.hpp
    gui/ParametersModel.hpp
    gui/FilteredLogView.hpp
    gui/ScriptModel.hpp
    gui/SimpleClient.hpp
    gui/DumpTool.hpp
    gui/ReflectiveGUI.hpp)

foreach(_header ${corbasim_gui_EXPORTED_HEADERS})
    get_filename_component(_header_path ${_header} PATH)
    install(FILES ${_header}
        DESTINATION include/corbasim/${_header_path})
endforeach(_header ${corbasim_gui_EXPORTED_HEADERS})

set(corbasim_gui_MOC_HDRS
    gui/InputRequestProcessor.hpp
    gui/OperationSequence.hpp
    gui/SimpleScriptEditor.hpp
    gui/RequestDialog.hpp
    gui/LogModel.hpp
    gui/ParametersModel.hpp
    gui/FilteredLogView.hpp
    gui/ScriptModel.hpp
    gui/SimpleClient.hpp
    gui/DumpTool.hpp
    gui/ReflectiveGUI.hpp)

set(corbasim_gui_SRCS
    gui/json.cpp
    gui/utils.hpp
    gui/utils.cpp
    gui/qvariant.cpp
    gui/InputRequestProcessor.cpp
    gui/ModelNode.cpp
    gui/OperationSequence.cpp
    gui/SimpleScriptEditor.cpp
    gui/RequestDialog.cpp
    gui/ParametersModel.cpp
    gui/LogModel.cpp
    gui/FilteredLogView.cpp
    gui/ScriptModel.cpp
    gui/SimpleClient.cpp
    gui/DumpTool.cpp
    gui/ReflectiveGUI.cpp)

set(corbasim_gui_LIBS
    ${QT_LIBRARIES}
    corbasim
    corbasim_qt)

# NOTE: QT bug: https://bugreports.qt.nokia.com/browse/QTBUG-22829
qt4_wrap_cpp(corbasim_gui_MOC_SRCS ${corbasim_gui_MOC_HDRS} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

add_library(corbasim_gui SHARED ${corbasim_gui_SRCS}
    ${corbasim_gui_MOC_SRCS})
target_link_libraries(corbasim_gui
    ${corbasim_gui_LIBS})

set_target_properties(corbasim_gui PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})

add_library(corbasim_gui_s STATIC ${corbasim_gui_SRCS}
    ${corbasim_gui_MOC_SRCS})

install(TARGETS corbasim_gui corbasim_gui_s DESTINATION lib)
