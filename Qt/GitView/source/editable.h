#ifndef EDITABLE_H
#define EDITABLE_H
#include <QString>
#include <QWidget>

class Editable
{
protected:
    Editable();

public:
    void set_file_path(const QString& file_path);
    const QString& get_file_path() const;
    void set_changed(bool changed);
    bool get_changed() const;
    void set_active(bool active);
    bool get_active() const;

protected:
    QString                 m_FilePath;
    bool                    m_FileChanged;
    bool                    m_active;
};


const QString& get_file_path(QWidget*widget);
bool           get_changed(QWidget*widget);
bool           get_active(QWidget*widget);
void           set_active(QWidget*widget, bool active);

#endif // EDITABLE_H
