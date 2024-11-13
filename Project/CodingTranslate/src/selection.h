#ifndef SELECTION_H
#define SELECTION_H

#include <QObject>

class QTimer;
class QMimeData;

class Selection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Selection)

public:
    ~Selection() override;
    static Selection& Instance();

signals:
    void SgnRequestedSelectionAvailable(const QString& selection);

public slots:
    void RequestSelection();

protected:
    Selection();

private slots:
    void GetSelection();

private:
    QScopedPointer<QMimeData> original_clipboard_data_;
    QTimer* max_selection_delay_;
};

#endif // SELECTION_H
