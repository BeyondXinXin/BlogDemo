#include "QVtkWidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>

#include <vtkOpenGLRenderer.h>
#include <vtkAbstractWidget.h>
#include <vtkRenderWindowInteractor.h>

#include "DrawOverlayWidget.h"

namespace {
void UpdateWidgetGeometry(QWidget * widget, int borderThickness, int parentW, int parentH)
{
    if (!widget) {
        return;
    }
    widget->move(borderThickness, borderThickness);
    int pad = borderThickness * 2;
    int w = parentW - pad;
    int h = parentH - pad;
    if (w < 0) {
        w = 0;
    }
    if (h < 0) {
        h = 0;
    }
    widget->setFixedSize(w, h);
}
} // namespace

QVtkWidget::QVtkWidget(QWidget * parent)
  : QWidget(parent)
{
    m_VtkWidget = new QVTKOpenGLNativeWidget(this);
    m_VtkWidget->move(4, 4);
    m_VtkWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_DrawOverlay = new DrawOverlayWidget(this);
    m_DrawOverlay->move(4, 4);
    m_DrawOverlay->hide();

    InitializeUI();

    m_VtkWidget->installEventFilter(this);
}

QVtkWidget::~QVtkWidget()
{
    if (m_Renderer && m_RenderWindow) {
        m_RenderWindow->RemoveRenderer(m_Renderer);
    }
}

