#include "code_browser.h"
#include "logger.h"
#include "helper.h"
#include "mainwindow.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QScrollBar>
#include <QMenu>
#include <QTextCursor>
#include <QApplication>
#include <QToolTip>
#include <QDockWidget>

#ifdef WEB_ENGINE
#include <QWebEngineView>
#endif


code_browser::code_browser(QWidget *parent): QTextBrowser(parent)
  , m_line_number_area(new LineNumberArea(this))
  , m_show_line_numbers(false)
  , m_blame_characters(0)
  , m_actions(nullptr)
  , m_do_preview(false)
{
    connect(document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(updatecontentsChange(int,int,int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(vertical_scroll_value(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    updateLineNumberAreaWidth(blockCount());
    highlightCurrentLine();

    m_line_number_area->setToolTip("init");
    connect(this, SIGNAL(textChanged()), this, SLOT(own_text_changed()));
#ifdef WEB_ENGINE
    m_web_page = nullptr;
#else
    m_preview = nullptr;
#endif
}

code_browser::~code_browser()
{
    Q_EMIT show_web_view(false);
#ifdef WEB_ENGINE
    m_web_page = nullptr;
#else
    m_preview = nullptr;
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

        if (m_text_section_start.size())
        {
            digits += 2;
        }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        int space = 3 + fontMetrics().boundingRect("9").width() * digits;
#else
        int space = 3 + fontMetrics().charWidth("9", 0) * digits;
#endif
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
    QString current_language = mHighlighter->currentLanguage();
    if (is_any_equal_to(current_language, "html", "markdown"))
    {
        if (visible)
        {
            own_text_changed();
        }
        else
        {
            Q_EMIT show_web_view(false);
        }
    }
}

void code_browser::updatecontentsChange(int from, int charsRemoved, int charsAdded)
{
    set_sections_visible(true);
    m_text_section_start.clear();
    if (from == 0 && charsRemoved == 0 && charsAdded > 0)
    {
        parse_sections(toPlainText());
    }
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
#ifdef WEB_ENGINE
        cloned->m_web_page = m_web_page;
#else
        cloned->m_preview = m_preview;
#endif
        cloned->setWhatsThis(whatsThis());
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        cloned->setTabStopDistance(tabStopDistance());
#else
        cloned->setTabStopWidth(tabStopWidth());
#endif        
        cloned->setUndoRedoEnabled(isUndoRedoEnabled());
        cloned->setWordWrapMode(wordWrapMode());

        QMenu menu;
        m_actions->fillContextMenue(menu, git::Cmd::mContextMenuTextView, cloned);
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
        auto* action = m_actions->getAction(git::Cmd::CloneTextBrowser);
        if (action) action->setEnabled(!isReadOnly());
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

void code_browser::focusInEvent(QFocusEvent *fie)
{
    if (is_modified())
    {
        Q_EMIT check_reload(this);
    }
    QWidget* focused =  focusWidget();
    QDockWidget * dock_widget = dynamic_cast<QDockWidget*>(parent());
    if (dock_widget && dock_widget != focused && this != focused)
    {
        Q_EMIT send_focused(dock_widget);
    }
    QTextBrowser::focusInEvent(fie);
}

void code_browser::dropEvent(QDropEvent *de)
{
    QTextBrowser::dropEvent(de);
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
        selection.format.setBackground(Highlighter::Language::mSelectedLineBackground);
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

void code_browser::set_do_preview(bool preview)
{
    m_do_preview = preview;
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

void code_browser::changeSelection(selection command)
{
    auto text_cursor = textCursor();
    if (!text_cursor.hasSelection())
    {
        text_cursor.select(QTextCursor::WordUnderCursor);
    }

    if (text_cursor.hasSelection())
    {
        QString text;
        switch (command)
        {
        case selection::to_lower:       text = text_cursor.selectedText().toLower();    break;
        case selection::to_upper:       text = text_cursor.selectedText().toUpper();    break;
        case selection::toggle_comment: text = change_start_of_selection(command);      break;
        case selection::to_snake_case:  text = toSnakeCase(text_cursor.selectedText()); break;
        case selection::to_camel_case:  text = toCamelCase(text_cursor.selectedText()); break;
        case selection::tab_indent:     text = change_start_of_selection(command); break;
        case selection::tab_outdent:    text = change_start_of_selection(command); break;
        default:

            text.clear();
            break;
        }
        if (text.size())
        {
            text_cursor.beginEditBlock();
            text_cursor.insertText(text);
            text_cursor.endEditBlock();
        }
    }
}

void code_browser::setTabstopCharacters(int characters)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    float size = fontMetrics().boundingRect("#").width();
#else
    float size = fontMetrics().width(" ");
#endif
    if (size < 0)
    {
        size = font().pixelSize();
    }
    if (size < 0)
    {
        size = font().pointSizeF();
    }
    int width = characters * static_cast<int>(size);
    m_indent.fill(' ', characters);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        setTabStopDistance(width);
#else
        setTabStopWidth(width);
#endif
}

int code_browser::getTabstopCharacters()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    float size = fontMetrics().boundingRect("#").width();
#else
    float size = fontMetrics().width(" ");
#endif
    if (size < 0)
    {
        size = font().pixelSize();
    }
    if (size < 0)
    {
        size = font().pointSizeF();
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    int width = tabStopDistance();
#else
    int width = tabStopWidth();
#endif
    return width / static_cast<int>(size);
}

QString code_browser::change_start_of_selection(selection how_to)
{
    QString text = textCursor().selection().toPlainText();
    QStringList lines = text.split("\n");
    text.clear();
    int no_of_lines = lines.size();
    if (no_of_lines && lines[no_of_lines-1].size() == 0)
    {
        lines.removeLast();
        --no_of_lines;
    }
    if (no_of_lines)
    {
        QString pattern;
        switch (how_to)
        {
        case selection::toggle_comment:
        {
            pattern = mHighlighter->get_current_language_pattern(Highlighter::Language::mSingleLineCommentFormat);
            QString regex   = Highlighter::get_regex(Highlighter::single_line_comment);
            pattern = pattern.left(pattern.indexOf(regex));
        }   break;
        case selection::tab_indent:
        case selection::tab_outdent:
            pattern = m_indent;
            break;
        default:
            TRACEX(Logger::error, "not handled here: " << static_cast<int>(how_to));
            break;
        }

        if (pattern.size())
        {
            int nline = 0;
            if (    how_to == selection::tab_outdent
                || (how_to == selection::toggle_comment && pattern == lines[0].left(pattern.size()))) // uncomment
            {
                for (const QString& line : std::as_const(lines))
                {
                    text += pattern == line.left(pattern.size()) ? line.mid(pattern.size()) : line;
                    if (++nline == no_of_lines && line.size() == 0) break;
                    text += "\n";
                }
            }
            else // comment
            {
                for (const QString& line : std::as_const(lines))
                {
                    text += pattern + line;
                    if (++nline == no_of_lines && line.size() == 0) break;
                    text += "\n";
                }
            }
        }
    }
    return text;
}

QString code_browser::toCamelCase(const QString& text)
{
    if (is_whole_word(text))
    {
        QString result;
        int n = text.size();
        for (int i=0; i<n; ++i)
        {
            if (i==0 && text[i].isLetter())
            {
                result += text[i].toUpper();
            }
            else if (text[i] == '_' && i < n-1)
            {
                result += text[++i].toUpper();
            }
            else
            {
                result += text[i];
            }
        }
        return result;
    }
    return {};
}

QString code_browser::toSnakeCase(const QString& text)
{
    if (is_whole_word(text))
    {
        auto ll = std::locale();
        QString result;
        int n = text.size();
        for (int i=0; i<n; ++i)
        {
            if (i==0)
            {
                result += text[i].toLower();
            }
            else if (text[i].isUpper())
            {
                result += '_';
                result += text[i].toLower();
            }
            else
            {
                result += text[i];
            }
        }
        return result;
    }
    return {};
}

void code_browser::parse_sections(const QString &text)
{
    const QRegularExpression& start_section = mHighlighter->get_language().get_regex(Highlighter::startsection);
    const QRegularExpression& stop_section  = mHighlighter->get_language().get_regex(Highlighter::stopsection);
    if (start_section.isValid() && start_section.pattern().size() && stop_section.isValid() && stop_section.pattern().size())
    {
        QStringList lines = text.split("\n");
        int level = 0;
        int line_number = 1;
        for (const QString &line : std::as_const(lines))
        {
            int start = line.indexOf(start_section);
            int stop  = line.indexOf(stop_section);
            if (start != -1)
            {
                auto& hide_section = m_text_section_start[line_number];
                ++level;
                hide_section.level = level;
            }
            if (stop != -1)
            {
                auto hide_section = std::find_if(m_text_section_start.begin(), m_text_section_start.end(), [level](const auto& section) { return (section.second.end_line == 0 && section.second.level == level); });
                if (hide_section != m_text_section_start.end())
                {
                    --level;
                    hide_section->second.end_line = line_number;
                }
            }
            ++line_number;
        }

        for (auto section = m_text_section_start.begin(); section != m_text_section_start.end(); )
        {
            if (section->first == section->second.end_line)
            {
                section = m_text_section_start.erase(section);
            }
            else
            {
                ++section;
            }
        }
    }
}

void code_browser::set_sections_visible(bool visible)
{
    bool update = false;
    for (auto section_iter = m_text_section_start.begin(); section_iter != m_text_section_start.end(); ++section_iter)
    {
        if (section_iter->second.visible != visible)
        {
            update = true;
            int line = section_iter->first;
            QTextBlock block = document()->findBlockByLineNumber(line - 1);
            s_text_section &section = section_iter->second;
            section.visible = !section.visible;
            block = block.next();
            for (++line; line <= section.end_line && block.isValid(); ++line)
            {
                block.setVisible(section.visible);
                block.setUserState(0);
                block = block.next();
            }
        }
    }
    if (update)
    {
        auto end_block = document()->end().previous();
        document()->markContentsDirty(0, end_block.position());
        viewport()->update();
        m_line_number_area->update();
    }
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
    for (const QString &line : std::as_const(lines))
    {
        QStringList parts = line.split("\t");
        if (parts.size() == 4)
        {
            int pos = parts[3].indexOf(')');
            if (pos != -1)
            {
                current_line_number = parts[3].left(pos).toInt();
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
                for (const auto& text : std::as_const(blame_entry_text))
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void code_browser::set_encoding(const std::optional<QStringConverter::Encoding>& encoding)
{
    m_encoding = encoding;
}

std::optional<QStringConverter::Encoding> code_browser::get_encoding()
{
    return m_encoding;
}
#endif

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

    std::vector<s_text_section*> current_hide;
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
            else if (m_text_section_start.size() > 0)
            {
                if (m_text_section_start.count(line))
                {
                    current_hide.push_back(&m_text_section_start[line]);
                    if (current_hide.back()->visible) number += " <";
                    else
                    {
                        number += " #";
                        current_hide.pop_back();
                    }
                }
                else if (current_hide.size())
                {
                    if (current_hide.back()->end_line == line)
                    {
                        number += " >";
                        current_hide.pop_back();
                    }
                    else
                    {
                        number += " |";
                    }
                }
                else
                {
                    number += "  ";
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
            if (codeEditor->m_text_section_start.size())
            {
                QTextCursor cursor = codeEditor->cursorForPosition(he->pos());
                QTextBlock    block = cursor.block();
                if (block.isValid() )
                {
                    if (block.isVisible())
                    {
                        int line = block.blockNumber() + 1;
                        auto hide_section = codeEditor->m_text_section_start.find(line);
                        if (hide_section != codeEditor->m_text_section_start.end())
                        {
                            code_browser::s_text_section &section = hide_section->second;
                            std::stringstream text;
                            text << "Code block parameters:\n- lines in section: " << (section.end_line - line) << "\n- indentation level: " << section.level;
                            QToolTip::showText(mapToGlobal(pos), text.str().c_str(), this);
                        }
                    }
                }
            }
            return true;
        }
    }
    return QWidget::event(event);
}

void LineNumberArea::regard_nested_section(const code_browser::s_text_section &section, QTextBlock& block, int& line)
{
    (void)(block);
    /// TODO: regard nested sections, but how?
    /// here the nested section visibility is set to parent visibility
    /// NOTE: if all are hidden, show all sections with level + 1
    auto nested_section = codeEditor->m_text_section_start.find(line);
    if (   nested_section != codeEditor->m_text_section_start.end()
        //&& nested_section->second.visible != section.visible
        )
    {
        nested_section->second.visible = section.visible;
        // block.setVisible(section.visible);
        // block = block.next();
        // for (++line; line <= nested_section->second.end_line && block.isValid(); ++line)
        // {
        //     regard_nested_section(nested_section->second, block, line);
        //     block = block.next();
        // }
    }
}

void LineNumberArea::mousePressEvent(QMouseEvent *me)
{
/// TODO: Jump to start or end of section
    QTextCursor cursor = codeEditor->cursorForPosition(me->pos());
    QTextBlock    block = cursor.block();
    bool update = false;
    if (block.isValid() )
    {
        if (block.isVisible())
        {
            int line = block.blockNumber() + 1;
            auto hide_section = codeEditor->m_text_section_start.find(line);
            if (hide_section != codeEditor->m_text_section_start.end())
            {
                code_browser::s_text_section &section = hide_section->second;
                section.visible = !section.visible;
                block = block.next();
                for (++line; line <= section.end_line && block.isValid(); ++line)
                {
                    regard_nested_section(section, block, line);
                    block.setVisible(section.visible);
                    block = block.next();
                }
                update = true;
            }
        }
        block = block.next();
    }
    if (update)
    {
        auto end_block = codeEditor->document()->end().previous();
        codeEditor->document()->markContentsDirty(0, end_block.position());
        codeEditor->viewport()->update();
        codeEditor->m_line_number_area->update();
    }

    QWidget::mousePressEvent(me);
}

void code_browser::lineNumberAreaHelpEvent(const QStringList& text_list, const QPoint& pos)
{
    static const QStringList prefix = { tr("commit:\t"), tr("author:\t"), tr("date:\t"), tr("time:\t") };
    QString text;
    int i=0;
    for (const auto& line : std::as_const(text_list))
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
    if (m_active)
    {
        Q_EMIT text_of_active_changed(m_FileChanged);
    }
#ifdef WEB_ENGINE
    if (m_web_page && m_do_preview)
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
            Q_EMIT text_changed(text);
            Q_EMIT show_web_view(text.size() ? true : false);
            setFocus();
        }
        else
        {
            Q_EMIT show_web_view(false);
        }
    }
    else
    {
        Q_EMIT show_web_view(false);
    }
#else
    if (m_preview && m_do_preview)
    {
        QString current_language = mHighlighter->currentLanguage();
        if (current_language == "html")
        {
            QString text = toPlainText();
            m_preview->setHtml(text);
            Q_EMIT show_web_view(text.size() ? true : false);
            setFocus();
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        else if (current_language == "markdown")
        {
            QString text = toPlainText();
            m_preview->setMarkdown(text);
            Q_EMIT show_web_view(text.size() ? true : false);
            setFocus();
        }
#endif
        else
        {
            Q_EMIT show_web_view(false);
        }
    }
    else
    {
        Q_EMIT show_web_view(false);
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

bool PreviewPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    (void)(isMainFrame);
    const QString& path = url.path();
    if (m_type == type::html)
    {
        return true;
    }
    else if (m_type == type::markdown && url.scheme() == QString("qrc") && path.indexOf("resource/index.html") != -1) // Only allow qrc:/index.html.
    {
        return true;
    }
    else if (type == NavigationTypeLinkClicked && (url.scheme().indexOf("http") != -1 ||url.scheme().indexOf("mailto") != -1))
    {
        Q_EMIT open_link(url.toString());
        return false;
    }
    else
    {
        return false;
    }
}

void MarkdownProxy::setText(const QString &text)
{
    if (text == m_text)
    {
        return;
    }
    m_text = text;
    emit textChanged(m_text);
}

#else
void code_browser::set_page(QTextBrowser *page)
{
    m_preview = page;
}
#endif
