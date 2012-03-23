#include "tst_jobs.h"

#include "dummyserver.h"

#include <bodega/assetjob.h>
#include <bodega/changelanguagejob.h>
#include <bodega/channelsjob.h>
#include <bodega/globals.h>
#include <bodega/session.h>
#include <bodega/signonjob.h>

#include <QSettings>
#include <QDir>

using namespace Bodega;

static const char *signOnConf = "0001_signon.conf";
static const char *languageConf = "0002_language.conf";
static const char *channelsConf = "0003_channels.conf";
static const char *assetConf = "0004_assets.conf";


static QStringList
getStringList(QSettings *settings, const char *key)
{
    QStringList lst = settings->value(QString::fromLatin1(key)).toStringList();

    if (lst.count() == 1 && lst[0].isEmpty())
        lst = QStringList();

    return lst;
}

TestJobs::TestJobs()
  : QObject()
{
    initSettings();

    server = new DummyHttpServer(settingsMap.values());

    session = new Session(this);
    QUrl base;
    base.setScheme(QString::fromLatin1("http"));
    base.setHost(QString::fromLatin1("localhost"));
    base.setPort(DUMMY_PORT);
    session->setBaseUrl(base);


    {
        QSettings settings(QLatin1String("hello.conf"),
                           QSettings::IniFormat);

        QStringList lst;

        lst.append(QLatin1String("name"));
        lst.append(QLatin1String("description hello, world.\" sentence,\" "));
        lst.append(QLatin1String("ha 123"));
        settings.beginGroup(QLatin1String("hello"));
        settings.setValue(QLatin1String("mymap"), lst);
        settings.endGroup();
    }
}

TestJobs::~TestJobs()
{
    delete server;
}

void TestJobs::initSettings()
{
    QString confDir = QString::fromLatin1("%1/queries")
                      .arg(QString::fromLatin1(SOURCE_DIR));
    QDir dir(confDir);

    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << QLatin1String("*.conf"));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QSettings *settings = new QSettings(fileInfo.absoluteFilePath(),
                                            QSettings::IniFormat);
        //without this the conf file doesn't parse...
        settings->childGroups();
        settingsMap.insert(fileInfo.fileName(), settings);
    }
}

void TestJobs::sessionBasics()
{
    Session *session = new Session();
    QCOMPARE(session->isAuthenticated(), false);
    QCOMPARE(session->userName(), QString());
    QCOMPARE(session->password(), QString());
    QCOMPARE(session->deviceId(), QString());
    QCOMPARE(session->baseUrl(), QUrl());

    session->setUserName(QLatin1String("zack@kde.org"));
    QCOMPARE(session->userName(),
             QLatin1String("zack@kde.org"));
    session->setPassword(QLatin1String("zack"));
    QCOMPARE(session->password(),
             QLatin1String("zack"));
    session->setDeviceId(QLatin1String("2"));
    QCOMPARE(session->deviceId(),
             QLatin1String("2"));
    session->setBaseUrl(
        QUrl(QLatin1String("http://127.0.0.1:3000")));
    QCOMPARE(session->baseUrl(),
             QUrl(QLatin1String("http://127.0.0.1:3000")));

}

