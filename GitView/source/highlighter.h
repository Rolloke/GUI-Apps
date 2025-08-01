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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <string>
#include <map>

class QTextDocument;
class QDomDocument;
class QSettings;

typedef QMap<QString, QTextCharFormat> TextCharFormatMap;

QString weight_name(int weight);

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    enum keys
    {
        instre1,
        instre2,
        type1,
        type2,
        type3,
        type4,
        type5,
        type6,
        type7,
        startsection,
        stopsection,
        keyword_formats,
        numbers = keyword_formats,
        single_line_comment,
        multi_line_comment,
        quotation,
        function,
        preprocessor,
        all_formats
    };

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
        keys            key;
    };

    struct Language
    {
        static void load(QSettings&);
        static void store(QSettings&);
        static QString to_string(const QTextCharFormat&);
        static void convert(QTextCharFormat&, const QString&);
        static TextCharFormatMap getHighlightFormats();
        static void invokeHighlighterDlg();
        QString get_pattern(const QTextCharFormat& format) const;
        const QRegularExpression &get_regex(const QTextCharFormat& format) const;
        const QRegularExpression &get_regex(const keys key) const;

        QVector<HighlightingRule> highlightingRules;

        QRegularExpression commentStartExpression;
        QRegularExpression commentEndExpression;

        static QTextCharFormat mKeywordFormat[keyword_formats];
        static QTextCharFormat mNumbersFormat;
        static QTextCharFormat mSingleLineCommentFormat;
        static QTextCharFormat mMultiLineCommentFormat;
        static QTextCharFormat mQuotationFormat;
        static QTextCharFormat mFunctionFormat;
        static QTextCharFormat mPreprocessorFormat;
        static QColor          mSelectedLineBackground;
    };

    Highlighter(QTextDocument *parent = 0);

    void setExtension(const QString& ext);
    void setLanguage(const QString& language);
    const QString& currentLanguage() const;
    bool hasExtension(const QString &ext) const;
    static const QStringList& getLanguages();
    QString get_current_language_pattern(const QTextCharFormat& format) const;
    static const char *get_regex(keys key);
    const Language& get_language() const ;


protected:
    void highlightBlock(const QString &text) override;

private:
    void load_language_list();
    void load_language(QString fLanguage);
    void load_default_language();

Q_SIGNALS:
    void updateExtension(const QString&);


private:
    static QMap<QString, Language> mLanguages;
    static QMap<QString, QString>  mExtensionToLanguage;
    static QSharedPointer<QDomDocument> mDoc;
    static QStringList mLanguageNames;

    QString mCurrentLanguage;
    static const QString mDefault;
    static QMap<QString, int>  mKeywordMap;
};

#endif // HIGHLIGHTER_H

