#ifndef FRAMELESSMAINWINDOWWIN_H
#define FRAMELESSMAINWINDOWWIN_H

#include "frame_less_main_window.h"

class FramelessMainWindowWin : public FramelessMainWindow
{
    Q_OBJECT
public:
    FramelessMainWindowWin(bool p_frameless, QWidget* p_parent = nullptr);

protected:
    bool nativeEvent(const QByteArray& p_eventType, void* p_message, long* p_result) override;
    void moveEvent(QMoveEvent* p_event) override;
    void setWindowFlagsOnUpdate() override;

private:
    void forceRedraw();
    void updateMargins();

private:
    int m_resizeAreaWidth = 6;
    QTimer* m_redrawTimer = nullptr;
    QSize m_sizeBeforeMove;
    QMargins m_maximizedMargins;
    int m_titleBarHeight = 0;
};

#endif // FRAMELESSMAINWINDOWWIN_H
