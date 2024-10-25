#include "S3DWidget.h"

#include <QtWidgets>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>

S3DWidget::S3DWidget(QWidget * parent)
  : QVTKOpenGLNativeWidget(parent)
{
    installEventFilter(this);
    vtkObject::GlobalWarningDisplayOff();

    renderWindow()->SetMultiSamples(0);
    renderWindow()->AddRenderer(m_Renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    interactor()->SetInteractorStyle(style);

    LoadAndDisplayModel("");
}

S3DWidget::~S3DWidget()
{
    m_Options.activateShortcutKeys = false;
    m_Options.showGrid = false;
    m_Options.showAxis = false;
    m_Options.showCameraOrientation = false;
    m_Options.showEdge = false;
    m_Options.useSSAOPass = false;
    m_Options.useToneMappingPass = false;
    SetOptions(m_Options);
}

void S3DWidget::LoadAndDisplayModel(const QString & filePath)
{
    m_Renderer->Initialize();

    vtkNew<vtkOBJImporter> m_Importer;
    m_Importer->SetRenderWindow(renderWindow());
    m_Importer->SetFileName(filePath.toStdString().c_str());
    m_Importer->Update();

    m_Renderer->SetupRenderPasses();
    m_Renderer->InitializeCamera();

    SetOptions(m_Options);
}

void S3DWidget::SwitchModel(const int & direction)
{
    if (direction == 0 && m_ObjFiles.size() > 1) {
        m_CurrentObjIndex = 0;
        LoadAndDisplayModel(m_ObjFiles.at(m_CurrentObjIndex));
        return;
    }

    if (m_ObjFiles.size() < 2) {
        return;
    }

    m_CurrentObjIndex += direction;
    if (m_CurrentObjIndex < 0) {
        m_CurrentObjIndex = m_ObjFiles.size() - 1;
    }
    if (m_CurrentObjIndex >= m_ObjFiles.size()) {
        m_CurrentObjIndex = 0;
    }
    if (m_ObjFiles.size() > m_CurrentObjIndex) {
        LoadAndDisplayModel(m_ObjFiles.at(m_CurrentObjIndex));
    }
}

void S3DWidget::SetOptions(const Options & options)
{
    m_Options = options;
    m_Renderer->ShowAxis(options.showAxis);
    m_Renderer->ShowCameraOrientation(options.showCameraOrientation);
    m_Renderer->ShowGrid(options.showGrid);
    m_Renderer->ShowEdge(options.showEdge);
    m_Renderer->SetUseSSAOPass(options.useSSAOPass);
    m_Renderer->SetUseToneMappingPass(options.useToneMappingPass);
    renderWindow()->Render();
}

void S3DWidget::SetObjFiles(const QStringList & objFiles)
{
    m_ObjFiles = objFiles;
    SwitchModel();
}

bool S3DWidget::eventFilter(QObject * obj, QEvent * event)
{
    if (event->type() == QEvent::KeyPress) {
        if (!m_Options.activateShortcutKeys) {
            return true;
        }

        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_A) {
            m_Options.showAxis = !m_Options.showAxis;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_C) {
            m_Options.showCameraOrientation = !m_Options.showCameraOrientation;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_G) {
            m_Options.showGrid = !m_Options.showGrid;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_E) {
            m_Options.showEdge = !m_Options.showEdge;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_S) {
            m_Options.useSSAOPass = !m_Options.useSSAOPass;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_T) {
            m_Options.useToneMappingPass = !m_Options.useToneMappingPass;
            SetOptions(m_Options);
        } else if (keyEvent->key() == Qt::Key_Left) {
            SwitchModel(-1);
        } else if (keyEvent->key() == Qt::Key_Right) {
            SwitchModel(1);
        }
        return true;
    }

    return false;
}
