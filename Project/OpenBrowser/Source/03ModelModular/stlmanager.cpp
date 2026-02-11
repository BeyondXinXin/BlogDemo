// 01 Frame includes
#include "stlmanager.h"
#include "quihelper.h"

// 04 Ui includes
#include "form3dreconstruction.h"
#include "formmaskwidget.h"

// VTK includes
#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkFillHolesFilter.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtksys/SystemTools.hxx>

STLManager::STLManager(QVTKOpenGLWidget &vtk_widget,
                       QObject *parent)
  : QObject(parent)
  , vtk_widget_(vtk_widget)
{
    this->Initial();
}

STLManager::~STLManager()
{ // 析构
    form3dreconstruction_->deleteLater();
    reade_->deleteLater();
    write_->deleteLater();
    line_clipper_->deleteLater();
    auto_division_->deleteLater();
    refine_->deleteLater();
    smooth_selector_->deleteLater();
    subdivision_->deleteLater();
    //    cgal_subdivision_->deleteLater();
    cgal_selector_->deleteLater();
}

void STLManager::Initial()
{ // 初始化
    this->renderer_ = new CustomVtkRenderer(&this->vtk_widget_);
    this->renderer_->SetBackground({ 90 / 255.0, 90 / 255.0, 90 / 255.0 });
    this->renderer_->Execute();
    this->viewer_ = new CustomSurfaceViewer();
    this->viewer_->SetVmtkRenderer(renderer_);
    // 三维重构
    this->form3dreconstruction_ = new VtkThreadForm3DReconstruction();
    connect(this->form3dreconstruction_,
            &VtkThreadForm3DReconstruction::finished,
            this, &STLManager::SlotRunFinished);
    connect(this->form3dreconstruction_,
            &VtkThreadForm3DReconstruction::SignalVtkThreadProgressOut,
            this, &STLManager::SignalVtkThreadProgressOut);
    // 读取
    this->reade_ = new VtkThreadPolyDataRead();
    connect(this->reade_,
            &VtkThreadPolyDataRead::finished,
            this, &STLManager::SlotRunFinished);
    connect(this->reade_,
            &VtkThreadPolyDataRead::SignalVtkThreadProgressOut,
            this, &STLManager::SignalVtkThreadProgressOut);
    // 等值面提取
    marching_cubes_ = new VtkThreadMarchingCubes();
    connect(marching_cubes_, &VtkThreadMarchingCubes::finished,
            this, &STLManager::SlotRunFinished);
    // 保存
    this->write_ = new VtkThreadPolyDataWrite();
    connect(write_, &VtkThreadPolyDataWrite::finished,
            this, &STLManager::SlotRunFinished);
    connect(this->write_,
            &VtkThreadPolyDataWrite::SignalVtkThreadProgressOut,
            this, &STLManager::SignalVtkThreadProgressOut);
    // 自动提取连通域
    this->auto_division_ = new AutomaticDivision();
    connect(auto_division_,
            &AutomaticDivision::SignalClippedFinish,
            this, &STLManager::SlotRunFinished);
    // 线性剪裁
    this->line_clipper_ = new LineSurfaceClipper();
    this->line_clipper_->SetVmtkRenderer(renderer_);
    connect(line_clipper_,
            &LineSurfaceClipper::SignalClippedFinish,
            this, &STLManager::SlotRunFinished);
    // 区域平滑和修剪
    this->smooth_selector_ = new SmoothSurfaceSelector();
    this->smooth_selector_->SetVmtkRenderer(renderer_);
    connect(smooth_selector_,
            &SmoothSurfaceSelector::SignalSelectorFinish,
            this, &STLManager::SlotRunFinished);
    // vtk细分
    this->subdivision_ = new VtkThreadSubdivision();
    connect(this->subdivision_,
            &VtkThreadSubdivision::finished,
            this, &STLManager::SlotRunFinished);
    connect(this->subdivision_,
            &VtkThreadSubdivision::SignalVtkThreadProgressOut,
            this, &STLManager::SignalVtkThreadProgressOut);
    // 细化
    this->refine_ = new RefineSurface();
    connect(refine_, &RefineSurface::SignalClippedFinish,
            this, &STLManager::SlotRunFinished);
    // CGAL表面细分
    //    this->cgal_subdivision_ = new CGALThreadSubdivision();
    //    connect(this->cgal_subdivision_, &CGALThreadSubdivision::finished,
    //            this, &STLManager::SlotRunFinished);
    // CGAL封闭
    cgal_selector_ = new FillSurfaceSelector();
    cgal_selector_->SetVmtkRenderer(renderer_);
    connect(cgal_selector_, &FillSurfaceSelector::SignalFillFinish,
            this, &STLManager::SlotRunFinished);
}

