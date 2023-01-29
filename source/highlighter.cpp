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
#include "highlighterdialog.h"

#include <QtXml/QDomDocument>
#include <QSettings>
#include <QDir>

const QString Highlighter::mDefault {"git"};

namespace
{
    constexpr char groupHighlighter[] = "Highlighter";
}

QMap<QString, Highlighter::Language> Highlighter::mLanguages;
QMap<QString, QString>  Highlighter::mExtensionToLanguage;
QSharedPointer<QDomDocument> Highlighter::mDoc;
QStringList Highlighter::mLanguageNames;

QTextCharFormat Highlighter::Language::mKeywordFormat[Highlighter::keyword_formats];
QTextCharFormat Highlighter::Language::mNumbersFormat;
QTextCharFormat Highlighter::Language::mSingleLineCommentFormat;
QTextCharFormat Highlighter::Language::mMultiLineCommentFormat;
QTextCharFormat Highlighter::Language::mQuotationFormat;
QTextCharFormat Highlighter::Language::mFunctionFormat;
QTextCharFormat Highlighter::Language::mPreprocessorFormat;

QMap<QString, int>  Highlighter::mKeywordMap;


#define LOAD_FORMAT(SETTINGS, ITEM) Highlighter::Language::convert(ITEM, SETTINGS.value(getSettingsName(#ITEM)).toString());

#define ADD_MAP_ENTRY(MAP, X) { MAP[#X] = X; }
#define ADD_FRIENDLY_NAME_MAP_ENTRY(MAP, X) { MAP[getSettingsName(#X)] = X; }
#define SET_FRIENDLY_NAME_MAP_ITEM(MAP, X) { X = MAP[getSettingsName(#X)]; }

void Highlighter::Language::load(QSettings& fSettings)
{
    mSingleLineCommentFormat.setForeground(Qt::darkCyan);
    mMultiLineCommentFormat.setForeground(Qt::darkCyan);

    mKeywordFormat[instre1].setForeground(Qt::darkBlue);
    mKeywordFormat[instre1].setFontWeight(QFont::Medium);

    mKeywordFormat[instre2].setForeground(Qt::darkCyan);
    mKeywordFormat[instre2].setFontWeight(QFont::Medium);

    mKeywordFormat[type1].setForeground(Qt::darkGreen);
    mKeywordFormat[type1].setFontWeight(QFont::Medium);

    mKeywordFormat[type2].setForeground(Qt::darkYellow);
    mKeywordFormat[type2].setFontWeight(QFont::Medium);

    mKeywordFormat[type3].setForeground(Qt::magenta);
    mKeywordFormat[type3].setFontWeight(QFont::Medium);

    mKeywordFormat[type4].setForeground(Qt::darkMagenta);
    mKeywordFormat[type4].setFontWeight(QFont::Medium);

    mKeywordFormat[type5].setForeground(QColor("royalblue"));
    mKeywordFormat[type5].setFontWeight(QFont::Medium);

    mKeywordFormat[type6].setForeground(QColor("seagreen"));
    mKeywordFormat[type6].setFontWeight(QFont::Medium);

    mKeywordFormat[type7].setForeground(Qt::darkGray);
    mKeywordFormat[type7].setFontWeight(QFont::Medium);

    mNumbersFormat.setForeground(Qt::blue);
    mNumbersFormat.setFontWeight(QFont::Medium);

    mQuotationFormat.setForeground(Qt::darkGreen);

    mPreprocessorFormat.setFontWeight(QFont::Bold);
    mPreprocessorFormat.setForeground(Qt::darkGreen);

    mFunctionFormat.setFontItalic(true);
    mFunctionFormat.setForeground(Qt::blue);

    ADD_MAP_ENTRY(mKeywordMap, instre1);
    ADD_MAP_ENTRY(mKeywordMap, instre2);
    ADD_MAP_ENTRY(mKeywordMap, type1);
    ADD_MAP_ENTRY(mKeywordMap, type2);
    ADD_MAP_ENTRY(mKeywordMap, type3);
    ADD_MAP_ENTRY(mKeywordMap, type4);
    ADD_MAP_ENTRY(mKeywordMap, type5);
    ADD_MAP_ENTRY(mKeywordMap, type6);
    ADD_MAP_ENTRY(mKeywordMap, type7);

    fSettings.beginGroup(groupHighlighter);
    LOAD_FORMAT(fSettings, mSingleLineCommentFormat);
    LOAD_FORMAT(fSettings, mMultiLineCommentFormat);
    LOAD_FORMAT(fSettings, mKeywordFormat[instre1]);
    LOAD_FORMAT(fSettings, mKeywordFormat[instre2]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type1]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type2]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type3]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type4]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type5]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type6]);
    LOAD_FORMAT(fSettings, mKeywordFormat[type7]);
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
    STORE_STRF(fSettings, mKeywordFormat[instre1], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[instre2], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type1], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type2], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type3], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type4], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type5], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type6], Highlighter::Language::to_string);
    STORE_STRF(fSettings, mKeywordFormat[type7], Highlighter::Language::to_string);
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
    }
    return "";
}

