#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <vector>
#include <string>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void restoreWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool event(QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private slots:
    void handleEnableButton();
    void handleDisableButton();
    void handleOpenEditorDialog();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void exitButtonClicked();

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *restoreAction;
    QAction *quitAction;

    std::vector<std::string> read_lines(const QString &filename) const;
    bool is_ip_address(const std::string &str) const;
    void block_sites(const std::vector<std::string> &sites) const;
    void unblock_sites(const std::vector<std::string> &sites) const;
};

#endif // MAINWINDOW_H
