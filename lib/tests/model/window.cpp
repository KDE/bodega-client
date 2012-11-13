#include "window.h"

#include <QDebug>

#include "bodega/signonjob.h"

#include "modeltest.h"

using namespace Bodega;

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    init();
}

void Window::init()
{
    ui.setupUi(this);

    connect(ui.connectButton, SIGNAL(clicked()), SLOT(signOn()));

    m_session = new Session(this);
    connect(m_session, SIGNAL(authenticated(bool)), this, SLOT(authenticated(bool)));

    m_model = new Bodega::Model(this);
    m_model->setSession(m_session);
    ui.treeView->setModel(m_model);

    new ModelTest(m_model, this);

    QAction *action = new QAction(tr("Quit"), this);
    action->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(action);
}

void Window::authenticated(bool connected)
{
    ui.connectButton->setText(connected ? QLatin1String("Disconnect") : QLatin1String("Connect"));
    m_model->setTopChannel(QString());
}

void Window::error(Bodega::NetworkJob *job, const Bodega::Error &error)
{
    qDebug() << "Connection error:";
    qDebug() << "     " << error.type() << error.errorId();
    qDebug() << "     " << error.title();
    qDebug() << "     " << error.description();
}

void Window::signOn()
{
    if (m_session->isAuthenticated()) {
        m_session->disconnect();
    } else {
        m_session->setBaseUrl(QUrl(QLatin1String("http://127.0.0.1:3000/bodega/v1/json/")));
        m_session->setUserName(ui.usernameEdit->text());
        m_session->setPassword(ui.passwordEdit->text());
        m_session->setStoreId(ui.storeIdEdit->text());
        connect(m_session->signOn(), SIGNAL(error(Bodega::NetworkJob*,Bodega::Error)),
                this, SLOT(error(Bodega::NetworkJob*,Bodega::Error)));
    }
}

#include "window.moc"
