#include "qbinarytableview.h"
#include "logger.h"
#include "helper.h"
#include <assert.h>
#include <QScrollBar>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

/// TODO: byte position change does not lead to correct row and column position when typed_display_values are shown

namespace
{
    /// brief set the temporary length for display with variable length
    /// @note this is used to determine the object length for initialization
    ///       and for displaying string, wstrings or binary values
    /// @param display binary data display object (CDisplayType*)
    /// @param length length for (int)
    /// - ascii string
    /// - unicode string
    /// - binary data
    /// @return the length of variable display type was set (bool)
    bool set_variable_display_type_length(CDisplayType* display, int length)
    {
        bool length_was_set = false;
        if (length && display)
        {
            length_was_set = true;
            switch (display->getType())
            {
            case CDisplayType::Ascii:
                display->SetBytes(length > 1 ? length : 0);
                break;
            case CDisplayType::Unicode:
                display->SetBytes(length > 1 ? length * 2 : 0);
                break;
            case CDisplayType::Binary:
                display->SetBytes(length);
                break;
            default:
                length_was_set = false;
                break;
            }
        }
        return length_was_set;
    }
}

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

void qbinarytableview::update_rows(bool refresh_all)
{
    auto& themodel = *get_model();
    if (refresh_all)
    {
        themodel.removeRows(0, themodel.rowCount());
        if (themodel.has_typed_display_values())
        {
            themodel.update_typed_display_rows();
        }
        else
        {
            themodel.insertRows(0, themodel.get_rows(), QModelIndex());
        }
    }
    else
    {
        int first = rowAt(rect().top());
        int last  = rowAt(rect().bottom());
        if (last == -1)
        {
            last = themodel.rowCount();
        }
        for (int row = first; row < last; ++row)
        {
            update_complete_row(row);
        }
    }
}

void qbinarytableview::update_complete_row(int row)
{
    for (int col=0; col<model()->columnCount(); ++col)
    {
        update(model()->index(row, col));
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
    update_complete_row(current.row());
    update_complete_row(previous.row());
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
            update_complete_row(row);
        }
        else
        {
            int cursor = themodel.m_byte_cursor;
            update_complete_row(index.row());
            setCurrentIndex(model()->index(row, 0));
            themodel.m_byte_cursor = cursor;
        }
    }
    else
    {
        QTableView::keyPressEvent(event);
    }
}

void qbinarytableview::mousePressEvent(QMouseEvent* event)
{
    auto& themodel = *get_model();

    const int bytes_per_row  = themodel.get_bytes_per_row();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int column         = columnAt(event->position().x());
    const int row            = rowAt(event->position().y());
#else
    const int column         = columnAt(event->x());
    const int row            = rowAt(event->y());
#endif
    QTableView::mousePressEvent(event);
    if (row < 0) return;

    float size = font().pixelSize();
    if (size < 0)
    {
        size = font().pointSizeF();
    }
    int x_position = column;
    for (int i=0; i<column;++i)
    {
        x_position -= columnWidth(i);
    }
    int char_position = INT(x_position / size + 0.5);

    int cursor = -1;
    if (themodel.has_typed_display_values())
    {
        if (column == INT(BinaryTableModel::Table::Typed))
        {
            auto string_left = themodel.get_typed_content(row).toString().left(char_position);
            int pos = string_left.count(" ");
            const auto& value = themodel.m_td_values[themodel.m_td_index[row]];
            int length = 0;
            themodel.display_type(value, row, &length);
            int bytes = pos * length;
            cursor = themodel.m_td_offset[row] + bytes;
        }
    }
    else if (column == INT(BinaryTableModel::Table::Character))
    {
        cursor = char_position + row * bytes_per_row;
    }
    else if (column == INT(BinaryTableModel::Table::Typed))
    {
        int pos   = themodel.get_typed_content(row).toString().left(char_position).count(" ");
        int bytes = INT(pos * themodel.get_type()->GetByteLength());
        cursor    = row * bytes_per_row + bytes;
    }

    if (cursor >= 0 && cursor != themodel.m_byte_cursor)
    {
        /// NOTE: byte cursor position is not perfect, but OK
        int old_row = themodel.m_byte_cursor / bytes_per_row;
        themodel.m_byte_cursor = cursor;
        update_complete_row(old_row);
        if (old_row != row)
        {
            update_complete_row(row);
        }
        change_cursor();
    }
}

