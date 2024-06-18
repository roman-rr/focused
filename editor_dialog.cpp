#include "editor_dialog.h"
#include "ui_editor_dialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QPushButton>

EditorDialog::EditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorDialog)
{
    ui->setupUi(this);

    // Set the QTextEdit to read-only
    // ui->textEdit->setReadOnly(true);

    // Determine a writable location for the distractors file
    distractorsFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/distractors.txt";
    qDebug() << "Distractors file path:" << distractorsFilePath;

    // Copy the file from resources to writable location if it doesn't exist
    copyResourceToWritableLocation();

    // Load the file content by default
    loadFile();

    // Connect the Restore Defaults button
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &EditorDialog::on_buttonBox_restoreDefaults);
}

EditorDialog::~EditorDialog()
{
    delete ui;
}

void EditorDialog::copyResourceToWritableLocation()
{
    QFile file(distractorsFilePath);
    if (!file.exists()) {
        QFile::copy(":/data/distractors.txt", distractorsFilePath);
        QFile::setPermissions(distractorsFilePath, QFileDevice::WriteOwner | QFileDevice::ReadOwner |
                                                       QFileDevice::WriteUser | QFileDevice::ReadUser |
                                                       QFileDevice::WriteGroup | QFileDevice::ReadGroup |
                                                       QFileDevice::WriteOther | QFileDevice::ReadOther);
        qDebug() << "Copied distractors.txt to writable location." << distractorsFilePath;
    }
}

void EditorDialog::resetToDefault() // Add this function
{
    QFile::remove(distractorsFilePath); // Remove the existing file
    copyResourceToWritableLocation(); // Copy the default file again
}

void EditorDialog::loadFile()
{
    QFile file(distractorsFilePath);
    if (!file.exists()) {
        qDebug() << "File does not exist:" << distractorsFilePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    ui->textEdit->setPlainText(in.readAll());
    file.close();
}

void EditorDialog::saveFile()
{
    QFile file(distractorsFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.close();
}

// save button handler
void EditorDialog::on_buttonBox_accepted()
{
    qDebug() << "Save button clicked";
    saveFile();
    accept();
}

void EditorDialog::on_buttonBox_restoreDefaults()
{
    qDebug() << "reset to defaults clicked";
    resetToDefault();
    loadFile(); // Reload the file to update the text edit with default content
}
