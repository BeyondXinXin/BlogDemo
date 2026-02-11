# openBrowser

# ----------------------------------------------------
# API Reference
# -----------------------
# Indicates an imported shared library function.
# All Developer.

#   VTK version:8.1.0
#       enable the module setting
#               Module_vtkGUISupportQtOpenGL:BOOL=ON
#               VTK_Group_Qt:BOOL=ON
#               Qt5_DIR:STRING=qt install path/gcc_64/lib/cmake/Qt5

#   ITK version:4.13
#       enable the module setting
#               Module_ITKReview:BOOL=ON
#               Module_ITKVtkGlue:BOOL=ON
#               VTK_DIR:STRING=VTK 8.1.0 Bulid path

#   opencv version:3.3.0
#       enable the module setting

#   Eigen version:3.0
#       enable the module setting

#   CGAL version:4.1.3
#       enable the module setting
# -----------------------


# ----------------------------------------------------
# CMake Minimum version number required
# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.1.0)
# Set up third-party libraries to be packaged into the project
# 设置第三方库封装到项目中
include (ExternalProject)
# Project information
# 项目信息
project(OpenBrowser VERSION "0.5.0")
configure_file(
    "${PROJECT_SOURCE_DIR}/DeeplvConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Source/01Business/DeeplvConfig.h"
)
# ----------------------------------------------------



list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.9.7/5.9.7/msvc2015_64/lib/cmake/Qt5")
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.9.7/5.9.7/msvc2015_64/lib/cmake/Qt5LinguistTools")
list(APPEND CMAKE_PREFIX_PATH "D:/lib/third-party")
list(APPEND CMAKE_MODULE_PATH "D:/lib/third-party/cmake")
list(APPEND CMAKE_PREFIX_PATH "D:/lib/opencv/build")
list(APPEND CMAKE_PREFIX_PATH "C:/lib/ThirdParty")
list(APPEND CMAKE_MODULE_PATH "C:/lib/ThirdParty/cmake")
list(APPEND CMAKE_PREFIX_PATH "C:/lib/opencv/build")







# ----------------------------------------------------
# Set 参数
# Setting definition parameters

# Qt 编译需要
# Qt compilation requires
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# 是否使用自己的库
# Whether to use own library
option(USE_SYSTEM_VTK "" ON)
option(USE_SYSTEM_ITK "" ON)

# 设置可执行文件的输出目录
# Set the output directory of the executable
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/win32/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/win32/Release)
    set(VTK_DIR  "D:/lib/VTK-8.2.0/bulid")
    set(ITK_DIR  "D:/lib/InsightToolkit-4.13.2/bulid")
    set(OpenCV_DIR  "D:/lib/opencv-3.4.9/bulid")
    set(CGAL_DIR  "D:/lib/CGAL/bulid")
elseif(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/unix/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/unix/Release)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)
endif()

# 查找依赖包(头文件包含路径、库路径、库名字、版本号) && 编译第三方依赖包
# Find dependent packages (header file contains path, library path, library name, version number) && compile third-party dependent packages
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql Test Xml)
find_package(Qt5LinguistTools)
find_package(Qt5Network)
find_package(Qt5SerialPort)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} )
#find_package(CGAL REQUIRED)
#include(${CGAL_USE_FILE})
find_package(Eigen3 REQUIRED)
include( ${EIGEN3_USE_FILE} )
# ----------------------------------------------------











# ----------------------------------------------------
# 提供头文件路径
# Provide header file path
include_directories(Source/01Business)
include_directories(Source/02Resources)
include_directories(Source/03ImageModular)
include_directories(Source/03ImageModular/PreProcessing)
include_directories(Source/03ModelModular)
include_directories(Source/03ModelModular/PreProcessing)
include_directories(Source/03ModelModular/PreProcessing/VTKThread)
include_directories(Source/03ModelModular/PreProcessing/CGALThread)
include_directories(Source/03DcmModular)
include_directories(Source/03DcmModular/PreProcessing)
include_directories(Source/03ImagesModular)
include_directories(Source/03ImagesModular/PreProcessing)
include_directories(Source/04Ui)
include_directories(Source/04Widget)
include_directories(Source/04Widget/CustomControl)
include_directories(Source/04Widget/CustomControl/NetCom)
include_directories(Source/06Test)
include_directories(Source/06Test/MinPath)
include_directories(Source/06Test/EdgeAdjustment)
include_directories(Source/06Test/IVUSread)
include_directories(Source/06Test/Anisotropicfiltering)

