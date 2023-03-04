#include "editable.h"


Editable::Editable():
   m_FileChanged(false)
 , m_active(false)
{

}

void Editable::set_file_path(const QString &file_path)
{
    m_FilePath = file_path;
}

const QString& Editable::get_file_path() const
{
    return m_FilePath;
}

void Editable::set_changed(bool changed)
{
    m_FileChanged = changed;
}

bool Editable::get_changed() const
{
    return m_FileChanged;
}

void Editable::set_active(bool active)
{
    m_active = active;
}

bool Editable::get_active() const
{
    return m_active;
}