void qbinarytableview::set_binary_data(const QByteArray &content)
{
    auto& themodel = *get_model();
    themodel.m_binary_content = content;
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(themodel.m_binary_content.data());
    CDisplayType::set_end_ptr(buffer_pointer + themodel.m_binary_content.size());

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
    themodel.clear_typed_display();
    themodel.m_binary_content.clear();
    CDisplayType::set_end_ptr(nullptr);
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
    return INT(get_model()->m_display_type);
}

void qbinarytableview::set_offset(int offset)
{
    get_model()->m_start_offset = offset;
    update_rows(false);
}

int qbinarytableview::get_offset() const
{
    return get_model()->m_start_offset;
}

void qbinarytableview::open_binary_format_file(const QString& filename, bool &opened)
{
    auto& themodel = *get_model();
    themodel.clear_typed_display();

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    bool file_parsing_ok = false;

    if (file.isOpen())
    {
        QJsonParseError error;
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            QString what = error.errorString();
            TRACEX(Logger::to_browser, data.left(error.offset-1) + "<< " + what + "\n" + data.right(data.size()-error.offset+1));
        }

        if (!doc.isEmpty())
        {
            if (doc.isObject())
            {
                QJsonObject obj      = doc.object();
                const auto file      = obj.take("file");
                auto       file_obj  = file.toObject();
                const auto file_keys = file_obj.keys();
                for (const auto& key : file_keys)
                {
                    // TRACEX(Logger::info, "- "<< key << ": "<< file_obj.take(key).toString());
                    if (key == "endian")
                    {
                        QString endian = file_obj.take(key).toString();
# if __BYTE_ORDER == __LITTLE_ENDIAN
                        bool changed = CDisplayType::setDifferentEndian(endian == "big");
#else
                        bool changed = CDisplayType::setDifferentEndian(endian == "little");
#endif
                        if (changed)
                        {
                            Q_EMIT endian_changed();
                        }
                    }
                }

                file_parsing_ok = true;
                /// parse structs before content
                auto structs = obj.take("structs");
                if (structs.isObject())
                {
                    auto struct_object = structs.toObject();
                    file_parsing_ok = themodel.insert_binary_structs(struct_object);
                    if (file_parsing_ok)
                    {
                        file_parsing_ok = themodel.update_binary_struct_dependencies();
                    }
                }

                QJsonValue content = obj.take("content");
                /// parse content
                if (file_parsing_ok && content.isArray())
                {
                    QJsonArray array = content.toArray();
                    for (int i=0; i<array.count(); ++i)
                    {
                        if (!themodel.insert_display_value(array[i]))
                        {
                            file_parsing_ok = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        update_rows();
    }
    if (file_parsing_ok)
    {
        update_rows();
    }
    opened = file_parsing_ok;
}

void qbinarytableview::update_table()
{
    update_rows();
}

void qbinarytableview::change_cursor()
{
    auto& themodel = *get_model();
    if (themodel.m_binary_content.size())
    {
        Q_EMIT set_value(themodel.m_binary_content, themodel.m_byte_cursor);
        Q_EMIT cursor_changed(themodel.m_byte_cursor);
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
            const auto list = selectedIndexes();
            if (list.size())
            {
                update_complete_row(list[0].row());
            }
            m_FileChanged = true;
            if (m_active)
            {
                Q_EMIT contentChanged(m_FileChanged);
            }
        }
    }
    else
    {
        Q_EMIT set_value(themodel.m_binary_content, position);
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool qbinarytableview::is_binary(QFile& file, std::optional<QStringConverter::Encoding>& encoding)
{
    bool binary = false;
    unsigned char buffer[16];
    auto read_bytes = file.peek(reinterpret_cast<char*>(&buffer[0]), sizeof(buffer));

    QStringDecoder decoder;
    encoding = decoder.encodingForData(QByteArrayView(buffer, read_bytes));
    if (!encoding.has_value())
    {
        for (qint64 i=0; i<read_bytes; ++i)
        {
            if ( !isascii(buffer[i]))
            {
                binary = true;
                break;
            }
        }
    }
    return binary;
}
#else
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
#endif
// model section

/// brief returns the member index for structs
/// @param row member index is determined for this row
/// @param [length] optional display type length for this row
/// @note the length is only returned, if available and wanted
/// @param [array_index] optional display type array_index for this row
/// @note the array_index is only returned, if available and wanted
/// @returns index for this row
int BinaryTableModel::DisplayValue::get_index(int row, int *length, int *array_index) const
{
    int index = 0;
    const auto member_index =  m_td_row_to_index.find(row);
    if (member_index != m_td_row_to_index.end())
    {
        index = member_index->second[r2i::index];
        if (length && r2i::lenght < member_index->second.size())
        {
            *length = member_index->second[r2i::lenght];
        }
        if (array_index && r2i::array_index < member_index->second.size())
        {
            *array_index = member_index->second[r2i::array_index];
        }
    }
    return index;
}

void BinaryTableModel::DisplayValue::set_index(int row, int index)
{
    auto member_index =  m_td_row_to_index.find(row);
    if (member_index != m_td_row_to_index.end())
    {
        member_index->second[r2i::index] = index;
    }
    else
    {
        m_td_row_to_index[row] = {index};
    }
}

void BinaryTableModel::DisplayValue::set_length(int row, int length)
{
    auto member_index =  m_td_row_to_index.find(row);
    if (member_index != m_td_row_to_index.end())
    {
        if (r2i::lenght >= member_index->second.size())
        {
            member_index->second.resize(r2i::lenght+1, 0);
        }
        member_index->second[r2i::lenght] = length;
    }
    else
    {
        m_td_row_to_index[row] = {0, length };
    }
}

void BinaryTableModel::DisplayValue::set_array_index(int row, int array_index)
{
    auto member_index =  m_td_row_to_index.find(row);
    if (member_index != m_td_row_to_index.end())
    {
        if (r2i::array_index >= member_index->second.size())
        {
            member_index->second.resize(r2i::array_index+1, 0);
        }
        member_index->second[r2i::array_index] = array_index;
    }
    else
    {
        m_td_row_to_index[row] = {0, 0, array_index};
    }
}

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
  , m_display_type(CDisplayType::HEX32)
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
    if (has_typed_display_values())
    {
        return get_typed_display_values(row);
    }
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    const int bytes_per_row  = get_bytes_per_row();
    const int bytes_per_type = INT(get_bytes_per_type());
    const bool is_hex_display = m_display_type >= CDisplayType::HEX8 && m_display_type <= CDisplayType::HEX64;
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
            /// TODO: show parts of the string with different colors
            /// label->setText(R"**(<span style="background-color:red;">00</span>-<span style="background-color:blue;">01</span>-02-03-04-05)**");
        }
        line += part;
        line += " ";
        start += bytes_per_type;
    }

    CDisplayType::setDifferentEndian(different_endian);
    return QVariant(line);
}

int BinaryTableModel::get_typed_display_array_length(const DisplayValue &value, int itdv, const std::vector<DisplayValue> &value_vector,
                                          const std::vector<int> &index_vector, const std::vector<int> &offset_vector)
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    int length = DisplayValue::default_length;
    if (value.array_length)
    {
        QJsonValue jv = value.array_length.value();
        switch (jv.type())
        {
        case QJsonValue::Type::Double:      // fixed lenght within value
            length = INT(jv.toDouble());
            break;
        case QJsonValue::String:            // length within data variable identified by name
        {
            QString name = jv.toString();
            bool is_byte_size_limit = false;
            if (name.indexOf('#') == 0)
            {
                is_byte_size_limit = true;
                name.remove(0, 1);
            }
            auto length_variable = std::find_if(value_vector.rbegin(), value_vector.rend(), [&](const DisplayValue& value)
            {
               return value.name.compare(name, Qt::CaseInsensitive) == 0;
            });
            if (length_variable != value_vector.rend())
            {
                size_t value_index = std::distance(value_vector.rend()-1, length_variable);
                if (INT(value_index) < itdv && length_variable->display)
                {
                    bool ok = false;
                    if (index_vector.size())
                    {
                        auto found_value_index = std::find(index_vector.begin(), index_vector.end(), value_index);
                        if (found_value_index != index_vector.end())
                        {
                            size_t index = std::distance(index_vector.begin(), found_value_index);
                            length = length_variable->display->Display(&buffer_pointer[offset_vector[index]]).toInt(&ok);
                            if (!ok) length = DisplayValue::invalid_length;
                        }
                    }
                    else
                    {
                        int index = INT(offset_vector.size() - (itdv - value_index));
                        length = length_variable->display->Display(&buffer_pointer[offset_vector[index]]).toInt(&ok);
                        if (!ok) length = DisplayValue::invalid_length;
                    }
                }
            }
            if (is_byte_size_limit && length > DisplayValue::default_length)
            {
                length = -length; // indicate byte size limit as negative length
            }
        }break;
        default:
            TRACEX(Logger::to_browser, "Error: : not a valid QJsonValue::type " << jv.type());
            break;
        }
    }
    return length;
}

void BinaryTableModel::update_typed_display_rows()
{
    m_td_index.clear();                     // cleanup positions
    m_td_offset.clear();                    // and offsets
    for (auto& td_struct : m_td_structs)
    {                                       // cleanup row to index map
        td_struct.second.m_td_row_to_index.clear();
    }

    int offset = 0;
    for (int itdv = 0; itdv < INT(m_td_values.size()); ++itdv)
    {
        auto& value = m_td_values[itdv];
        value.m_td_row_to_index.clear();    // cleanup row to index map
        int length = get_typed_display_array_length(value, itdv, m_td_values, m_td_index, m_td_offset);
        if (length == DisplayValue::invalid_length) break;
        std::vector<int> inserted_rows { -1 };
        update_typed_display_value(value, offset, length, itdv, inserted_rows);
        for (const auto& inserted_row: inserted_rows)
        {
            TRACEX(Logger::trace, "top inserted row: " << inserted_row);
        }
    }
    m_td_offset.push_back(offset);
    insertRows(0, INT(m_td_index.size()), QModelIndex());
}

void BinaryTableModel::update_typed_display_value(DisplayValue& value, int &offset, int length, int itdv, std::vector<int> &rows)
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    const bool is_byte_size_limit = length <= DisplayValue::byte_size_limit;
    if (value.display)
    {
        if (is_byte_size_limit)
        {
            length = -length;
        }
        else if (length == DisplayValue::is_struct_member)
        {
            length = DisplayValue::default_length;
        }
        else if (set_variable_display_type_length(value.display, length))
        {
            value.set_length(INT(m_td_index.size()), length);
            length = DisplayValue::default_length;
        }
        int index = length > 1 ? 0 : -1;
        while (length > 0 && offset < m_binary_content.size())
        {
            int byte_length = INT(value.display->GetByteLength(&buffer_pointer[offset]) * std::min(length, m_columns_per_row));
            if (index >= 0)
            {
                value.set_array_index(INT(m_td_index.size()), index);
                index += m_columns_per_row;
            }
            m_td_index.push_back(itdv);
            m_td_offset.push_back(offset);
            offset += byte_length;
            if (is_byte_size_limit)
            {
                length -= m_columns_per_row * offset;
            }
            else
            {
                length -= m_columns_per_row;
            }
        }
    }
    else
    {
        const auto struct_iter = m_td_structs.find(value.name);
        if (struct_iter != m_td_structs.end())
        {
            int offset_limit = m_binary_content.size();
            if (is_byte_size_limit)
            {
                length = -length;
                offset_limit = offset + length;
            }
            DisplayValue& structure = struct_iter->second;
            for (int index = 0; index < length && offset < offset_limit; ++index)
            {
                for (std::uint32_t member = 0; member < structure.member.size(); ++member)
                {
                    if (length > 1)
                    {
                        structure.set_array_index(INT(m_td_index.size()), index);
                    }
                    int member_length = DisplayValue::default_length;
                    if (member)
                    {
                        const int row = INT(m_td_index.size());
                        structure.set_index(row, INT(member));
                        rows.push_back(row);
                        member_length = get_typed_display_array_length(structure.member[member], member, structure.member, {}, m_td_offset);
                        if (member_length > DisplayValue::default_length)
                        {   // store length of member
                            structure.set_length(row, member_length);
                        }
                    }

                    if (set_variable_display_type_length(structure.member[member].display, member_length))
                    {   // variable display type length was set for correct offset
                        member_length = DisplayValue::is_struct_member;
                    }
                    std::vector<int> inserted_rows;
                    update_typed_display_value(structure.member[member], offset, member_length, itdv, inserted_rows);
                    for (const auto& inserted_row: inserted_rows)
                    {
                        structure.set_index(inserted_row, INT(member));
                        TRACEX(Logger::info, "inserted row: "<< inserted_row << " for " << member << " of " << structure.name);
                    }
                    /// NOTE: check update also parent struct, if member is > 0
                    if (member)
                    {
                        rows.insert(rows.end(), inserted_rows.begin(), inserted_rows.end());
                    }
                }
            }
        }
    }
}

