#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectWithFacebookButton, SIGNAL(clicked()),
            this, SLOT(connectWithFacebook()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::connectWithFacebook()
{
    qDebug() << "connectWithFacebook button pressed!!";
    facebookDialog = new QWebView(this);
    facebookDialog->load(QUrl("https://www.facebook.com/dialog/oauth?client_id=375162172308&redirect_uri=https://www.facebook.com/connect/login_success.html&display=touch"));
    connect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(openFacebookDialog(bool)));
}

void MainWindow::openFacebookDialog(bool isOk)
{
    if (isOk) {
        ui->verticalLayout->addWidget(facebookDialog);
        disconnect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(openFacebookDialog(bool)));
        connect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(handleResponse(bool)));
    } else {
        qDebug() << "an error occured!!";
    }
}

void MainWindow::handleResponse(bool isOk)
{
    if (isOk) {
        if (facebookDialog->url().toString().startsWith("https://www.facebook.com/connect/login_success.html")) {
            QString url = facebookDialog->url().toString();
            int length = url.length();
            int index = QString("https://www.facebook.com/connect/login_success.html?code=").size();
            QString code = facebookDialog->url().toString().right(length - index);
            // request auth token
            // authRequest = new QNetworkRequest(QUrl("https://graph.facebook.com/oauth/access_token?client_id=375162172308&redirect_uri=YOUR_URL&client_secret=f1d1a539b62dc2a490a3bfc3547eab19&code=THE_CODE_FROM_ABOVE"));

            qDebug() << "length: " << length << " index: " << index << " code: " << code;
        }
    } else {
        qDebug() << "an error occured!!";
    }
}
