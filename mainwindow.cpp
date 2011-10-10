#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleResponse(QNetworkReply*)));
    connect(ui->connectWithFacebookButton, SIGNAL(clicked()),
            this, SLOT(connectWithFacebook()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;
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
    ui->connectWithFacebookButton->setEnabled(false);
    facebookDialog = new QWebView(this);
    facebookDialog->load(QUrl("https://www.facebook.com/dialog/oauth?client_id=375162172308&redirect_uri=https://www.facebook.com/connect/login_success.html&display=popup&scope=friends_birthday,friends_groups,offline_access&response_type=token"));
    connect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(openFacebookDialog(bool)));
    label = new QLabel(this);
    label->setStyleSheet("QLabel { background-color : black; color : white; }");
    label->setText("loading...");
    ui->verticalLayout->addWidget(label);
    textWindow = new QTextEdit(this);
}

void MainWindow::openFacebookDialog(bool isOk)
{
    ui->verticalLayout->removeWidget(label);
    label->setParent(0);
    if (isOk) {
        ui->verticalLayout->addWidget(facebookDialog);
        disconnect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(openFacebookDialog(bool)));
        connect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(handleFacebookResponse(bool)));
    } else {
        qDebug() << "an error occured!!";
        ui->verticalLayout->addWidget(textWindow);
        textWindow->setText("an error occured!!");
    }
}

void MainWindow::handleFacebookResponse(bool isOk)
{
    if (isOk) {
        if (facebookDialog->url().toString().startsWith("https://www.facebook.com/connect/login_success.html")) {
            QString url = facebookDialog->url().toString();
            int length = url.length();
            int index = QString("https://www.facebook.com/connect/login_success.html?").size();
            QString response = facebookDialog->url().toString().right(length - index);
            qDebug() << "response: " << response;
            foreach(QString s, response.split('&')) {
                if(s.startsWith("access_token")) {
                    access_token = s.mid(QString("access_token=").size(), s.size());
                }
            }
            qDebug() << "access_token: " << access_token;
            // remove facebookDialog from layout since we have access token now
            if (!access_token.isEmpty()) {
                disconnect(facebookDialog, SIGNAL(loadFinished(bool)), this, SLOT(handleFacebookResponse(bool)));
                ui->verticalLayout->removeWidget(facebookDialog);
                delete facebookDialog;
                getBirthdays();
            }
        }
    } else {
        qDebug() << "an error occured!!";
        ui->verticalLayout->addWidget(textWindow);
        textWindow->setText("an error occured!!");
    }
}

void MainWindow::getBirthdays()
{
    label->setParent(this);
    ui->verticalLayout->addWidget(label);
    QUrl url("https://graph.facebook.com/me/friends?access_token=" + access_token);
    networkManager->get(QNetworkRequest(url));
}

void MainWindow::handleResponse(QNetworkReply *reply)
{
    ui->verticalLayout->removeWidget(label);
    label->setParent(0);
    if (reply->error() == QNetworkReply::NoError)
    {
        QString response(reply->readAll());
        qDebug() << response;
        ui->verticalLayout->addWidget(textWindow);
        textWindow->setText(response);

    }
    // https://graph.facebook.com/539123475?access_token=375162172308%7C2.AQB1WTIXLrIvKLhr.86400.1318201200.3-576754183%7C_NT1OEMlx8PMKHqpn5d7kLtVu2Y
    // to get birthday of a friend
}
