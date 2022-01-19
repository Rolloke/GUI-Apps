#include "code_browser.h"
#include "logger.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QMenu>
#include <QTextCursor>
#include <QApplication>

// NOTE: source for this solution, but a little bit modified
// https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit
// https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html

code_browser::code_browser(QWidget *parent): QTextBrowser(parent)
  , m_line_number_area(new LineNumberArea(this))
  , m_show_line_numbers(false)
  , m_actions(nullptr)
  , m_dark_mode(false)
  , m_bytes_per_part(4)
  , m_parts_per_line(8)
{
    connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(vertical_scroll_value(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(blockCount());
    highlightCurrentLine();
}

int code_browser::lineNumberAreaWidth()
{
    if (m_show_line_numbers)
    {
        int digits = 1;
        int max = qMax(1, blockCount());
        while (max >= 10)
        {
            max /= 10;
            ++digits;
        }

        int space = 3 + fontMetrics().charWidth("9", 0) * digits;

        return space;
    }
    return 0;
}

void code_browser::set_show_line_numbers(bool show)
{
    m_show_line_numbers = show;
    updateLineNumberAreaWidth(blockCount());
    m_line_number_area->setVisible(show);
}

void code_browser::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void code_browser::vertical_scroll_value(int value)
{
    if (m_show_line_numbers)
    {
        Q_EMIT updateRequest(contentsRect(), value);
    }
}

void code_browser::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
    {
        m_line_number_area->scroll(0, dy);
    }
    else
    {
        m_line_number_area->update(0, rect.y(), m_line_number_area->width(), rect.height());
    }
    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(blockCount());
    }
}

void code_browser::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e);
    QRect cr = contentsRect();
    m_line_number_area->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void code_browser::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_actions && git::Cmd::mContextMenuTextView.size())
    {
        QMenu *menu = createStandardContextMenu();
        m_actions->fillContextMenue(*menu, git::Cmd::mContextMenuTextView);
        menu->exec(event->globalPos());
        delete menu;
    }
    else
    {
        QTextBrowser::contextMenuEvent(event);
    }
}

void code_browser::keyPressEvent(QKeyEvent *e)
{
    if (m_binary_content.size())
    {
        switch (e->key())
        {
        case Qt::Key_Left:   case Qt::Key_Right:
        case Qt::Key_Up:     case Qt::Key_Down:
        case Qt::Key_PageUp: case Qt::Key_PageDown:
        case Qt::Key_Home:   case Qt::Key_End:
            QTextBrowser::keyPressEvent(e);
            break;
        }
    }
    else
    {
        QTextBrowser::keyPressEvent(e);
    }
}

void code_browser::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(m_dark_mode ? Qt::darkYellow : Qt::yellow).lighter(m_dark_mode ? 0 : 160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void code_browser::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    const QPalette p = QApplication::palette(this);
    const auto text_color = p.color(QPalette::Normal, QPalette::ButtonText);
    const auto button_color = p.color(QPalette::Normal, QPalette::Button);
    QPainter painter(m_line_number_area);

    painter.fillRect(event->rect(), button_color);
    int top = 0;
    QTextBlock block = firstVisibleBlock(top);
    int blockNumber = block.blockNumber();

    QRectF block_rect = blockBoundingRect(block);
    int bottom = top + qRound(block_rect.height());
    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(text_color);
            painter.drawText(0, top, m_line_number_area->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        block_rect = blockBoundingRect(block);
        bottom = top + qRound(block_rect.height());
        ++blockNumber;
    }
}

int code_browser::blockCount() const
{
    return document()->blockCount();
}

QTextBlock code_browser::firstVisibleBlock(int& diff)
{
    QPointF content_offset = contentOffset();
    for (QTextBlock block = document()->begin(); block.isValid(); block = block.next())
    {
        if (block.isVisible())
        {
            QRectF block_rect = blockBoundingRect(block);
            if (block_rect.top() >= content_offset.y())
            {
                diff = block_rect.top() - content_offset.y();
                return block;
            }
        }
    }
    diff = -1;
    return document()->begin();
}

//! Returns the bounding rectangle of the text block in the block's own coordinates.
QRectF code_browser::blockBoundingRect(const QTextBlock &block) const
{
    QAbstractTextDocumentLayout *layout = document()->documentLayout();
    return layout->blockBoundingRect(block);
}

QPointF code_browser::contentOffset() const
{
    return QPointF(horizontalScrollBar()->value(), verticalScrollBar()->value());
}

int code_browser::current_line() const
{
    const auto selections = extraSelections();
    if (selections.size())
    {
        return selections.first().cursor.blockNumber()+1;
    }
    return 0;
}

void code_browser::go_to_line(int line)
{
    QTextBlock text_block = document()->findBlockByLineNumber(line-1);
    QTextCursor text_cursor(text_block);
    text_cursor.select(QTextCursor::LineUnderCursor);
    setTextCursor(text_cursor);
}

void code_browser::set_actions(ActionList *list)
{
    m_actions = list;
}

void code_browser::set_dark_mode(bool dark)
{
    m_dark_mode = dark;
}

void code_browser::set_binary_data(const QByteArray& array)
{
    QString binary_coded_line;
    QString ascii_coded_line;
    int bytes = 0;
    int parts = 0;
    for (const auto& data : array)
    {
        const unsigned char byte = data;
        binary_coded_line.append(QString::asprintf("%02X", byte));
        ascii_coded_line.append((byte >= 32 && byte <= 127) ? byte : '.');
        if (++bytes == m_bytes_per_part)
        {
            binary_coded_line.append(" ");
            bytes = 0;
            ++parts;
        }
        if (parts == m_parts_per_line)
        {
            insertPlainText(binary_coded_line + "\t" + ascii_coded_line + "\n");
            binary_coded_line.clear();
            ascii_coded_line.clear();
            parts = 0;
        }
    }
    insertPlainText(binary_coded_line + "\t" + ascii_coded_line);
    m_binary_content = array;
}

const QByteArray& code_browser::get_binary_data() const
{
    return m_binary_content;
}

void code_browser::clear_binary_content()
{
    m_binary_content.clear();
}

bool code_browser::is_binary(QFile& file)
{
    bool binary = false;
    unsigned char buffer[64];
    auto read_bytes = file.peek(reinterpret_cast<char*>(&buffer[0]), sizeof(buffer));
    union unicode_id
    {
        std::uint16_t utf16;
        std::uint32_t utf32;
    };

    for (qint64 i=0; i<read_bytes; ++i)
    {
        if (buffer[i] == 0 || !isascii(buffer[i]))
        {
            if (i < 6)
            {
                unicode_id *unicode = reinterpret_cast<unicode_id*>(&buffer[0]);
                if (unicode->utf32 == 0xfffe0000 || unicode->utf32 == 0x0000fffe)
                {
                    break;  // UTF-32-LE | UTF-32-BE
                }
                else if (unicode->utf16 == 0xfffe || unicode->utf16 == 0xfeff)
                {
                    break;  // UTF-16-LE | UTF-16-BE
                }
                else if (buffer[0] == 0x2B && buffer[1] == 0x2F && buffer[2] == 0x76 && buffer[3] == 0x38 && buffer[4] == 0x2D )
                {
                    break;  // UTF-7 	endianless
                }
                else if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
                {
                    break;  // UTF-8 	endianless
                }
                else
                {
                    binary = true;
                }
            }
            else
            {
                binary = true;
            }
            break;
        }
    }
    return binary;
}

