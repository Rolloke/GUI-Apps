#ifndef QBINARYTABLEVIEW_H
#define QBINARYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include <QFile>
#include <QJsonValue>
#include <QItemDelegate>

#include "DisplayType.h"
#include "editable.h"
#ifdef USE_BOOST
#include <boost/optional/optional.hpp>
#else
#include <optional>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif


class BinaryTableModel;
class QReadonlyEditItemDelegate;

class qbinarytableview : public QTableView, public Editable
{
    friend class BinaryTableModel;
    friend class QReadonlyEditItemDelegate;
    Q_OBJECT
public:
    explicit qbinarytableview(QWidget *parent = nullptr);
    virtual ~qbinarytableview();

    void set_binary_data(const QByteArray& content);
    const QByteArray& get_binary_data() const;
    void clear_binary_content();
    int  get_type() const;
    int  get_columns() const;
    int  get_offset() const;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    static bool is_binary(QFile& file, std::optional<QStringConverter::Encoding>& encoding);
#else
    static bool is_binary(QFile& file);
#endif

protected:
    void resizeEvent(QResizeEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;


Q_SIGNALS:
    void set_value(const QByteArray& array, int position);
    void publish_has_binary_content(bool );
    void contentChanged(bool);
    void type_changed(int);
    void columns_changed(int);
    void offset_changed(int);
    void cursor_changed(int);
    void endian_changed();

public Q_SLOTS:
    void set_type(int);
    void set_columns(int);
    void set_offset(int);
    void receive_value(const QByteArray& array, int position);
    void open_binary_format_file(const QString& filename, bool& opened);
    void update_table();

private:
    BinaryTableModel* get_model() const;
    void update_rows(bool refresh_all=true);
    void update_complete_row(int row);
    void change_cursor();

public:
    static const QString begin_mark;
    static const QString end_mark;
    static const QString space;
    static const QString hash;
    static const char    unprintable;
    static const QList<QStringList> m_section_names;

private:
    std::vector<double> mColumnWidth;

    QReadonlyEditItemDelegate* m_item_delegate;
};


class BinaryTableModel : public QStandardItemModel
{
    friend class qbinarytableview;

    struct DisplayValue
    {
        typedef std::map<int, std::vector<int>> map2intvector;
        struct r2i { enum { index, lenght, array_index }; };
        enum
        {
            byte_size_limit  = -2,
            is_struct_member = -1,
            invalid_length   =  0,
            default_length   =  1
        };

        QString                     name;
        CDisplayType*               display = 0;
#ifdef USE_BOOST
        boost::optional<QJsonValue> array_length;
#else
        std::optional<QJsonValue> array_length;
#endif
        std::vector<DisplayValue>   member;
        map2intvector               m_td_row_to_index;

        int  get_index(int row, int* length=nullptr, int *array_index=nullptr) const;
        void set_index(int row, int index);
        void set_length(int row, int length);
        void set_array_index(int row, int length);
    };
public:
    enum class Table
    {
        Typed, Character, Last
    };
    enum view_type
    {
        single_data_type,
        type_format_file
    };

    BinaryTableModel(int rows, int columns, qbinarytableview *parent = nullptr);

    ~BinaryTableModel();
    void set_type_display_value_index(int row, int column);

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
    qbinarytableview* get_parent() const;

    void     clear_typed_display();
    bool     insert_binary_structs(QJsonObject &structs_obj);
    bool     update_binary_struct_dependencies();
    bool     insert_display_value(const QJsonValue&, std::vector<DisplayValue>* dv=0);
    void     update_typed_display_rows();
    void     update_typed_display_value(DisplayValue &value, int &offset, int length, int itdv, std::vector<int> &rows);
    QString  display_typed_value(const DisplayValue& value, int row, int length) const;
    QString  display_type(const DisplayValue& value, int row, int *length=nullptr) const;
    int      get_typed_display_array_length(const DisplayValue& value, int itdv, const std::vector<DisplayValue>& value_vector,
                                 const std::vector<int>& index_vector, const std::vector<int>& offset_vector);
    void     update_type_display_value_index();

    QByteArray                      m_binary_content;
    int                             m_columns_per_row;
    int                             m_start_offset;
    int                             m_byte_cursor;
    CDisplayType::eType             m_display_type;
    CDisplayType::type_map&         m_display;

    std::vector<DisplayValue>       m_td_values;
    std::map<QString, DisplayValue> m_td_structs;
    std::vector<int>                m_td_offset;
    std::vector<int>                m_td_index;
    std::pair<int,int>              m_value_index;
};

class QReadonlyEditItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:

    explicit QReadonlyEditItemDelegate(QObject *parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,     const QModelIndex &index) const  override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

private:
};

#endif // QBINARYTABLEVIEW_H
