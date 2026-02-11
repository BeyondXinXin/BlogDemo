// 01 Frame includes
#include "smoothsurfaceselector.h"

// VTK includes
#include <vtkCamera.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkClipDataSet.h>
#include <vtkAppendFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkParametricSpline.h>
#include <vtkSeedRepresentation.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkRenderWindowInteractor.h>


SmoothSurfaceSelector::SmoothSurfaceSelector(QObject *parent) :
    QObject(parent) {
    this->Initial();
}

SmoothSurfaceSelector::~SmoothSurfaceSelector() {
    if (this->own_renderer_ == true) {
        this->renderer_->deleteLater();
    }
}

void SmoothSurfaceSelector::Execute() {
    qDebug();
    if (this->renderer_ == nullptr) {
        this->renderer_ = new CustomVtkRenderer();
        this->renderer_->Execute();
        this->own_renderer_ = true;
    }
    if (this->seed_widget_ == nullptr) {
        this->seed_widget_ = vtkSmartPointer<vtkSeedWidget>::New();
        vtkNew<vtkSphereHandleRepresentation> handle_rep;
        handle_rep->GetProperty()->SetColor(0.85, 0.85, 0.85);
        handle_rep->SetHandleSize(4.0);
        vtkNew<vtkSeedRepresentation> widget_rep;
        widget_rep->SetHandleRepresentation(handle_rep);
        this->seed_widget_->SetInteractor(
            this->renderer_->GetRenderWindowInteractor());
        this->seed_widget_->SetRepresentation(widget_rep);
    }
    if (this->contour_widget_ == nullptr) {
        this->contour_widget_ = vtkSmartPointer<vtkContourWidget>::New();
        vtkNew<vtkOrientedGlyphContourRepresentation> contour_rep;
        contour_rep->GetLinesProperty()->SetColor(1, 0, 0);
        contour_rep->GetLinesProperty()->SetLineWidth(static_cast<float>(0.4));
        this->contour_widget_->SetRepresentation(contour_rep);
        this->contour_widget_->SetInteractor(
            this->renderer_->GetRenderWindowInteractor());
    }
    qint32 num_seeds =
        this->seed_widget_->GetSeedRepresentation()->GetNumberOfSeeds();
    for (qint32 i = 0; i < num_seeds; ++i) {
        this->seed_widget_->GetSeedRepresentation()->RemoveLastHandle();
        this->seed_widget_->DeleteSeed(
            this->seed_widget_->GetSeedRepresentation()->GetNumberOfSeeds());
    }
    this->contour_widget_->Off();
    this->seed_widget_->On();
    if (first_connect_) {
        connect(this->renderer_, &CustomVtkRenderer::SignalKeyPressed,
                this, &SmoothSurfaceSelector::SlotKeyPressed);
        this->vtk_connections_ = vtkSmartPointer<vtkEventQtSlotConnect>::New();
        vtk_connections_->Connect(seed_widget_, vtkCommand::PlacePointEvent,
                                  this, SLOT(SlotSeedChanged(vtkObject *, unsigned long,
                                             void *, void *)));
        vtk_connections_->Connect(seed_widget_, vtkCommand::InteractionEvent,
                                  this, SLOT(SlotSeedChanged(vtkObject *, unsigned long,
                                             void *, void *)));
        first_connect_ = false;
    }
    if (this->own_renderer_) {
        this->renderer_->Deallocate();
    }
}

void SmoothSurfaceSelector::SetMethod(const SmoothSurfaceSelector::Method value) {
    this->method_ = value;
}

void SmoothSurfaceSelector::SetVmtkRenderer(const QPointer<CustomVtkRenderer> value) {
    this->renderer_ = value;
}

void SmoothSurfaceSelector::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->surface_ = value;
}

vtkSmartPointer<vtkPolyData> SmoothSurfaceSelector::GetSurface() {
    return this->surface_;
}

void SmoothSurfaceSelector::Initial() {
    this->first_connect_ = true;
    this->own_renderer_ = false;
    this->method_ = SMOOTH;
    this->renderer_ = nullptr;
    this->vtk_connections_ = nullptr;
    this->seed_widget_ = nullptr;
    this->contour_widget_ = nullptr;
    this->surface_ = nullptr;
    this->curved_surface_ = nullptr;
}

