#include "panelwidget.h"

#include <QBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QScrollArea>

PanelWidget::PanelWidget(QWidget *parent)
  : QWidget(parent)
{
    scroll_area_ = new QScrollArea(this);
    scroll_area_->setObjectName("scrollAreaMain");
    scroll_area_->setWidgetResizable(true);

    scroll_area_widget_contents_ = new QWidget();
    scroll_area_widget_contents_->setGeometry(QRect(0, 0, 100, 100));

    vertical_layout_ = new QVBoxLayout(scroll_area_widget_contents_);
    vertical_layout_->setSpacing(0);
    vertical_layout_->setContentsMargins(0, 0, 0, 0);

    frame_ = new QFrame(scroll_area_widget_contents_);
    frame_->setObjectName("frameMain");

    grid_layout_ = new QGridLayout(frame_);
    grid_layout_->setSpacing(0);

    vertical_layout_->addWidget(frame_);
    scroll_area_->setWidget(scroll_area_widget_contents_);
    scroll_area_->setStyleSheet("QScrollArea#scrollAreaMain,QFrame#frameMain{border:none;}");

    margin_ = 0;
    space_ = 0;
    auto_width_ = false;
    auto_height_ = false;
}

void PanelWidget::resizeEvent(QResizeEvent *)
{
    scroll_area_->move(1, 1);
    scroll_area_->resize(this->size() - QSize(2, 2));
}

void PanelWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QColor(39, 141, 255));
    painter.drawRect(1, 1, width() - 2, height() - 2);
}

QSize PanelWidget::sizeHint() const
{
    return QSize(300, 200);
}

QSize PanelWidget::minimumSizeHint() const
{
    return QSize(20, 20);
}

int PanelWidget::GetMargin() const
{
    return this->margin_;
}

int PanelWidget::GetSpace() const
{
    return this->space_;
}

bool PanelWidget::GetAutoWidth() const
{
    return this->auto_width_;
}

bool PanelWidget::GetAutoHeight() const
{
    return this->auto_height_;
}

QList<QWidget *> PanelWidget::GetWidgets()
{
    return this->widgets_;
}

int PanelWidget::GetColumnCount()
{
    return this->column_count_;
}

void PanelWidget::SetWidget(QList<QWidget *> widgets, int columnCount)
{
    this->widgets_ = widgets;
    this->column_count_ = columnCount;

    //先清空原有所有元素
    qDeleteAll(frame_->findChildren<QWidget *>());

    int row = 0;
    int column = 0;
    int index = 0;

    foreach (QWidget *widget, widgets) {
        grid_layout_->addWidget(widget, row, column);
        column++;
        index++;

        if (index % columnCount == 0) {
            row++;
            column = 0;
        }
    }

    row++;

    //设置右边弹簧
    if (!auto_width_) {
        QSpacerItem *hSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
        grid_layout_->addItem(hSpacer, 0, grid_layout_->columnCount());
    }

    //设置底边弹簧
    if (!auto_height_) {
        QSpacerItem *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        grid_layout_->addItem(vSpacer, row, 0);
    }
}

void PanelWidget::SetMargin(int left, int top, int right, int bottom)
{
    grid_layout_->setContentsMargins(left, top, right, bottom);
}

void PanelWidget::SetMargin(int margin)
{
    if (this->margin_ != margin) {
        SetMargin(margin, margin, margin, margin);
    }
}

void PanelWidget::SetSpace(int space)
{
    if (this->space_ != space) {
        grid_layout_->setSpacing(space);
    }
}

void PanelWidget::SetAutoWidth(bool autoWidth)
{
    if (this->auto_width_ != autoWidth) {
        this->auto_width_ = autoWidth;
    }
}

void PanelWidget::SetAutoHeight(bool autoHeight)
{
    if (this->auto_height_ != autoHeight) {
        this->auto_height_ = autoHeight;
    }
}
