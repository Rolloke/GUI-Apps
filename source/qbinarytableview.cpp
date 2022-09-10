#include "qbinarytableview.h"
#include "logger.h"
#include <assert.h>
#include <QScrollBar>
#include <QKeyEvent>

#define INT(X) static_cast<int>(X)

/// TODO: validate litle and big endian representation
/// validate cursor representation
/// validate order of data representation

qbinarytableview::qbinarytableview(QWidget *parent) : QTableView(parent)
{
    BinaryTableModel* ListModel = new BinaryTableModel(0, INT(BinaryTableModel::Table::Last), this);
    QStringList fSectionNames = {  tr("Typed"), tr("Character") };
    mColumnWidth   = { 0.6, 0 };

    for (int fSection = 0; fSection < INT(BinaryTableModel::Table::Last); ++fSection)
    {
        ListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
    }
    setModel(ListModel);

    double fItemWidth = 0;
    std::for_each(mColumnWidth.begin(), mColumnWidth.end()-1, [&fItemWidth](double fItem ) { fItemWidth += fItem; });
    mColumnWidth.back() = 1.0 - fItemWidth;

}

void qbinarytableview::update_rows()
{
    auto& themodel = *get_model();
    themodel.removeRows(0, themodel.rowCount());
    themodel.insertRows(0, themodel.get_rows(), QModelIndex());
}

void qbinarytableview::update_complete_row(const QModelIndex &index)
{
    for (int col=0; col<model()->columnCount(); ++col)
    {
        update(model()->index(index.row(), col));
    }
}

void qbinarytableview::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    const int vertical_width = verticalScrollBar()->sizeHint().width();
    int fWidth = rect().width() - vertical_width;

    for (int fColumn = 0; fColumn<model()->columnCount(); ++fColumn)
    {
        setColumnWidth(fColumn, INT(mColumnWidth[fColumn]*fWidth));
    }
}

void qbinarytableview::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    get_model()->set_current_row(current.row());
    update_complete_row(current);
    update_complete_row(previous);
    QTableView::currentChanged(current, previous);
}

void qbinarytableview::keyPressEvent(QKeyEvent *event)
{
    auto& themodel = *get_model();
    int row = -1;
    if (event->key() == Qt::Key_Left)
    {
        row = themodel.decrease_cursor();
    }
    else if (event->key() == Qt::Key_Right)
    {
        row = themodel.increase_cursor();
    }

    if (row != -1)
    {
        auto index = currentIndex();
        if (row == index.row())
        {
            update_complete_row(index);
        }
        else
        {
            int cursor = themodel.m_byte_cursor;
            update_complete_row(index);
            setCurrentIndex(model()->index(row, 0));
            themodel.m_byte_cursor = cursor;
        }
    }
    else
    {
        QTableView::keyPressEvent(event);
    }
}

void qbinarytableview::set_binary_data(const QByteArray &content)
{
    auto& themodel = *get_model();
    themodel.m_binary_content = content;
    update_rows();
    Q_EMIT publish_has_binary_content(true);
    Q_EMIT set_value(themodel.m_binary_content, 0);
}

const QByteArray& qbinarytableview::get_binary_data() const
{
    return get_model()->m_binary_content;
}

void qbinarytableview::clear_binary_content()
{
    auto& themodel = *get_model();
    themodel.m_binary_content.clear();
    Q_EMIT set_value(themodel.m_binary_content, 0);
    Q_EMIT publish_has_binary_content(false);
    themodel.m_start_offset = 0;
    Q_EMIT offset_changed(themodel.m_start_offset);
}

void qbinarytableview::set_columns(int ppl)
{
    get_model()->m_columns_per_row = ppl;
    update_rows();
}

int qbinarytableview::get_columns() const
{
    return get_model()->m_columns_per_row;
}

void qbinarytableview::set_type(int type)
{
    get_model()->m_display_type = static_cast<CDisplayType::eType>(type);
    update_rows();
}

int qbinarytableview::get_type() const
{
    return static_cast<int>(get_model()->m_display_type);
}

void qbinarytableview::set_offset(int offset)
{
    get_model()->m_start_offset = offset;
    update_rows();
    // TODO: update only visible rows?
//    int first = rowAt(rect().top());
//    int last  = rowAt(rect().bottom());
//    for (int row = first; row < last; ++row)
//    {
//        update_complete_row(model()->index(row, 0));
//    }
}

int qbinarytableview::get_offset() const
{
    return get_model()->m_start_offset;
}

void qbinarytableview::change_cursor()
{
    auto& themodel = *get_model();
    if (themodel.m_binary_content.size())
    {
        Q_EMIT set_value(themodel.m_binary_content, themodel.m_byte_cursor);
    }
}

void qbinarytableview::receive_value(const QByteArray &array, int position)
{
    auto& themodel = *get_model();
    if (array.size())
    {
        if (position + array.size() < themodel.m_binary_content.size())
        {
            for (int i=0; i<array.size(); ++i)
            {
                themodel.m_binary_content[position + i] = array[i];
            }
            int row = position / themodel.get_bytes_per_row();
            update_complete_row(model()->index(row, 0));
            Q_EMIT contentChanged();
        }
    }
    else
    {
        Q_EMIT set_value(themodel.m_binary_content, position);
    }
}