void TestJobs::signOn_data()
{
    QSettings *settings = settingsMap[QLatin1String(signOnConf)];
    Q_ASSERT(settings);

    foreach(QString topGroup, settings->childGroups()) {

        settings->beginGroup(topGroup);

        QTest::addColumn<QString>("url");
        QTest::addColumn<QString>("username");
        QTest::addColumn<QString>("password");
        QTest::addColumn<QString>("deviceId");
        QTest::addColumn<bool>("finished");
        QTest::addColumn<bool>("failed");
        QTest::addColumn<QString>("errorId");
        QTest::addColumn<QString>("deviceIdRet");
        QTest::addColumn<int>("points");
        QTest::addColumn<QUrl>("tinyUrl");
        QTest::addColumn<QUrl>("smallUrl");
        QTest::addColumn<QUrl>("mediumUrl");
        QTest::addColumn<QUrl>("largeUrl");
        QTest::addColumn<QUrl>("hugeUrl");
        QTest::addColumn<QUrl>("previewsUrl");

        foreach(QString group, settings->childGroups()) {
            settings->beginGroup(group);
            QString query = settings->value(QString::fromLatin1("url")).toString();
            QString username = settings->value(QString::fromLatin1("arg1")).toString();
            QString password = settings->value(QString::fromLatin1("arg2")).toString();
            QString deviceId = settings->value(QString::fromLatin1("arg3")).toString();
            bool finished = settings->value(QString::fromLatin1("finished")).toBool();
            bool failed = settings->value(QString::fromLatin1("failed")).toBool();
            QString error = settings->value(QString::fromLatin1("error")).toString();
            QString deviceIdRet = settings->value(QString::fromLatin1("device")).toString();
            int points = settings->value(QString::fromLatin1("points")).toInt();
            QUrl tinyUrl = settings->value(QString::fromLatin1("tinyUrl")).toUrl();
            QUrl smallUrl = settings->value(QString::fromLatin1("smallUrl")).toUrl();
            QUrl mediumUrl = settings->value(QString::fromLatin1("mediumUrl")).toUrl();
            QUrl largeUrl = settings->value(QString::fromLatin1("largeUrl")).toUrl();
            QUrl hugeUrl = settings->value(QString::fromLatin1("hugeUrl")).toUrl();
            QUrl previewsUrl = settings->value(QString::fromLatin1("previewsUrl")).toUrl();

            //qDebug()<<"adding row " <<group;
            QString groupName = QString::fromLatin1("%1/%2")
                                .arg(topGroup)
                                .arg(group);
            QTest::newRow(groupName.toLatin1().constData())
                << query << username << password
                << deviceId << finished << failed
                << error << deviceIdRet << points
                << tinyUrl << smallUrl << mediumUrl
                << largeUrl << hugeUrl << previewsUrl;
            settings->endGroup();
        }

        settings->endGroup();
    }
}

void TestJobs::signOn()
{
    QFETCH(QString,  url);
    QFETCH(QString,  username);
    QFETCH(QString,  password);
    QFETCH(QString,  deviceId);
    QFETCH(bool,  finished);
    QFETCH(bool,  failed);
    QFETCH(QString,  errorId);
    QFETCH(QString,  deviceIdRet);
    QFETCH(int,  points);
    QFETCH(QUrl,  tinyUrl);
    QFETCH(QUrl,  smallUrl);
    QFETCH(QUrl,  mediumUrl);
    QFETCH(QUrl,  largeUrl);
    QFETCH(QUrl,  hugeUrl);
    QFETCH(QUrl,  previewsUrl);

    session->setUserName(username);
    session->setPassword(password);
    session->setDeviceId(deviceId);
    SignOnJob *job = session->signOn();

    QVERIFY(job);
    QCOMPARE(job, job);

    QTestEventLoop::instance().connect(
        job, SIGNAL(jobFinished(Bodega::NetworkJob*)),
        SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);

    QString jobUrl = QString::fromLatin1("%1%2").arg(
        session->baseUrl().toString()).arg(url);
    QCOMPARE(job->url().toString(), jobUrl);
    QCOMPARE(job->isFinished(), finished);
    QCOMPARE(job->failed(), failed);
    QCOMPARE(job->error().errorId(), errorId);
    QCOMPARE(job->deviceId(), deviceIdRet);
    QCOMPARE(job->points(), points);
    QCOMPARE(job->imageUrls()[ImageTiny],
             tinyUrl);
    QCOMPARE(job->imageUrls()[ImageSmall],
             smallUrl);
    QCOMPARE(job->imageUrls()[ImageMedium],
             mediumUrl);
    QCOMPARE(job->imageUrls()[ImageLarge],
             largeUrl);
    QCOMPARE(job->imageUrls()[ImageHuge],
             hugeUrl);
    QCOMPARE(job->imageUrls()[ImagePreviews],
             previewsUrl);
}


