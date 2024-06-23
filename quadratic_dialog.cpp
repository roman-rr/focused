#include "quadratic_dialog.h"
#include "ui_quadratic_dialog.h"
#include <QGraphicsPixmapItem>

QuadraticDialog::QuadraticDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuadraticDialog),
    userInput(0)
{
    ui->setupUi(this);

    // Disable window resizing by modifying window flags
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(size());

    setUpImage();

    // Set the legend text
    ui->labelLegend->setText("Solve quadratic equation to disable blocking. Write one of the correct roots of the equation.");
}

QuadraticDialog::~QuadraticDialog()
{
    delete ui;
}

void QuadraticDialog::setUpImage()
{
    // Initialize the QGraphicsScene
    scene = new QGraphicsScene(this);

    // Check if the scene is initialized correctly
    if (!scene) {
        qDebug() << "Failed to create QGraphicsScene";
        return;
    }

    // Load the image
    QPixmap pixmap(":/public/plant.png"); // Ensure the path matches your .qrc file
    if (pixmap.isNull()) {
        qDebug() << "Failed to load image";
        return;
    }

    // Create a QGraphicsPixmapItem with the QPixmap
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    if (!item) {
        qDebug() << "Failed to create QGraphicsPixmapItem";
        return;
    }

    // Add the item to the scene
    qDebug() << "Adding item to the scene";
    scene->addItem(item);

    // Set the scene to the QGraphicsView

    ui->graphicsView->setScene(scene);
    // Set the view to have a transparent background and no frame
    ui->graphicsView->setStyleSheet("background: transparent; border: none;");
    ui->graphicsView->setFrameShape(QFrame::NoFrame);
    ui->graphicsView->setAttribute(Qt::WA_OpaquePaintEvent, false);
    ui->graphicsView->setAttribute(Qt::WA_NoSystemBackground, true);
    ui->graphicsView->setBackgroundBrush(Qt::NoBrush);

    // Optionally, adjust the view's properties
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}


void QuadraticDialog::on_buttonBox_accepted()
{
    userInput = ui->inputLineEdit->text().toInt();
}

int QuadraticDialog::getUserInput() const
{
    return userInput;
}

void QuadraticDialog::setEquation(int a, int b, int c, int root1, int root2)
{
    QString equation = QString("%1x^2 + %2x + %3 = 0").arg(a).arg(b).arg(c);
    ui->equationLabel->setText(equation);
}
