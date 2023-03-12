#include "code_browser.h"
#include "logger.h"
#include "helper.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QMenu>
#include <QTextCursor>
#include <QApplication>
#include <QToolTip>

#ifdef WEB_ENGINE
#include <QWebEngineView>
#endif


code_browser::code_browser(QWidget *parent): QTextBrowser(parent)
  , m_line_number_area(new LineNumberArea(this))
  , m_show_line_numbers(false)
  , m_blame_characters(0)
  , m_actions(nullptr)
  , m_dark_mode(false)
{
    connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(vertical_scroll_value(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    updateLineNumberAreaWidth(blockCount());
    highlightCurrentLine();

    m_line_number_area->setToolTip("init");
    connect(this, SIGNAL(textChanged()), this, SLOT(own_text_changed()));
#ifdef WEB_ENGINE
    m_web_page = nullptr;
#endif
}

code_browser::~code_browser()
{
    Q_EMIT show_web_view(false);
#ifdef WEB_ENGINE
    m_web_page = nullptr;
#endif
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

        digits += m_blame_characters;

        int space = 3 + fontMetrics().charWidth("9", 0) * digits;

        return space + 1;
    }
    return 0;
}

void code_browser::set_show_line_numbers(bool show)
{
    m_show_line_numbers = show;
    updateLineNumberAreaWidth(blockCount());
    m_line_number_area->setVisible(show);
}

void code_browser::change_visibility(bool visible)
{
#ifdef WEB_ENGINE
    if (visible)
    {
        own_text_changed();
    }
    else
    {
        QString current_language = mHighlighter->currentLanguage();
        if (current_language == "html" || current_language == "markdown")
        {
            Q_EMIT show_web_view(false);
        }
    }
#endif
}

code_browser* code_browser::clone(bool all_parameter, bool with_text)
{
    auto *cloned =  new code_browser(parentWidget());
    cloned->reset();
    cloned->m_show_line_numbers = m_show_line_numbers;
    cloned->setFont(font());
    if (with_text)
    {
        cloned->setText(toPlainText());
        cloned->mHighlighter->setLanguage(mHighlighter->currentLanguage());
    }
    if (all_parameter)
    {
        cloned->m_actions  = m_actions;
        cloned->m_web_page = m_web_page;
        cloned->setWhatsThis(whatsThis());
    }
    return cloned;
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
        m_actions->getAction(git::Cmd::CloneTextBrowser)->setEnabled(!isReadOnly());
        menu->exec(event->globalPos() + menu_offset);
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

bool code_browser::event(QEvent *event)
{
    if (event->type() == QEvent::HoverMove)
    {
        TRACE(Logger::trace, "HoverMove"); // **
    }
    else if (event->type() == QEvent::HoverEnter)
    {
        TRACE(Logger::trace, "HoverEnter");
    }
    else if (event->type() == QEvent::HoverLeave)
    {
        TRACE(Logger::trace, "HoverLeave");
    }
    else // event 69
    {
        TRACE(Logger::trace, "Event %d", event->type());
    }
    return QTextBrowser::event(event);
}

void code_browser::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    selection.cursor = textCursor();

    if (!isReadOnly())
    {
        // QColor lineColor = QGuiApplication::palette().color(QPalette::Highlight);
        // QPalette::Highlight
        // QPalette::HighlightedText

        QColor lineColor = QColor(m_dark_mode ? Qt::darkYellow : Qt::yellow).lighter(m_dark_mode ? 0 : 160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);

    Q_EMIT line_changed(selection.cursor.blockNumber()+1);
    Q_EMIT column_changed(selection.cursor.columnNumber());
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

int code_browser::current_line(QString *text) const
{
    const auto selections = extraSelections();
    if (selections.size())
    {
        if (text)
        {
            *text = selections.first().cursor.block().text();
        }
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

void code_browser::reset()
{
    if (mHighlighter)
    {
        disconnect(mHighlighter.data(), SIGNAL(updateExtension(QString)), this, SLOT(call_updateExtension(QString)));
    }
    mHighlighter.reset(new Highlighter(document()));
    connect(mHighlighter.data(), SIGNAL(updateExtension(QString)), this, SLOT(call_updateExtension(QString)));
    reset_blame();
}

const  QString& code_browser::currentLanguage() const
{
    return mHighlighter->currentLanguage();
}
void code_browser::setExtension(const QString &ext)
{
    mHighlighter->setExtension(ext);
}

bool code_browser::hasExtension(const QString& ext)
{
    return mHighlighter->hasExtension(ext);
}

void code_browser::setLanguage(const QString& language)
{
    mHighlighter->setLanguage(language);
}

void code_browser::reset_blame()
{
    m_blame_map.clear();
    m_blame_start_line.clear();
    m_blame_characters = 0;
}

void code_browser::parse_blame(const QString &blame)
{
    s_blame*        old_blame           { nullptr };
    int             current_line_number { 0 };
    ColorSelector   color_selector;

    setText("");
    reset_blame();
    color_selector.unapply_color(Qt::yellow);

    QStringList lines = blame.split("\n");
    for (QString &line : lines)
    {
        QStringList parts = line.split("\t");
        if (parts.size() == 4)
        {
            int pos = parts[3].indexOf(')');
            if (pos != -1)
            {
                current_line_number = parts[3].leftRef(pos).toInt();
                append(parts[3].mid(pos+1));
            }
            if (!m_blame_map.contains(parts[0]))
            {
                auto& blame_entry = m_blame_map[parts[0]];
                old_blame = &blame_entry;
                blame_entry.text.append(parts[0]);
                blame_entry.text.append(parts[1].mid(1));
                QStringList date_time = parts[2].split(" ");
                blame_entry.text.append(date_time[0]);
                blame_entry.text.append(date_time[1]);
                const auto& blame_entry_text = blame_entry.text;
                for (const auto& text : blame_entry_text)
                {
                    const auto length = text.size();
                    if (length > m_blame_characters)
                    {
                        m_blame_characters = length;
                    }
                }
                blame_entry.color  = color_selector.get_color_and_increment();
                auto& line_entry = m_blame_start_line[current_line_number];
                line_entry.blame_data = &blame_entry;
            }
            else
            {
                auto& blame_entry = m_blame_map[parts[0]];
                if (&blame_entry != old_blame)
                {
                    auto& line_entry = m_blame_start_line[current_line_number];
                    line_entry.blame_data = &blame_entry;
                }
                old_blame = &blame_entry;
            }
        }
    }
    if (m_blame_start_line.size() > 1)
    {
        ++m_blame_characters;
    }
    else
    {
        reset_blame();
    }
    go_to_line(1);
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    codeEditor->lineNumberAreaPaintEvent(event, m_blame_position);
}
// NOTE: source for this solution, but a little bit modified
// https://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit
// https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
void code_browser::lineNumberAreaPaintEvent(QPaintEvent *event, pos_to_blame& blame_pos)
{
    const QPalette p = QGuiApplication::palette();
    auto text_color = p.color(QPalette::Normal, QPalette::ButtonText);
    const auto button_color = p.color(QPalette::Normal, QPalette::Button);
    QPainter painter(m_line_number_area);
    painter.fillRect(event->rect(), button_color);

    int           top         = 0;
    QTextBlock    block       = firstVisibleBlock(top);
    int           blockNumber = block.blockNumber();
    QRectF        block_rect  = blockBoundingRect(block);
    int           bottom      = top + qRound(block_rect.height());
    int           align       = m_blame_start_line.size() ? Qt::AlignLeft : Qt::AlignRight;
    s_blame_line* current_blame { nullptr };
    int           current_line = 0;
    blame_pos.clear();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            int line = blockNumber + 1;
            QString number = QString::number(line);
            if (m_blame_start_line.size() > 1)
            {
                if (m_blame_start_line.contains(line))
                {
                    current_blame = &m_blame_start_line[line];
                    current_line  = 0;
                    text_color    = current_blame->blame_data->color;
                }
                if (!current_blame)
                {
                    for (auto iter = m_blame_start_line.begin(); iter != m_blame_start_line.end(); ++iter)
                    {
                        if (iter.key() >= line)
                        {
                            iter--;
                            current_blame = &iter.value();
                            text_color    = current_blame->blame_data->color;
                            break;
                        }
                    }
                }
                if (current_blame && current_line < current_blame->blame_data->text.size())
                {
                    if (current_line == 0)
                    {
                        blame_pos[top] = &current_blame->blame_data->text;
                    }
                    number += ":";
                    number += current_blame->blame_data->text[current_line++];
                }
            }
            painter.setPen(text_color);
            painter.drawText(0, top, m_line_number_area->width(), fontMetrics().height(), align, number);
        }

        block = block.next();
        top = bottom;
        block_rect = blockBoundingRect(block);
        bottom = top + qRound(block_rect.height());
        ++blockNumber;
    }
}

bool LineNumberArea::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent * he = dynamic_cast<QHelpEvent*>(event);
        if (he)
        {
            QPoint pos = he->pos();
            for (auto blame = m_blame_position.begin(); blame != m_blame_position.end(); ++blame)
            {
                if (blame.key() > pos.ry())
                {
                    if (blame != m_blame_position.begin()) blame--;
                    codeEditor->lineNumberAreaHelpEvent(*blame.value(), mapToGlobal(pos));
                    break;
                }
            }
            return true;
        }
    }
    return QWidget::event(event);
}

void code_browser::lineNumberAreaHelpEvent(const QStringList& text_list, const QPoint& pos)
{
    static const QStringList prefix = { tr("commit:\t"), tr("author:\t"), tr("date:\t"), tr("time:\t") };
    QString text;
    int i=0;
    for (const auto& line : text_list)
    {
        if (i < prefix.size()) text += prefix[i++];
        text += line;
        text += getLineFeed();
    }
    QToolTip::showText(pos, text, this);
}

void code_browser::own_text_changed()
{
    m_FileChanged = document()->isModified();
    if (m_active && m_FileChanged)
    {
        Q_EMIT text_of_active_changed(m_FileChanged);
    }
#ifdef WEB_ENGINE
    if (m_web_page)
    {
        QString current_language = mHighlighter->currentLanguage();
        if (current_language == "html")
        {
            m_web_page->set_type(PreviewPage::type::html);
            QString text = toPlainText();
            m_web_page->setHtml(text);
            Q_EMIT show_web_view(text.size() ? true : false);
            setFocus();
        }
        else if (current_language == "markdown")
        {
            m_web_page->set_type(PreviewPage::type::markdown);
            QString text = toPlainText();
            Q_EMIT textChanged(text);
            Q_EMIT show_web_view(text.size() ? true : false);
        }
        else
        {
            Q_EMIT show_web_view(false);
        }
    }
#endif
}

#ifdef WEB_ENGINE
void code_browser::set_page(PreviewPage*page)
{
    m_web_page = page;
}

// m_web_page->printToPdf()

PreviewPage::PreviewPage(QObject *parent, QWebEngineView* view) : QWebEnginePage(parent)
, m_type(type::html)
, m_web_enginge_view(view)
{

}

void PreviewPage::set_type(PreviewPage::type type)
{
    bool changed = m_type != type;
    m_type = type;
    if (changed && m_type == type::markdown)
    {
        load_markdown_page();
    }
}

void PreviewPage::load_markdown_page()
{
    m_web_enginge_view->setUrl(QUrl("qrc:/resource/index.html"));
}

bool PreviewPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType, bool)
{
    if (m_type == type::html)
    {
        return true;
    }
    else if (m_type == type::markdown && url.scheme() == QString("qrc")) // Only allow qrc:/index.html.
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MarkdownProxy::setText(const QString &text)
{
    if (text == m_text)
        return;
    m_text = text;
    emit textChanged(m_text);
}

#endif
