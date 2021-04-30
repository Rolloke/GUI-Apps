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
#include "helper.h"

#include <QtXml/QDomDocument>
#include <QSettings>
#include <QDir>

const QString Highlighter::mDefault {"git"};

namespace
{
    const QString groupHighlighter {"Highlighter"};
}

QMap<QString, Highlighter::Language> Highlighter::mLanguages;
QMap<QString, QString>  Highlighter::mExtensionToLanguage;
QSharedPointer<QDomDocument> Highlighter::mDoc;
QStringList Highlighter::mLanguageNames;

QTextCharFormat Highlighter::Language::mKeywordFormat[Highlighter::Language::keyword_formats];
QTextCharFormat Highlighter::Language::mNumbersFormat;
QTextCharFormat Highlighter::Language::mSingleLineCommentFormat;
QTextCharFormat Highlighter::Language::mMultiLineCommentFormat;
QTextCharFormat Highlighter::Language::mQuotationFormat;
QTextCharFormat Highlighter::Language::mFunctionFormat;
QTextCharFormat Highlighter::Language::mPreprocessorFormat;


#define LOAD_FORMAT(SETTINGS, ITEM) Highlighter::Language::convert(ITEM, SETTINGS.value(getSettingsName(#ITEM)).toString());



void Highlighter::Language::load(QSettings& fSettings)
{
    mSingleLineCommentFormat.setForeground(Qt::darkCyan);
    mMultiLineCommentFormat.setForeground(Qt::darkCyan);

    mKeywordFormat[0].setForeground(Qt::darkBlue);
    mKeywordFormat[0].setFontWeight(QFont::Medium);

    mKeywordFormat[1].setForeground(Qt::darkCyan);
    mKeywordFormat[1].setFontWeight(QFont::Medium);

    mKeywordFormat[2].setForeground(Qt::darkGreen);
    mKeywordFormat[2].setFontWeight(QFont::Medium);

    mKeywordFormat[3].setForeground(Qt::darkYellow);
    mKeywordFormat[3].setFontWeight(QFont::Medium);

    mKeywordFormat[4].setForeground(Qt::magenta);
    mKeywordFormat[4].setFontWeight(QFont::Medium);

    mKeywordFormat[5].setForeground(Qt::darkMagenta);
    mKeywordFormat[5].setFontWeight(QFont::Medium);

    mKeywordFormat[6].setForeground(Qt::cyan);
    mKeywordFormat[6].setFontWeight(QFont::Medium);

    mKeywordFormat[7].setForeground(Qt::green);
    mKeywordFormat[7].setFontWeight(QFont::Medium);

    mNumbersFormat.setForeground(Qt::blue);
    mNumbersFormat.setFontWeight(QFont::Medium);

    mQuotationFormat.setForeground(Qt::darkGreen);

    mPreprocessorFormat.setFontWeight(QFont::Bold);
    mPreprocessorFormat.setForeground(Qt::darkGreen);

    mFunctionFormat.setFontItalic(true);
    mFunctionFormat.setForeground(Qt::blue);

    fSettings.beginGroup(groupHighlighter);
    LOAD_FORMAT(fSettings, mSingleLineCommentFormat);
    LOAD_FORMAT(fSettings, mMultiLineCommentFormat);
    LOAD_FORMAT(fSettings, mKeywordFormat[0]);
    LOAD_FORMAT(fSettings, mKeywordFormat[1]);
    LOAD_FORMAT(fSettings, mKeywordFormat[2]);
    LOAD_FORMAT(fSettings, mKeywordFormat[3]);
    LOAD_FORMAT(fSettings, mKeywordFormat[4]);
    LOAD_FORMAT(fSettings, mKeywordFormat[5]);
    LOAD_FORMAT(fSettings, mKeywordFormat[6]);
    LOAD_FORMAT(fSettings, mKeywordFormat[7]);
    LOAD_FORMAT(fSettings, mFunctionFormat);
    LOAD_FORMAT(fSettings, mNumbersFormat);
    LOAD_FORMAT(fSettings, mPreprocessorFormat);
    LOAD_FORMAT(fSettings, mQuotationFormat);
    fSettings.endGroup();
}


