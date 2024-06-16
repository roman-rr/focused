#include "editor_dialog.h"
#include "ui_editor_dialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

EditorDialog::EditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorDialog)
{
    ui->setupUi(this);

    // Set the QTextEdit to read-only
    // ui->textEdit->setReadOnly(true);


    // Connect the save button
    connect(ui->saveButton, &QPushButton::clicked, this, &EditorDialog::handleSaveButton);

    // Load the file content by default
    loadFile();
}

EditorDialog::~EditorDialog()
{
    delete ui;
}

void EditorDialog::handleSaveButton()
{
    saveFile();
}

void EditorDialog::loadFile()
{
    QFile file(distractorsFilePath);
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
