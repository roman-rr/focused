#ifndef QUADRATIC_DIALOG_H
#define QUADRATIC_DIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class QuadraticDialog;
}

class QuadraticDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuadraticDialog(QWidget *parent = nullptr);
    ~QuadraticDialog();
    int getUserInput() const;
    void setEquation(int a, int b, int c, int root1, int root2);

private slots:
    void setUpImage();
    void on_buttonBox_accepted();

private:
    Ui::QuadraticDialog *ui;
    int userInput;
    QGraphicsScene *scene;
};

#endif // QUADRATIC_DIALOG_H
