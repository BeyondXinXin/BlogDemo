// 01 Frame includes
#include "cgalthreadfill.h"

// C++ includes
#include <fstream>
#include <iostream>
#include <vector>
//#include <boost/foreach.hpp>

//// CGAL includes
//#include <CGAL/Polyhedron_3.h>
//#include <CGAL/Surface_mesh.h>
//#include <CGAL/read_vtk_image_data.h>
//#include <CGAL/Polygon_mesh_processing/corefinement.h>
//#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// VTK includes
#include <vtkActor.h>
#include <vtkAppendFilter.h>
#include <vtkCellIterator.h>
#include <vtkCleanPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkGenericCell.h>
#include <vtkGeometryFilter.h>
#include <vtkMassProperties.h>
#include <vtkNamedColors.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkThreshold.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

FillSurfaceSelector::FillSurfaceSelector(QObject *parent)
  : QObject(parent)
{
    this->Initial();
}

FillSurfaceSelector::~FillSurfaceSelector()
{
    if (this->own_renderer_ == true) {
        this->vmtk_renderer_->deleteLater();
    }
}

void FillSurfaceSelector::Execute()
{
    qDebug();
    if (this->vmtk_renderer_ == nullptr) {
        this->vmtk_renderer_ = new CustomVtkRenderer();
        this->vmtk_renderer_->Execute();
        this->own_renderer_ = true;
    }
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkFillHolesFilter> fill_holes_filter;
    fill_holes_filter->SetInputData(this->surface_);
    fill_holes_filter->SetHoleSize(100000.0);
    fill_holes_filter->Update();
    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputData(fill_holes_filter->GetOutput());
    normals->ConsistencyOn();
    normals->SplittingOff();
    normals->Update();
    normals->GetOutput()->GetPointData()->SetNormals(
      this->surface_->GetPointData()->GetNormals());
    vtkIdType num_original_cells = this->surface_->GetNumberOfCells();
    vtkIdType num_new_cells = normals->GetOutput()->GetNumberOfCells();
    vtkSmartPointer<vtkCellIterator> it = normals->GetOutput()->NewCellIterator();
    vtkIdType num_cells = 0;
    for (it->InitTraversal();
         !it->IsDoneWithTraversal() && num_cells < num_original_cells;
         it->GoToNextCell(), ++num_cells) {
    }
    vtkNew<vtkPolyData> hole_poly_data;
    hole_poly_data->Allocate(normals->GetOutput(), num_new_cells - num_original_cells);
    hole_poly_data->SetPoints(normals->GetOutput()->GetPoints());
    vtkNew<vtkGenericCell> cell;
    for (; !it->IsDoneWithTraversal(); it->GoToNextCell()) {
        it->GetCell(cell);
        hole_poly_data->InsertNextCell(it->GetCellType(), cell->GetPointIds());
    }
    hole_poly_data->Modified();
    vtkNew<vtkConnectivityFilter> connectivity;
    connectivity->SetInputData(hole_poly_data);
    connectivity->SetExtractionModeToAllRegions();
    connectivity->ColorRegionsOn();
    connectivity->Update();
    if (this->first_connect_) {
        connect(this->vmtk_renderer_, &CustomVtkRenderer::SignalMouseClicked,
                this, &FillSurfaceSelector::SlotMouseClicked);
        connect(this->vmtk_renderer_, &CustomVtkRenderer::SignalKeyPressed,
                this, &FillSurfaceSelector::SlotKeyPressed);
    }
    while (this->filled_actor_list_.size() > 0) {
        vtkSmartPointer<vtkActor> actor = filled_actor_list_.takeFirst();
        this->vmtk_renderer_->GetRenderer()->RemoveActor(actor);
    }
    this->filled_surface_list_.clear();
    this->fill_id_ = -1;
    for (qint32 i = 0; i < connectivity->GetNumberOfExtractedRegions(); ++i) {
        vtkNew<vtkThreshold> threshold;
        threshold->SetInputData(connectivity->GetOutput());
        threshold->ThresholdBetween(i - 0.5, i + 0.5);
        threshold->SetInputArrayToProcess(0, 0, 0,
                                          vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                          "RegionId");
        threshold->Update();
        vtkNew<vtkGeometryFilter> geometry_filter;
        geometry_filter->SetInputData(threshold->GetOutput());
        geometry_filter->Update();
        vtkNew<vtkTriangleFilter> triangle_filter;
        triangle_filter->SetInputData(geometry_filter->GetOutput());
        triangle_filter->Update();
        vtkNew<vtkCleanPolyData> clean_poly_data;
        clean_poly_data->SetInputData(triangle_filter->GetOutput());
        clean_poly_data->Update();
        this->filled_surface_list_.append(clean_poly_data->GetOutput());
        vtkNew<vtkPolyDataMapper> filled_mapper;
        filled_mapper->SetInputData(geometry_filter->GetOutput());
        filled_mapper->SetScalarRange(geometry_filter->GetOutput()->GetPointData()->GetArray("RegionId")->GetRange());
        filled_mapper->SetArrayId(i);
        filled_mapper->ScalarVisibilityOff();
        filled_mapper->Update();
        vtkNew<vtkActor> filled_actor;
        filled_actor->SetMapper(filled_mapper);
        filled_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->vmtk_renderer_->GetRenderer()->AddActor(filled_actor);
        this->filled_actor_list_.append(filled_actor);
    }
    this->enable_ = true;
    this->vmtk_renderer_->Render();
    if (this->own_renderer_) {
        this->vmtk_renderer_->Deallocate();
    }
}

