#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkRequest>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void connectWithFacebook();
    void openFacebookDialog(bool);
    void handleResponse(bool);

private:
    Ui::MainWindow *ui;
    QWebView *facebookDialog;
    QNetworkRequest *authRequest;
};

#endif // MAINWINDOW_H
