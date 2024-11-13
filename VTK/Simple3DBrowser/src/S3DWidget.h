#ifndef S3DWidget_H
#define S3DWidget_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkNew.h>
#include <vtkOBJImporter.h>

#include "renderer/S3DRenderer.h"

class S3DWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT

public:
    struct Options
    {
        bool activateShortcutKeys = false;
        bool showGrid = true;
        bool showAxis = true;
        bool showCameraOrientation = true;
        bool showEdge = false;
        bool useSSAOPass = false;
        bool useToneMappingPass = false;
    };

public:
    explicit S3DWidget(QWidget * parent = nullptr);
    ~S3DWidget() override;

    void LoadAndDisplayModel(const QString & filePath = "");
    void SwitchModel(const int & direction = 0);

    void SetOptions(const Options & options);
    void SetObjFiles(const QStringList & objFiles);

protected:
    bool eventFilter(QObject * obj, QEvent * event);

private:
    vtkNew<S3DRenderer> m_Renderer;
    Options m_Options;

    QStringList m_ObjFiles;
    int m_CurrentObjIndex = 0;
};

#endif // SimpleWidget_H
