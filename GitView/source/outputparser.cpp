#include "outputparser.h"
#include "ui_outputparser.h"
#include "helper.h"

#include <QScrollBar>



ParseMessagePattern::ParseMessagePattern()
{

}

void ParseMessagePattern::set_pattern(const QString &pattern)
{
    m_regex.setPattern(pattern);
}

void ParseMessagePattern::set_text_line(const QString &text)
{
    m_text_line = text;
}

void ParseMessagePattern::set_filename_position(int pos)
{
    m_filename_position = pos;
}

void ParseMessagePattern::set_line_position(int pos)
{
    m_line_position = pos;
}

void ParseMessagePattern::set_message_text_position(int pos)
{
    m_message_text_position = pos;
}

int ParseMessagePattern::get_filename_position() const
{
    return m_filename_position;
}

int ParseMessagePattern::get_line_position() const
{
    return m_line_position;
}

int ParseMessagePattern::get_message_text_position() const
{
    return m_message_text_position;
}

QString ParseMessagePattern::get_pattern() const
{
    return m_regex.pattern();
}

QString ParseMessagePattern::get_filename() const
{
    return get_regex_match_at(m_filename_position);
}

QString ParseMessagePattern::get_line() const
{
    return get_regex_match_at(m_line_position);
}

QString ParseMessagePattern::get_message_text() const
{
    return get_regex_match_at(m_message_text_position);
}

bool ParseMessagePattern::parse()
{
    return parse(m_text_line);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

bool ParseMessagePattern::parse(const QString & text_line)
{
    m_match = m_regex.match(text_line);
    return m_match.hasMatch();
}

QString ParseMessagePattern::get_regex_match_at(int pos) const
{
    if (m_match.hasMatch() && m_match.capturedLength(pos))
    {
        return m_match.captured(pos);
    }
    return {};
}

#else

bool ParseMessagePattern::parse(const QString & text_line)
{
    return m_regex.indexIn(text_line) != -1;
}

QString ParseMessagePattern::get_regex_match_at(int pos) const
{
    if (m_regex.isValid() && pos <= m_regex.captureCount())
    {
        return m_regex.capturedTexts().at(pos);
    }
    return {};
}

#endif

bool ParseMessagePattern::operator == (const ParseMessagePattern &pmp) const
{
    return    pmp.get_pattern()               == get_pattern()
           && pmp.get_filename_position()     == get_filename_position()
           && pmp.get_line_position()         == get_line_position()
           && pmp.get_message_text_position() == get_message_text_position();
}


OutputParser::OutputParser(
    QList<QSharedPointer<ParseMessagePattern> > & msg_pattern,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QList<QPair<QRegularExpression, QString> > & filter_pattern,
#else
    QList<QPair<QRegEx, QString> > & filter_pattern,
#endif
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OutputParser)
    , mMessagePatterns(msg_pattern)
    , mFilterPatterns(filter_pattern)
{
    ui->setupUi(this);
    ui->tablePatternForReplacement->setColumnCount(2);
    ui->tablePatternForReplacement->setHorizontalHeaderLabels({tr("Search expression"), tr("Replace expression") });
    ui->tablePatternForReplacement->setRowCount(mFilterPatterns.size());
    int index = 0;
    for (const auto& filter : mFilterPatterns)
    {
        ui->tablePatternForReplacement->setRowHeight(index, 8);
        ui->tablePatternForReplacement->setItem(index, column::search, new QTableWidgetItem(filter.first.pattern()));
        ui->tablePatternForReplacement->setItem(index, column::replace, new QTableWidgetItem(filter.second));
        index++;
    }

    ui->tablePatternForMessage->setColumnCount(column::count);
    ui->tablePatternForMessage->setHorizontalHeaderLabels({tr("Parse expression"), ui->labelFilenamePosition->text(), ui->labelLinePosition->text(), ui->labelMsgPosition->text() });
    ui->tablePatternForMessage->setRowCount(mMessagePatterns.size());
    index = 0;
    for (const auto& message : mMessagePatterns)
    {
        ui->tablePatternForMessage->setRowHeight(index, 8);
        ui->tablePatternForMessage->setItem(index, column::search, new QTableWidgetItem(message->get_pattern()));
        ui->tablePatternForMessage->setItem(index, column::file_name, new QTableWidgetItem(QString::number(message->get_filename_position())));
        ui->tablePatternForMessage->setItem(index, column::line, new QTableWidgetItem(QString::number(message->get_line_position())));
        ui->tablePatternForMessage->setItem(index, column::message, new QTableWidgetItem(QString::number(message->get_message_text_position())));
        index++;
    }
}

OutputParser::~OutputParser()
{
    delete ui;
}

void OutputParser::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    int    vertical_width = ui->tablePatternForReplacement->verticalScrollBar()->sizeHint().width();
    int    width          = ui->tablePatternForReplacement->rect().width() - vertical_width;
    double half           = 0.5;

    for (int fColumn = 0; fColumn<ui->tablePatternForReplacement->columnCount(); ++fColumn)
    {
        ui->tablePatternForReplacement->setColumnWidth(fColumn, INT(half * width));
    }

    vertical_width   = ui->tablePatternForMessage->verticalScrollBar()->sizeHint().width();
    width            = ui->tablePatternForMessage->rect().width() - vertical_width;
    double small_col = (1.0 - half) / 3;

    for (int fColumn = 0; fColumn<ui->tablePatternForMessage->columnCount(); ++fColumn)
    {
        ui->tablePatternForMessage->setColumnWidth(fColumn, INT(((fColumn == 0) ? half : small_col) * width));
    }
}