bool BinaryTableModel::has_typed_display_values() const
{
    return m_display_type == CDisplayType::FormatFile && m_td_values.size() > 0;
}

void BinaryTableModel::clear_typed_display()
{
    m_td_values.clear();
    m_td_structs.clear();
    m_td_offset.clear();
    m_td_index.clear();
}

bool BinaryTableModel::insert_binary_structs(QJsonObject& structs_obj)
{
    const auto keys = structs_obj.keys();
    for (const auto& key : keys)
    {
        DisplayValue display_value;
        display_value.name = key;
        const auto structs_children = structs_obj.take(key);
        if (structs_children.isArray())
        {
            const auto structs_children_array = structs_children.toArray();
            for (const auto& element : structs_children_array)
            {
                if (!insert_display_value(element, &display_value.member))
                {
                    return false;
                }
            }
        }
        m_td_structs[key] = display_value;
    }
    return true;
}

bool BinaryTableModel::update_binary_struct_dependencies()
{
    /// check structs for sub content
    for (auto& struct_obj : m_td_structs)
    {
        if (struct_obj.second.member.empty())
        {
            auto stored_struct = m_td_structs.find(struct_obj.second.name);
            if (stored_struct != m_td_structs.end())
            {                               // store struct members, if already there
                struct_obj.second.name = stored_struct->second.name;
                struct_obj.second.member.push_back(stored_struct->second.member.front());
            }
            else
            {
                TRACEX(Logger::to_browser, "Error: struct "<< struct_obj.second.name << " is not defined or has wrong name");
                return false;
            }
        }
    }
    return true;
}

