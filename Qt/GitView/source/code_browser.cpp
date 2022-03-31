#include "code_browser.h"
#include "logger.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QMenu>
#include <QTextCursor>
#include <QApplication>
#include <QWebEnginePage>


// NOTE: source for this solution, but a little bit modified
// https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit
// https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html

code_browser::code_browser(QWidget *parent): QTextBrowser(parent)
  , m_line_number_area(new LineNumberArea(this))
  , m_show_line_numbers(false)
  , m_actions(nullptr)
  , m_dark_mode(false)
  , m_web_page(nullptr)
{
    connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(vertical_scroll_value(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(textChanged()), this, SLOT(own_text_changed()));

    updateLineNumberAreaWidth(blockCount());
    highlightCurrentLine();
}

code_browser::~code_browser()
{
    m_web_page = nullptr;
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

void code_browser::call_updateExtension(const QString& ext)
{
    Q_EMIT updateExtension(ext);
}

void code_browser::own_text_changed()
{
    QString current_language = mHighlighter->currentLanguage();
    if (current_language == "html")
    {
        if (m_web_page)
        {
            QString text = toPlainText();
            m_web_page->setHtml(text);
            Q_EMIT show_web_view(text.size() ? true : false);
        }
    }
    else if (current_language == "markdown")
    {
        QString text = toPlainText();
        Q_EMIT textChanged(text);
        Q_EMIT show_web_view(text.size() ? true : false);
    }
    else
    {
        Q_EMIT show_web_view(false);
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
     QTextBrowser::keyPressEvent(e);
}

void code_browser::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    selection.cursor = textCursor();

    if (!isReadOnly())
    {
        QColor lineColor = QColor(m_dark_mode ? Qt::darkYellow : Qt::yellow).lighter(m_dark_mode ? 0 : 160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
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

void code_browser::set_page(QWebEnginePage*page)
{
    m_web_page = page;
}

void code_browser::reset()
{
    if (mHighlighter)
    {
        disconnect(mHighlighter.data(), SIGNAL(updateExtension(QString)), this, SLOT(call_updateExtension(QString)));
    }
    mHighlighter.reset(new Highlighter(document()));
    connect(mHighlighter.data(), SIGNAL(updateExtension(QString)), this, SLOT(call_updateExtension(QString)));
}

const  QString& code_browser::currentLanguage() const
{
    return mHighlighter->currentLanguage();
}
void code_browser::setExtension(const QString &ext)
{
    mHighlighter->setExtension(ext);
}

void code_browser::setLanguage(const QString& language)
{
    mHighlighter->setLanguage(language);
}