void SmoothSurfaceSelector::ClipCallback() {
    qDebug();
    this->seed_widget_->Off();
    this->contour_widget_->Off();
    vtkNew<vtkImplicitPolyDataDistance> implicit_poly_data_distance;
    implicit_poly_data_distance->SetInput(this->curved_surface_);
    vtkNew<vtkClipDataSet> clipper;
    clipper->SetInputData(this->surface_);
    clipper->SetClipFunction(implicit_poly_data_distance);
    clipper->SetValue(0);
    clipper->GenerateClippedOutputOn();
    clipper->Update();
    vtkNew<vtkDataSetSurfaceFilter> surface_in;
    surface_in->SetInputConnection(clipper->GetOutputPort(0));
    surface_in->Update();
    vtkNew<vtkDataSetSurfaceFilter> surface_out;
    surface_out->SetInputConnection(clipper->GetOutputPort(1));
    surface_out->Update();
    this->surface_ = surface_in->GetOutput();
}

void SmoothSurfaceSelector::SmoothCallback() {
    this->seed_widget_->Off();
    this->contour_widget_->Off();
    vtkNew<vtkImplicitPolyDataDistance> implicit_poly_data_distance;
    implicit_poly_data_distance->SetInput(this->curved_surface_);
    vtkNew<vtkExtractPolyDataGeometry> extract_poly_data_geometry;
    extract_poly_data_geometry->SetInputData(this->surface_);
    extract_poly_data_geometry->ExtractInsideOff();
    extract_poly_data_geometry->SetImplicitFunction(implicit_poly_data_distance);
    extract_poly_data_geometry->Update();
    vtkNew<vtkDataSetSurfaceFilter> surface_in;
    surface_in->SetInputData(extract_poly_data_geometry->GetOutput());
    surface_in->Update();
    extract_poly_data_geometry->SetInputData(this->surface_);
    extract_poly_data_geometry->ExtractInsideOn();
    extract_poly_data_geometry->ExtractBoundaryCellsOn();
    extract_poly_data_geometry->SetImplicitFunction(implicit_poly_data_distance);
    extract_poly_data_geometry->Update();
    vtkNew<vtkDataSetSurfaceFilter> surface_out;
    surface_out->SetInputData(extract_poly_data_geometry->GetOutput());
    surface_out->Update();
    vtkNew<vtkWindowedSincPolyDataFilter> smooth_filter;
    smooth_filter->SetInputData(surface_in->GetOutput());
    smooth_filter->SetNumberOfIterations(10);
    smooth_filter->SetPassBand(0.1);
    smooth_filter->BoundarySmoothingOff();
    smooth_filter->FeatureEdgeSmoothingOff();
    smooth_filter->SetNormalizeCoordinates(1);
    smooth_filter->Update();
    vtkNew<vtkTriangleFilter> tri1;
    tri1->SetInputData(smooth_filter->GetOutput());
    vtkNew<vtkCleanPolyData> clean_poly_data;
    clean_poly_data->SetInputData(tri1->GetOutput());
    clean_poly_data->Update();
    vtkNew<vtkSmoothPolyDataFilter> smoothfilter;
    smoothfilter->SetInputData(surface_out->GetOutput());
    smoothfilter->SetNumberOfIterations(3000);
    smoothfilter->BoundarySmoothingOff();
    smoothfilter->Update();
    vtkNew<vtkTriangleFilter> tri2;
    tri2->SetInputData(smoothfilter->GetOutput());
    tri2->Update();
    vtkNew<vtkCleanPolyData> clean_poly_data_out;
    clean_poly_data_out->SetInputData(tri2->GetOutput());
    clean_poly_data_out->Update();
    vtkNew<vtkAppendFilter> append_filter;
    append_filter->AddInputData(smooth_filter->GetOutput());
    append_filter->AddInputData(clean_poly_data_out->GetOutput());
    append_filter->Update();
    vtkNew<vtkGeometryFilter> geometry_filter;
    geometry_filter->SetInputData(append_filter->GetOutput());
    geometry_filter->Update();
    vtkNew<vtkCleanPolyData> clean_filter;
    clean_filter->SetInputConnection(geometry_filter->GetOutputPort());
    clean_filter->Update();
    this->surface_ = clean_filter->GetOutput();
}

