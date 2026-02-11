//// 01 Frame includes
//#include "cgalthreadsubdivision.h"

//// C++ includes
//#include <fstream>
//#include <iostream>
//#include <string.h>

//// VTK includes
//#include <vtkPoints.h>
//#include <vtkPolyData.h>
//#include <vtkCellArray.h>
//#include <vtkSmartPointer.h>

//// CGAL includes
//#include <CGAL/Timer.h>
//#include <CGAL/Surface_mesh.h>
//#include <boost/lexical_cast.hpp>
//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/subdivision_method_3.h>
//#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>

//CGALThreadSubdivision::CGALThreadSubdivision(QObject *parent) :
//    CGALThread(parent) {
//    this->surface_ = nullptr;
//}

//CGALThreadSubdivision::~CGALThreadSubdivision() {
//}

//void CGALThreadSubdivision::doWork() {
//    this->SetResult(false);
//    QFile::remove("subdivision.off");
//    if (STL2OFF("subdivision.off")) {
//        if (CGALFunctionSubdivision()) {
//            OFF2STL("subdivision.off");
//            this->SetResult(true);
//        } else {
//            this->SetResult(false);
//        }
//    }
//}

//void CGALThreadSubdivision::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
//    this->surface_ = value;
//}

//vtkSmartPointer<vtkPolyData> CGALThreadSubdivision::GetSurface() {
//    return this->surface_;
//}

//void CGALThreadSubdivision::run() {
//    this->InitialResult();
//    this->doWork();
//}

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Surface_mesh.h>
//#include <CGAL/Polygon_mesh_processing/self_intersections.h>
//#include <fstream>
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Simple_cartesian<double>         Kernel;
//typedef CGAL::Surface_mesh<Kernel::Point_3>             PolygonMesh;

//typedef boost::graph_traits<PolygonMesh>::face_descriptor face_descriptor;
//namespace PMP = CGAL::Polygon_mesh_processing;

//bool CGALThreadSubdivision::CGALFunctionSubdivision() {
////    typedef CGAL::Simple_cartesian<double>         Kernel;
////    typedef CGAL::Surface_mesh<Kernel::Point_3>    PolygonMesh;
////    using namespace std;
////    using namespace CGAL;
////    namespace params = CGAL::parameters;
////    PolygonMesh pmesh;
////    std::ifstream in("subdivision.off");
////    if (in.fail()) {
////        qWarning() << "Could not open input file ";
////        return false;
////    }
////    in >> pmesh;
////    Subdivision_method_3::CatmullClark_subdivision(
////        pmesh, params::number_of_iterations(1));
////    Timer t;
////    t.start();
////    Subdivision_method_3::CatmullClark_subdivision(
////        pmesh, params::number_of_iterations(1));
////    std::cerr << "Done (" << t.time() << " s)" << std::endl;
////    std::ofstream out("subdivision.off");
////    out << pmesh;
////    out.close();
////    return true;

//    const char *filename =  "subdivision.off";
//    std::ifstream input(filename);
//    PolygonMesh mesh;
//    if (!input || !(input >> mesh) || !CGAL::is_triangle_mesh(mesh)) {
//        std::cerr << "Not a valid input file." << std::endl;
//        return 1;
//    }
//    bool intersecting = PMP::does_self_intersect(mesh,
//                        PMP::parameters::vertex_point_map(
//                            get(CGAL::vertex_point, mesh)));
//    std::cout
//            << (intersecting ?
//                "There are self-intersections." :
//                "There is no self-intersection.")
//            << std::endl;
//    std::vector<std::pair<face_descriptor, face_descriptor> > intersected_tris;
//    PMP::self_intersections(mesh, std::back_inserter(intersected_tris));
//    std::cout << intersected_tris.size()
//              << " pairs of triangles intersect." << std::endl;

//    return false;
//}

//bool CGALThreadSubdivision::STL2OFF(const QString off_filename) {
//    qDebug();
//    if (off_filename.isEmpty()) {
//        return false;
//    }
//    if (this->surface_ == nullptr) {
//        return false;
//    }
//    double x[3];
//    QFile file(off_filename);
//    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
//        QTextStream stream(&file);
//        stream.seek(file.size());
//        stream << "OFF" << "\n";
//        if (this->surface_->GetNumberOfPoints() == 0) {
//            qDebug() << "vtkPolyData 为空";
//            file.close();
//            return false;
//        }
//        stream << this->surface_->GetNumberOfPoints() << " "
//               << this->surface_->GetNumberOfCells() << " 0\n";
//        for (qint32 ww = 0; ww < this->surface_->GetNumberOfPoints() ; ww++) {
//            this->surface_->GetPoint(ww, x);
//            stream << x[0] << " " << x[1] << " " << x[2] << "\n";
//        }
//        for (qint32 ww = 0; ww < this->surface_->GetNumberOfCells() ; ww++) {
//            stream << this->surface_->GetCell(ww)->GetNumberOfPoints() << " ";
//            for (qint32 i = 0; i <
//                    this->surface_->GetCell(ww)->GetNumberOfPoints(); i++) {
//                stream << this->surface_->GetCell(ww)->GetPointId(i) << " ";
//            }
//            stream << "\n";
//        }
//    }
//    file.close();
//    return true;
//}

//void CGALThreadSubdivision::OFF2STL(const QString off_filename) {
//    std::string inputFilename = off_filename.toLocal8Bit().data();
//    std::ifstream fin(inputFilename.c_str());
//    if (this->surface_ == nullptr) {
//        return;
//    }
//    this->surface_ = vtkSmartPointer<vtkPolyData>::Take(CustomReader(fin));
//    fin.close();
//}

//vtkPolyData *CGALThreadSubdivision::CustomReader(istream &infile) {
//    qDebug();
//    char buf[1000];
//    infile.getline(buf, 1000);
//    if (strcmp(buf, "off") == 0 || strcmp(buf, "OFF") == 0) {
//        vtkIdType number_of_points, number_of_triangles, number_of_lines;
//        infile >> number_of_points >> number_of_triangles >> number_of_lines;
//        vtkSmartPointer<vtkPoints> points
//            = vtkSmartPointer<vtkPoints>::New();
//        points->SetNumberOfPoints(number_of_points);
//        for (vtkIdType i = 0; i < number_of_points; i++) {
//            double x, y, z;
//            infile >> x >> y >> z;
//            points->SetPoint(i, x, y, z);
//        }
//        vtkSmartPointer<vtkCellArray> polys
//            = vtkSmartPointer<vtkCellArray>::New();
//        int n;
//        vtkIdType type;
//        for (vtkIdType i = 0; i < number_of_triangles; i++) {
//            infile >> n;
//            polys->InsertNextCell(n);
//            for (; n > 0; n--) {
//                infile >> type;
//                polys->InsertCellPoint(type);
//            }
//        }
//        vtkPolyData *polydata = vtkPolyData::New();
//        polydata->SetPoints(points);
//        polydata->SetPolys(polys);
//        return polydata;
//    }
//    vtkPolyData *polydata = vtkPolyData::New();
//    return polydata;
//}
