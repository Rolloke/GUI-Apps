#ifndef CODE_BROWSER_H
#define CODE_BROWSER_H

#include <QTextBrowser>

#include <QPlainTextEdit>


class code_browser : public QTextBrowser
{
    Q_OBJECT

public:
    code_browser(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();
    int  blockCount() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void blockCountChanged(int newBlockCount);
    void updateRequest(const QRect &rect, int dy);

public Q_SLOTS:
    void set_show_line_numbers(bool);

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void vertical_scroll_value(int );

private:
    QTextBlock firstVisibleBlock(int& diff);
    QRectF     blockBoundingRect(const QTextBlock &block) const;
    QPointF    contentOffset() const;


private:
    QWidget *m_line_number_area;
    bool m_show_line_numbers;
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
