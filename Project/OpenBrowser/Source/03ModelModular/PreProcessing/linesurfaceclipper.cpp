// 01 Frame includes
#include "linesurfaceclipper.h"

// VTK includes
#include <vtkPoints.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkProperty2D.h>
#include <vtkBoxClipDataSet.h>
#include <vtkSeedRepresentation.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPointHandleRepresentation2D.h>

LineSurfaceClipper::LineSurfaceClipper(QObject *parent) : QObject(parent) {
    this->Initial();
}

LineSurfaceClipper::~LineSurfaceClipper() {
    if (this->own_renderer_ == true) {
        this->renderer_->deleteLater();
    }
}

void LineSurfaceClipper::Execute() {
    if (this->renderer_ == nullptr) {
        this->renderer_ = new CustomVtkRenderer();
        this->renderer_->Execute();
        this->own_renderer_ = true;
    }
    if (this->seed_widget_ == nullptr) {
        this->seed_widget_ = vtkSmartPointer<vtkSeedWidget>::New();
        vtkNew<vtkPointHandleRepresentation2D> handle_rep;
        handle_rep->GetProperty()->SetColor(1, 0, 0);
        vtkNew<vtkSeedRepresentation> widget_rep;
        widget_rep->SetHandleRepresentation(handle_rep);
        this->seed_widget_->SetInteractor(
            this->renderer_->GetRenderWindowInteractor());
        this->seed_widget_->SetRepresentation(widget_rep);
    }
    qint32 num_seeds =
        this->seed_widget_->GetSeedRepresentation()->GetNumberOfSeeds();
    for (qint32 i = 0; i < num_seeds; ++i) {
        this->seed_widget_->GetSeedRepresentation()->RemoveLastHandle();
        this->seed_widget_->DeleteSeed(
            this->seed_widget_->GetSeedRepresentation()->GetNumberOfSeeds());
    }
    this->seed_widget_->On();
    if (first_connect_) {
        connect(this->renderer_, &CustomVtkRenderer::SignalKeyPressed,
                this, &LineSurfaceClipper::SlotKeyPressed);
        this->vtk_connections_ = vtkSmartPointer<vtkEventQtSlotConnect>::New();
        vtk_connections_->Connect(seed_widget_, vtkCommand::PlacePointEvent,
                                  this, SLOT(SlotSeedChanged(vtkObject *, unsigned long,
                                             void *, void *)));
        first_connect_ = false;
    }
    if (this->own_renderer_) {
        this->renderer_->Deallocate();
    }
}

void LineSurfaceClipper::SetVmtkRenderer(const QPointer<CustomVtkRenderer> value) {
    this->renderer_ = value;
}

void LineSurfaceClipper::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->surface_ = value;
}

vtkSmartPointer<vtkPolyData> LineSurfaceClipper::GetSurface() {
    return this->surface_;
}

void LineSurfaceClipper::Initial() {
    this->first_connect_ = true;
    this->own_renderer_ = false;
    this->renderer_ = nullptr;
    this->vtk_connections_ = nullptr;
    this->seed_widget_ = nullptr;
    this->surface_ = nullptr;
}

