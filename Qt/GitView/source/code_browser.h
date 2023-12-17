#ifndef CODE_BROWSER_H
#define CODE_BROWSER_H

#include "actions.h"
#include "highlighter.h"
#include "editable.h"

#include <QTextBrowser>
#include <QPlainTextEdit>
#ifdef WEB_ENGINE
#include <QWebEnginePage>
class    QWebEngineView;
class    PreviewPage;
#endif

enum class selection
{
    toggle_comment,
    to_upper,
    to_lower,
    to_snake_case,
    to_camel_case,
    unknown=-1
};

class code_browser : public QTextBrowser, public Editable
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
    int  current_line(QString*text = nullptr) const;
    void go_to_line(int);

    void set_actions(ActionList* list);
    void set_do_preview(bool preview);

    void reset();
    const QString& currentLanguage() const;
    void setExtension(const QString& ext);
    void setLanguage(const QString& language);
    void parse_blame(const QString& blame);
    bool hasExtension(const QString& ext);
    void changeSelection(selection command);

    code_browser* clone(bool all_parameter=false, bool with_text=true);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool event(QEvent *event) override;
    void focusInEvent(QFocusEvent *) override;

Q_SIGNALS:
    void text_changed(const QString &text);
    void blockCountChanged(int newBlockCount);
    void updateRequest(const QRect &rect, int dy);
    void updateExtension(const QString&);
    void show_web_view(bool);
    void line_changed(int);
    void column_changed(int);
    void text_of_active_changed(bool);
    void check_reload(code_browser*);

public Q_SLOTS:
    void set_show_line_numbers(bool);
    void change_visibility(bool visible);

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void vertical_scroll_value(int );
    void call_updateExtension(const QString&);
    void own_text_changed();

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
    QString    comment_uncomment_selection();
    static QString    toCamelCase(const QString&text);
    static QString    toSnakeCase(const QString& text);

private:
    QPointer<QWidget>       m_line_number_area;
    bool                    m_show_line_numbers;
    QMap<QString, s_blame>  m_blame_map;
    QMap<int, s_blame_line> m_blame_start_line;
    std::int32_t            m_blame_characters;

    ActionList *m_actions;
    bool        m_do_preview;
    QSharedPointer<Highlighter> mHighlighter;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
public:
    void set_encoding(const std::optional<QStringConverter::Encoding>& encoding);
    std::optional<QStringConverter::Encoding> get_encoding();
private:
    std::optional<QStringConverter::Encoding> m_encoding;
#endif

#ifdef WEB_ENGINE
    PreviewPage * m_web_page;

public:
    void set_page(PreviewPage*);
#else
    QTextBrowser* m_preview;
public:
    void set_page(QTextBrowser*);
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


Q_SIGNALS:
    void textChanged(const QString &text);

public Q_SLOTS:
    void setText(const QString &text);

private:
    QString m_text;
};

#endif

#endif // CODE_BROWSER_H
