// https://github.com/Greedysky/TTKWidgetTools


#ifndef Label_H
#define Label_H

#include <QLabel>
#include <QTimer>

class Label : public QLabel {
	Q_OBJECT

public:

	explicit Label(QWidget* parent = nullptr);
	explicit Label(const QString& text, QWidget* parent = nullptr);
	~Label();

	void SetFontMargin(int height, int width);
	int TimerInterval() const;
	void SetFontSize(int size);
	void Popup(QWidget* parent = nullptr);
	void SetText(const QString& text);

Q_SIGNALS:

	void SgnFinished();

private:

	void CloseAnimation();

private:

	virtual void paintEvent(QPaintEvent* event) override final;

private:

	QFont m_Font;
	QPoint m_Margin;
	QTimer m_Timer;
};

#endif // TTKTOASTLABEL_H