# 为工程引入给定的源文件 && 生成翻译文件
# Introduce given source file for project && generate translation file
set(01Business
    Source/01Business/openbrowserapplication.h
    Source/01Business/openbrowserapplication.cpp
    Source/01Business/commandlinemanager.h
    Source/01Business/commandlinemanager.cpp
    Source/01Business/quiapplication.h
    Source/01Business/quiapplication.cpp
    Source/01Business/quimenu.cpp
    Source/01Business/quimenu.h
    Source/01Business/quihelper.h
    Source/01Business/quihelper.cpp
    Source/01Business/quiconfig.h
    Source/01Business/quiconfig.cpp
    Source/01Business/quitrayicon.h
    Source/01Business/quitrayicon.cpp
    Source/01Business/quiiconhelper.h
    Source/01Business/quiiconhelper.cpp
    Source/01Business/quimessagebox.h
    Source/01Business/quimessagebox.cpp
    Source/01Business/quiwidgetmove.h
    Source/01Business/quiwidgetmove.cpp
    Source/01Business/quilogmanager.h
    Source/01Business/quilogmanager.cpp
    Source/01Business/stable.h
    )
set(02Resources
    Source/02Resources/style.qrc
    Source/02Resources/Image.qrc
    )
set(03ImageMode
    Source/03ImageModular/imagebrowsermanager.h
    Source/03ImageModular/imagebrowsermanager.cpp
    Source/03ImageModular/PreProcessing/qopencvprocessing.cpp
    Source/03ImageModular/PreProcessing/qopencvprocessing.h
    Source/03ImageModular/PreProcessing/qopencvwidget.h
    Source/03ImageModular/PreProcessing/qopencvwidget.cpp
    Source/03ImageModular/PreProcessing/formimagehandle.h
    Source/03ImageModular/PreProcessing/formimagehandle.cpp
    Source/03ImageModular/PreProcessing/formimagehandle.ui
    )
set(03ModelModular
    Source/03ModelModular/stlmanager.h
    Source/03ModelModular/stlmanager.cpp
    Source/03ModelModular/PreProcessing/customsurfaceviewer.h
    Source/03ModelModular/PreProcessing/customsurfaceviewer.cpp
    Source/03ModelModular/PreProcessing/customvtkrenderer.h
    Source/03ModelModular/PreProcessing/customvtkrenderer.cpp
    Source/03ModelModular/PreProcessing/linesurfaceclipper.h
    Source/03ModelModular/PreProcessing/linesurfaceclipper.cpp
    Source/03ModelModular/PreProcessing/automaticdivision.h
    Source/03ModelModular/PreProcessing/automaticdivision.cpp
    Source/03ModelModular/PreProcessing/smoothsurfaceselector.h
    Source/03ModelModular/PreProcessing/smoothsurfaceselector.cpp
    Source/03ModelModular/PreProcessing/refinesurface.cpp
    Source/03ModelModular/PreProcessing/refinesurface.h
    Source/03ModelModular/PreProcessing/form3dreconstruction.cpp
    Source/03ModelModular/PreProcessing/form3dreconstruction.h
    Source/03ModelModular/PreProcessing/form3dreconstruction.ui
    Source/03ModelModular/PreProcessing/VTKThread/vtkthread.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthread.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadimagedataread.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadimagedataread.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadpolydataread.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadpolydataread.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadmarchingcubes.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadmarchingcubes.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadpolydatawrite.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadpolydatawrite.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadsubdivision.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadsubdivision.cpp
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadform3dreconstruction.h
    Source/03ModelModular/PreProcessing/VTKThread/vtkthreadform3dreconstruction.cpp
    Source/03ModelModular/PreProcessing/CGALThread/cgalthread.h
    Source/03ModelModular/PreProcessing/CGALThread/cgalthread.cpp
    Source/03ModelModular/PreProcessing/CGALThread/cgalthreadsubdivision.h
    Source/03ModelModular/PreProcessing/CGALThread/cgalthreadsubdivision.cpp
    Source/03ModelModular/PreProcessing/CGALThread/cgalthreadfill.h
    Source/03ModelModular/PreProcessing/CGALThread/cgalthreadfill.cpp
    )
set(03DcmModular
    Source/03DcmModular/dcmmanager.h
    Source/03DcmModular/dcmmanager.cpp
    )
set(03ImagesModular
    Source/03ImagesModular/imagesbrowermanager.cpp
    Source/03ImagesModular/imagesbrowermanager.h
    Source/03ImagesModular/PreProcessing/formshow.h
    Source/03ImagesModular/PreProcessing/formshow.ui
    Source/03ImagesModular/PreProcessing/formshow.cpp
    )
set(04Ui
    Source/04Ui/formmain.h
    Source/04Ui/formmain.cpp
    Source/04Ui/formmain.ui
    Source/04Ui/formtitle.h
    Source/04Ui/formtitle.cpp
    Source/04Ui/formtitle.ui
    )
