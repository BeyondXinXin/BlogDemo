#include "WindowScreenRecorder.h"

#include <QtWidgets>
#include <QWindowCapture>
#include <QMediaCaptureSession>
#include <QMediaRecorder>

WindowScreenRecorder::WindowScreenRecorder(QGraphicsView * view, QObject * parent)
  : QObject(parent)
{
    m_WindowCapture = new QWindowCapture(this);
    QList<QCapturableWindow> capturableWindows = QWindowCapture::capturableWindows();

    WId viewWinId = view->window()->winId();
    for (const QCapturableWindow & window : capturableWindows) {
        if (view->windowTitle() == window.description()) {
            m_WindowCapture->setWindow(window);
            qInfo() << window.description();
        }
    }

    m_Recorder = new QMediaRecorder(this);
    m_CaptureSession = new QMediaCaptureSession();
    m_CaptureSession->setRecorder(m_Recorder);
    m_CaptureSession->setWindowCapture(m_WindowCapture);

    // 在桌面保存当前时间命名的视频
    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
      + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + ".mp4";
    m_Recorder->setOutputLocation(QUrl::fromLocalFile(filePath));
    m_Recorder->setQuality(QMediaRecorder::HighQuality);
}

void WindowScreenRecorder::StartRecording()
{
    m_WindowCapture->start();
    m_Recorder->record();
}

void WindowScreenRecorder::StopRecording()
{
    m_WindowCapture->stop();
    m_Recorder->stop();
}