bool QVtkWidget::eventFilter(QObject * watched, QEvent * event)
{
    if (watched == m_VtkWidget && m_InteractionMode == DrawCutLine) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::RightButton) {
                m_IsDrawing = true;
                m_CuttingPoints.clear();
                m_CuttingPoints.append(mouseEvent->pos());
                if (m_DrawOverlay) {
                    m_DrawOverlay->SetPoints(m_CuttingPoints);
                }
                SetCutButtonsVisible(false);
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (m_IsDrawing) {
                m_CuttingPoints.append(mouseEvent->pos());
                if (m_DrawOverlay) {
                    m_DrawOverlay->SetPoints(m_CuttingPoints);
                }
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::RightButton && m_IsDrawing) {
                m_IsDrawing = false;
                if (m_CuttingPoints.size() >= 2) {
                    SetCutButtonsVisible(true);
                }
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
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

void QVtkWidget::InitializeUI()
{
    m_ButtonContainer = new QWidget(this);

    m_ButtonContainer->setObjectName("ButtonContainer");
    m_ButtonContainer->setStyleSheet(
      "QWidget#ButtonContainer {"
      "  background-color: rgb(180, 180, 180);"
      "}");

    QVBoxLayout * root = new QVBoxLayout(m_ButtonContainer);
    root->setContentsMargins(10, 8, 10, 8);
    root->setSpacing(6);

    QHBoxLayout * row1 = new QHBoxLayout();
    row1->setContentsMargins(0, 0, 0, 0);
    row1->setSpacing(10);

    QHBoxLayout * row2 = new QHBoxLayout();
    row2->setContentsMargins(0, 0, 0, 0);
    row2->setSpacing(10);

    m_ChkCpu = new QRadioButton("CPU", m_ButtonContainer);
    m_ChkGpu = new QRadioButton("GPU", m_ButtonContainer);
    m_ChkCpu->setChecked(true);
    m_ChkGpu->setChecked(false);

    m_BtnCutInside = new QPushButton("剪切内部", m_ButtonContainer);
    m_BtnCutOutside = new QPushButton("剪切外部", m_ButtonContainer);
    m_BtnCancel = new QPushButton("取消", m_ButtonContainer);

    m_BtnCutInside->setMinimumHeight(30);
    m_BtnCutOutside->setMinimumHeight(30);
    m_BtnCancel->setMinimumHeight(30);

    row1->addWidget(m_BtnCutInside);
    row1->addWidget(m_BtnCutOutside);
    row1->addWidget(m_BtnCancel);

    row2->addStretch(1);
    row2->addWidget(m_ChkCpu);
    row2->addWidget(m_ChkGpu);
    row2->addStretch(1);

    root->addLayout(row1);
    root->addLayout(row2);

    m_ButtonContainer->setLayout(root);
    m_ButtonContainer->hide();

    connect(m_BtnCutInside, &QPushButton::clicked, this, &QVtkWidget::OnBtnClicked);
    connect(m_BtnCutOutside, &QPushButton::clicked, this, &QVtkWidget::OnBtnClicked);
    connect(m_BtnCancel, &QPushButton::clicked, this, &QVtkWidget::OnBtnClicked);
}

void QVtkWidget::SetupDefaultRenderer()
{
    m_Renderer = vtkSmartPointer<vtkOpenGLRenderer>::New();
    m_RenderWindow->AddRenderer(m_Renderer);
}

void QVtkWidget::OnBtnClicked()
{
    QPushButton * senderBtn = qobject_cast<QPushButton *>(sender());

    bool useGpu = m_ChkGpu ? m_ChkGpu->isChecked() : false;

    if (senderBtn == m_BtnCutInside) {
        QImage mask = CreateMaskFromPoints(m_CuttingPoints, m_VtkWidget->size());
        emit SgnCutActionTriggered(true, useGpu, mask);
    } else if (senderBtn == m_BtnCutOutside) {
        QImage mask = CreateMaskFromPoints(m_CuttingPoints, m_VtkWidget->size());
        emit SgnCutActionTriggered(false, useGpu, mask);
    } else if (senderBtn == m_BtnCancel) {
    }

    ClearCuttingPoints();
    SetCutButtonsVisible(false);
}

void QVtkWidget::Render()
{
    if (m_RenderWindow) {
        m_RenderWindow->Render();
        m_Renderer->Render();
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

vtkRenderer * QVtkWidget::GetRenderer() const
{
    return m_Renderer;
}

vtkGenericOpenGLRenderWindow * QVtkWidget::GetRenderWindow() const
{
    return m_RenderWindow;
}

void QVtkWidget::SetBoxWidget2(vtkSmartPointer<vtkAbstractWidget> boxWidget)
{
    m_BoxWidget2 = boxWidget;
}

void QVtkWidget::SetInteractionMode(InteractionMode mode)
{
    m_InteractionMode = mode;

    m_DrawOverlay->hide();
    ClearCuttingPoints();
    SetCutButtonsVisible(false);
    m_BoxWidget2->Off();

    if (mode == DrawCutLine) {
        m_DrawOverlay->show();
        m_DrawOverlay->raise();
    } else if (mode == BoxWidget2) {
        m_BoxWidget2->On();
    }

    update();
    Render();
}

void QVtkWidget::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
    InitializeVTK();
}

void QVtkWidget::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_L) {
        if (m_InteractionMode == DrawCutLine) {
            SetInteractionMode(Normal);
        } else {
            SetInteractionMode(DrawCutLine);
        }
        event->accept();
    } else if (event->key() == Qt::Key_B) {
        if (m_InteractionMode == BoxWidget2) {
            SetInteractionMode(Normal);
        } else {
            SetInteractionMode(BoxWidget2);
        }
        event->accept();
    }
    event->accept();
}

void QVtkWidget::resizeEvent(QResizeEvent * event)
{
    UpdateWidgetGeometry(m_VtkWidget, m_BorderThickness, width(), height());
    UpdateWidgetGeometry(m_DrawOverlay, m_BorderThickness, width(), height());
    if (m_ButtonContainer && m_ButtonContainer->isVisible()) {
        SetCutButtonsVisible(true);
    }
    QWidget::resizeEvent(event);
}

void QVtkWidget::paintEvent(QPaintEvent * event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(m_BorderColor, m_BorderThickness, Qt::SolidLine));
    QRect r = rect();
    int half = m_BorderThickness / 2;
    r.adjust(half, half, -half, -half);
    painter.drawRect(r);
}

void QVtkWidget::SetCutButtonsVisible(bool visible)
{
    if (visible) {
        if (!m_ButtonContainer) {
            return;
        }

        int btnWidth = 350;
        int btnHeight = 70;
        int x = (width() - btnWidth) / 2;
        int y = height() - btnHeight - 20;

        m_ButtonContainer->setGeometry(x, y, btnWidth, btnHeight);
        m_ButtonContainer->show();
        m_ButtonContainer->raise();
    } else {
        if (m_ButtonContainer) {
            m_ButtonContainer->hide();
        }
    }
}

void QVtkWidget::ClearCuttingPoints()
{
    m_CuttingPoints.clear();
    m_IsDrawing = false;
    if (m_DrawOverlay) {
        m_DrawOverlay->SetPoints(m_CuttingPoints);
    }
}

QImage QVtkWidget::CreateMaskFromPoints(const QList<QPointF> & points, const QSize & size)
{
    QImage mask(size, QImage::Format_Grayscale8);
    mask.fill(Qt::black);

    if (points.size() < 3) {
        return mask;
    }

    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);

    QPolygonF polygon;
    for (const auto & pt : points) {
        polygon << pt;
    }
    painter.drawPolygon(polygon);

    return mask;
}