bool  BinaryTableModel::insert_display_value(const QJsonValue& jval, std::vector<DisplayValue>* dv)
{
    if (dv == 0) dv = &m_td_values;
    DisplayValue value;
    QString type_name;
    if (jval.isArray()) // array may have more parameter
    {                                     // name, type name, array length
        value.name = jval.toArray()[0].toString();  // variable name
        type_name  = jval.toArray()[1].toString();  // type name
        if (jval.toArray().count() > 2)
        {
            value.array_length = jval.toArray()[2]; // optional array length
        }
    }
    else if (jval.isObject()) // object is a pair of variable name and type name
    {
        auto       obj  = jval.toObject();
        const auto keys = obj.keys();
        value.name      =  keys[0];           // variable name
        type_name = obj.take(value.name).toString(); // type name
        if (obj.count() > 1)              // this is a syntax error
        {
            TRACE(Logger::to_browser, "Error: more than one variable:");
            for (const auto& key : keys)
            {
                TRACEX(Logger::to_browser, "- " << key);
            }
            return false;
        }
    }
    else
    {
        TRACE(Logger::to_browser, "Error: : not a variable or struct");
        return false;
    }

    CDisplayType::eType type = CDisplayType::getTypeOfName(type_name);
    if (type != CDisplayType::Unknown && m_display.count(type))  // determine display type
    {
        value.display = m_display[type].get();
    }
    else                                // unknown display type is a struct
    {
        value.display = 0;              // type is empty, name is stored
        auto stored_struct = m_td_structs.find(type_name);
        value.name   = type_name;
        if (stored_struct != m_td_structs.end())
        {   // store first struct member, if struct is already there
            value.member.push_back(stored_struct->second.member.front());
        }
    }
    dv->push_back(value);
    return true;
}

