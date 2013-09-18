/*
 *   Copyright 2012 Coherent Theory LLC
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "forumjob.h"
#include "session.h"
#include <QDebug>

using namespace Bodega;

ForumCategoryJob::ForumCategoryJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent),
      m_session(parent)
{
}

ForumCategoryJob::~ForumCategoryJob()
{
}

Bodega::ForumCategory ForumCategoryJob::category() const
{
    return m_category;
}

void ForumCategoryJob::netFinished(const QVariantMap &result)
{
    if (!failed()) {
        parseCategory(result);
    }
}

void ForumCategoryJob::parseCategory(const QVariantMap &result)
{
    QVariantList categories = result[QLatin1String("categories")].toList();
    QVariantMap category = categories.at(0).toMap();

    m_category.categoryId = category[QLatin1String("id")].toString();
    m_category.categoryName = category[QLatin1String("name")].toString();

    QVariantList topics = result[QLatin1String("topic_list")].toMap()[QLatin1String("topics")].toList();
    QVariantList::const_iterator itr;

    if (topics.size() > 0) {
        // when netFinished is done the SIGNAL
        // jobFinished will be called and we will end
        // up with a race condition. jobFinished of ForumCategoryJob
        // will be emitted before the jobFinished of ForumTopicJob.
        // So block all the signals, we will handle them manually.
        blockSignals(true);
    }

    for (itr = topics.constBegin(); itr != topics.constEnd(); ++itr) {
        QVariantMap info = itr->toMap();
        ForumCategory::ForumTopic topic;
        topic.topicName = info[QLatin1String("fancy_title")].toString();
        topic.topicId = info[QLatin1String("id")].toString();
        topic.topicSlug = info[QLatin1String("slug")].toString();
        m_category.topics.append(topic);
        QUrl url = QLatin1String("http://localhost:3000/t/") + topic.topicSlug + QLatin1String("/") + topic.topicId + QLatin1String(".json"); //FIXME
        qDebug() << url;
        ForumTopicJob *topicJob = new ForumTopicJob(get(url), m_session);

        connect(topicJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(topicFinished(Bodega::NetworkJob *)));
    }
}

QNetworkReply *ForumCategoryJob::get(const QUrl &url)
{
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Bodega 0.1");
    request.setUrl(url);
    QNetworkAccessManager *netManager = new QNetworkAccessManager(m_session);
    return netManager->get(request);
}

void ForumCategoryJob::topicFinished(Bodega::NetworkJob *job)
{
    ForumTopicJob *topicJob = qobject_cast<ForumTopicJob*>(job);
    if (job) {
        topicJob->deleteLater();
        if (!topicJob->failed()) {
            for (int i =0; i < m_category.topics.size(); i++) {
                ForumCategory::ForumTopic tmp = m_category.topics.at(i);
                if (tmp.topicId == topicJob->post().topicId) {
                    tmp = m_category.topics.takeAt(i);
                    tmp.post = topicJob->post();
                    m_category.topics.append(tmp);
                }
            }
        }
    }
    // now we have our data so its
    // time to bring everything back
    // to normal. So unblock the signals.
    blockSignals(false);

    // jobFinished has been blocked before, so
    // we have to emit it.
    setFinished();
}



ForumTopicJob::ForumTopicJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent)
{
}

ForumTopicJob::~ForumTopicJob()
{
}

Bodega::ForumCategory::ForumTopic::ForumPost ForumTopicJob::post() const
{
    return m_post;
}

void ForumTopicJob::netFinished(const QVariantMap &result)
{
    if (!failed()) {
        parsePost(result);
    }
}

void ForumTopicJob::parsePost(const QVariantMap &result)
{
    QVariantMap post = result[QLatin1String("post_stream")].toMap()[QLatin1String("posts")].toList().at(0).toMap();

    m_post.post = post[QLatin1String("cooked")].toString();
    m_post.topicId = post[QLatin1String("topic_id")].toString();
}


#include "forumjob.moc"
