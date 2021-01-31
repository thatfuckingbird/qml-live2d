cmake_minimum_required(VERSION 3.14)

project(qml-live2d LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)
find_package(OpenGL)
find_package(GLEW)

include_directories(3rdparty/CubismCore/include)
include_directories(include)
include_directories(src/CubismNativeOpenGLSample)

add_library(Live2DCubismCore STATIC IMPORTED)
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/CubismCore/lib/linux/x86_64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/CubismCore/include
)

add_definitions(-DCSM_TARGET_LINUX_GL)
set(FRAMEWORK_SOURCE OpenGL)
add_subdirectory(3rdparty/CubismNativeFramework)



add_library(qml-live2d STATIC
    src/live2ditem.cpp
    include/Live2DItem.h
    src/live2ditemrenderer.cpp
    include/Live2DItemRenderer.h
    src/CubismNativeOpenGLSample/LAppAllocator.cpp
    src/CubismNativeOpenGLSample/LAppAllocator.hpp
    src/CubismNativeOpenGLSample/LAppDefine.cpp
    src/CubismNativeOpenGLSample/LAppDefine.hpp
    src/CubismNativeOpenGLSample/LAppLive2DManager.cpp
    src/CubismNativeOpenGLSample/LAppLive2DManager.hpp
    src/CubismNativeOpenGLSample/LAppModel.cpp
    src/CubismNativeOpenGLSample/LAppModel.hpp
    src/CubismNativeOpenGLSample/LAppPal.cpp
    src/CubismNativeOpenGLSample/LAppPal.hpp
    src/CubismNativeOpenGLSample/LAppSprite.cpp
    src/CubismNativeOpenGLSample/LAppSprite.hpp
    src/CubismNativeOpenGLSample/LAppTextureManager.cpp
    src/CubismNativeOpenGLSample/LAppTextureManager.hpp
    src/CubismNativeOpenGLSample/LAppView.cpp
    src/CubismNativeOpenGLSample/LAppView.hpp
    src/CubismNativeOpenGLSample/TouchManager.cpp
    src/CubismNativeOpenGLSample/TouchManager.hpp
)

add_executable(qml-live2d-example
    example/main.cpp
    example/qml.qrc
    include/Live2DItem.h
    include/Live2DItemRenderer.h
)

target_link_libraries(qml-live2d PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Framework OpenGL::GL GLEW::glew Live2DCubismCore)
target_compile_definitions(qml-live2d PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(qml-live2d-example PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick qml-live2d)
target_compile_definitions(qml-live2d-example PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
