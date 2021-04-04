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

const QString Highlighter::mDefault {"git"};
QMap<QString, Highlighter::Language> Highlighter::mLanguages;
QMap<QString, QString>  Highlighter::mExtensionToLanguage;
QSharedPointer<QDomDocument> Highlighter::mDoc;


Highlighter::Language::Language()
{
    singleLineCommentFormat.setForeground(Qt::red);
    multiLineCommentFormat.setForeground(Qt::red);

    keywordFormat[0].setForeground(Qt::darkBlue);
    keywordFormat[0].setFontWeight(QFont::Medium);

    keywordFormat[1].setForeground(Qt::darkCyan);
    keywordFormat[1].setFontWeight(QFont::Medium);

    keywordFormat[2].setForeground(Qt::darkGreen);
    keywordFormat[2].setFontWeight(QFont::Medium);

    keywordFormat[3].setForeground(Qt::darkYellow);
    keywordFormat[3].setFontWeight(QFont::Medium);

    keywordFormat[4].setForeground(Qt::darkBlue);
    keywordFormat[4].setFontWeight(QFont::Bold);

    keywordFormat[5].setForeground(Qt::darkCyan);
    keywordFormat[5].setFontWeight(QFont::Bold);

    keywordFormat[6].setForeground(Qt::darkYellow);
    keywordFormat[6].setFontWeight(QFont::Bold);

    keywordFormat[7].setForeground(Qt::darkGreen);
    keywordFormat[7].setFontWeight(QFont::Bold);


    numbersFormat.setFontWeight(QFont::Medium);
    numbersFormat.setForeground(Qt::blue);

    quotationFormat.setForeground(Qt::darkGreen);

    preprocessorFormat.setFontWeight(QFont::Bold);
    preprocessorFormat.setForeground(Qt::darkGreen);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    if (!mDoc)
    {
        QString file_name {"./langs.model.xml"};
        QFile file(file_name);
        if (file.open(QIODevice::ReadOnly))
        {
            mDoc.reset(new QDomDocument(file_name));
            mDoc->setContent(&file);
            file.close();
        }

        load_default_language();
        load_language_list();
    }
    mCurrentLanguage = "";
}

void Highlighter::setExtension(const QString& ext)
{
    if (ext.size() == 0)
    {
        mCurrentLanguage = ext;
    }
    auto language = mExtensionToLanguage.find(ext);
    if (language != mExtensionToLanguage.end())
    {
        mCurrentLanguage = language.value();
        if (mLanguages.count(mCurrentLanguage) == 0)
        {
            load_language(mCurrentLanguage);
        }
    }
}

void Highlighter::load_language_list()
{
    if (mDoc)
    {
        auto language_node = findElement(mDoc->firstChild(), "Language", true);
        if (!language_node.isNull())
        {
            for(QDomNode n = language_node;  !n.isNull(); n = n.nextSibling())
            {
                QString name       = getValue(n.attributes().namedItem("name"), QString(""), true);
                QString extensions = getValue(n.attributes().namedItem("ext"), QString(""), true);
                if (extensions.size())
                {
                    auto list = extensions.split(" ");
                    for (auto& entry : list)
                    {
                        mExtensionToLanguage[entry] = name;
                    }
                }
            }
        }
    }
}
void Highlighter::load_language(QString language_name)
{
    if (mDoc)
    {
        auto language_node = findElement(mDoc->firstChild(), "Language#name=" + language_name, true);
        if (!language_node.isNull())
        {
            HighlightingRule rule;
            Language language;

            QString commentLine  = getValue(language_node.attributes().namedItem("commentLine"), QString(""), true);
            if (commentLine.size())
            {
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
                language.commentEndExpression = QRegularExpression(commentEnd);
            }

            rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
            rule.format = language.quotationFormat;
            language.highlightingRules.append(rule);

            int k=0;
            for(QDomNode n = language_node.firstChild(); !n.isNull(); n = n.nextSibling())
            {
                QString name = getValue(n.attributes().namedItem("name"), QString(""), true);
                if (name.size())
                {
                    QString keys = getValue(n, QString(""));
                    if (name == "function")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.functionFormat;
                        language.highlightingRules.append(rule);
                    }
                    else if (name == "preprocessor")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.preprocessorFormat;
                        language.highlightingRules.append(rule);
                    }
                    else if (name == "numbers")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.numbersFormat;
                        language.highlightingRules.append(rule);
                    }
                    else
                    {
                        for (auto& character : keys)
                        {
                            if (character == ' ') character = '|';
                        }
                        QString pattern = tr("\\b(?:%1)\\b").arg(keys);
                        rule.pattern = QRegularExpression(pattern);
                        rule.format = language.keywordFormat[k];
                        language.highlightingRules.append(rule);
                        if (k < Language::keyword_formats) ++k;
                    }
                }
            }
            mLanguages[language_name] = language;
        }
    }
}

void Highlighter::load_default_language()
{
    HighlightingRule rule;
    Language language;

    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9\\+\\-\\.]+\\b"));
    rule.format = language.numbersFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("#.*"));
    rule.format = language.singleLineCommentFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = language.quotationFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:git|add|status|difftool|mergetool|diff|rm|reset|commit|mv|checkout|log|push|pull|show|branch)\\b"));
    rule.format = language.keywordFormat[0];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:D|M|A|R|DD|AU|UD|UA|DU|AA|UU|\\?)\\b"));
    rule.format = language.keywordFormat[1];
    language.highlightingRules.append(rule);

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

    if (language.commentStartExpression.pattern().size() && language.commentEndExpression.pattern().size())
    {
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
            if (commentLength)
            {
                setFormat(startIndex, commentLength, language.multiLineCommentFormat);
                startIndex = text.indexOf(language.commentStartExpression, startIndex + commentLength);
            }
            else
            {
                startIndex = -1;
            }
        }
    }
}

