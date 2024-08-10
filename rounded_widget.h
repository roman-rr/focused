#ifndef ROUNDED_WIDGET_H
#define ROUNDED_WIDGET_H

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QRegion>
#include <QResizeEvent>

class RoundedWidget : public QWidget {
    Q_OBJECT

public:
    explicit RoundedWidget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // ROUNDED_WIDGET_H
