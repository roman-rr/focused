#ifndef EDITORDIALOG_H
#define EDITORDIALOG_H

#include <QDialog>

namespace Ui {
class EditorDialog;
}

class EditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorDialog(QWidget *parent = nullptr);
    ~EditorDialog();

private slots:
    void handleSaveButton();

private:
    Ui::EditorDialog *ui;
    QString distractorsFilePath;

    void loadFile();
    void saveFile();
};

#endif // EDITORDIALOG_H
