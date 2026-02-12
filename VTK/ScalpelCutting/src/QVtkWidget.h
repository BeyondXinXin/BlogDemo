#ifndef QVtkWidget_H
#define QVtkWidget_H

#include <QWidget>
#include <QImage>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>

class DrawOverlayWidget;
class QPushButton;
class QRadioButton;
class vtkAbstractWidget;

class QVtkWidget : public QWidget
{
    Q_OBJECT

public:
    enum InteractionMode
    {
        Normal,
        DrawCutLine,
        BoxWidget2
    };

    explicit QVtkWidget(QWidget * parent = nullptr);
    ~QVtkWidget() override;

    void Render();
    void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);
    vtkRenderer * GetRenderer() const;
    vtkGenericOpenGLRenderWindow * GetRenderWindow() const;

    void SetBoxWidget2(vtkSmartPointer<vtkAbstractWidget> boxWidget);

signals:
    void SgnCutActionTriggered(bool inside, bool useGpu, const QImage & mask);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent * event) override;
    void showEvent(QShowEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private:
    void InitializeVTK();
    void InitializeUI();
    void SetupDefaultRenderer();

    void OnBtnClicked();

    void SetCutButtonsVisible(bool visible);
    void SetInteractionMode(InteractionMode mode);

    void ClearCuttingPoints();

    QImage CreateMaskFromPoints(const QList<QPointF> & points, const QSize & size);

private:
    bool m_Initialized = false;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_RenderWindow;
    vtkSmartPointer<vtkRenderer> m_Renderer;
    vtkSmartPointer<vtkAbstractWidget> m_BoxWidget2;
    QVTKOpenGLNativeWidget * m_VtkWidget = nullptr;

    int m_BorderThickness = 4;
    QColor m_BorderColor = QColor(180, 80, 80);

    InteractionMode m_InteractionMode = Normal;

    QList<QPointF> m_CuttingPoints;
    bool m_IsDrawing = false;

    DrawOverlayWidget * m_DrawOverlay = nullptr;

    QPushButton * m_BtnCutInside = nullptr;
    QPushButton * m_BtnCutOutside = nullptr;
    QPushButton * m_BtnCancel = nullptr;
    QRadioButton * m_ChkCpu = nullptr;
    QRadioButton * m_ChkGpu = nullptr;
    QWidget * m_ButtonContainer = nullptr;
};

#endif // QVtkWidget_H