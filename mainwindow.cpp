#include "mainwindow.h"
#include "QtGui/qpainter.h"
#include "macos_privileges.h"
#include "ui_mainwindow.h"
#include "editor_dialog.h"
#include "privileges.h"
#include "get_tray_icon_position.h"
#include <fstream>
#include <vector>
#include <regex>
#include <cstdlib>
#include <unistd.h>
#include <QProcess>
#include "quadratic_dialog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPainterPath>
#include <QEvent>
#include <QTimer> // Add this include
#include <QVBoxLayout>

const QString distractorsFilePath = ":/data/distractors.txt"; // Use a single variable for the resource path

// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set window flags to disable moving and resizing
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(size());

    // Create tray icon and menu
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/public/icon.png"));
    trayIcon->show();

    // Connections
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
    connect(ui->enableButton, &QPushButton::clicked, this, &MainWindow::handleEnableButton);
    connect(ui->disableButton, &QPushButton::clicked, this, &MainWindow::handleDisableButton);
    connect(ui->editorButton, &QPushButton::clicked, this, &MainWindow::handleOpenEditorDialog);

    renderLayoutElements();

    setWindowOpacity(0);
    setAttribute(Qt::WA_TranslucentBackground);

    QTimer::singleShot(500, this, &MainWindow::restoreWindow);

}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete trayIconMenu;
    delete restoreAction;
    delete quitAction;
}


void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Define the pen for the border
    QPen pen(QColor(50, 50, 50));
    pen.setWidth(1);
    painter.setPen(pen);

    // Set brush for filling the shape
    QBrush brush(QColor(50, 50, 50));
    painter.setBrush(brush);

    // Adjust y-coordinates to draw the shapes higher within the widget
    int yOffset = -6; // Adjust this value to move the drawing upward

    // Draw rounded rectangle with a radius of 20
    QRect rect(0, 20 + yOffset, width(), height() - 20); // Adjust the height for the notch
    painter.drawRoundedRect(rect, 30, 30);

    // Draw the smaller triangle notch at the top
    QPolygon triangle;
    int triangleWidth = 25;  // Adjust this value to change the width of the triangle
    int triangleHeight = 12; // Adjust this value to change the height of the triangle

    triangle << QPoint(width() / 2 - triangleWidth / 2, 20 + yOffset)
             << QPoint(width() / 2 + triangleWidth / 2, 20 + yOffset)
             << QPoint(width() / 2, 20 - triangleHeight + yOffset);
    painter.drawPolygon(triangle);
}

void MainWindow::renderLayoutElements() {
    QPushButton *menuButton = new QPushButton("Menu", this);
    QPushButton *exitButton = new QPushButton("Exit", this);

    menuButton->setFixedWidth(60);
    exitButton->setFixedWidth(60);

    // Create a horizontal layout for the buttons
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(menuButton);
    hLayout->addStretch();
    hLayout->addWidget(exitButton);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addSpacerItem(new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    ui->centralwidget->setLayout(vLayout);

    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitButtonClicked);
}

// Handle close event to minimize to tray
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

// Handle change event to catch window minimize action
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            QTimer::singleShot(0, this, &QWidget::hide);
        }
    }
    QMainWindow::changeEvent(event);
}

// Override the event method to handle window deactivation
bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        qDebug() << "Window deactivated";
        hide();
    }
    return QMainWindow::event(event);
}


// Restore window from minimized state
void MainWindow::restoreWindow()
{
    QPoint trayIconPos = getTrayIconPosition();

    // Calculate the position for the window (above the tray icon)
    int x = trayIconPos.x() - (width() / 2) + 5; // Center below the tray icon
    int y = 0; // Set the top margin to zero

    // Ensure x is within screen bounds
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    if (x < screenGeometry.left()) {
        x = screenGeometry.left();
    } else if (x + width() > screenGeometry.right()) {
        x = screenGeometry.right() - width();
    }

    // Log the calculated window position
    qDebug() << "Window Restored with Position: " << x << y;

    // Set the position of the window
    move(x, 0);

    // Show window after few renders
    if (trayIconPos.x() >= 0 && trayIconPos.y() >= 0) {
        setWindowOpacity(1);
        show();
        raise();
        activateWindow();
        qDebug() << "ActivateWindow() called: ";
    }
}

// Handle tray icon activation (e.g., single-click or double-click)
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        qDebug() << "Click from system tray";
        if (isVisible()) {
            hide();
        } else {
            restoreWindow();
        }
    }
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    // Prevent the window from moving by resetting its position
    //restoreWindow();
    event->ignore();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore(); // Ignore mouse press events to prevent window movement
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore(); // Ignore mouse release events to prevent window movement
}

