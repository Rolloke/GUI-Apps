#ifndef CODE_BROWSER_H
#define CODE_BROWSER_H

#include "actions.h"

#include <QTextBrowser>

#include <QPlainTextEdit>


class code_browser : public QTextBrowser
{
    Q_OBJECT

    friend class LineNumberArea;
public:
    code_browser(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  blockCount() const;
    int  current_line() const;
    void go_to_line(int);
    void set_actions(ActionList* list);
    void set_dark_mode(bool );

    void set_binary_data(const QByteArray& data);
    void display_binary_data();
    const QByteArray& get_binary_data() const;
    void clear_binary_content();
    int  get_bytes_per_part() { return m_bytes_per_part; }
    int  get_parts_per_line() { return m_parts_per_line; }
    void set_bytes_per_part(int b) { m_bytes_per_part = b; }
    void set_parts_per_line(int p) { m_parts_per_line = p; }

    static bool is_binary(QFile& file);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

Q_SIGNALS:
    void blockCountChanged(int newBlockCount);
    void updateRequest(const QRect &rect, int dy);
    void set_value(const QByteArray& array, int position);

public Q_SLOTS:
    void set_show_line_numbers(bool);
    void receive_value(const QByteArray& array, int position);

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void vertical_scroll_value(int );

private:
    QTextBlock firstVisibleBlock(int& diff);
    QRectF     blockBoundingRect(const QTextBlock &block) const;
    QPointF    contentOffset() const;
    int        lineNumberAreaWidth();
    void       change_cursor(int block, int position);

private:
    QWidget *   m_line_number_area;
    bool        m_show_line_numbers;
    ActionList *m_actions;
    bool        m_dark_mode;
    int         m_bytes_per_part;
    int         m_parts_per_line;
    QByteArray  m_binary_content;
    bool        m_displaying;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(code_browser *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    code_browser *codeEditor;
};

#endif // CODE_BROWSER_H