QString  BinaryTableModel::display_typed_value(const DisplayValue& value, int row, int length) const
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    QString display_value;
    if (value.display)
    {
        bool is_variable_length_display_type = set_variable_display_type_length(value.display, length);
        int array_index = -1;
        value.get_index(row, nullptr, &array_index);
        if (array_index >= 0)
        {
            display_value = value.name + tr("[%1]: ").arg(array_index) + value.display->Display(&buffer_pointer[m_td_offset[row]]);
        }
        else
        {
            display_value = value.name + ": " + value.display->Display(&buffer_pointer[m_td_offset[row]]);
        }
        if (!is_variable_length_display_type && value.array_length)
        {
            const int next_offset = m_td_offset[row+1];
            const int value_bytes = INT(value.display->GetByteLength());
            int offset            = m_td_offset[row] + value_bytes;
            for (int i=1; i<m_columns_per_row && offset < next_offset; ++i)
            {
                display_value += " " + value.display->Display(&buffer_pointer[offset]);
                offset += value_bytes;
            }
        }
    }
    else
    {
        const auto struct_iter = m_td_structs.find(value.name);
        if (struct_iter != m_td_structs.end())
        {
            const DisplayValue& structure = struct_iter->second;
            int length = 1;
            int array_index = -1;
            int index = structure.get_index(row, &length, &array_index);
            if (index < INT(structure.member.size()))
            {
                if (array_index >= 0)
                {
                    display_value = structure.name+ tr("[%1]::").arg(array_index) + display_typed_value(structure.member[index], row, length);
                }
                else
                {
                    display_value = structure.name + "::" + display_typed_value(structure.member[index], row, length);
                }
            }
        }
    }
    return display_value;
}

