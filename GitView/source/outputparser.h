#ifndef OUTPUTPARSER_H
#define OUTPUTPARSER_H

#include <QDialog>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

class ParseMessagePattern
{
public:
    ParseMessagePattern();

    void set_pattern(const QString& pattern);
    void set_text_line(const QString& text);

    void set_filename_position(int pos);
    void set_line_position(int pos);
    void set_message_text_position(int pos);

    int  get_filename_position() const;
    int  get_line_position() const;
    int  get_message_text_position() const;
    QString get_pattern() const;

    QString get_filename() const;
    QString get_line() const;
    QString get_message_text() const;

    bool parse();
    bool parse(const QString& text_line);
    bool operator == (const ParseMessagePattern& pmp) const;

private:
    QString get_regex_match_at(int pos) const;
    QString m_text_line;

    int m_filename_position = 1;
    int m_line_position = 2;
    int m_message_text_position = 3;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression m_regex;
    QRegularExpressionMatch m_match;
#else
    QRegExp m_regex;
#endif
};



namespace Ui {
class OutputParser;
}

class OutputParser : public QDialog
{
    Q_OBJECT
    struct column { enum e { search=0, replace=1, file_name = 1, line = 2, message = 3, count}; };
public:
    explicit OutputParser(QList<QSharedPointer<ParseMessagePattern>>&msg_pattern,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                          QList<QPair<QRegularExpression, QString>>& filter_pattern,
#else
                          QList<QPair<QRegExp, QString>>& filter_pattern,
#endif
                          QWidget *parent = nullptr);
    ~OutputParser();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void parseCurrentRecognitionPattern();

private slots:
    void on_btnAddPatternForReplacement_clicked();
    void on_btnModifyPatternForReplacement_clicked();
    void on_btnRemovePatternForReplacement_clicked();
    void on_btnAddPatternForMessage_clicked();
    void on_btnModifyPatternForMessage_clicked();
    void on_btnRemovePatternForMessage_clicked();
    void on_edtPattern_textChanged(const QString &pattern);
    void on_edtTestMessageLine_textChanged(const QString &sample);
    void on_spinFilenamePosition_valueChanged(int pos);
    void on_spinLinePosition_valueChanged(int pos);
    void on_spinMessageTextPositon_valueChanged(int pos);

    void on_tablePatternForReplacement_cellClicked(int, int);
    void on_tablePatternForReplacement_cellDoubleClicked(int row, int);
    void on_tablePatternForMessage_cellClicked(int row, int column);
    void on_tablePatternForMessage_cellDoubleClicked(int row, int column);

    void on_OutputParser_accepted();


private:
    ParseMessagePattern m_parser;
    Ui::OutputParser *ui;

    QList<QSharedPointer<ParseMessagePattern>>& mMessagePatterns;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QList<QPair<QRegularExpression, QString>>& mFilterPatterns;
#else
    QList<QPair<QRegExp, QString>>& mFilterPatterns;
#endif
};

#endif // OUTPUTPARSER_H
