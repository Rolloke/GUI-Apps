#ifndef QBINARYTABLEVIEW_H
#define QBINARYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include <QFile>
#include "DisplayType.h"

class BinaryTableModel;

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

Q_SIGNALS:
    void set_value(const QByteArray& array, int position);
    void publish_has_binary_content(bool );
    void contentChanged();
    void type_changed(int);
    void columns_changed(int);
    void offset_changed(int);

public Q_SLOTS:
    void set_type(int);
    void set_columns(int);
    void set_offset(int);
    void receive_value(const QByteArray& array, int position);

private:
    BinaryTableModel* get_model() const;
    void update_rows();
    void update_complete_row(const QModelIndex &);
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
    int      get_rows() const;
    int      get_bytes_per_type() const;
    int      get_bytes_per_row() const;
    CDisplayType* get_type() const;
    void     set_current_row(int row);
    int      increase_cursor();
    int      decrease_cursor();
    qbinarytableview* get_parent();

    QByteArray  m_binary_content;
    int         m_columns_per_row;
    int         m_start_offset;
    int         m_byte_cursor;
    CDisplayType::eType m_display_type;
    CDisplayType::type_map& m_display;
};

#endif // QBINARYTABLEVIEW_H