QString Highlighter::Language::to_string(const QTextCharFormat & format)
{
    return tr("%1,%2,%3").arg(format.foreground().color().name(), weight_name(format.fontWeight())).arg(format.fontItalic());
}

void Highlighter::Language::convert(QTextCharFormat& format, const QString& string)
{
    if (string.size())
    {
        auto string_list = string.split(',');
        if (string_list.count() == 3)
        {
            format.setForeground(QBrush(QColor(string_list[0])));
            if      (string_list[1] == weight_name(QFont::Thin)) format.setFontWeight(QFont::Thin);
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
    auto language = mExtensionToLanguage.find(ext);
    if (language != mExtensionToLanguage.end())
    {
        mCurrentLanguage = language.value();
        if (mLanguages.count(mCurrentLanguage) == 0)
        {
            load_language(mCurrentLanguage);
        }
    }
    else
    {
        mCurrentLanguage = mDefault;
        if (mLanguages.count(mCurrentLanguage) == 0)
        {
            load_language(mCurrentLanguage);
        }
    }
    Q_EMIT updateExtension(mCurrentLanguage);
}

void Highlighter::setLanguage(const QString &language)
{
    mCurrentLanguage = language;
    Q_EMIT updateExtension(language);
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
                    const auto list = extensions.split(" ");
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

TextCharFormatMap  Highlighter::Language::getHighlightFormats()
{
    QMap<QString, QTextCharFormat> list;
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mNumbersFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mSingleLineCommentFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mMultiLineCommentFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mQuotationFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mFunctionFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mPreprocessorFormat);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[instre1]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[instre2]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type1]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type2]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type3]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type4]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type5]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type6]);
    ADD_FRIENDLY_NAME_MAP_ENTRY(list, mKeywordFormat[type7]);
    return list;
}

void Highlighter::Language::invokeHighlighterDlg()
{
    HighlighterDialog dlg(nullptr, getHighlightFormats());
    if (dlg.exec() == QDialog::Accepted)
    {
        const auto list = dlg.getTextFormatMap();
        SET_FRIENDLY_NAME_MAP_ITEM(list, mNumbersFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mSingleLineCommentFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mMultiLineCommentFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mQuotationFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mFunctionFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mPreprocessorFormat);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[instre1]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[instre2]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type1]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type2]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type3]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type4]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type5]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type6]);
        SET_FRIENDLY_NAME_MAP_ITEM(list, mKeywordFormat[type7]);
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
                        auto key_map_item = mKeywordMap.find(name);
                        if (key_map_item != mKeywordMap.end())
                        {
                            k = key_map_item.value();
                        }
                        for (auto& character : keys)
                        {
                            if (character == ' ') character = '|';
                        }
                        QString pattern = tr("\\b(?:%1)\\b").arg(keys);
                        rule.pattern = QRegularExpression(pattern);
                        rule.format = language.mKeywordFormat[k];
                        language.highlightingRules.append(rule);
                        if (k < keyword_formats) ++k;
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

//            rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
            rule.pattern = QRegularExpression(QStringLiteral("\"[\\w\\d\\s\\(\\)\\*/#\\{\\}\\[\\];|\\-\\+\\.\\\\^\"]*\""));
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
    rule.format = language.mKeywordFormat[instre1];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:D|M|A|R|DD|AU|UD|UA|DU|AA|UU|MM|\\?|\\?\\?)\\b"));
    rule.format = language.mKeywordFormat[instre2];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("-[\\w]+|--[\\w]+"));
    rule.format = language.mKeywordFormat[type1];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("^[\\+].*"));
    rule.format = language.mKeywordFormat[type2];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\?"));
    rule.format = language.mKeywordFormat[type2];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("^[-].*"));
    rule.format = language.mKeywordFormat[type3];
    language.highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\b(?:clean|gc|fsck|reflog|filter-branch|instaweb|archive|bundle|daemon|update-server-info|cat-file|check-ignore|checkout-index|commit-tree|count-objects|diff-index|for-each-ref|hash-object|ls-files|ls-tree|merge-base|read-tree|rev-list|rev-parse|show-ref|symbolic-ref|update-index|update-ref|verify-pack|write-tree)\\b"));
    rule.format = language.mKeywordFormat[type4];
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