// Function to read lines from a file
std::vector<std::string> MainWindow::read_lines(const QString &filename) const {
    QFile file(filename);
    std::vector<std::string> lines;
    QString line;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return lines;
    }

    QTextStream in(&file);
    while (in.readLineInto(&line)) {

        // Skip comments and empty lines
        if (line.isEmpty() || line[0] == '#') {
            continue;
        }
        lines.push_back(line.toStdString());
    }

    qDebug() << "Total valid lines read:" << lines.size();
    return lines;
}

// Function to check if a string is an IP address
bool MainWindow::is_ip_address(const std::string &str) const {
    std::regex ip_regex(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    return std::regex_match(str, ip_regex);
}

// Function to check if a line exists in the hosts file
bool line_exists_in_hosts(const std::string &line) {
    std::ifstream hosts_file("/etc/hosts");
    std::string current_line;
    while (std::getline(hosts_file, current_line)) {
        if (current_line.find(line) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Function to block sites
void MainWindow::block_sites(const std::vector<std::string> &sites) const {
    std::string command;
    for (const auto &site : sites) {
        std::string site_with_www = (site.find("www.") == 0) ? site : "www." + site;
        std::string line_to_add = "127.0.0.1 " + site;
        std::string line_with_www_to_add = "127.0.0.1 " + site_with_www;

        if (!is_ip_address(site)) {
            if (!line_exists_in_hosts(line_to_add)) {
                command += "echo \"" + line_to_add + "\" >> /etc/hosts; ";
            }
            if (!line_exists_in_hosts(line_with_www_to_add)) {
                command += "echo \"" + line_with_www_to_add + "\" >> /etc/hosts; ";
            }
        } else {
            if (!line_exists_in_hosts(line_to_add)) {
                command += "echo \"" + line_to_add + "\" >> /etc/hosts; ";
            }
        }
    }

    // Check if command is not empty
    if (command.empty()) {
        qDebug() << "No new entries to add.";
        return;
    }

    // qDebug() << "Block command: " << QString::fromStdString(command);

    // Execute the command with privileges
    if (!execute_with_privileges(command)) {
        qDebug() << "Failed to obtain privileges.";
        return;
    }

    // ui->statusbar->showMessage("Sites blocked successfully.", 5000);  // Message displayed for 5 seconds
}

// Function to unblock sites
void MainWindow::unblock_sites(const std::vector<std::string> &sites) const {
    std::string command = "awk '!/";

    for (size_t i = 0; i < sites.size(); ++i) {
        if (i != 0) command += "|";
        command += sites[i];
        std::string site_with_www = (sites[i].find("www.") == 0) ? sites[i] : "www." + sites[i];
        command += "|" + site_with_www;
    }

    command += "/' /etc/hosts > /etc/hosts.tmp && mv /etc/hosts.tmp /etc/hosts";

    qDebug() << "Unblock command: " << QString::fromStdString(command);

    // Execute the command with privileges
    if (!execute_with_privileges(command)) {
        qDebug() << "Failed to obtain privileges.";
        return;
    }

   // ui->statusbar->showMessage("Sites unblocked successfully.", 5000);  // Message displayed for 5 seconds
}

void MainWindow::handleEnableButton() {
    qDebug() << "Enable button clicked";
    std::vector<std::string> sites = read_lines(distractorsFilePath);
    if (sites.empty()) {
        qDebug() << "No sites to block.";
        return;
    }
    block_sites(sites);
}

void MainWindow::handleDisableButton() {
    qDebug() << "Disable button clicked";

    // Solve quadratic equation to unblock sites
    int a = 1, b, c;
    int root1 = arc4random() % 19 - 9;
    int root2 = arc4random() % 19 - 9;
    b = -(root1 + root2);
    c = root1 * root2;

    QuadraticDialog dialog(this);
    dialog.setEquation(a, b, c, root1, root2);
    dialog.setWindowIcon(QIcon(":/public/palm.png")); // Set the custom icon for the dialog

    if (dialog.exec() == QDialog::Accepted) {
        int user_input = dialog.getUserInput();
        if (user_input == root1 || user_input == root2) {
            std::vector<std::string> sites = read_lines(distractorsFilePath);
            if (sites.empty()) {
                qDebug() << "No sites to unblock.";
                return;
            }
            unblock_sites(sites);
        } else {
            QMessageBox::warning(this, "Incorrect Root", "The root you entered is incorrect. Sites remain blocked.");
        }
    }
}

void MainWindow::handleOpenEditorDialog()
{
    EditorDialog editorDialog(this);
    editorDialog.exec();
}


void MainWindow::exitButtonClicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLabel *label = new QLabel("Are you sure you want to exit?", dialog);
    layout->addWidget(label);

    QPushButton *yesButton = new QPushButton("Yes", dialog);
    QPushButton *noButton = new QPushButton("No", dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);

    layout->addLayout(buttonLayout);

    QObject::connect(yesButton, &QPushButton::clicked, dialog, [=]() { qApp->quit(); });
    QObject::connect(noButton, &QPushButton::clicked, dialog, &QDialog::reject);

    dialog->exec();
}


