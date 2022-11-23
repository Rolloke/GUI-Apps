#ifndef QBINARYTABLEVIEW_H
#define QBINARYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include <QFile>
#include <QJsonValue>
#include "DisplayType.h"

class BinaryTableModel;

struct DisplayValue
{
    struct r2i { enum { index, lenght }; };
    enum { is_struct_member=-1, invalid_length = 0, default_length = 1 };

    QString name;
    CDisplayType* display = 0;
    std::optional<QJsonValue> array_length;
    std::vector<DisplayValue> member;
    std::map<std::uint32_t, std::vector<std::uint32_t>> m_td_row_to_index;

    int get_index(int row, int* length=nullptr) const;
};


class qbinarytableview : public QTableView
{
    friend class BinaryTableModel;
    Q_OBJECT
public:
    explicit qbinarytableview(QWidget *parent = nullptr);

    void set_binary_data(const QByteArray& content);
    const QByteArray& get_binary_data() const;
    void clear_binary_content();
    int  get_type() const;
    int  get_columns() const;
    int  get_offset() const;

    static bool is_binary(QFile& file);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;

Q_SIGNALS:
    void set_value(const QByteArray& array, int position);
    void publish_has_binary_content(bool );
    void contentChanged();
    void type_changed(int);
    void columns_changed(int);
    void offset_changed(int);
    void cursor_changed(int);

public Q_SLOTS:
    void set_type(int);
    void set_columns(int);
    void set_offset(int);
    void receive_value(const QByteArray& array, int position);
    void open_binary_format_file(const QString& filename, bool& opened);

private:
    BinaryTableModel* get_model() const;
    void update_rows(bool refresh_all=true);
    void update_complete_row(int row);
    void change_cursor();

    std::vector<double> mColumnWidth;

};

class BinaryTableModel : public QStandardItemModel
{
    friend class qbinarytableview;
public:
    enum class Table
    {
        Typed, Character, Last
    };
    BinaryTableModel(int rows, int columns, qbinarytableview *parent = nullptr);

    ~BinaryTableModel();

private:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags( const QModelIndex &index) const override;

    QVariant get_typed_content(int row) const;
    QVariant get_character_content(int row) const;
    QVariant get_typed_display_values(int row) const;
    bool     has_typed_display_values() const;

    int      get_rows() const;
    size_t   get_bytes_per_type() const;
    int      get_bytes_per_row() const;
    CDisplayType* get_type() const;
    void     set_current_row(int row);
    int      increase_cursor();
    int      decrease_cursor();
    qbinarytableview* get_parent();

    void     clear_typed_display();
    bool     insert_display_value(const QJsonValue&, std::vector<DisplayValue>* dv=0);
    void     update_typed_display_rows();
    void     update_typed_display_value(DisplayValue &value, int &offset, int length, int itdv);
    QString  display_typed_value(const DisplayValue& value, int row, int length) const;
    QString  display_type(const DisplayValue& value, int row, int *length=nullptr) const;
    int      get_td_array_length(const DisplayValue& value, int itdv, const std::vector<DisplayValue>& value_vector,
                                 const std::vector<int>& index_vector, const std::vector<int>& offset_vector);

    QByteArray  m_binary_content;
    int         m_columns_per_row;
    int         m_start_offset;
    int         m_byte_cursor;
    CDisplayType::eType m_display_type;
    CDisplayType::type_map& m_display;

    std::vector<DisplayValue>       m_td_values;
    std::map<QString, DisplayValue> m_td_structs;
    std::vector<int>                m_td_offset;
    std::vector<int>                m_td_index;
};

#endif // QBINARYTABLEVIEW_H