void SmoothSurfaceSelector::UpdateSeedContour(const QList<QList<double>> &pos_list) {
    double direction[3];
    this->renderer_->GetRenderer()->GetActiveCamera()
    ->GetDirectionOfProjection(direction);
    vtkNew<vtkPoints> points;
    for (qint32 i = 0; i < pos_list.size(); ++i) {
        points->InsertNextPoint(pos_list[i][0], pos_list[i][1], pos_list[i][2]);
    }
    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(points);
    spline->SetClosed(true);
    vtkNew<vtkParametricFunctionSource> function_source;
    function_source->SetParametricFunction(spline);
    function_source->Update();
    vtkNew<vtkLinearExtrusionFilter> extrude;
    extrude->SetInputData(function_source->GetOutput());
    extrude->SetExtrusionTypeToNormalExtrusion();
    extrude->SetVector(direction[0] * 1000,
                       direction[1] * 1000,
                       direction[2] * 1000);
    extrude->CappingOn();
    extrude->Update();
    this->curved_surface_ = extrude->GetOutput();
    vtkNew<vtkPoints> contour_points;
    for (qint32 i = 0; i < pos_list.size(); ++i) {
        contour_points->InsertNextPoint(pos_list[i][0] + direction[0] * 20,
                                        pos_list[i][1] + direction[1] * 20,
                                        pos_list[i][2] + direction[2] * 20);
    }
    vtkIdType *vertex_indices = new vtkIdType[
     static_cast<quint64>(pos_list.size() + 1)];
    for (qint32 i = 0; i < pos_list.size(); ++i) {
        vertex_indices[i] = static_cast<vtkIdType>(i);
    }
    vertex_indices[pos_list.size()] = 0;
    vtkNew<vtkCellArray> contour_lines;
    contour_lines->InsertNextCell(pos_list.size() + 1, vertex_indices);
    delete[] vertex_indices;
    vtkNew<vtkPolyData> contour_poly_data;
    contour_poly_data->SetPoints(contour_points);
    contour_poly_data->SetLines(contour_lines);
    contour_widget_->On();
    contour_widget_->Initialize(contour_poly_data);
}

void SmoothSurfaceSelector::SlotKeyPressed(const QString &key) {
    if (this->seed_widget_->GetEnabled() == 1) {
        if (key == "space") {
            if (this->method_ == SMOOTH) {
                this->SmoothCallback();
                emit SignalSelectorFinish();
            } else if (this->method_ == CUT) {
                this->ClipCallback();
                emit SignalSelectorFinish();
            }
        } else if (key == "q" || key == "Q") {
            this->seed_widget_->Off();
            this->contour_widget_->Off();
            this->renderer_->Render();
            emit SignalSelectorFinish();
        }
    }
}

void SmoothSurfaceSelector::SlotSeedChanged(
    vtkObject *caller, unsigned long vtk_event,
    void *client_data, void *call_data) {
    Q_UNUSED(client_data);
    if (vtk_event == vtkCommand::PlacePointEvent ||
            vtk_event == vtkCommand::InteractionEvent) {
        qint32 n = *static_cast<int *>(call_data);
        vtkSmartPointer<vtkSeedWidget> widget = dynamic_cast<vtkSeedWidget *>(caller);
        if (n >= 0 && widget) {
            qint32 num_seeds = widget->GetSeedRepresentation()->GetNumberOfSeeds();
            if (num_seeds > 0) {
                QList<QList<double>> pos_list;
                double world_pos[3];
                for (qint32 i = 0; i < num_seeds; ++i) {
                    this->seed_widget_->GetSeedRepresentation()->GetSeedWorldPosition(
                        static_cast<quint32>(i), world_pos);
                    pos_list.append({world_pos[0], world_pos[1], world_pos[2]});
                }
                this->UpdateSeedContour(pos_list);
            }
        }
    }
}
