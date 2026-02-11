#include "QVtkWidget.h"

#include <QPainter>
#include <QResizeEvent>

#include <vtkOpenGLRenderer.h>
#include <vtkCamera.h>

namespace {
void UpdateVtkWidgetGeometry(QVTKOpenGLNativeWidget * vtkWidget, int borderThickness, int parentW, int parentH)
{
    if (!vtkWidget) {
        return;
    }
    vtkWidget->move(borderThickness, borderThickness);
    int pad = borderThickness * 2;
    int w = parentW - pad;
    int h = parentH - pad;
    if (w < 0) {
        w = 0;
    }
    if (h < 0) {
        h = 0;
    }
    vtkWidget->setFixedSize(w, h);
}
} // namespace

QVtkWidget::QVtkWidget(QWidget * parent)
  : QWidget(parent)
{
    m_VtkWidget = new QVTKOpenGLNativeWidget(this);
    m_VtkWidget->move(4, 4);
    m_VtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QVtkWidget::~QVtkWidget()
{
    if (m_Renderer && m_RenderWindow) {
        m_RenderWindow->RemoveRenderer(m_Renderer);
    }
}

void QVtkWidget::InitializeVTK()
{
    if (m_Initialized) {
        return;
    }

    m_RenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    m_VtkWidget->setRenderWindow(m_RenderWindow);
    m_RenderWindow->SetDoubleBuffer(true);

    if (!m_Renderer) {
        SetupDefaultRenderer();
    } else {
        m_RenderWindow->AddRenderer(m_Renderer);
    }

    m_RenderWindow->GetInteractor()->SetDesiredUpdateRate(50);
    m_Initialized = true;
}

void QVtkWidget::SetupDefaultRenderer()
{
    m_Renderer = vtkSmartPointer<vtkOpenGLRenderer>::New();
    m_RenderWindow->AddRenderer(m_Renderer);

    m_Renderer->SetUseDepthPeeling(true);
    m_Renderer->SetMaximumNumberOfPeels(100);
    m_Renderer->SetOcclusionRatio(0.0);
}

void QVtkWidget::Render()
{
    if (m_RenderWindow) {
        m_RenderWindow->Render();
    }
}

void QVtkWidget::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
    if (!renderer) {
        return;
    }

    if (m_Renderer && m_RenderWindow) {
        m_RenderWindow->RemoveRenderer(m_Renderer);
    }
    m_Renderer = renderer;
    if (m_RenderWindow) {
        m_RenderWindow->AddRenderer(m_Renderer);
        Render();
    }
}

vtkRenderer * QVtkWidget::Renderer() const
{
    return m_Renderer;
}

vtkGenericOpenGLRenderWindow * QVtkWidget::RenderWindow() const
{
    return m_RenderWindow;
}

void QVtkWidget::SetBorderThickness(int thickness)
{
    if (thickness < 0) {
        thickness = 0;
    }
    if (m_BorderThickness == thickness) {
        return;
    }
    m_BorderThickness = thickness;

    UpdateVtkWidgetGeometry(m_VtkWidget, m_BorderThickness, width(), height());
    update();
}

void QVtkWidget::SetBorderColor(const QColor & color)
{
    if (m_BorderColor == color) {
        return;
    }
    m_BorderColor = color;
    update();
}

void QVtkWidget::SetInteractionCenter(const QVector3D & center)
{
    if (m_Renderer) {
        vtkCamera * camera = m_Renderer->GetActiveCamera();
        camera->SetFocalPoint(center.x(), center.y(), center.z());
        m_Renderer->ResetCamera();
        Render();
    }
}

void QVtkWidget::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
    InitializeVTK();
}

void QVtkWidget::resizeEvent(QResizeEvent * event)
{
    UpdateVtkWidgetGeometry(m_VtkWidget, m_BorderThickness, width(), height());
    QWidget::resizeEvent(event);
}

void QVtkWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(QPen(m_BorderColor, m_BorderThickness, Qt::SolidLine));
    QRect r = rect();
    int half = m_BorderThickness / 2;
    r.adjust(half, half, -half, -half);
    painter.drawRect(r);
}