#include "mainwindow.h"
#include "macos_privileges.h"
#include "ui_mainwindow.h"
#include "privileges.h"
#include <fstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDebug>
#include <QCloseEvent>
#include <QEvent>
#include <QTimer> // Add this include

const QString distractorsFilePath = ":/data/distractors.txt"; // Use a single variable for the resource path

// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Connecting buttons";
    connect(ui->enableButton, &QPushButton::clicked, this, &MainWindow::handleEnableButton);
    connect(ui->disableButton, &QPushButton::clicked, this, &MainWindow::handleDisableButton);

    // Create tray icon and menu
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);
    restoreAction = new QAction(tr("Restore"), this);
    quitAction = new QAction(tr("Quit"), this);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreWindow);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/public/icon.png"));
    trayIcon->show();

    // Set up the tray icon activation behavior
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
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

// Restore window from minimized state
void MainWindow::restoreWindow()
{
    this->showNormal();
    this->raise();         // Bring window to the front
    this->activateWindow(); // Bring window to the foreground
    this->update();        // Force update the window
    this->repaint();       // Force repaint the window
}

// Handle tray icon activation (e.g., single-click or double-click)
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        restoreWindow();
    }
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
        qDebug() << "Read line:" << line;

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

    qDebug() << "Block command: " << QString::fromStdString(command);

    // Execute the command with privileges
    if (!execute_with_privileges(command)) {
        qDebug() << "Failed to obtain privileges.";
        return;
    }

    ui->statusLabel->setText("Sites blocked successfully.");
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

    ui->statusLabel->setText("Sites unblocked successfully.");
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
    std::vector<std::string> sites = read_lines(distractorsFilePath);
    if (sites.empty()) {
        qDebug() << "No sites to unblock.";
        return;
    }
    unblock_sites(sites);
}
