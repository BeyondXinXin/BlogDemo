#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>

class EditorManager;
class PaletteManager;
class TmpWidget;
class ToolManager;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void Launch();

private:
    void SetupUi();

    void SetupWindowTitleSetupUI(QWidget* wid);
    void SetupPaletteUI(QWidget* wid);
    void SetupToolUI(QWidget* wid);
    void SetupToolSetupUI(QWidget* wid);

    void GenerateDefaultView();
    void InitConnect();
    void ChangeTool();

private:
    EditorManager* editor_mag_;
    PaletteManager* palette_mag_;

    QWidget* edit_views_;
    QGridLayout* lay_edit_views_;
    QVBoxLayout* lay_palette_;
    QVBoxLayout* lay_tool_;
    QHBoxLayout* lay_tool_setup_;

    QPushButton *btn_save, *btn_undo, *btn_redo;
};

// 调整布局用
class TmpWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TmpWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
};

#endif // MAIN_WINDOW_H