void Highlighter::Language::store( QSettings& fSettings)
{
    fSettings.beginGroup(groupHighlighter);
    STORE_STRF(fSettings, mSingleLineCommentFormat, Highlighter::Language::to_string);
    STORE_STRF(fSettings, mMultiLineCommentFormat, Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[0], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[1], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[2], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[3], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[4], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[5], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[6], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[7], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mFunctionFormat, Highlighter::Language::to_string);
    STORE_STRF(fSettings, mNumbersFormat, Highlighter::Language::to_string);
    STORE_STRF(fSettings, mPreprocessorFormat, Highlighter::Language::to_string);
    STORE_STRF(fSettings, mQuotationFormat, Highlighter::Language::to_string);
    fSettings.endGroup();
}

QString weight_name(int weight)
{
    switch (weight)
    {
        case QFont::Thin:       return "Thin"; break;
        case QFont::ExtraLight: return "ExtraLight"; break;
        case QFont::Light:      return "Light"; break;
        case QFont::Normal:     return "Normal"; break;
        case QFont::Medium:     return "Medium"; break;
        case QFont::DemiBold:   return "DemiBold"; break;
        case QFont::Bold:       return "Bold"; break;
        case QFont::ExtraBold:  return "ExtraBold"; break;
        case QFont::Black:      return "Black"; break;
        default: return "";  break;
    }
}

QString Highlighter::Language::to_string(const QTextCharFormat & format)
{
    return tr("%1,%2,%3").arg(format.foreground().color().name()).arg(weight_name(format.fontWeight())).arg(format.fontItalic());
}

void Highlighter::Language::convert(QTextCharFormat& format, const QString& string)
{
    if (string.size())
    {
        auto string_list = string.split(',');
        if (string_list.count() == 3)
        {
            format.setForeground(QBrush(QColor(string_list[0])));
            if (string_list[1] == weight_name(QFont::Thin)) format.setFontWeight(QFont::Thin);
            else if (string_list[1] == weight_name(QFont::ExtraLight)) format.setFontWeight(QFont::ExtraLight);
            else if (string_list[1] == weight_name(QFont::Light)) format.setFontWeight(QFont::Light);
            else if (string_list[1] == weight_name(QFont::Normal)) format.setFontWeight(QFont::Normal);
            else if (string_list[1] == weight_name(QFont::Medium)) format.setFontWeight(QFont::Medium);
            else if (string_list[1] == weight_name(QFont::DemiBold)) format.setFontWeight(QFont::DemiBold);
            else if (string_list[1] == weight_name(QFont::Bold)) format.setFontWeight(QFont::Bold);
            else if (string_list[1] == weight_name(QFont::ExtraBold)) format.setFontWeight(QFont::ExtraBold);
            else if (string_list[1] == weight_name(QFont::Black)) format.setFontWeight(QFont::Black);
            format.setFontItalic(string_list[2].toInt());
        }
    }
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    if (!mDoc)
    {
        QString file_name {"langs.model.xml"};
#ifdef __linux__
        file_name = QDir::homePath() + "/.config/" + file_name;
#endif
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
    mCurrentLanguage = mDefault;
}

void Highlighter::setExtension(const QString& ext)
{
    if (ext.count() > 0)
    {
        mCurrentLanguage = ext;
    }
    else
    {
        mCurrentLanguage = mDefault;
    }
    auto language = mExtensionToLanguage.find(mCurrentLanguage);
    if (language != mExtensionToLanguage.end())
    {
        mCurrentLanguage = language.value();
        if (mLanguages.count(mCurrentLanguage) == 0)
        {
            load_language(mCurrentLanguage);
        }
    }
}

void Highlighter::setLanguage(const QString &language)
{
    mCurrentLanguage = language;
    load_language(mCurrentLanguage);
    rehighlight();
}

const QString& Highlighter::currentLanguage() const
{
    return mCurrentLanguage;
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
                if (extensions.count())
                {
                    auto list = extensions.split(" ");
                    for (auto& entry : list)
                    {
                        mExtensionToLanguage[entry] = name;
                    }
                }
                if (name.count())
                {
                    mExtensionToLanguage[name] = name;
                    mLanguageNames.push_back(name);
                }
            }
        }
    }
    mLanguageNames.sort();
}