void STLManager::SlotVTKThreadMaskWidgetIn(int value)
{ // 遮层罩
    FormMaskWidget *w = new FormMaskWidget;
    w->SlotsStackedWidgetNum(value);
    connect(this, &STLManager::SignalVtkThreadProgressOut,
            w, &FormMaskWidget::SlotThreadAICurrent);
    connect(this, &STLManager::SingnalFinished,
            w, &FormMaskWidget::SlotThisCloseIn);
    w->SetCancleHide();
    w->show();
    qDebug();
    QThread::msleep(100);
}

void STLManager::SlotCGLAThreadMaskWidgetIn()
{ // 遮层罩
    FormMaskWidget *w = new FormMaskWidget;
    w->SlotsStackedWidgetNum(0);
    connect(this, &STLManager::SingnalFinished,
            w, &FormMaskWidget::SlotThisCloseIn);
    w->SetCancleHide();
    w->show();
    qDebug();
    QThread::msleep(100);
}

void STLManager::OpenModelFile(const QString &file_path,
                               vtkSmartPointer<vtkImageData> value)
{ // 打开文件
    if (nullptr != value) {
        marching_cubes_->Setnum(500);
        marching_cubes_->SetInputImageData(value);
        marching_cubes_->start();
    }
    reade_->SetInputFileName(file_path);
    SlotVTKThreadMaskWidgetIn();
    reade_->start();
}

void STLManager::SlotPolyDataHandle(const int &operation, const QString text)
{
    // polydata 处理
    Q_UNUSED(text);
    if (this->polydata_list_.size() == 0
        && operation != 21
        && operation != 20) {
        emit SingnalFinished();
        emit SignalPromptInformationOut(QString("PolyData 数据错误"));
        return;
    }
    switch (operation) {
    case 1: { // 自动提取连通域
        auto_division_->SetSurface(this->polydata_list_.back());
        auto_division_->Execute();
        break;
    }
    case 2: { // 直线剪裁
        line_clipper_->SetSurface(this->polydata_list_.back());
        line_clipper_->Execute();
        emit SignalPromptInformationOut(
          QString("左键：选取剪制区域\n"
                  "空格：确认选取区域\n"
                  " Q ：取消选区区域"));
        break;
    }
    case 3: { // 修剪
        this->smooth_selector_->SetMethod(SmoothSurfaceSelector::CUT);
        this->smooth_selector_->SetSurface(this->polydata_list_.back());
        this->smooth_selector_->Execute();
        emit SignalPromptInformationOut(
          QString("左键：选取修剪区域\n空格：确认选取区域\n"
                  " Q ：取消选区区域\n"));
        break;
    }
    case 4: { // 平滑
        this->smooth_selector_->SetMethod(SmoothSurfaceSelector::SMOOTH);
        this->smooth_selector_->SetSurface(this->polydata_list_.back());
        this->smooth_selector_->Execute();
        emit SignalPromptInformationOut(
          QString("左键：选取平滑区域\n空格：确认选取区域\n"
                  " Q ：取消选区区域\n"));
        break;
    }
    case 5: { // 细分
        emit SignalPromptInformationOut(QString("正在细分模型"));
        subdivision_->SetSurface(this->polydata_list_.back());
        subdivision_->SetMethod(VtkThreadSubdivision::LINEAR);
        SlotVTKThreadMaskWidgetIn();
        subdivision_->start();
        break;
    }
    case 6: { // 表面细化
        refine_->SetSurface(this->polydata_list_.back());
        refine_->Execute();
        break;
    }
    case 11: { // CGAL细分
        //        cgal_subdivision_->SetSurface(this->polydata_list_.back());
        //        SlotCGLAThreadMaskWidgetIn();
        //        cgal_subdivision_->start();
        break;
    }
    case 12: { // CGAL自相交检测
        emit SingnalFinished();
        break;
    }
    case 13: { // CGAL封闭
        cgal_selector_->SetSurface(
          this->polydata_list_.back());
        cgal_selector_->Execute();
        emit SignalPromptInformationOut(
          QString(
            QString("共有%1个未封闭区域"
                    "\n左键：选取封闭区域\n空格：确认选取区域\nEsc：取消选区区域"
                    "\n前处理完成请单击#选择主动脉入口#"))
            .arg(cgal_selector_->GetFillCount()));
        break;
    }
    case 20: { // 三维重构
        Form3DReconstruction *reconstruction = new Form3DReconstruction;
        if (reconstruction->exec() == QDialog::Accepted) {
            form3dreconstruction_->SetForm3DReconstructionValue(
              reconstruction->GetForm3DReconstructionValue());
            form3dreconstruction_->start();
            SlotCGLAThreadMaskWidgetIn();
            delete reconstruction;
            emit SignalPromptInformationOut(QString("开始重构"));
        } else {
            emit SingnalFinished();
        }
        break;
    }
    case 21: { // 打开文件
        emit SingnalFinished();
        QString tmp_file = QUIHelper::GetFileName("*.stl");
        if (tmp_file.isEmpty()) {
            break;
        }
        QFileInfo file_info(tmp_file);
        if (file_info.suffix() == "stl") {
            OpenModelFile(tmp_file);
        } else {
            emit SignalPromptInformationOut(QString("文件错误"));
        }
        break;
    }
    case 22: { // 保存文件
        emit SingnalFinished();
        QString tmp_file = QUIHelper::GetSaveName("*.stl *.vtk", "***.stl");
        if (tmp_file.isEmpty()) {
            break;
        }
        this->write_->SetOutputFileName(tmp_file);
        this->write_->SetSurface(this->polydata_list_.back());
        SlotVTKThreadMaskWidgetIn(0);
        this->write_->start();
    } break;
    case 23: { // 撤销
        if (this->polydata_list_.size() == 1) {
            emit SingnalFinished();
            emit SignalPromptInformationOut(QString("PolyData 数据错误"));
            return;
        }
        this->polydata_list_.pop_back();
        this->viewer_->SetSurface(this->polydata_list_.back());
        this->viewer_->Execute();
        this->renderer_->ResetCamera();
        emit SignalPromptInformationOut(QString("已撤销"));
        emit SingnalFinished();
        break;
    }
    default: {
        emit SingnalFinished();
        break;
    }
    }
}