void OutputParser::on_btnAddPatternForReplacement_clicked()
{
    int count = ui->tablePatternForReplacement->rowCount();
    ui->tablePatternForReplacement->setRowCount(count + 1);
    ui->tablePatternForReplacement->setItem(count, column::search, new QTableWidgetItem(ui->edtSearchPattern->text()));
    ui->tablePatternForReplacement->setItem(count, column::replace, new QTableWidgetItem(ui->edtReplacePattern->text()));
    ui->tablePatternForReplacement->setRowHeight(count, 8);
}

void OutputParser::on_btnModifyPatternForReplacement_clicked()
{
    int row = ui->tablePatternForReplacement->currentRow();
    ui->tablePatternForReplacement->item(row, column::search)->setText(ui->edtSearchPattern->text());
    ui->tablePatternForReplacement->item(row, column::replace)->setText(ui->edtReplacePattern->text());
}

void OutputParser::on_btnRemovePatternForReplacement_clicked()
{
    ui->tablePatternForReplacement->removeRow(ui->tablePatternForReplacement->currentRow());
    ui->btnModifyPatternForReplacement->setEnabled(false);
    ui->btnRemovePatternForReplacement->setEnabled(false);
}

void OutputParser::on_tablePatternForReplacement_cellClicked(int , int )
{
    ui->btnModifyPatternForReplacement->setEnabled(true);
    ui->btnRemovePatternForReplacement->setEnabled(true);
}


void OutputParser::on_tablePatternForReplacement_cellDoubleClicked(int row, int )
{
    ui->btnModifyPatternForReplacement->setEnabled(true);
    ui->btnRemovePatternForReplacement->setEnabled(true);
    ui->edtSearchPattern->setText(ui->tablePatternForReplacement->item(row, column::search)->text());
    ui->edtReplacePattern->setText(ui->tablePatternForReplacement->item(row, column::replace)->text());
}


void OutputParser::on_btnAddPatternForMessage_clicked()
{
    int count = ui->tablePatternForMessage->rowCount();
    ui->tablePatternForMessage->setRowCount(count + 1);
    ui->tablePatternForMessage->setItem(count, column::search, new QTableWidgetItem(ui->edtPattern->text()));
    ui->tablePatternForMessage->setItem(count, column::file_name, new QTableWidgetItem(ui->spinFilenamePosition->text()));
    ui->tablePatternForMessage->setItem(count, column::line, new QTableWidgetItem(ui->spinLinePosition->text()));
    ui->tablePatternForMessage->setItem(count, column::message, new QTableWidgetItem(ui->spinMessageTextPositon->text()));
    ui->tablePatternForMessage->setRowHeight(count, 8);
}