void TestJobs::channels_data()
{
    QSettings *settings = settingsMap[QLatin1String(channelsConf)];
    Q_ASSERT(settings);

    foreach(QString topGroup, settings->childGroups()) {

        settings->beginGroup(topGroup);

        QTest::addColumn<QString>("channelId");
        QTest::addColumn<int>("offset");
        QTest::addColumn<int>("pageSize");
        QTest::addColumn<QString>("url");
        QTest::addColumn<bool>("finished");
        QTest::addColumn<bool>("failed");
        QTest::addColumn<int>("num_channels");
        QTest::addColumn<QStringList>("channels");
        QTest::addColumn<int>("num_assets");
        QTest::addColumn<QStringList>("assets");;

        foreach(QString group, settings->childGroups()) {
            settings->beginGroup(group);
            QString channelId = settings->value(QString::fromLatin1("channelId")).toString();
            int offset = settings->value(QString::fromLatin1("offset")).toInt();
            int pageSize = settings->value(QString::fromLatin1("pagesize")).toInt();
            QString name = settings->value(QString::fromLatin1("name")).toString();
            QString query = settings->value(QString::fromLatin1("url")).toString();
            bool finished = settings->value(QString::fromLatin1("finished")).toBool();
            bool failed = settings->value(QString::fromLatin1("failed")).toBool();
            //QString error = settings->value(QString::fromLatin1("error")).toString();
            int num_channels = settings->value(QString::fromLatin1("num_channels")).toInt();
            QStringList channels = getStringList(settings, "channels");
            int num_assets = settings->value(QString::fromLatin1("num_assets")).toInt();
            QStringList assets = getStringList(settings, "assets");

            //qDebug()<<"adding row " <<name;
            QTest::newRow(name.toLatin1().constData())
                << channelId << offset << pageSize
                << query << finished << failed
                //<< error
                << num_channels << channels
                << num_assets << assets;

            settings->endGroup();
        }

        settings->endGroup();
    }
}

static QMap<QString, ChannelInfo>
channelsFromStringList(const QStringList &channels, int num_channels)
{
    int idx = 0;
    QMap<QString, ChannelInfo> ret;
    for (int i = 0; i < num_channels; ++i) {
        ChannelInfo info;
        info.id = channels[idx++];
        //info.image =
        channels[idx++];
        info.name = channels[idx++];
        info.description = channels[idx++];
        ret.insert(info.id, info);
    }
    return ret;
}

static QMap<QString, AssetInfo>
assetsFromStringList(const QStringList &assets, int num_assets)
{
    int idx = 0;
    QMap<QString, AssetInfo> ret;
    for (int i = 0; i < num_assets; ++i) {
        AssetInfo info;

        info.id = assets[idx++];
        info.license = assets[idx++];
        info.partnerId = assets[idx++];
        info.version = assets[idx++];
        info.path = assets[idx++];
        //info.image =
        assets[idx++];
        info.name = assets[idx++];

        ret.insert(info.id, info);
    }
    return ret;
}

void TestJobs::channels()
{
    QFETCH(QString,  channelId);
    QFETCH(int,  offset);
    QFETCH(int,  pageSize);
    QFETCH(QString,  url);
    QFETCH(bool,  finished);
    QFETCH(bool,  failed);
    QFETCH(int,  num_channels);
    QFETCH(QStringList,  channels);
    QFETCH(int,  num_assets);
    QFETCH(QStringList,  assets);
    QMap<QString, ChannelInfo> refChannelsMap;
    QMap<QString, AssetInfo> refAssetsMap;

    refChannelsMap = channelsFromStringList(channels, num_channels);
    refAssetsMap = assetsFromStringList(assets, num_assets);

    ChannelsJob *job = session->channels(channelId, offset, pageSize);

    QVERIFY(job);
    QCOMPARE(job->channelId(), channelId);
    QCOMPARE(job->offset(), offset);
    QCOMPARE(job->pageSize(), pageSize);

    QTestEventLoop::instance().connect(
        job,
        SIGNAL(jobFinished(Bodega::NetworkJob*)),
        SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);

    QString jobUrl = QString::fromLatin1("%1%2")
                     .arg(session->baseUrl().toString())
                     .arg(url);
    QCOMPARE(job->url().toString(), jobUrl);
    QCOMPARE(job->isFinished(), finished);
    QCOMPARE(job->failed(), failed);
    QCOMPARE(job->channels().count(), num_channels);
    for (int i = 0; i < num_channels; ++i) {
        ChannelInfo info = job->channels()[i];
        ChannelInfo ref = refChannelsMap[info.id];
        QCOMPARE(info.id, ref.id);
        QCOMPARE(info.name, ref.name);
        QCOMPARE(info.description, ref.description);
        //QCOMPARE(info.image, ref.image);
    }
    QCOMPARE(job->assets().count(), num_assets);
    for (int i = 0; i < num_assets; ++i) {
        AssetInfo info = job->assets()[i];
        AssetInfo ref = refAssetsMap[info.id];

        QCOMPARE(info.id, ref.id);
        QCOMPARE(info.license, ref.license);
        QCOMPARE(info.partnerId, ref.partnerId);
        QCOMPARE(info.name, ref.name);
        QCOMPARE(info.version, ref.version);
        QCOMPARE(info.path, ref.path);
        //QCOMPARE(info.image, ref.image);
        //QCOMPARE(info.description, ref.description);
        //QCOMPARE(info.points, ref.points);
    }

}