set(04Widget
    Source/04Widget/formleft.h
    Source/04Widget/formleft.cpp
    Source/04Widget/formleft.ui
    Source/04Widget/aboutinfo.h
    Source/04Widget/aboutinfo.cpp
    Source/04Widget/aboutinfo.ui
    Source/04Widget/formmaskwidget.h
    Source/04Widget/formmaskwidget.cpp
    Source/04Widget/formmaskwidget.ui
    Source/04Widget/CustomControl/setupini.cpp
    Source/04Widget/CustomControl/setupini.h
    Source/04Widget/CustomControl/cpumemorylabel.cpp
    Source/04Widget/CustomControl/cpumemorylabel.h
    Source/04Widget/CustomControl/maskwidget.cpp
    Source/04Widget/CustomControl/maskwidget.h
    Source/04Widget/CustomControl/progressbarwait.cpp
    Source/04Widget/CustomControl/progressbarwait.h
    Source/04Widget/CustomControl/progressbarcolor.cpp
    Source/04Widget/CustomControl/progressbarcolor.h
    Source/04Widget/CustomControl/sliderbar.cpp
    Source/04Widget/CustomControl/sliderbar.h
    Source/04Widget/CustomControl/screenwidget.cpp
    Source/04Widget/CustomControl/screenwidget.h
    Source/04Widget/CustomControl/sliderrange.cpp
    Source/04Widget/CustomControl/sliderrange.h
    Source/04Widget/CustomControl/NetCom/settingsdialog.h
    Source/04Widget/CustomControl/NetCom/settingsdialog.cpp
    Source/04Widget/CustomControl/NetCom/settingsdialog.ui
    Source/04Widget/CustomControl/NetCom/frmtcpclient.h
    Source/04Widget/CustomControl/NetCom/frmtcpclient.cpp
    Source/04Widget/CustomControl/NetCom/frmtcpclient.ui
    Source/04Widget/CustomControl/NetCom/frmtcpserver.h
    Source/04Widget/CustomControl/NetCom/frmtcpserver.cpp
    Source/04Widget/CustomControl/NetCom/frmtcpserver.ui
    Source/04Widget/CustomControl/NetCom/frmudpserver.h
    Source/04Widget/CustomControl/NetCom/frmudpserver.cpp
    Source/04Widget/CustomControl/NetCom/frmudpserver.ui
    )
set(06Test
#    Source/06Test/MinPath/minimalpathextraction.h
#    Source/06Test/MinPath/custom_item.h
#    Source/06Test/MinPath/custom_item.cpp
#    Source/06Test/MinPath/mainwindow.h
#    Source/06Test/MinPath/mainwindow.cpp
#    Source/06Test/MinPath/mainwindow.ui
    Source/06Test/EdgeAdjustment/formedgeadjustment.h
    Source/06Test/EdgeAdjustment/formedgeadjustment.cpp
    Source/06Test/EdgeAdjustment/formedgeadjustment.ui
    Source/06Test/IVUSread/readivus.cpp
    Source/06Test/IVUSread/readivus.h
    Source/06Test/IVUSread/readivus.ui
    Source/06Test/Anisotropicfiltering/anisotropicfiltering.h
    Source/06Test/Anisotropicfiltering/anisotropicfiltering.cpp
    )

set(Source
    main.cpp
    ${01Business}
    ${02Resources}
    ${03ImageMode}
    ${03ModelModular}
    ${03DcmModular}
    ${03ImagesModular}
    ${04Ui}
    ${04Widget}
    ${06Test}
    )

if(WIN32)
    add_executable(
        ${PROJECT_NAME}
        WIN32
        ${Source}
        )
else()
    add_executable(
        ${PROJECT_NAME}
        ${Source}
        )
endif()
# ----------------------------------------------------










# ----------------------------------------------------
# 将可执行程序文件与库文件进行链接
# Linking executable files with library files

target_link_libraries(# qt lib文件
    ${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
    Qt5::Sql
    Qt5::Xml
    Qt5::Network
    Qt5::SerialPort
    )

target_link_libraries(# itk lib文件
    ${PROJECT_NAME}
    ${ITK_LIBRARIES}
    )

target_link_libraries(# vtk lib文件
    ${PROJECT_NAME}
    ${VTK_LIBRARIES}
    )

target_link_libraries(# cgal lib文件
    ${PROJECT_NAME}
    "${CGAL_LIBS}"
    )

target_link_libraries(# opencv lib文件
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
    "opencv_core"
    "opencv_highgui"
    "opencv_imgcodecs"
    "opencv_imgproc")
# ----------------------------------------------------

