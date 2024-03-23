#include "source_text_edit.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTimer>

#include "naming_conversion.h"

using namespace std::chrono_literals;

SourceTextEdit::SourceTextEdit(QWidget* parent)
  : QPlainTextEdit(parent)
  , timer_(new QTimer(this))
{
    timer_->setSingleShot(true);
    timer_->callOnTimeout(this, &SourceTextEdit::SgnTextEdited);
    connect(this, &SourceTextEdit::textChanged,
            this, &SourceTextEdit::CheckSourceEmptyChanged);

    btn_naming_conversion_ = new QPushButton(this);
    btn_naming_conversion_->setFixedSize(30, 30);
    btn_naming_conversion_->setIcon(QIcon(":/img/xml.svg"));
    connect(btn_naming_conversion_, &QPushButton::clicked,
            this, [&]() {
                QString str = toPlainText();
                str = NamingConversion::ConversionSource(str);
                ReplaceText(str);
                emit SgnTextEdited();
            });

    btn_copy_ = new QPushButton(this);
    btn_copy_->setFixedSize(30, 30);
    btn_copy_->setIcon(QIcon(":/img/copy.svg"));
    connect(btn_copy_, &QPushButton::clicked,
            this, [&]() {
                QApplication::clipboard()->setText(toPlainText());
            });
}

void SourceTextEdit::SetListenForEdits(bool listen)
{
    if (listen) {
        connect(this, &SourceTextEdit::textChanged,
                this, &SourceTextEdit::StartTimerDelay);
    } else {
        timer_->stop();
        disconnect(this, &SourceTextEdit::textChanged,
                   this, &SourceTextEdit::StartTimerDelay);
    }
}

QString SourceTextEdit::ToSourceText() const
{
    return toPlainText().trimmed();
}

void SourceTextEdit::ReplaceText(const QString& text)
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    if (text.isEmpty()) {
        cursor.removeSelectedText();
    } else {
        cursor.insertText(text);
    }
    setTextCursor(cursor);
    timer_->stop();
}

void SourceTextEdit::RemoveText()
{
    ReplaceText({});
}

void SourceTextEdit::resizeEvent(QResizeEvent* event)
{
    btn_copy_->move(this->width() - 40, 10);
    btn_naming_conversion_->move(this->width() - 40 - 40, 10);
}

void SourceTextEdit::StopEditTimer()
{
    timer_->stop();
}

void SourceTextEdit::StartTimerDelay()
{
    timer_->start(500);
}

void SourceTextEdit::CheckSourceEmptyChanged()
{
    if (toPlainText().isEmpty() != empty_) {
        empty_ = toPlainText().isEmpty();
        emit SgnSourceEmpty(empty_);
    }
}
