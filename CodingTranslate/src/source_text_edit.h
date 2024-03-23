#ifndef SOURCETEXTEDIT_H
#define SOURCETEXTEDIT_H

#include <QPlainTextEdit>

class QPushButton;
class QTimer;

class SourceTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(SourceTextEdit)

public:
    explicit SourceTextEdit(QWidget* parent = nullptr);

    void SetListenForEdits(bool listen);

    QString ToSourceText() const;
    void ReplaceText(const QString& text);
    void RemoveText();

protected:
    void resizeEvent(QResizeEvent* event) override;

public slots:
    void StopEditTimer();

signals:
    void SgnTextEdited();
    void SgnSourceEmpty(bool empty);

private slots:
    void StartTimerDelay();
    void CheckSourceEmptyChanged();

private:
    QTimer* timer_;
    QPushButton* btn_naming_conversion_;
    QPushButton* btn_copy_;
    bool empty_ = true;
};

#endif // SOURCETEXTEDIT_H
