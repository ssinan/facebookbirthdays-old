#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTextEdit>
#include <QLabel>

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
    void handleFacebookResponse(bool);
    void handleResponse(QNetworkReply*);

private:
    Ui::MainWindow *ui;
    QWebView* facebookDialog;
    QLabel* label;
    QTextEdit* textWindow;
    QNetworkAccessManager *networkManager;
    QString access_token;

    void getBirthdays();
};

#endif // MAINWINDOW_H