void FillSurfaceSelector::SelectorOff()
{
    for (const vtkSmartPointer<vtkActor> &actor : this->filled_actor_list_) {
        this->vmtk_renderer_->GetRenderer()->RemoveActor(actor);
    }
    this->last_picked_actor_ = nullptr;
    this->last_picked_property_ = vtkSmartPointer<vtkProperty>::New();
    enable_ = false;
}

void FillSurfaceSelector::SelectorOn()
{
    for (const vtkSmartPointer<vtkActor> &actor : this->filled_actor_list_) {
        this->vmtk_renderer_->GetRenderer()->AddActor(actor);
    }
    enable_ = true;
}

void FillSurfaceSelector::SetVmtkRenderer(const QPointer<CustomVtkRenderer> value)
{
    this->vmtk_renderer_ = value;
}

void FillSurfaceSelector::SetSurface(const vtkSmartPointer<vtkPolyData> value)
{
    this->surface_ = value;
}

vtkSmartPointer<vtkPolyData> FillSurfaceSelector::GetSurface()
{
    return this->surface_;
}

qint32 FillSurfaceSelector::GetFillCount()
{
    return this->filled_actor_list_.size();
}

void FillSurfaceSelector::Initial()
{
    this->enable_ = false;
    this->first_connect_ = true;
    this->own_renderer_ = false;
    this->fill_id_ = -1;
    this->vmtk_renderer_ = nullptr;
    this->surface_ = nullptr;
    this->surface_region_ = nullptr;
    this->last_picked_actor_ = nullptr;
    this->last_picked_property_ = vtkSmartPointer<vtkProperty>::New();
}

void FillSurfaceSelector::FillerCallback()
{
    qDebug();
    vtkNew<vtkAppendFilter> append_filter;
    append_filter->AddInputData(this->filled_surface_list_.at(this->fill_id_));
    append_filter->Update();
    vtkNew<vtkGeometryFilter> geometry_filter;
    geometry_filter->SetInputData(append_filter->GetOutput());
    geometry_filter->Update();
    vtkNew<vtkCleanPolyData> clean_filter;
    clean_filter->SetInputConnection(geometry_filter->GetOutputPort());
    clean_filter->Update();
    this->surface_region_ = this->filled_surface_list_.at(this->fill_id_);
    STL2OFF("fill.off", 0);
    STL2OFF("fill_region.off", 1);
    if (CGALFunctionFill() && (OFF2STL("fill.off"))) {
        emit SignalFillFinish();
        return;
    } else {
        vtkNew<vtkAppendFilter> append_filter;
        append_filter->AddInputData(this->surface_);
        append_filter->AddInputData(this->surface_region_);
        append_filter->Update();
        vtkNew<vtkGeometryFilter> geometry_filter;
        geometry_filter->SetInputData(append_filter->GetOutput());
        geometry_filter->Update();
        vtkNew<vtkCleanPolyData> clean_filter;
        clean_filter->SetInputConnection(geometry_filter->GetOutputPort());
        clean_filter->Update();
        this->surface_ = clean_filter->GetOutput();
        emit SignalFillFinish();
    }
}