bool qbinarytableview::is_binary(QFile& file)
{
    bool binary = false;
    unsigned char buffer[16];
    auto read_bytes = file.peek(reinterpret_cast<char*>(&buffer[0]), sizeof(buffer));
    union unicode_id
    {
        std::uint16_t utf16;
        std::uint32_t utf32;
    };

    for (qint64 i=0; i<read_bytes; ++i)
    {
        if (buffer[i] == 0 || !isascii(buffer[i]))
        {
            if (i < 6)
            {
                unicode_id *unicode = reinterpret_cast<unicode_id*>(&buffer[0]);
                if (unicode->utf32 == 0xfffe0000 || unicode->utf32 == 0x0000fffe)
                {
                    break;  // UTF-32-LE | UTF-32-BE
                }
                else if (unicode->utf16 == 0xfffe || unicode->utf16 == 0xfeff)
                {
                    break;  // UTF-16-LE | UTF-16-BE
                }
                else if (buffer[0] == 0x2B && buffer[1] == 0x2F && buffer[2] == 0x76 && buffer[3] == 0x38 && buffer[4] == 0x2D )
                {
                    break;  // UTF-7 	endianless
                }
                else if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
                {
                    break;  // UTF-8 	endianless
                }
                else
                {
                    binary = true;
                }
            }
            else
            {
                binary = true;
            }
            break;
        }
    }
    return binary;
}

// model section
BinaryTableModel* qbinarytableview::get_model() const
{
    auto * themodel = dynamic_cast<BinaryTableModel*>(model());
    assert(themodel != nullptr);
    return themodel;
}

BinaryTableModel::BinaryTableModel(int rows, int columns, qbinarytableview *parent):
    QStandardItemModel(rows, columns, parent)
  , m_columns_per_row(4)
  , m_start_offset(0)
  , m_byte_cursor(0)
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
        case INT(Table::Character): return get_character_content(index.row()); break;
        case INT(Table::Typed):     return get_typed_content(index.row()); break;
        }
    }
    // roles requested here
    //  6: FontRole
    //  7: TextAlignmentRole
    //  9: ForegroundRole, TextColorRole
    // 10: CheckStateRole
    //  1: DecorationRole
    //  8: BackgroundRole
    //  TRACE(Logger::info, "row %d, column %d, role %d", index.row(), index.column(), role);
    return QStandardItemModel::data(index, role);
}

Qt::ItemFlags BinaryTableModel::flags( const QModelIndex &index) const
{
     Qt::ItemFlags flags = QStandardItemModel::flags(index);
     flags &= ~Qt::ItemIsEditable;
     return flags;
}


QVariant BinaryTableModel::get_typed_content(int row) const
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    const int bytes_per_row  = get_bytes_per_row();
    const int bytes_per_type = get_bytes_per_type();
    const bool is_hex_display = m_display_type >= CDisplayType::HEX2 && m_display_type <= CDisplayType::HEX16;
    const bool different_endian = CDisplayType::getDifferentEndian();
    if (is_hex_display)
    {
        CDisplayType::setDifferentEndian(!different_endian);
    }

    CDisplayType& thetype = *get_type();
    int start = row * bytes_per_row + m_start_offset;
    int end   = m_binary_content.size() - bytes_per_type;
    QString line;
    for (int i=0; i<m_columns_per_row && start <=end; ++i)
    {
        QString part = thetype.Display(&buffer_pointer[start]);
        if (m_byte_cursor >= start  && m_byte_cursor < (start+bytes_per_type))
        {
            if (is_hex_display)
            {
                int offset = (m_byte_cursor - start) * 2;
                part.insert(offset, '[');
                part.insert(offset+3, ']');
            }
            else
            {
                part.insert(0, '[');
                part.append(']');
            }
        }
        line += part;
        line += " ";
        start += bytes_per_type;
    }

    CDisplayType::setDifferentEndian(different_endian);
    return QVariant(line);
}

QVariant BinaryTableModel::get_character_content(int row) const
{
    const int bytes_per_row  = get_bytes_per_row();
    int start = row * bytes_per_row + m_start_offset;
    int end   = start + bytes_per_row;
    if (end > m_binary_content.size())
    {
        end = m_binary_content.size();
    }
    QString ascii_coded_line;
    for (int i=start; i< end; ++i)
    {
        const std::uint8_t byte = m_binary_content[i];
        const char character = (byte >= 32 && byte <= 127) ? byte : '.';
        if (i == m_byte_cursor)
        {
            ascii_coded_line.append('[');
            ascii_coded_line.append(character);
            ascii_coded_line.append(']');
        }
        else
        {
            ascii_coded_line.append(character);
        }
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
    return get_bytes_per_type() * m_columns_per_row;
}

CDisplayType* BinaryTableModel::get_type() const
{
    return m_display[m_display_type].get();
}

void BinaryTableModel::set_current_row(int row)
{
    m_byte_cursor = row * get_bytes_per_row();
    get_parent()->change_cursor();
}

int BinaryTableModel::increase_cursor()
{
    ++m_byte_cursor;
    if (m_byte_cursor >= m_binary_content.size() - m_start_offset)
    {
        m_byte_cursor = 0;
    }
    get_parent()->change_cursor();
    return m_byte_cursor / get_bytes_per_row();
}

int BinaryTableModel::decrease_cursor()
{
    if (m_byte_cursor > 0)
    {
        --m_byte_cursor;
    }
    get_parent()->change_cursor();
    return m_byte_cursor / get_bytes_per_row();
}

qbinarytableview* BinaryTableModel::get_parent()
{
    auto * theparent = dynamic_cast<qbinarytableview*>(parent());
    assert(theparent != 0);
    return theparent;
}
