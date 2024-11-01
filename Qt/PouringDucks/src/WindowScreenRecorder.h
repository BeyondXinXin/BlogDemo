#ifndef WindowScreenRecorder_H_
#define WindowScreenRecorder_H_

#include <QObject>

class QWindowCapture;
class QMediaCaptureSession;
class QMediaRecorder;
class QGraphicsView;

/*
    简陋的窗口录制器,有很多限制
*/
class WindowScreenRecorder : public QObject
{
    Q_OBJECT

public:
    WindowScreenRecorder(QGraphicsView * view, QObject * parent = nullptr);

    void StartRecording();
    void StopRecording();

private:
    QMediaRecorder * m_Recorder;
    QMediaCaptureSession * m_CaptureSession;
    QWindowCapture * m_WindowCapture;
};

#endif //
