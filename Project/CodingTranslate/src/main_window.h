#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelessmainwindow/frame_less_main_window_win.h"

#include <QBitArray>
#include <QHotkey>
#include <QOnlineTranslator>
#include <QSet>
#include <QSharedPointer>

class QPlainTextEdit;
class QTextEdit;
class SourceTextEdit;
class SettingDialog;
class QMediaPlayer;

class MainWindow : public FramelessMainWindowWin
{
    Q_OBJECT
public:
    friend class DockWidgetHelper;

    explicit MainWindow(QWidget* p_parent = nullptr);

    ~MainWindow();
    MainWindow(const MainWindow&) = delete;
    void operator=(const MainWindow&) = delete;

protected:
    void closeEvent(QCloseEvent* p_event) override;
    void changeEvent(QEvent* p_event) override;
    void keyPressEvent(QKeyEvent* event) override;

    bool eventFilter(QObject*, QEvent*) override;

private:
    void SetupUI();
    void SetupToolBar();
    void SetupMainWidget();

    void TranslateSelection();
    void ReplaceSelectionText(const QString& text);
    void SourceTextEdited();
    void SourceTextEmpty(bool empty);

    void LoadSetting();

private:
    QWidget* center_wid_;
    QTextEdit* translation_edit_;
    SourceTextEdit* source_edit_;

    QOnlineTranslator* translator_;
    QOnlineTranslator::Engine cur_engine_;

    QHotkey* key_translate_selection_;

    SettingDialog* setting_dia_;

    QMediaPlayer* media_player_;
};

#endif // MAINWINDOW_H
