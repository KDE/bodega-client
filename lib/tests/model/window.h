#ifndef WINDOW_H
#define WINDOW_H

#include "ui_window.h"

#include <bodega/bodegamodel.h>
#include <bodega/session.h>

#include <QMainWindow>

class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window(QWidget *parent=0);

private:
    void init();

private slots:
    void signOn();
    void authenticated(bool connected);
    void error(Bodega::NetworkJob *job, const Bodega::Error &error);

private:
    Ui_MainWindow ui;
    Bodega::Session *m_session;
    Bodega::Model *m_model;
};

#endif