void TestJobs::asset_data()
{
    QSettings *settings = settingsMap[QLatin1String(assetConf)];
    Q_ASSERT(settings);

    foreach(QString topGroup, settings->childGroups()) {

        settings->beginGroup(topGroup);

        QTest::addColumn<QString>("assetId");
        QTest::addColumn<QString>("url");
        QTest::addColumn<bool>("showPreviews");
        QTest::addColumn<bool>("showChangeLog");
        QTest::addColumn<bool>("finished");
        QTest::addColumn<bool>("failed");
        QTest::addColumn<QString>("error");
        QTest::addColumn<QStringList>("asset");
        QTest::addColumn<QStringList>("tags");
        QTest::addColumn<QStringList>("previews");

        foreach(QString group, settings->childGroups()) {
            settings->beginGroup(group);
            QString name = settings->value(QString::fromLatin1("name")).toString();
            QString assetId = settings->value(QString::fromLatin1("assetId")).toString();
            QString query = settings->value(QString::fromLatin1("url")).toString();
            bool showPreviews  = settings->value(QString::fromLatin1("showPreviews")).toBool();
            bool showChangeLog = settings->value(QString::fromLatin1("showChangeLog")).toBool();
            bool finished = settings->value(QString::fromLatin1("finished")).toBool();
            bool failed = settings->value(QString::fromLatin1("failed")).toBool();
            QString error = settings->value(QString::fromLatin1("error")).toString();
            QStringList asset = getStringList(settings, "asset");
            QStringList tags = getStringList(settings, "tags");
            QStringList previews = getStringList(settings, "previews");

            if (tags.count() == 1 && tags[0].isEmpty())
                tags = QStringList();

            if (tags.count() == 1 && tags[0].isEmpty())
                tags = QStringList();

            //qDebug()<<"adding row " <<name;
            QTest::newRow(name.toLatin1().constData())
                << assetId
                << query
                << showPreviews << showChangeLog
                << finished << failed
                << error << asset
                << tags << previews;

            settings->endGroup();
        }

        settings->endGroup();
    }
}


static AssetInfo
assetFromStringList(const QStringList &assets)
{
    int idx = 0;
    AssetInfo info;

    if (assets.count() < 8)
        return info;

    info.id = assets[idx++];
    info.license = assets[idx++];
    info.partnerId = assets[idx++];
    info.version = assets[idx++];
    info.path = assets[idx++];
    //info.image =
    assets[idx++];
    info.name = assets[idx++];
    info.description = assets[idx++];

    if (idx < assets.count())
        info.points = assets[idx++].toInt();

    return info;
}

static Tags
tagsFromStringList(const QStringList &lst)
{
    int idx = 0;
    Tags tags;

    while (idx < lst.count()) {
        const QString type = lst[idx++];
        const QString value = lst[idx++];
        tags.insert(type, value);
    }
    return tags;
}

