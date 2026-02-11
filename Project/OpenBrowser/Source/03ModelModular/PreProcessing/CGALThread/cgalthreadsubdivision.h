//#ifndef CGALTHREADSUBDIVISIONMETHODS_H
//#define CGALTHREADSUBDIVISIONMETHODS_H

///*
// * CGAL 表面细分算法
// * */

//// 01 Frame includes
//#include "stable.h"

//// 03 CGAL includes
//#include "cgalthread.h"

//// VTK includes
//#include <vtkPolyData.h>
//#include <vtkSmartPointer.h>

//class CGALThreadSubdivision : public CGALThread {
//    Q_OBJECT
//  public:
//    explicit CGALThreadSubdivision(QObject *parent = nullptr);
//    virtual ~CGALThreadSubdivision() override;
//    void doWork();
//    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
//    vtkSmartPointer<vtkPolyData> GetSurface();
//  protected:
//    virtual void run() override;
//    bool STL2OFF(const QString off_filename);
//    void OFF2STL(const QString off_filename);
//    vtkPolyData *CustomReader(std::istream &infile);
//  private:
//    bool CGALFunctionSubdivision();
//    vtkSmartPointer<vtkPolyData> surface_;// polydata 数据
//};

//#endif // CGALSURFACESUBDIVISIONMETHODS_H
