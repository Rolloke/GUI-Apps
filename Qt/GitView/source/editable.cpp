#include "editable.h"
#include "code_browser.h"
#include "qbinarytableview.h"

#include <QFileInfo>
Editable::Editable():
   m_FileChanged(false)
 , m_active(false)
{

}

void Editable::set_file_path(const QString &file_path)
{
    m_FilePath = file_path;
    update_modified();
}

const QString& Editable::get_file_path() const
{
    return m_FilePath;
}

void Editable::set_changed(bool changed)
{
    if (!changed)
    {
        update_modified();
    }
    m_FileChanged = changed;
}

bool Editable::get_changed() const
{
    return m_FileChanged;
}

bool Editable::set_active(bool active)
{
    bool activated = !m_active && active;
    m_active = active;
    return activated;
}

bool Editable::get_active() const
{
    return m_active;
}

bool Editable::is_modified()
{
    QFileInfo info(m_FilePath);
    return m_last_modified != info.lastModified();
}

void Editable::update_modified()
{
    QFileInfo info(m_FilePath);
    m_last_modified = info.lastModified();
}

bool set_file_path(QWidget *widget, const QString &file_path)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(widget);
    if (text_browser)
    {
        text_browser->set_file_path(file_path);
        return true;
    }
    qbinarytableview* binary_view = dynamic_cast<qbinarytableview*>(widget);
    if (binary_view)
    {
        binary_view->set_file_path(file_path);
        return true;
    }
    return false;
}

const QString& get_file_path(QWidget*widget)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(widget);
    if (text_browser)
    {
        return text_browser->get_file_path();
    }
    qbinarytableview* binary_view = dynamic_cast<qbinarytableview*>(widget);
    if (binary_view)
    {
        return binary_view->get_file_path();
    }
    static const QString dummy {};
    return dummy;
}

bool get_changed(QWidget*widget)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(widget);
    if (text_browser)
    {
        return text_browser->get_changed();
    }
    qbinarytableview* binary_view = dynamic_cast<qbinarytableview*>(widget);
    if (binary_view)
    {
        return binary_view->get_changed();
    }
    return false;
}

bool get_active(QWidget*widget)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(widget);
    if (text_browser)
    {
        return text_browser->get_active();
    }
    qbinarytableview* binary_view = dynamic_cast<qbinarytableview*>(widget);
    if (binary_view)
    {
        return binary_view->get_active();
    }
    return false;
}

void set_active(QWidget*widget, bool active)
{
    code_browser* text_browser = dynamic_cast<code_browser*>(widget);
    if (text_browser)
    {
        text_browser->set_active(active);
    }
    qbinarytableview* binary_view = dynamic_cast<qbinarytableview*>(widget);
    if (binary_view)
    {
        binary_view->set_active(active);
    }
}
