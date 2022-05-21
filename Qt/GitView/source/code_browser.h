#ifndef CODE_BROWSER_H
#define CODE_BROWSER_H

#include "actions.h"
#include "highlighter.h"

#include <QTextBrowser>
#include <QPlainTextEdit>
#ifdef WEB_ENGINE
#include <QWebEnginePage>
class    QWebEngineView;
class    PreviewPage;
#endif

class code_browser : public QTextBrowser
{
    Q_OBJECT

    friend class LineNumberArea;
    typedef QMap<int, QStringList*> pos_to_blame;
public:
    code_browser(QWidget *parent = nullptr);
    virtual ~code_browser();

    void lineNumberAreaPaintEvent(QPaintEvent *event, pos_to_blame&);
    void lineNumberAreaHelpEvent(const QStringList&, const QPoint&);
    int  blockCount() const;
    int  current_line() const;
    void go_to_line(int);

    void set_actions(ActionList* list);
    void set_dark_mode(bool );

    void reset();
    const QString& currentLanguage() const;
    void setExtension(const QString& ext);
    void setLanguage(const QString& language);

    void parse_blame(const QString& blame);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool event(QEvent *event) override;

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
#ifdef WEB_ENGINE
    void own_text_changed();
#endif


private:
    struct s_blame
    {
         QStringList text;
         QColor  color;
    };
    struct s_blame_line
    {
        s_blame* blame_data { nullptr };
    };

    QTextBlock firstVisibleBlock(int& diff);
    QRectF     blockBoundingRect(const QTextBlock &block) const;
    QPointF    contentOffset() const;
    int        lineNumberAreaWidth();
    void       reset_blame();

private:
    QPointer<QWidget>       m_line_number_area;
    bool                    m_show_line_numbers;
    QMap<QString, s_blame>  m_blame_map;
    QMap<int, s_blame_line> m_blame_start_line;
    std::int32_t            m_blame_characters;

    ActionList *m_actions;
    bool        m_dark_mode;
    QSharedPointer<Highlighter> mHighlighter;

#ifdef WEB_ENGINE
    PreviewPage * m_web_page;

public:
    void set_page(PreviewPage*);
#endif
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(code_browser *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event)override;
    bool event(QEvent *event) override;

private:
    code_browser *codeEditor;
    code_browser::pos_to_blame m_blame_position;
};

#ifdef WEB_ENGINE

class PreviewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    enum class type { html, markdown };
    explicit PreviewPage(QObject *parent = nullptr, QWebEngineView* view = nullptr);

    void set_type(type type);

    void load_markdown_page();

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;

private:
    type m_type;
    QWebEngineView* m_web_enginge_view;
};

class MarkdownProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged FINAL)
public:
    explicit MarkdownProxy(QObject *parent = nullptr) : QObject(parent) {}

    void setText(const QString &text);

signals:
    void textChanged(const QString &text);

private:
    QString m_text;
};

#endif

#endif // CODE_BROWSER_H
