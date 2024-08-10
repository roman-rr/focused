#include "rounded_widget.h"

#include <QApplication>
#include <QWidget>
#include <QRegion>
#include <QPainter>
#include <QResizeEvent>

RoundedWidget::RoundedWidget(QWidget *parent)
    : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void RoundedWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);

    // Create a rounded region
    QRegion region(0, 0, width(), height(), QRegion::Rectangle);
    QRegion roundedCorners(0, 0, width(), height(), QRegion::Ellipse);
    region = region.subtracted(roundedCorners);
    setMask(region);
}

void RoundedWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(255, 255, 255)); // Set background color
    painter.setPen(Qt::NoPen);

    // Draw the main rectangle with rounded corners
    QRect rect = this->rect();
    painter.drawRoundedRect(rect, 20, 20); // Adjust the 20, 20 values for your desired corner radius
}