void LineSurfaceClipper::ClipCallback() {
    qDebug();
    if (this->seed_widget_->GetSeedRepresentation()->GetNumberOfSeeds() != 2
            || this->surface_ == nullptr) {
        return ;
    }
    double pos1[3], pos2[3];
    this->seed_widget_->GetSeedRepresentation()->GetSeedWorldPosition(0, pos1);
    this->seed_widget_->GetSeedRepresentation()->GetSeedWorldPosition(1, pos2);
    this->seed_widget_->Off();
    QList<QList<double>> pts = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 1}
    };
    vtkNew<vtkPoints> points;
    for (qint32 i = 0; i < pts.size(); ++i) {
        points->InsertPoint(i, pts[i][0], pts[i][1], pts[i][2]);
    }
    double direction[3];
    this->renderer_->GetRenderer()->GetActiveCamera()
    ->GetDirectionOfProjection(direction);
    points->SetPoint(0,
                     pos1[0] - direction[0] * 1000,
                     pos1[1] - direction[1] * 1000,
                     pos1[2] - direction[2] * 1000);
    points->SetPoint(3,
                     pos1[0] + direction[0] * 1000,
                     pos1[1] + direction[1] * 1000,
                     pos1[2] + direction[2] * 1000);
    points->SetPoint(1,
                     pos2[0] - direction[0] * 1000,
                     pos2[1] - direction[1] * 1000,
                     pos2[2] - direction[2] * 1000);
    points->SetPoint(2,
                     pos2[0] + direction[0] * 1000,
                     pos2[1] + direction[1] * 1000,
                     pos2[2] + direction[2] * 1000);
    double direction2[3], direction_offset[3];
    direction2[0] = pos1[0] - pos2[0];
    direction2[1] = pos1[1] - pos2[1];
    direction2[2] = pos1[2] - pos2[2];
    vtkMath::Cross(direction, direction2, direction_offset);
    points->SetPoint(4,
                     pos1[0] - direction[0] * 1000 + direction_offset[0] * 0.01,
                     pos1[1] - direction[1] * 1000 + direction_offset[1] * 0.01,
                     pos1[2] - direction[2] * 1000 + direction_offset[2] * 0.01);
    points->SetPoint(7,
                     pos1[0] + direction[0] * 1000 + direction_offset[0] * 0.01,
                     pos1[1] + direction[1] * 1000 + direction_offset[1] * 0.01,
                     pos1[2] + direction[2] * 1000 + direction_offset[2] * 0.01);
    points->SetPoint(5,
                     pos2[0] - direction[0] * 1000 + direction_offset[0] * 0.01,
                     pos2[1] - direction[1] * 1000 + direction_offset[1] * 0.01,
                     pos2[2] - direction[2] * 1000 + direction_offset[2] * 0.01);
    points->SetPoint(6,
                     pos2[0] + direction[0] * 1000 + direction_offset[0] * 0.01,
                     pos2[1] + direction[1] * 1000 + direction_offset[1] * 0.01,
                     pos2[2] + direction[2] * 1000 + direction_offset[2] * 0.01);
    vtkNew<vtkBoxClipDataSet> box_clip;
    box_clip->SetInputData(this->surface_);
    box_clip->GenerateClippedOutputOn();
    double n0[3], n1[3], n2[3], n3[3], n4[3], n5[3];
    double p0[3], p1[3], p2[3], p3[3], p4[3], p5[3];
    double pt0[3], pt1[3], pt2[3], pt3[3], pt4[3], pt5[3], pt6[3], pt7[3];
    points->GetPoint(0, pt0);
    points->GetPoint(1, pt1);
    points->GetPoint(2, pt2);
    points->GetPoint(3, pt3);
    points->GetPoint(4, pt4);
    points->GetPoint(5, pt5);
    points->GetPoint(6, pt6);
    points->GetPoint(7, pt7);
    for (qint32 i = 0; i < 3; ++i) {
        p0[i] = (pt1[i] + pt3[i]) / 2;
        n0[i] = (pt1[i] - pt5[i]);
        p1[i] = (pt5[i] + pt7[i]) / 2;
        n1[i] = (pt5[i] - pt1[i]);
        p2[i] = (pt2[i] + pt5[i]) / 2;
        n2[i] = (pt5[i] - pt4[i]);
        p3[i] = (pt3[i] + pt4[i]) / 2;
        n3[i] = (pt4[i] - pt5[i]);
        p4[i] = (pt0[i] + pt5[i]) / 2;
        n4[i] = (pt5[i] - pt6[i]);
        p5[i] = (pt2[i] + pt7[i]) / 2;
        n5[i] = (pt6[i] - pt5[i]);
    }
    box_clip->SetBoxClip(n0, p0,
                         n1, p1,
                         n2, p2,
                         n3, p3,
                         n4, p4,
                         n5, p5);
    vtkNew<vtkDataSetSurfaceFilter> surface_out;
    surface_out->SetInputConnection(box_clip->GetOutputPort(1));
    vtkNew<vtkPolyDataConnectivityFilter> connectivity_filter;
    connectivity_filter->SetInputConnection(surface_out->GetOutputPort());
    connectivity_filter->SetExtractionModeToLargestRegion();
    connectivity_filter->Update();
    this->surface_ = connectivity_filter->GetOutput();
}

void LineSurfaceClipper::SlotKeyPressed(const QString &key) {
    if (this->seed_widget_->GetEnabled() == 1) {
        if (key == "space") {
            this->ClipCallback();
            emit SignalClippedFinish();
        } else if (key == "q" || key == "Q") {
            this->seed_widget_->Off();
            emit SignalClippedFinish();
        }
    }
}

void LineSurfaceClipper::SlotSeedChanged(
    vtkObject *caller, unsigned long vtk_event,
    void *client_data, void *call_data) {
    Q_UNUSED(client_data);
    if (vtk_event == vtkCommand::PlacePointEvent) {
        qint32 n = *static_cast<int *>(call_data);
        vtkSmartPointer<vtkSeedWidget> widget = dynamic_cast<vtkSeedWidget *>(caller);
        if (n >= 0 && widget) {
            qint32 num_seeds = widget->GetSeedRepresentation()->GetNumberOfSeeds();
            if (num_seeds > 2) {
                this->seed_widget_->DeleteSeed(0);
            }
        }
    }
}
