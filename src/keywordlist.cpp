/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywordlist.h"

#include <QXmlStreamReader>

using namespace SyntaxHighlighting;

KeywordList::KeywordList() :
    m_caseSensitive(Qt::CaseSensitive)
{
}

KeywordList::~KeywordList()
{
}

bool KeywordList::isEmpty() const
{
    return m_keywords.isEmpty();
}

QString KeywordList::name() const
{
    return m_name;
}

bool KeywordList::contains(const QStringRef &str) const
{
    // TODO avoid the copy in toString!

    if (m_caseSensitive == Qt::CaseSensitive)
        return m_keywords.contains(str.toString());
    return m_keywords.contains(str.toString().toLower());
}

void KeywordList::load(QXmlStreamReader& reader)
{
    Q_ASSERT(reader.name() == QLatin1String("list"));
    Q_ASSERT(reader.tokenType() == QXmlStreamReader::StartElement);

    m_name = reader.attributes().value(QStringLiteral("name")).toString();

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("item")) {
                    m_keywords.insert(reader.readElementText().trimmed());
                    reader.readNextStartElement();
                    break;
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                reader.readNext();
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void KeywordList::setCaseSensitivity(Qt::CaseSensitivity caseSensitive)
{
    // we turn the index into lower-case, so this isn't reversible
    Q_ASSERT(m_caseSensitive != Qt::CaseInsensitive || caseSensitive == m_caseSensitive);
    m_caseSensitive = caseSensitive;

    if (m_caseSensitive == Qt::CaseSensitive)
        return;

    QSet<QString> lcWords;
    foreach (const auto &kw, m_keywords)
        lcWords.insert(kw.toLower());
    m_keywords = lcWords;
}
