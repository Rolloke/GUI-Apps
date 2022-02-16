#include "qbinarytableview.h"
#include "logger.h"
#include <assert.h>

#define INT(X) static_cast<int>(X)

// TODO: resize table colums

qbinarytableview::qbinarytableview(QWidget *parent) : QTableView(parent)
{
    BinaryTableModel* ListModel = new BinaryTableModel(0, INT(BinaryTableModel::Table::Last), this);
    QStringList fSectionNames = { tr("Pos"), tr("Typed"), tr("Character") };
    for (int fSection = 0; fSection < INT(BinaryTableModel::Table::Last); ++fSection)
    {
        ListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
    }
    setModel(ListModel);
}

void qbinarytableview::update_rows()
{
    auto& themodel = *get_model();
    themodel.removeRows(0, themodel.rowCount());
    themodel.insertRows(0, themodel.get_rows(), QModelIndex());
}

void qbinarytableview::set_binary_data(const QByteArray &content)
{
    get_model()->m_binary_content = content;
    update_rows();
}


const QByteArray& qbinarytableview::get_binary_data() const
{
    return get_model()->m_binary_content;
}

void qbinarytableview::clear_binary_content()
{
    get_model()->m_binary_content.clear();
}

void qbinarytableview::set_parts_per_line(int ppl)
{
    get_model()->m_parts_per_line = ppl;
}

int qbinarytableview::get_parts_per_line() const
{
    return get_model()->m_parts_per_line;
}

BinaryTableModel* qbinarytableview::get_model() const
{
    auto * themodel = dynamic_cast<BinaryTableModel*>(model());
    assert(themodel != 0);
    return themodel;
}

BinaryTableModel::BinaryTableModel(int rows, int columns, qbinarytableview *parent):
    QStandardItemModel(rows, columns, parent)
  , m_parts_per_line(4)
  , m_display_type(CDisplayType::HEX8)
  , m_display(CDisplayType::get_type_map())
{

}

BinaryTableModel::~BinaryTableModel()
{
    TRACE(Logger::info, "BinaryTableModel::~BinaryTableModel()");
}

QVariant BinaryTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case INT(Table::Index):     return QVariant(tr("%1").arg(index.row())); break;
        case INT(Table::Character): return get_character_content(index.row()); break;
        case INT(Table::Typed):     return get_typed_content(index.row()); break;
        }
    }
    // TODO: set font of code browser
    //  6: FontRole
    //  7: TextAlignmentRole
    //  9: ForegroundRole, TextColorRole
    // 10: CheckStateRole
    //  1: DecorationRole
    //  8: BackgroundRole
//    TRACE(Logger::info, "row %d, column %d, role %d", index.row(), index.column(), role);
    return QStandardItemModel::data(index, role);
}

QVariant BinaryTableModel::get_typed_content(int row) const
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    const int bytes_per_row  = get_bytes_per_row();
    const int bytes_per_type = get_bytes_per_type();
    CDisplayType& thetype = *get_type();
    int start = row * bytes_per_row;
    int end   = m_binary_content.size() - bytes_per_type;
    QString line;
    for (int i=0; i<m_parts_per_line && start < end; ++i)
    {
        line += thetype.Display(&buffer_pointer[start]);
        line += " ";
        start += bytes_per_type;
    }
    return QVariant(line);
}

QVariant BinaryTableModel::get_character_content(int row) const
{
    const int bytes_per_row  = get_bytes_per_row();
    int start = row * bytes_per_row;
    int end   = start +bytes_per_row;
    if (end > m_binary_content.size())
    {
        end = m_binary_content.size();
    }
    QString ascii_coded_line;
    for (int i=start; i< end; ++i)
    {
        const std::uint8_t byte = m_binary_content[i];
        ascii_coded_line.append((byte >= 32 && byte <= 127) ? byte : '.');
    }
    return QVariant(ascii_coded_line);
}

int BinaryTableModel::get_rows() const
{
    int rows = m_binary_content.size() / get_bytes_per_row() + 1;
    return rows;
}

int BinaryTableModel::get_bytes_per_type() const
{
    return get_type()->GetByteLength();
}

int BinaryTableModel::get_bytes_per_row() const
{
    return get_bytes_per_type() * m_parts_per_line;
}

CDisplayType* BinaryTableModel::get_type() const
{
    return m_display[m_display_type].get();
}
