#ifndef CODE_BROWSER_H
#define CODE_BROWSER_H

#include "actions.h"
#include "highlighter.h"

#include <QTextBrowser>

#include <QPlainTextEdit>

class QWebEnginePage;

class code_browser : public QTextBrowser
{
    Q_OBJECT

    friend class LineNumberArea;
public:
    code_browser(QWidget *parent = nullptr);
    virtual ~code_browser();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  blockCount() const;
    int  current_line() const;
    void go_to_line(int);

    void set_actions(ActionList* list);
    void set_dark_mode(bool );

    void set_page(QWebEnginePage*);

    void reset();
    const QString& currentLanguage() const;
    void setExtension(const QString& ext);
    void setLanguage(const QString& language);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

Q_SIGNALS:
    void textChanged(const QString &text);
    void blockCountChanged(int newBlockCount);
    void updateRequest(const QRect &rect, int dy);
    void updateExtension(const QString&);
    void show_web_view(bool);

public Q_SLOTS:
    void set_show_line_numbers(bool);

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void vertical_scroll_value(int );
    void call_updateExtension(const QString&);
    void own_text_changed();


private:
    QTextBlock firstVisibleBlock(int& diff);
    QRectF     blockBoundingRect(const QTextBlock &block) const;
    QPointF    contentOffset() const;
    int        lineNumberAreaWidth();

private:
    QWidget *   m_line_number_area;
    bool        m_show_line_numbers;
    ActionList *m_actions;
    bool        m_dark_mode;
    QSharedPointer<Highlighter> mHighlighter;
    QWebEnginePage * m_web_page;
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
