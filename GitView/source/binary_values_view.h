#ifndef BINARY_VALUES_VIEW_H
#define BINARY_VALUES_VIEW_H

#include <QDialog>
#include <QVector>
#include <QCheckBox>
#include <QLineEdit>
#include <QStandardItemModel>

#include <map>
#include <memory>

#ifdef USE_BOOST
#include <boost/optional/optional.hpp>
#else
#include <optional>
#endif

#include "DisplayType.h"

namespace Ui {
class binary_values_view;
}

class CheckboxItemModel;

class binary_values_view : public QDialog
{
    Q_OBJECT

public:
    explicit binary_values_view(QWidget *parent = nullptr);
    ~binary_values_view();

private:
    struct Table
    {
        enum
        {
            checkbox,
            name,
            type,
            value
        };
    };

    std::uint32_t       type2index(CDisplayType::eType type);
    CDisplayType::eType index2type(std::uint32_t index);
    QVariantList        get_search_list();
    void                update_table_buttons();

public:
Q_SIGNALS:
    void set_value(const QByteArray& array, int position);
    void status_message(const QString&, int);
    void change_table_type(int type);
    void change_table_columns(int columns);
    void change_table_offset(int offset);
    void open_binary_format_file(const QString& filename, int &json_flag);
    void edit_binary_format_file(QString& filename, int &json_flag);
    void endian_changed();
    void search_expression(const QVariantList& list, bool forward);

public Q_SLOTS:
    void receive_value(const QByteArray& array, int position);
    void receive_external_data(bool external);
    void set_table_type(int type);
    void set_table_columns(int columns);
    void set_table_offset(int offset);
    void notify_endian_changed();

private Q_SLOTS:
    void on_btnReadValue_clicked();
    void on_btnWriteValue_clicked();
    void on_radioEndian1_clicked();
    void on_radioEndian2_clicked();
    void on_checkStandAlone_toggled(bool checked);
    void on_btnOpenTypeFile_clicked();
    void on_btnEditJsonFile_clicked();
    void on_btnReloadJsonText_clicked();
    void on_btnInsertType_clicked();
    void on_btnRemoveType_clicked();
    void on_btnFindNext_clicked();
    void on_btnFindPrevious_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_btnStoreTableContent_clicked();
    void on_btnLoadTableContent_clicked();

    void check_box_clicked(bool checked);
    void table_type_changed(QString type_name);
    void table_columns_changed(int columns);
    void table_offset_changed(int offset);
    void editing_finished();
    void showLink(const QString& link_name);



private:
    Ui::binary_values_view *ui;
    CDisplayType::type_map& m_display;
    QVector<QCheckBox*> m_Checkboxes;
    QVector<QLineEdit*> m_Edit;
    int                 m_current_position;
    QByteArray          m_array;
    bool                m_setting_table_property;

    CheckboxItemModel* mListModel = nullptr;

#ifdef USE_BOOST
    boost::optional<int>  m_current_combo_index;
#else
    std::optional<int>  m_current_combo_index;
#endif
public:
    QString             m_type_format_files_location;
};

#endif // BINARY_VALUES_VIEW_H
