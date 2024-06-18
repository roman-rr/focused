#ifndef EDITORDIALOG_H
#define EDITORDIALOG_H

#include <QDialog>
#include <QAbstractButton>

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
    void on_buttonBox_accepted();
    void on_buttonBox_restoreDefaults();

private:
    Ui::EditorDialog *ui;
    QString distractorsFilePath;

    void loadFile();
    void saveFile();
    void copyResourceToWritableLocation();
    void resetToDefault(); // Add this line
};

#endif // EDITORDIALOG_H
