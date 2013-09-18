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

#ifndef BODEGA_FORUMJOB_H
#define BODEGA_FORUMJOB_H

#include <bodega/networkjob.h>
#include <bodega/globals.h>

namespace Bodega {

    class Session;

    class ForumCategoryJob : public NetworkJob
    {
        Q_OBJECT
    public:
        ForumCategoryJob(QNetworkReply *reply,
                       Session *parent);
        ~ForumCategoryJob();

        Bodega::ForumCategory category() const;

    protected:
        virtual void netFinished(const QVariantMap &result);

    private Q_SLOTS:
        void topicFinished(Bodega::NetworkJob *job);

    private:
        void parseCategory(const QVariantMap &result);
        QNetworkReply * get(const QUrl &url);

        Session *m_session;
        Bodega::ForumCategory m_category;
    };

    class ForumTopicJob : public NetworkJob
    {
        Q_OBJECT
    public:
        ForumTopicJob(QNetworkReply *reply,
                       Session *parent);
        ~ForumTopicJob();

        Bodega::ForumCategory::ForumTopic::ForumPost  post() const;

    protected:
        virtual void netFinished(const QVariantMap &result);

    private:
        void parsePost(const QVariantMap &result);
        Bodega::ForumCategory::ForumTopic::ForumPost m_post;
    };

}

#endif
