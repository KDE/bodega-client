#ifndef TST_JOBS_H
#define TST_JOBS_H

#include <QtTest/QtTest>

class QSettings;
class DummyHttpServer;
namespace Bodega {
    class Session;
}

class TestJobs : public QObject
{
    Q_OBJECT
public:
    TestJobs();
    ~TestJobs();
private slots:
    void sessionBasics();

    void signOn_data();
    void signOn();

    void language_data();
    void language();

    void channels_data();
    void channels();

    void asset_data();
    void asset();

private:
    void initSettings();
private:
    DummyHttpServer *server;
    Bodega::Session *session;
    QMap<QString, QSettings *> settingsMap;
};

#endif
