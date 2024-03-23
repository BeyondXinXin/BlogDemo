#include "main_window.h"

#include "editor_manager.h"
#include "palette_manager.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "singleton.h"
#include "tool_manager.h"

#include <QGridLayout>
#include <QPainter>
#include <QPushButton>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
  : QWidget(parent)
  , editor_mag_(new EditorManager())
  , palette_mag_(new PaletteManager())
{
    SetupUi();
    InitConnect();
}

MainWindow::~MainWindow()
{
}

// 启动
void MainWindow::Launch()
{
    GenerateDefaultView();

    show();
    move(300, 100);

    resize(1920, 1080);
    editor_mag_->ViewCamReset();
}

// 全局UI设置
void MainWindow::SetupUi()
{
    // 初始化所有窗口
    auto wid_title = new TmpWidget(this);
    edit_views_ = new QWidget(this);
    lay_edit_views_ = new QGridLayout(edit_views_);
    lay_edit_views_->setContentsMargins(0, 0, 0, 0);
    auto wid_color = new TmpWidget(this);
    auto wid_time_axis = new TmpWidget(this);
    auto wid_tool_setup_bar = new TmpWidget(this);
    auto wid_tool_bar = new TmpWidget(this);
    auto wid_status_bar = new TmpWidget(this);

    // 尺寸限制
    wid_title->setFixedHeight(30);
    wid_status_bar->setFixedHeight(30);
    edit_views_->setMinimumSize(600, 300);
    wid_color->setMinimumSize(165, 120);
    wid_color->resize(165, 150);
    wid_time_axis->setMinimumSize(300, 150);
    wid_time_axis->resize(300, 150);
    wid_tool_setup_bar->setFixedHeight(30);
    wid_tool_bar->setFixedWidth(30);

    // 窗口布局管理
    // 中心窗口+标题栏+状态栏
    auto lay_window = new QVBoxLayout(this);
    lay_window->setSpacing(6);
    lay_window->setContentsMargins(0, 0, 0, 0);
    lay_window->addWidget(wid_title);

    // 中心窗口+颜色设置
    auto splitter_color = new QSplitter(this);
    splitter_color->setOrientation(Qt::Horizontal);
    auto splitter_color_view_wid = new QWidget(splitter_color);

    // 中心窗口+时间轴
    auto splitter_time_axis = new QSplitter(this);
    splitter_time_axis->setOrientation(Qt::Vertical);
    splitter_time_axis->addWidget(edit_views_);
    splitter_time_axis->setCollapsible(0, false);
    splitter_time_axis->addWidget(wid_time_axis);
    splitter_time_axis->setCollapsible(1, true);

    // 中心窗口+工具属性
    auto lay_tool_setup = new QVBoxLayout();
    lay_tool_setup->setContentsMargins(0, 0, 0, 0);
    lay_tool_setup->setStretch(0, 20);
    lay_tool_setup->addWidget(wid_tool_setup_bar);
    lay_tool_setup->addWidget(splitter_time_axis);

    // 中心窗口+工具
    auto lay_tool = new QHBoxLayout(splitter_color_view_wid);
    lay_tool->setContentsMargins(0, 0, 0, 0);
    lay_tool->addWidget(wid_tool_bar);
    lay_tool->addLayout(lay_tool_setup);

    // 中心窗口+颜色
    splitter_color->addWidget(splitter_color_view_wid);
    splitter_color->setCollapsible(0, false);
    splitter_color->addWidget(wid_color);
    splitter_color->setCollapsible(1, true);

    lay_window->addWidget(splitter_color);
    lay_window->addWidget(wid_status_bar);

    splitter_time_axis->setSizes({ width(), 0 });
    splitter_color->setSizes(QList<int> { 800, 150 });

    // 标题栏
    SetupWindowTitleSetupUI(wid_title);
    // 调色板
    SetupPaletteUI(wid_color);
    // 工具
    SetupToolUI(wid_tool_bar);
    // 工具属性
    SetupToolSetupUI(wid_tool_setup_bar);
}

void MainWindow::SetupWindowTitleSetupUI(QWidget* wid)
{
    btn_save = new QPushButton(wid);
    btn_save->setText(tr("Save"));
    btn_undo = new QPushButton(wid);
    btn_undo->setText(tr("Undo"));
    btn_redo = new QPushButton(wid);
    btn_redo->setText(tr("Redo"));

    QHBoxLayout* lay_tile = new QHBoxLayout;
    lay_tile->setContentsMargins(0, 0, 0, 0);
    wid->setLayout(lay_tile);

    lay_tile->addWidget(btn_redo);
    lay_tile->addWidget(btn_undo);
    lay_tile->addWidget(btn_save);
    lay_tile->addStretch(1);

    connect(btn_undo, &QPushButton::clicked, this, [&]() {
        editor_mag_->Undo();
    });

    connect(btn_redo, &QPushButton::clicked, this, [&]() {
        editor_mag_->Redo();
    });

    connect(btn_save, &QPushButton::clicked, this, [&]() {
        editor_mag_->SaveFile();
    });
}

// 调色板UI设置
void MainWindow::SetupPaletteUI(QWidget* wid)
{

    lay_palette_ = new QVBoxLayout;
    wid->setLayout(lay_palette_);
    lay_palette_->setContentsMargins(10, 10, 10, 10);
    lay_palette_->setSpacing(10);
    lay_palette_->addWidget(palette_mag_->GetMenuWid());
    lay_palette_->addWidget(palette_mag_->GetSelectWid());
    lay_palette_->addWidget(palette_mag_->GetActivationWid());
}

// 工具UI设置
void MainWindow::SetupToolUI(QWidget* wid)
{
    lay_tool_ = new QVBoxLayout;
    wid->setLayout(lay_tool_);
    lay_tool_->setContentsMargins(0, 0, 0, 0);
    lay_tool_->addWidget(Singleton<ToolManager>::Inst().GetToolWid());
}

// 工具设置UI设置
void MainWindow::SetupToolSetupUI(QWidget* wid)
{
    lay_tool_setup_ = new QHBoxLayout;
    wid->setLayout(lay_tool_setup_);
    lay_tool_setup_->setContentsMargins(10, 0, 10, 0);
    wid->setFocusPolicy(Qt::NoFocus);
}

// 测试：生成默认view
void MainWindow::GenerateDefaultView()
{
    lay_edit_views_->addWidget(editor_mag_->InitNewImage());
    palette_mag_->InitColor();
}

// 链接manger间通讯
void MainWindow::InitConnect()
{
    // 颜色切换
    connect(palette_mag_, &PaletteManager::SgnChangePSColor,
            editor_mag_, &EditorManager::ChangePSColor);

    // 绑定工具
    connect(&Singleton<ToolManager>::Inst(), &ToolManager::SgnChangeTool,
            this, &MainWindow::ChangeTool);

    connect(&Singleton<ToolManager>::Inst(), &ToolManager::SgnToolEvent,
            editor_mag_, &EditorManager::HandelToolEvent);
}

// 工具发生修改
void MainWindow::ChangeTool()
{
    // 更新属性窗口
    QLayoutItem* child;
    while ((child = lay_tool_setup_->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        }
    }

    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();
    foreach (auto wid, tool->GetToolAttributeWid()) {
        lay_tool_setup_->addWidget(wid);
    }
    lay_tool_setup_->addStretch(1);

    // 通知窗口修改光标
    editor_mag_->ChangeTool();
}

TmpWidget::TmpWidget(QWidget* parent)
  : QWidget(parent)
{
}

void TmpWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(125, 146, 158)));
    painter.drawRect(rect());
}