void TestJobs::asset()
{

    QFETCH(QString,  assetId);
    QFETCH(QString,  url);
    QFETCH(bool,  showPreviews);
    QFETCH(bool,  showChangeLog);
    QFETCH(bool,  finished);
    QFETCH(bool,  failed);
    QFETCH(QString,  error);
    QFETCH(QStringList,  asset);
    QFETCH(QStringList,  tags);
    QFETCH(QStringList,  previews);

    AssetJob::AssetFlags flags = AssetJob::None;
    AssetInfo ref = assetFromStringList(asset);
    Tags refTags = tagsFromStringList(tags);

    if (showPreviews)
        flags |= AssetJob::ShowPreviews;
    if (showChangeLog)
        flags |= AssetJob::ShowChangeLog;

    AssetJob *job = session->asset(assetId, flags);

    QVERIFY(job);
    QCOMPARE(job->assetId(), assetId);

    QTestEventLoop::instance().connect(
        job,
        SIGNAL(jobFinished(Bodega::NetworkJob*)),
        SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);

    QString jobUrl = QString::fromLatin1("%1%2")
                     .arg(session->baseUrl().toString())
                     .arg(url);
    QCOMPARE(job->url().toString(), jobUrl);
    QCOMPARE(job->isFinished(), finished);
    QCOMPARE(job->failed(), failed);

    AssetInfo info = job->info();

    QCOMPARE(info.id, ref.id);
    QCOMPARE(info.license, ref.license);
    QCOMPARE(info.partnerId, ref.partnerId);
    QCOMPARE(info.name, ref.name);
    QCOMPARE(info.version, ref.version);
    QCOMPARE(info.path, ref.path);
    //QCOMPARE(info.image, ref.image);
    QCOMPARE(info.description, ref.description);
    QCOMPARE(info.points, ref.points);

    Tags retTags = job->tags();
    QCOMPARE(retTags.count(), refTags.count());
    QCOMPARE(retTags, refTags);
    Tags::const_iterator i = retTags.constBegin();    
    while (i != retTags.constEnd()) {
        QVERIFY(refTags.values(i.key()).contains(i.value()));
        ++i;
    }
    QCOMPARE(job->previews(), previews);

#if 0
    ChangeLog ch = job->changeLog();
    QMap<QString, ChangeLog::Entry>::const_iterator itr;
    for ( itr = ch.entries.constBegin(); itr != ch.entries.constEnd(); ++itr ) {
        qDebug()<<"ChangeLog:";
        qDebug()<<"  Version = "<<itr.key();
        qDebug()<<"     Timestamp = "<<itr.value().timestamp;
        qDebug()<<"     Changes = "<<itr.value().changes;
    }
#endif
}

void TestJobs::language_data()
{
    QSettings *settings = settingsMap[QLatin1String(languageConf)];
    Q_ASSERT(settings);

    foreach(QString topGroup, settings->childGroups()) {

        settings->beginGroup(topGroup);

        QTest::addColumn<QString>("url");
        QTest::addColumn<QString>("lang");
        QTest::addColumn<bool>("finished");
        QTest::addColumn<bool>("failed");
        QTest::addColumn<QString>("errorId");

        foreach(QString group, settings->childGroups()) {
            settings->beginGroup(group);
            QString query = settings->value(QString::fromLatin1("url")).toString();
            QString lang = settings->value(QString::fromLatin1("lang")).toString();
            bool finished = settings->value(QString::fromLatin1("finished")).toBool();
            bool failed = settings->value(QString::fromLatin1("failed")).toBool();
            QString error = settings->value(QString::fromLatin1("error")).toString();

            //qDebug()<<"adding row " <<group;
            QString groupName = QString::fromLatin1("%1/%2")
                                .arg(topGroup)
                                .arg(group);
            QTest::newRow(groupName.toLatin1().constData())
                << query << lang
                << finished << failed
                << error;

            settings->endGroup();
        }

        settings->endGroup();
    }
}

void TestJobs::language()
{
    //disabled for now because we'll be changing them
#if 0
    QFETCH(QString,  url);
    QFETCH(QString,  lang);
    QFETCH(bool,  finished);
    QFETCH(bool,  failed);
    QFETCH(QString,  errorId);

    ChangeLanguageJob *job = session->changeLanguage(lang);

    QVERIFY(job);
    QCOMPARE(job->language(), lang);

    QTestEventLoop::instance().connect(
        job,
        SIGNAL(jobFinished(Bodega::NetworkJob*)),
        SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);

    QString jobUrl = QString::fromLatin1("%1%2")
                     .arg(session->baseUrl().toString())
                     .arg(url);
    QCOMPARE(job->url().toString(), jobUrl);
    QCOMPARE(job->isFinished(), finished);
    QCOMPARE(job->failed(), failed);
    QCOMPARE(job->error().errorId(), errorId);
#endif
}

QTEST_MAIN(TestJobs)


#include "tst_jobs.moc"