const QStringList& Highlighter::getLanguages()
{
    return mLanguageNames;
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

            int k=0;
            for(QDomNode n = language_node.firstChild(); !n.isNull(); n = n.nextSibling())
            {
                const QString& name = getValue(n.attributes().namedItem("name"), QString(""), true);
                if (name.size())
                {
                    QString keys = getValue(n, QString(""));
                    if (name == "function" || name == "section")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.mFunctionFormat;
                        language.highlightingRules.append(rule);
                    }
                    else if (name == "preprocessor")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.mPreprocessorFormat;
                        language.highlightingRules.append(rule);
                    }
                    else if (name == "numbers")
                    {
                        rule.pattern = QRegularExpression(keys);
                        rule.format = language.mNumbersFormat;
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
                        rule.format = language.mKeywordFormat[k];
                        language.highlightingRules.append(rule);
                        if (k < Language::keyword_formats) ++k;
                    }
                }
            }

            QString commentLine  = getValue(language_node.attributes().namedItem("commentLine"), QString(""), true);
            if (commentLine.size())
            {
                commentLine += "[^\n]*";
                rule.pattern = QRegularExpression(commentLine);
                rule.format = language.mSingleLineCommentFormat;
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
            rule.format = language.mQuotationFormat;
            language.highlightingRules.append(rule);

            mLanguages[language_name] = language;
        }
    }
}

void Highlighter::load_default_language()
{
    HighlightingRule rule;
    Language language;

    rule.pattern = QRegularExpression(QStringLiteral("\\b[0-9\\+\\-\\.]+\\b"));
    rule.format = language.mNumbersFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("#.*"));
    rule.format = language.mSingleLineCommentFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = language.mQuotationFormat;
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:git|config|help|bugreport|init|clone|add|status|diff|commit|notes|restore|reset|rm|mv|branch|checkout|switch|merge|mergetool|log|stash|tag|worktree|fetch|pull|push|remote|submodule|show|log|diff|difftool|range-diff|shortlog|describe|apply|cherry-pick|diff|rebase|revert)\\b"));
    rule.format = language.mKeywordFormat[0];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:D|M|A|R|DD|AU|UD|UA|DU|AA|UU|MM|\\?|\\?\\?)\\b"));
    rule.format = language.mKeywordFormat[1];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("-[\\w]+|--[\\w]+"));
    rule.format = language.mKeywordFormat[2];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("^[\\+].*"));
    rule.format = language.mKeywordFormat[3];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\?"));
    rule.format = language.mKeywordFormat[3];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("^[-].*"));
    rule.format = language.mKeywordFormat[4];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:clean|gc|fsck|reflog|filter-branch|instaweb|archive|bundle|daemon|update-server-info|cat-file|check-ignore|checkout-index|commit-tree|count-objects|diff-index|for-each-ref|hash-object|ls-files|ls-tree|merge-base|read-tree|rev-list|rev-parse|show-ref|symbolic-ref|update-index|update-ref|verify-pack|write-tree)\\b"));
    rule.format = language.mKeywordFormat[5];
    language.highlightingRules.append(rule);

    mLanguageNames.push_back(mDefault);
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
                setFormat(startIndex, commentLength, language.mMultiLineCommentFormat);
                startIndex = text.indexOf(language.commentStartExpression, startIndex + commentLength);
            }
            else
            {
                startIndex = -1;
            }
        }
    }
}