void FillSurfaceSelector::SlotKeyPressed(const QString &key)
{
    if (this->enable_) {
        if (key == "space" && (fill_id_ >= 0)) {
            this->SelectorOff();
            this->FillerCallback();
        } else if (key == "q") {
            this->SelectorOff();
            emit SignalFillFinish();
        }
    }
}

void FillSurfaceSelector::SlotMouseClicked(const qint32 x, const qint32 y)
{
    if (!this->enable_) {
        return;
    }
    vtkNew<vtkPropPicker> picker;
    picker->Pick(x, y, 0, this->vmtk_renderer_->GetRenderer());
    if (!this->filled_actor_list_.contains(picker->GetActor())) {
        return;
    }
    if (this->last_picked_actor_) {
        this->last_picked_actor_->GetProperty()->DeepCopy(this->last_picked_property_);
    }
    this->last_picked_actor_ = picker->GetActor();
    if (this->last_picked_actor_) {
        this->last_picked_property_->DeepCopy(this->last_picked_actor_->GetProperty());
        this->last_picked_actor_->GetProperty()->SetColor(0.0, 1.0, 0.0);
        this->last_picked_actor_->GetProperty()->SetDiffuse(1.0);
        this->last_picked_actor_->GetProperty()->SetSpecular(0.0);
        fill_id_ = this->last_picked_actor_->GetMapper()->GetArrayId();
    }
}

void FillSurfaceSelector::STL2OFF(const QString off_filename, const qint32 num)
{
    qDebug();
    if (num == 0) {
        if (this->surface_ == nullptr) {
            return;
        }
        if (off_filename.isEmpty()) {
            return;
        }
        double x[3];
        QFile file(off_filename);
        file.open(QIODevice::WriteOnly);
        file.close();
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.seek(file.size());
            stream << "OFF"
                   << "\n";
            stream << this->surface_->GetNumberOfPoints() << " "
                   << this->surface_->GetNumberOfCells() << " 0\n";
            for (qint32 ww = 0; ww < this->surface_->GetNumberOfPoints(); ww++) {
                this->surface_->GetPoint(ww, x);
                stream << x[0] << " " << x[1] << " " << x[2] << "\n";
            }
            for (qint32 ww = 0; ww < this->surface_->GetNumberOfCells(); ww++) {
                stream << this->surface_->GetCell(ww)->GetNumberOfPoints() << " ";
                for (qint32 i = 0; i < this->surface_->GetCell(ww)->GetNumberOfPoints(); i++) {
                    stream << this->surface_->GetCell(ww)->GetPointId(i) << " ";
                }
                stream << "\n";
            }
            file.close();
        }
    } else if (num == 1) {
        if (this->surface_region_ == nullptr) {
            return;
        }
        if (off_filename.isEmpty()) {
            return;
        }
        double x[3];
        QFile file(off_filename);
        file.open(QIODevice::WriteOnly);
        file.close();
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.seek(file.size());
            stream << "OFF"
                   << "\n";
            stream << this->surface_region_->GetNumberOfPoints() << " "
                   << this->surface_region_->GetNumberOfCells() << " 0\n";
            for (qint32 ww = 0; ww < this->surface_region_->GetNumberOfPoints(); ww++) {
                this->surface_region_->GetPoint(ww, x);
                stream << x[0] << " " << x[1] << " " << x[2] << "\n";
            }
            for (qint32 ww = 0; ww < this->surface_region_->GetNumberOfCells(); ww++) {
                stream << this->surface_region_->GetCell(ww)->GetNumberOfPoints() << " ";
                for (qint32 i = 0; i < this->surface_region_->GetCell(ww)->GetNumberOfPoints(); i++) {
                    stream << this->surface_region_->GetCell(ww)->GetPointId(i) << " ";
                }
                stream << "\n";
            }
            file.close();
        }
    }
}

