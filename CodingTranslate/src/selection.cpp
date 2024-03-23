#include "selection.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QTimer>
#include <windows.h>

using namespace std::chrono_literals;

Selection::~Selection() = default;

Selection& Selection::Instance()
{
    static Selection singletone;
    return singletone;
}

void Selection::RequestSelection()
{
    // 保存原始剪贴板
    original_clipboard_data_.reset(new QMimeData);
    const QMimeData* clipboardData = QGuiApplication::clipboard()->mimeData();
    for (const QString& format : clipboardData->formats())
        original_clipboard_data_->setData(format, clipboardData->data(format));

    // 等待直到所有修改器都将取消按下（以避免与其他快捷方式冲突）
    while (GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN)
           || GetAsyncKeyState(VK_SHIFT) || GetAsyncKeyState(VK_MENU)
           || GetAsyncKeyState(VK_CONTROL)) {
    };

    // Generate Ctrl + C input
    INPUT copyText[4];

    // Set the press of the "Ctrl" key
    copyText[0].ki.wVk = VK_CONTROL;
    copyText[0].ki.dwFlags = 0; // 0 for key press
    copyText[0].type = INPUT_KEYBOARD;

    // Set the press of the "C" key
    copyText[1].ki.wVk = 'C';
    copyText[1].ki.dwFlags = 0;
    copyText[1].type = INPUT_KEYBOARD;

    // Set the release of the "C" key
    copyText[2].ki.wVk = 'C';
    copyText[2].ki.dwFlags = KEYEVENTF_KEYUP;
    copyText[2].type = INPUT_KEYBOARD;

    // Set the release of the "Ctrl" key
    copyText[3].ki.wVk = VK_CONTROL;
    copyText[3].ki.dwFlags = KEYEVENTF_KEYUP;
    copyText[3].type = INPUT_KEYBOARD;

    // Send key sequence to system
    SendInput(static_cast<UINT>(std::size(copyText)), copyText, sizeof(INPUT));

    // Wait for the clipboard changes
    connect(
      QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &Selection::GetSelection);
    max_selection_delay_->start();
}

Selection::Selection()
  : max_selection_delay_(new QTimer(this))
{
    max_selection_delay_->setSingleShot(true);
    max_selection_delay_->setInterval(200ms);
    max_selection_delay_->callOnTimeout(this, &Selection::GetSelection);
}

void Selection::GetSelection()
{
    const QString selection = QGuiApplication::clipboard()->text();
    if (selection.isEmpty() && max_selection_delay_->isActive())
        return;

    max_selection_delay_->stop();
    disconnect(
      QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &Selection::GetSelection);

    // 退出事件循环后恢复剪贴板数据
    // clang-format off
    QMetaObject::invokeMethod(this, [this] {
        QGuiApplication::clipboard()->setMimeData(original_clipboard_data_.take());
    },  Qt::QueuedConnection);
    // clang-format on

    if (selection == original_clipboard_data_->text()) {
        emit SgnRequestedSelectionAvailable("");
    } else {
        emit SgnRequestedSelectionAvailable(selection);
    }
}