QVariant BinaryTableModel::get_typed_display_values(int row) const
{
    QString display_value {"n/a"};
    if (row >= 0 && row < INT(m_td_index.size()) && m_td_offset[row] <= m_binary_content.size())
    {
        const auto& value = m_td_values[m_td_index[row]];
        int length = 1;
        value.get_index(row, &length);
        display_value = display_typed_value(value, row, length);
    }
    return QVariant(display_value);
}

QString  BinaryTableModel::display_type(const DisplayValue& value, int row, int *length) const
{
    if (value.display)
    {
        if (length)
        {
            const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
            *length = INT(value.display->GetByteLength(&buffer_pointer[m_td_offset[row]]));
        }
        return value.display->Type();
    }
    else
    {
        const auto struct_iter = m_td_structs.find(value.name);
        if (struct_iter != m_td_structs.end())
        {
            const DisplayValue& structure = struct_iter->second;
            int index = structure.get_index(row);
            if (index < INT(structure.member.size()))
            {
                return display_type(structure.member[index], row, length);
            }
        }
    }
    return "";
}

QVariant BinaryTableModel::get_character_content(int row) const
{
    if (has_typed_display_values())
    {
        const auto& value = m_td_values[m_td_index[row]];
        return QVariant(display_type(value, row));
    }
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
            //ascii_coded_line.append("<font color=\"red\">");
            ascii_coded_line.append(character);
            ascii_coded_line.append(']');
            //ascii_coded_line.append("</font>");
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
    if (has_typed_display_values())
    {
        return rowCount();
    }
    else
    {
        int rows = m_binary_content.size() / get_bytes_per_row() + 1;
        return rows;
    }
}

size_t BinaryTableModel::get_bytes_per_type() const
{
    return get_type()->GetByteLength();
}

int BinaryTableModel::get_bytes_per_row() const
{
    if (m_columns_per_row)
    {
        return INT(get_bytes_per_type() * m_columns_per_row);
    }
    return 1;
}

CDisplayType* BinaryTableModel::get_type() const
{
    if (m_display.count(m_display_type))
    {
        return m_display[m_display_type].get();
    }
    /// NOTE: smallest type is fallback
    return m_display[CDisplayType::UChar].get();
}

void BinaryTableModel::set_current_row(int row)
{
    if (has_typed_display_values())
    {
        m_byte_cursor = m_td_offset[row];
    }
    else
    {
        m_byte_cursor = row * get_bytes_per_row();
    }
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