void STLManager::SlotRunFinished()
{ // Handle PolyData操作完成
    //    emit SingnalFinished();
    //    if (QObject::sender() == form3dreconstruction_) { // 三维重构
    //        if (!form3dreconstruction_->GetThreadResult()) {
    //            emit SignalPromptInformationOut(QString("三维重构失败"));
    //            return;
    //        }
    //        OpenModelFile(form3dreconstruction_->GetForm3DReconstructionValue());
    //        emit SignalPromptInformationOut(QString("三维重构成功"));
    //    } else if (QObject::sender() == reade_) { // 读取stl
    //        if (!reade_->GetThreadResult()) {
    //            emit SignalPromptInformationOut(QString("stl模型载入失败"));
    //            return;
    //        }
    //        if (this->polydata_list_.size() != 0) {
    //            this->polydata_list_.clear();
    //        }
    //        this->polydata_list_.push_back(this->reade_->GetOutput());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        this->renderer_->ResetCamera();
    //        this->renderer_->GetRenderer()->GetActiveCamera()->SetParallelProjection(1);
    //        emit SignalPromptInformationOut(QString("stl模型载入成功\n") + reade_->GetVolumeSurfaaceArea());
    //    } else if (QObject::sender() == this->marching_cubes_) { // imagedata 转 polydata
    //        if (!marching_cubes_->GetThreadResult()) {
    //            return;
    //        }
    //        this->polydata_list_.push_back(this->marching_cubes_->Getmarchingcubes()->GetOutput());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        this->renderer_->ResetCamera();
    //        this->renderer_->GetRenderer()->GetActiveCamera()->SetParallelProjection(1);
    //        emit SignalPromptInformationOut(QString(""));
    //    } else if (QObject::sender() == this->write_) { // stl 保存
    //        if (!this->write_->GetThreadResult()) {
    //            emit SignalPromptInformationOut(QString("stl模型保存失败"));
    //            return;
    //        }
    //        emit SignalPromptInformationOut(QString("stl模型保存成功"));
    //    } else if (QObject::sender() == auto_division_) { // 提取最大连通域
    //        this->polydata_list_.push_back(this->auto_division_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("自动分割完毕"));
    //    } else if (QObject::sender() == line_clipper_) { // 线性剪裁
    //        this->polydata_list_.push_back(this->line_clipper_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("手动剪裁完毕"));
    //    } else if (QObject::sender() == smooth_selector_) { // 修剪 平滑
    //        this->polydata_list_.push_back(this->smooth_selector_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("手动修剪 平滑完毕"));
    //    } else if (QObject::sender() == subdivision_) { // 细分
    //        if (!subdivision_->GetThreadResult()) {
    //            emit SignalPromptInformationOut(QString("模型细化失败"));
    //            return;
    //        }
    //        this->polydata_list_.push_back(this->subdivision_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("自动细分完毕"));
    //    } else if (QObject::sender() == refine_) { // 表面细分
    //        this->polydata_list_.push_back(this->refine_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("自动细分完毕"));
    //    } else if (QObject::sender() == cgal_subdivision_) { // CGAL表面细分
    //        if (!cgal_subdivision_->GetThreadResult()) {
    //            emit SignalPromptInformationOut(QString("CGAL表面细分失败"));
    //            return;
    //        }
    //        this->polydata_list_.push_back(
    //          this->cgal_subdivision_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("CGAL表面细分成功"));
    //    } else if (QObject::sender() == cgal_selector_) { // CGAL 封闭
    //        this->polydata_list_.push_back(
    //          this->cgal_selector_->GetSurface());
    //        this->viewer_->SetSurface(this->polydata_list_.back());
    //        this->viewer_->Execute();
    //        emit SignalPromptInformationOut(QString("CGAL表面封闭成功"));
    //    }
}
