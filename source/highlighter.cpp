/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "highlighter.h"
#include "xml_functions.h"
#include <QtXml/QDomDocument>

const QString Highlighter::mDefault {"default"};

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    mCurrentLanguage = "";
    load_default_language();
    load_language("cpp");
}

void Highlighter::load_language(QString aLanguage)
{
    QString file_name {"./langs.model.xml"};
    QDomDocument fDoc(file_name);
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly))
    {
        fDoc.setContent(&file);
        file.close();
    }

    auto language_node = findElement(fDoc.firstChild(), "Language#name=" + aLanguage, true);
    if (!language_node.isNull())
    {
        HighlightingRule rule;
        Language language;

        QString extensions   = getValue(language_node.attributes().namedItem("ext"), QString(""), true);
        if (extensions.size())
        {
            auto list = extensions.split(" ");
            for (auto& entry : list)
            {
                mExtensionToLanguage[entry] = aLanguage;
            }
        }
        QString commentLine  = getValue(language_node.attributes().namedItem("commentLine"), QString(""), true);
        if (commentLine.size())
        {
            language.singleLineCommentFormat.setForeground(Qt::red);
            commentLine += "[^\n]*";
            rule.pattern = QRegularExpression(commentLine);
            rule.format = language.singleLineCommentFormat;
            language.highlightingRules.append(rule);
        }
        QString commentStart = getValue(language_node.attributes().namedItem("commentStart"), QString(""), true);
        if (commentStart.size())
        {
            language.commentStartExpression = QRegularExpression(commentStart);
        }
        QString commentEnd   = getValue(language_node.attributes().namedItem("commentEnd"), QString(""), true);
        if (commentEnd.size())
        {
            language.commentStartExpression = QRegularExpression(commentEnd);
        }
        for(QDomNode n = language_node.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            QString name = getValue(n.attributes().namedItem("name"), QString(""), true);
            if (name.size())
            {
                language.keywordFormat.setForeground(Qt::darkBlue);
                language.keywordFormat.setFontWeight(QFont::Bold);
                QString keys = getValue(n, QString(""));
                auto list = keys.split(" ");
                for (auto& entry : list)
                {
                    QString pattern = tr("\\b%1\\b").arg(entry);
                    rule.pattern = QRegularExpression(pattern);
                    rule.format = language.keywordFormat;
                    language.highlightingRules.append(rule);
                }
            }
        }
        mLanguages[aLanguage] = language;
    }
}

void Highlighter::load_default_language()
{
    HighlightingRule rule;
    Language language;

    language.keywordFormat.setForeground(Qt::darkBlue);
    language.keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] =
    {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\bfor\\b"), QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bdo\\b"),
        QStringLiteral("\\bswitch\\b"), QStringLiteral("\\bcase\\b"), QStringLiteral("\\bif\\b"),
        QStringLiteral("\\bauto\\b"), QStringLiteral("\\belse\\b"), QStringLiteral("\\bstatic_cast\\b"),
        QStringLiteral("\\bdynamic_cast\\b"), QStringLiteral("\\breinterpret_cast\\b"), QStringLiteral("\\bstatic_assert\\b"),
        QStringLiteral("\\busing\\b")
    };

    for (const QString &pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = language.keywordFormat;
        language.highlightingRules.append(rule);
    }

    language.classFormat.setFontWeight(QFont::Bold);
    language.classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = language.classFormat;
    language.highlightingRules.append(rule);

    language.singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = language.singleLineCommentFormat;
    language.highlightingRules.append(rule);

    language.multiLineCommentFormat.setForeground(Qt::red);

    language.quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = language.quotationFormat;
    language.highlightingRules.append(rule);

    language.preprocessorFormat.setFontWeight(QFont::Bold);
    language.preprocessorFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("#.*"));
    rule.format = language.preprocessorFormat;
    language.highlightingRules.append(rule);

    language.functionFormat.setFontItalic(true);
    language.functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = language.functionFormat;
    language.highlightingRules.append(rule);

    language.commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    language.commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

    mLanguages[mDefault] = language;
}

const Highlighter::Language& Highlighter::get_language()
{
    const auto found_language = mLanguages.find(mCurrentLanguage);
    if (found_language != mLanguages.end())
    {
        return found_language.value();
    }
    return mLanguages[mDefault];
}

void Highlighter::highlightBlock(const QString &text)
{
    const Language& language = get_language();
    for (const HighlightingRule &rule : language.highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
    {
        startIndex = text.indexOf(language.commentStartExpression);
    }

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = language.commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, language.multiLineCommentFormat);
        startIndex = text.indexOf(language.commentStartExpression, startIndex + commentLength);
    }
}

