project(Inputs)
cmake_minimum_required(VERSION 3.0.2)

set(Inputs_HDR
    Inputs.hpp
    ToneGenerator.hpp
    PCM.hpp
    Rayman2.hpp
)

set(Inputs_SRC
    Inputs.cpp
    ToneGenerator.cpp
    PCM.cpp
    Rayman2.cpp
)

set(Inputs_RESOURCES
    icons.qrc
)

if(USE_QT5)
    qt5_add_resources(Inputs_RESOURCES_RCC ${Inputs_RESOURCES})
else()
    qt4_add_resources(Inputs_RESOURCES_RCC ${Inputs_RESOURCES})
endif()

include_directories(../../qmplay2/headers)

add_library(${PROJECT_NAME} MODULE
    ${Inputs_HDR}
    ${Inputs_SRC}
    ${Inputs_RESOURCES_RCC}
)

if(USE_QT5)
    qt5_use_modules(${PROJECT_NAME} Gui Widgets)
else()
    target_link_libraries(${PROJECT_NAME} Qt4::QtGui Qt4::QtCore)
endif()

add_dependencies(${PROJECT_NAME} qmplay2)
target_link_libraries(${PROJECT_NAME}
    $<TARGET_FILE:qmplay2>
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${MODULES_INSTALL_PATH}")