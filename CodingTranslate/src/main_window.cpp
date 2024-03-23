#include "main_window.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

#include "naming_conversion.h"
#include "selection.h"
#include "setting_dialog.h"
#include "source_text_edit.h"
#include "title_tool_bar.h"

MainWindow::MainWindow(QWidget* p_parent)
  : FramelessMainWindowWin(true, p_parent)
{
    translator_ = new QOnlineTranslator(this);
    key_translate_selection_ =
      new QHotkey(Qt::Key_Q, Qt::AltModifier, true, this);
    setting_dia_ = new SettingDialog();
    media_player_ = new QMediaPlayer;

    SetupUI();

    connect(key_translate_selection_, &QHotkey::activated,
            this, &MainWindow::TranslateSelection);

    connect(source_edit_, &SourceTextEdit::SgnTextEdited,
            this, &MainWindow::SourceTextEdited);
    connect(source_edit_, &SourceTextEdit::SgnSourceEmpty,
            this, &MainWindow::SourceTextEmpty);

    connect(setting_dia_, &SettingDialog::accepted,
            this, &MainWindow::LoadSetting);

    connect(&Selection::Instance(), &Selection::SgnRequestedSelectionAvailable,
            this, &MainWindow::ReplaceSelectionText);

    QObject::connect(translator_, &QOnlineTranslator::SgnFinished, [&]() {
        if (translator_->Error() == QOnlineTranslator::NoError) {
            translation_edit_->setText(translator_->GetTranslation());
        } else {
            translation_edit_->setText(translator_->ErrorString());
        }
    });

    LoadSetting();
    source_edit_->SetListenForEdits(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* p_event)
{
    FramelessMainWindowWin::closeEvent(p_event);
    qApp->exit(0);
}

void MainWindow::changeEvent(QEvent* p_event)
{
    FramelessMainWindowWin::changeEvent(p_event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        return;
    }
    return FramelessMainWindowWin::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            hide();
        }
    }

    return FramelessMainWindowWin::eventFilter(watched, event);
}

void MainWindow::SetupUI()
{
    SetupToolBar();
    SetupMainWidget();
    installEventFilter(this);
    resize(600, 200);
}

void MainWindow::SetupToolBar()
{
    auto toolBar = new TitleToolBar(tr("Global"), this);
    toolBar->setIconSize(QSize(28, 28));
    toolBar->setFixedHeight(50);

    auto action = toolBar->AddActionOnLeft(QIcon(":/img/audio.svg"), "Audio");
    connect(action, &QAction::triggered, this, [&]() {
        media_player_->stop();
        if (media_player_->playlist() == nullptr) {
            media_player_->setPlaylist(new QMediaPlaylist);
        }
        media_player_->playlist()->clear();

        const QList<QMediaContent> media =
          translator_->GenerateTextToSpeech(translator_->GetEnglishText());
        media_player_->playlist()->addMedia(media);
        media_player_->play();
    });

    action = toolBar->AddActionOnLeft(QIcon(":/img/settings.svg"), "Setting");
    connect(action, &QAction::triggered, this, [&]() {
        setting_dia_->open();
    });

    toolBar->AddTitleBarIcons(QIcon(), QIcon(), QIcon(), QIcon(":/img/close.svg"));
    setTitleBar(toolBar);
    addToolBar(toolBar);

    setContextMenuPolicy(Qt::NoContextMenu);
}

void MainWindow::SetupMainWidget()
{
    center_wid_ = new QWidget(this);
    setCentralWidget(center_wid_);

    QVBoxLayout* layout = new QVBoxLayout(center_wid_);
    source_edit_ = new SourceTextEdit(this);
    translation_edit_ = new QTextEdit(this);
    translation_edit_->setReadOnly(true);

    layout->addWidget(source_edit_);
    layout->addWidget(translation_edit_);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);

    center_wid_->setLayout(layout);
}

void MainWindow::TranslateSelection()
{
    Selection::Instance().RequestSelection();
}

void MainWindow::ReplaceSelectionText(const QString& text)
{
    source_edit_->ReplaceText(text);
    SourceTextEdited();
    move(QPoint(600, 300));
    activateWindow();
    setContextMenuPolicy(Qt::NoContextMenu);
    show();
    source_edit_->setFocus(Qt::MouseFocusReason);
}

void MainWindow::SourceTextEdited()
{
    translation_edit_->clear();
    QString str = source_edit_->toPlainText();
    translator_->translate(str, cur_engine_);
}

void MainWindow::SourceTextEmpty(bool empty)
{
    if (empty) {
        translation_edit_->setText(QString());
    }
}

void MainWindow::LoadSetting()
{
    QSettings* setting_ = new QSettings(this);
    translator_->SetBaiduKey(setting_->value("Baidu/appid", QString()).toString(),
                             setting_->value("Baidu/key", QString()).toString());
    switch (setting_->value("Global/api_id", "").toInt()) {
    case 0: {
        cur_engine_ = QOnlineTranslator::Google;
        break;
    }
    case 1: {
        cur_engine_ = QOnlineTranslator::Bing;
        break;
    }
    case 2: {
        cur_engine_ = QOnlineTranslator::Baidu;
        break;
    }
    }
}
