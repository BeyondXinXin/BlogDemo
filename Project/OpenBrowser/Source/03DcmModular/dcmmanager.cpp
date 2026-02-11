// 01 Frame includes
#include "dcmmanager.h"

// 04 Ui includes
#include "formmaskwidget.h"

// VTK includes
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkImageCast.h>
#include <vtkCellPicker.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapToColors.h>
#include <vtkResliceCursorWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkResliceCursorRepresentation.h>
#include <vtkInteractorStyleTrackballCamera.h>


DcmManager::DcmManager(QVTKOpenGLWidget *dcm_widget1,
                       QVTKOpenGLWidget *dcm_widget2,
                       QVTKOpenGLWidget *dcm_widget3,
                       QVTKOpenGLWidget *dcm_widget,
                       QObject *parent) :
    QObject(parent),
    dcm_widget1_(dcm_widget1),
    dcm_widget2_(dcm_widget2),
    dcm_widget3_(dcm_widget3),
    dcm_widget_(dcm_widget) {
    this->Initial();
}

DcmManager::~DcmManager() {
    imagedata_read_->deleteLater();
}

void DcmManager::OpenStlFile(const QString &file_path) {
    imagedata_read_->SetInputFileName(file_path);
    SlotVTKThreadMaskWidgetIn();
    imagedata_read_->start();
}

void DcmManager::SlotViewMaximization(const bool &value) { // 最大化显示
    QVTKOpenGLWidget *wid = static_cast<QVTKOpenGLWidget *>(sender()->parent());
    if (value) {
        dcm_widget1_->hide();
        dcm_widget2_->hide();
        dcm_widget3_->hide();
        dcm_widget_->hide();
        wid->show();
    } else {
        dcm_widget1_->show();
        dcm_widget2_->show();
        dcm_widget3_->show();
        dcm_widget_->show();
    }
}


void DcmManager::Initial() {
    imagedata_read_ = new VtkThreadImageDataRead();
    connect(imagedata_read_, &VtkThreadImageDataRead::finished,
            this, &DcmManager::SlotRunFinished);
    for (int i = 0; i < 3; i++) {
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        riw_[i]->SetRenderWindow(renderWindow);
    }
    dcm_widget1_->SetRenderWindow(riw_[0]->GetRenderWindow());
    riw_[0]->SetupInteractor(
        dcm_widget1_->GetRenderWindow()->GetInteractor());
    dcm_widget2_->SetRenderWindow(riw_[1]->GetRenderWindow());
    riw_[1]->SetupInteractor(
        dcm_widget2_->GetRenderWindow()->GetInteractor());
    dcm_widget3_->SetRenderWindow(riw_[2]->GetRenderWindow());
    riw_[2]->SetupInteractor(
        dcm_widget3_->GetRenderWindow()->GetInteractor());
    for (int i = 0; i < 3; i++) {
        // 使它们共享同一个Reslice光标对象.
        riw_[i]->SetResliceCursor(riw_[0]->GetResliceCursor());
    }
    vtkNew<vtkCellPicker> picker ;
    picker->SetTolerance(0.005);
    vtkNew<vtkProperty> ipwProp  ;
    dcm_widget_->SetRenderWindow(render_window_);
    render_window_->AddRenderer(renderer_);
    renderer_->SetBackground(90 / 255.0, 90 / 255.0, 90 / 255.0);
    vtkRenderWindowInteractor *iren = dcm_widget_->GetInteractor();
    for (int i = 0; i < 3; i++) {
        planeWidget_[i]->SetInteractor(iren);
        planeWidget_[i]->SetPicker(picker);
        planeWidget_[i]->RestrictPlaneToVolumeOn();
        riw_[i]->GetRenderer()->SetBackground(90 / 255.0, 90 / 255.0, 90 / 255.0);
        planeWidget_[i]->SetTexturePlaneProperty(ipwProp);
        planeWidget_[i]->SetDefaultRenderer(renderer_);
        riw_[i]->SetLookupTable(riw_[0]->GetLookupTable());
        planeWidget_[i]->GetColorMap()->SetLookupTable(riw_[0]->GetLookupTable());
        planeWidget_[i]->
        SetColorMap(riw_[i]->GetResliceCursorWidget()->
                    GetResliceCursorRepresentation()->GetColorMap());
    }
    for (int i = 0; i < 3; ++i) {
        riw_[i]->Render();
    }
    render_window_->Render();
}

void DcmManager::SlotRunFinished() {
    emit SingnalFinished();
    if (QObject::sender() == imagedata_read_) {
        if (imagedata_read_->GetThreadResult()) {
            // 界面0 显示dcm
            int imageDims[3];
            imagedata_read_->GetImageData()->GetDimensions(imageDims);//获取长宽高
            vtkNew<vtkImageCast> imagedata;
            imagedata_read_->GetImageData()->SetSpacing(1, 1, 0.5);
            imagedata->SetInputData(imagedata_read_->GetImageData());
            imagedata->SetOutputScalarTypeToFloat();
            for (int i = 0; i < 3; i++) {
                // 使它们共享同一个Reslice光标对象.
                riw_[i]->SetResliceCursor(riw_[0]->GetResliceCursor());
                riw_[i]->SetInputData(imagedata_read_->GetImageData());
                riw_[i]->SetSliceOrientation(i);//轴方向
                riw_[i]->SetResliceModeToAxisAligned();//切片设置为轴对齐
                riw_[i]->GetRenderer()->ResetCamera();
                riw_[i]->Render();
            }
            for (int i = 0; i < 3; ++i) {
                planeWidget_[i]->RestrictPlaneToVolumeOn();
                planeWidget_[i]->TextureInterpolateOff();
                planeWidget_[i]->SetResliceInterpolateToLinear();
                planeWidget_[i]->SetInputConnection(imagedata->GetOutputPort());
                planeWidget_[i]->SetPlaneOrientation(i);
                planeWidget_[i]->SetSliceIndex(imageDims[i] / 2);
                planeWidget_[i]->DisplayTextOff();
                planeWidget_[i]->SetWindowLevel(1358, -27);
                planeWidget_[i]->On();
                planeWidget_[i]->InteractionOn();
            }
            riw_[2]->SetSlice(2000);
            renderer_->ResetCamera();
            render_window_->Render();
        }
    }
}

void DcmManager::SlotVTKThreadMaskWidgetIn(int value) {
    FormMaskWidget *w = new FormMaskWidget;
    w->SlotsStackedWidgetNum(value);
    connect(this, &DcmManager::SignalVtkThreadProgressOut,
            w, &FormMaskWidget::SlotThreadAICurrent);
    connect(this, &DcmManager::SingnalFinished,
            w, &FormMaskWidget::SlotThisCloseIn);
    w->SetCancleHide();
    w->show();
    qDebug();
    QThread::msleep(100);
}