bool FillSurfaceSelector::OFF2STL(const QString off_filename)
{
    std::string inputFilename = off_filename.toLocal8Bit().data();
    std::ifstream fin(inputFilename.c_str());
    vtkSmartPointer<vtkPolyData> surface = CustomReader(fin);
    vtkSmartPointer<vtkTriangleFilter> triangleFilter =
      vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->SetInputData(surface);
    vtkSmartPointer<vtkPolyDataNormals> normals =
      vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputConnection(triangleFilter->GetOutputPort());
    normals->ConsistencyOn();
    normals->SplittingOff();
    vtkSmartPointer<vtkMassProperties> massProperties =
      vtkSmartPointer<vtkMassProperties>::New();
    massProperties->SetInputConnection(normals->GetOutputPort());
    massProperties->Update();
    fin.close();
    if (massProperties->GetSurfaceArea() > 0.01) {
        this->surface_ = surface;
        if (this->surface_ == nullptr) {
            return false;
        }
        return true;
    }
    return false;
}

vtkSmartPointer<vtkPolyData> FillSurfaceSelector::CustomReader(std::istream &infile)
{
    qDebug();
    char buf[1000];
    infile.getline(buf, 1000);
    if (strcmp(buf, "off") == 0 || strcmp(buf, "OFF") == 0) {
        vtkIdType number_of_points, number_of_triangles, number_of_lines;
        infile >> number_of_points >> number_of_triangles >> number_of_lines;
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        points->SetNumberOfPoints(number_of_points);
        for (vtkIdType i = 0; i < number_of_points; i++) {
            double x, y, z;
            infile >> x >> y >> z;
            points->SetPoint(i, x, y, z);
        }
        vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
        qint32 n;
        vtkIdType type;
        for (vtkIdType i = 0; i < number_of_triangles; i++) {
            infile >> n;
            polys->InsertNextCell(n);
            for (; n > 0; n--) {
                infile >> type;
                polys->InsertCellPoint(type);
            }
        }
        vtkPolyData *polydata = vtkPolyData::New();
        polydata->SetPoints(points);
        polydata->SetPolys(polys);
        return polydata;
    }
    vtkPolyData *polydata = vtkPolyData::New();
    return polydata;
}

bool FillSurfaceSelector::CGALFunctionFill()
{
    //    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    //    typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
    //    typedef Polyhedron::Halfedge_handle Halfedge_handle;
    //    typedef Polyhedron::Facet_handle Facet_handle;
    //    typedef Polyhedron::Vertex_handle Vertex_handle;
    //    typedef Kernel::Point_3 Point;
    //    typedef Polyhedron::Halfedge_iterator Iterator;
    //    std::ifstream input;
    //    input.open("fill_region.off");
    //    Polyhedron poly_region;
    //    if (!(input >> poly_region)) {
    //        std::cerr << "01 Not a valid off file." << std::endl;
    //        return false;
    //    }
    //    input.close();
    //    input.open("fill.off");
    //    Polyhedron poly;
    //    if (!(input >> poly)) {
    //        std::cerr << "02 Not a valid off file." << std::endl;
    //        return false;
    //    }
    //    input.close();
    //    QList<Point> region_list;
    //    Halfedge_handle region;
    //    qint32 num = 0;
    //    for (Iterator e = poly_region.halfedges_begin();
    //         num < 10; ++e) {
    //        ++num;
    //        region_list.push_back(e->vertex()->point());
    //    }
    //    BOOST_FOREACH (Halfedge_handle h, halfedges(poly)) {
    //        if (h->is_border()) {
    //            Point tmp;
    //            foreach (tmp, region_list) {
    //                if (h->vertex()->point() == tmp) {
    //                    std::vector<Facet_handle> patch_facets;
    //                    std::vector<Vertex_handle> patch_vertices;
    //                    if (CGAL::cpp11::get<0>(
    //                          CGAL::Polygon_mesh_processing::
    //                            triangulate_refine_and_fair_hole(
    //                              poly, h,
    //                              std::back_inserter(patch_facets),
    //                              std::back_inserter(patch_vertices),
    //                              CGAL::Polygon_mesh_processing::parameters::
    //                                vertex_point_map(get(CGAL::vertex_point, poly))
    //                                  .geom_traits(Kernel())))) {
    //                    }
    //                    if (patch_facets.size() > 0) {
    //                        std::ofstream out("fill.off");
    //                        out << poly;
    //                        out.close();
    //                        return true;
    //                    }
    //                }
    //            }
    //        }
    //    }
    return false;
}
