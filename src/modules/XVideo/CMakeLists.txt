cmake_minimum_required(VERSION 2.8.6)
if(COMMAND cmake_policy)
    if(POLICY CMP0003)
        cmake_policy(SET CMP0003 NEW)
    endif()
    if(POLICY CMP0020)
        cmake_policy(SET CMP0020 NEW)
    endif()
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif()
endif()
project(XVideo)

set(XVideo_HDR
    XVideoWriter.hpp
    XVideo.hpp
    xv.hpp
)

set(XVideo_SRC
    XVideoWriter.cpp
    XVideo.cpp
    xv.cpp
)

set(XVideo_RESOURCES
    icon.qrc
)

pkg_check_modules(LIB_X11_XV x11 xv REQUIRED)
link_directories(${LIB_X11_XV_LIBRARY_DIRS})

if(USE_QT5)
    qt5_add_resources(XVideo_RESOURCES_RCC ${XVideo_RESOURCES})
else()
    qt4_add_resources(XVideo_RESOURCES_RCC ${XVideo_RESOURCES})
endif()

include_directories(../../qmplay2/headers ${LIB_X11_XV_INCLUDE_DIRS})

add_library(${PROJECT_NAME} MODULE
    ${XVideo_HDR}
    ${XVideo_SRC}
    ${XVideo_RESOURCES_RCC}
)

if(USE_QT5)
    qt5_use_modules(${PROJECT_NAME} Gui Widgets)
else()
    target_link_libraries(${PROJECT_NAME} Qt4::QtCore Qt4::QtGui)
endif()

add_dependencies(${PROJECT_NAME} qmplay2)
target_link_libraries(${PROJECT_NAME}
    ${LIB_X11_XV_LIBRARIES}
    ${libqmplay2}
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${MODULES_INSTALL_PATH}")
