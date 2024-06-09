#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleEnableButton();
    void handleDisableButton();

private:
    Ui::MainWindow *ui;
    std::vector<std::string> read_lines(const QString &filename) const;
    bool is_ip_address(const std::string &str) const;
    void block_sites(const std::vector<std::string> &sites) const;
    void unblock_sites(const std::vector<std::string> &sites) const;
};

#endif // MAINWINDOW_H
