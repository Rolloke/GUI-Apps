#ifndef QDOCKWIDGETX_H
#define QDOCKWIDGETX_H

#include <QDockWidget>

class QDockWidgetX: public QDockWidget
{
    Q_OBJECT
public:
    explicit QDockWidgetX(const QString &title, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    bool is_closing();
    bool contains_cloned_view();

Q_SIGNALS:
    void signal_close(QDockWidgetX*, bool &close);
    void signal_dock_widget_activated(QDockWidget *dockWidget);
public Q_SLOTS:
    void change_visibility(bool visible);
protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *event) override;

private:
    bool m_closed;
public:
    static constexpr char new_textbrowser[]    = "new_textbrowser";
    static constexpr char textbrowser[]        = "textbrowser";
    static constexpr char graphicsviewer[]     = "graphicsviewer";
    static constexpr char binary_table_view[]  = "binary_table_view";
    static constexpr char historyview[]        = "historyview";
    static constexpr char branchview[]         = "branchview";
    static constexpr char stashview[]          = "stashview";
    static constexpr char findview[]           = "findview";
    static constexpr char binaryview[]         = "binaryview";
    static constexpr char markdown_view[]      = "markdown_view";
    static constexpr char cloned_textbrowser[] = "cloned_textbrowser";
    static constexpr char background_textbrowser[] = "background_textbrowser";
};


#endif // QDOCKWIDGETX_H