void OutputParser::on_btnModifyPatternForMessage_clicked()
{
    int row = ui->tablePatternForMessage->currentRow();
    ui->tablePatternForMessage->item(row, column::search)->setText(ui->edtPattern->text());
    ui->tablePatternForMessage->item(row, column::file_name)->setText(ui->spinFilenamePosition->text());
    ui->tablePatternForMessage->item(row, column::line)->setText(ui->spinLinePosition->text());
    ui->tablePatternForMessage->item(row, column::message)->setText(ui->spinMessageTextPositon->text());
}

void OutputParser::on_btnRemovePatternForMessage_clicked()
{
    ui->tablePatternForMessage->removeRow(ui->tablePatternForMessage->currentRow());
    ui->btnModifyPatternForMessage->setEnabled(false);
    ui->btnRemovePatternForMessage->setEnabled(false);
}

void OutputParser::on_tablePatternForMessage_cellClicked(int , int )
{
    ui->btnModifyPatternForMessage->setEnabled(true);
    ui->btnRemovePatternForMessage->setEnabled(true);
}


void OutputParser::on_tablePatternForMessage_cellDoubleClicked(int row, int )
{
    ui->btnModifyPatternForMessage->setEnabled(true);
    ui->btnRemovePatternForMessage->setEnabled(true);
    ui->edtPattern->setText(ui->tablePatternForMessage->item(row, column::search)->text());
    ui->spinFilenamePosition->setValue(ui->tablePatternForMessage->item(row, column::file_name)->text().toInt());
    ui->spinLinePosition->setValue(ui->tablePatternForMessage->item(row, column::line)->text().toInt());
    ui->spinMessageTextPositon->setValue(ui->tablePatternForMessage->item(row, column::message)->text().toInt());
}


void OutputParser::on_OutputParser_accepted()
{
    mFilterPatterns.clear();
    for (int index = 0; index < ui->tablePatternForReplacement->rowCount(); ++index)
    {
        QPair<QRegularExpression, QString> value;
        value.first.setPattern(ui->tablePatternForReplacement->item(index, column::search)->text());
        value.second = ui->tablePatternForReplacement->item(index, column::replace)->text();
        mFilterPatterns.append(value);
    }

    mMessagePatterns.clear();
    for (int index = 0; index < ui->tablePatternForMessage->rowCount(); ++index)
    {
        QSharedPointer<ParseMessagePattern> value(new ParseMessagePattern);
        value->set_pattern(ui->tablePatternForMessage->item(index, column::search)->text());
        value->set_filename_position(ui->tablePatternForMessage->item(index, column::file_name)->text().toInt());
        value->set_line_position(ui->tablePatternForMessage->item(index, column::line)->text().toInt());
        value->set_message_text_position(ui->tablePatternForMessage->item(index, column::message)->text().toInt());
        mMessagePatterns.append(value);
    }
}

void OutputParser::on_edtPattern_textChanged(const QString &pattern)
{
    m_parser.set_pattern(pattern);
    parseCurrentRecognitionPattern();
}


void OutputParser::on_edtTestMessageLine_textChanged(const QString &sample)
{
    m_parser.set_text_line(sample);
    parseCurrentRecognitionPattern();
}


void OutputParser::on_spinFilenamePosition_valueChanged(int pos)
{
    m_parser.set_filename_position(pos);
    parseCurrentRecognitionPattern();
}


void OutputParser::on_spinLinePosition_valueChanged(int pos)
{
    m_parser.set_line_position(pos);
    parseCurrentRecognitionPattern();
}


void OutputParser::on_spinMessageTextPositon_valueChanged(int pos)
{
    m_parser.set_message_text_position(pos);
    parseCurrentRecognitionPattern();
}

void OutputParser::parseCurrentRecognitionPattern()
{
    if (m_parser.parse())
    {
        ui->edtFileName->setText(m_parser.get_filename());
        ui->edtLineNumber->setText(m_parser.get_line());
        ui->edtMessageOutput->setText(m_parser.get_message_text());
    }
    else
    {
        ui->edtFileName->setText("");
        ui->edtLineNumber->setText("");
        ui->edtMessageOutput->setText("");
    }
}

