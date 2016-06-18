project(Extensions)
cmake_minimum_required(VERSION 3.0.2)

set(Extensions_HDR
    Extensions.hpp
    YouTube.hpp
    Downloader.hpp
    Radio.hpp
    LastFM.hpp
    ProstoPleer.hpp
    MPRIS2.hpp
)

set(Extensions_SRC
    Extensions.cpp
    YouTube.cpp
    Downloader.cpp
    Radio.cpp
    LastFM.cpp
    ProstoPleer.cpp
    MPRIS2.cpp
)

set(Extensions_RESOURCES
    icons.qrc
)

add_definitions(-DUSE_MPRIS2 -DUSE_PROSTOPLEER)

if(USE_QT5)
    qt5_add_resources(Extensions_RESOURCES_RCC ${Extensions_RESOURCES})
else()
    qt4_add_resources(Extensions_RESOURCES_RCC ${Extensions_RESOURCES})
endif()

include_directories(../../qmplay2/headers)

add_library(${PROJECT_NAME} MODULE
    ${Extensions_HDR}
    ${Extensions_SRC}
    ${Extensions_RESOURCES_RCC}
)

if(USE_QT5)
    qt5_use_modules(${PROJECT_NAME} Gui Widgets DBus Network)
else()
    target_link_libraries(${PROJECT_NAME} Qt4::QtCore Qt4::QtGui Qt4::QtDBus Qt4::QtNetwork)
endif()

add_dependencies(${PROJECT_NAME} qmplay2)
target_link_libraries(${PROJECT_NAME}
    $<TARGET_FILE:qmplay2>
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${MODULES_INSTALL_PATH}")