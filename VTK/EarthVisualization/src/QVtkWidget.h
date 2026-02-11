#ifndef QVtkWidget_H
#define QVtkWidget_H

#include <QWidget>
#include <QVector3D>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>

class QVtkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QVtkWidget(QWidget * parent = nullptr);
    ~QVtkWidget() override;

    vtkRenderer * Renderer() const;
    vtkGenericOpenGLRenderWindow * RenderWindow() const;

    void Render();
    void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);

    void SetBorderThickness(int thickness);
    void SetBorderColor(const QColor & color);
    void SetInteractionCenter(const QVector3D & center);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void paintEvent(QPaintEvent * event) override;
    void showEvent(QShowEvent * event) override;

private:
    void InitializeVTK();
    void SetupDefaultRenderer();

private:
    bool m_Initialized = false;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_RenderWindow;
    vtkSmartPointer<vtkRenderer> m_Renderer;
    QVTKOpenGLNativeWidget * m_VtkWidget = nullptr;

    int m_BorderThickness = 4;
    QColor m_BorderColor = QColor(180, 80, 80);
};

#endif // QVtkWidget_H