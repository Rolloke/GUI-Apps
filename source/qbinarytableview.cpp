#include "qbinarytableview.h"
#include "logger.h"
#include <assert.h>
#include <QScrollBar>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define INT(X) static_cast<int>(X)

/// TODO: validate litle and big endian representation

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
    bool call_press_event { true };
    auto& themodel = *get_model();

    if (themodel.has_typed_display_values())
    {
        /// TODO: 4. evaluate mousePressEvent for typed display values
        QTableView::mousePressEvent(event);
        return;
    }
    const int bytes_per_row  = themodel.get_bytes_per_row();
    const int column         = columnAt(event->x());
    const int row            = rowAt(event->y());

    float size = font().pixelSize();
    if (size < 0)
    {
        size = font().pointSizeF();
    }
    int x_position = event->x();
    for (int i=0; i<column;++i)
    {
        x_position -= columnWidth(i);
    }
    int char_position = static_cast<int>(x_position / size + 0.5);

    int cursor = -1;
    if (column == INT(BinaryTableModel::Table::Character))
    {
        cursor = char_position + row * bytes_per_row;
    }
    else if (column == INT(BinaryTableModel::Table::Typed))
    {
        int pos = themodel.get_typed_content(row).toString().leftRef(char_position).count(" ");
        int bytes = pos * themodel.get_type()->GetByteLength();
        cursor = row * bytes_per_row + bytes;
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
        call_press_event = false;
        change_cursor();
    }
    if (call_press_event)
    {
        QTableView::mousePressEvent(event);
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
    update_rows(false);
}

int qbinarytableview::get_offset() const
{
    return get_model()->m_start_offset;
}

void qbinarytableview::open_binary_format_file(const QString& filename, bool &opened)
{
    auto& themodel = *get_model();
    themodel.m_td_values.clear();
    themodel.m_td_structs.clear();
    themodel.m_td_offset.clear();
    themodel.m_td_index.clear();

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
            QString string = data.left(error.offset-1) + "<< " + what + "\n" + data.right(data.size()-error.offset+1);
            TRACE(Logger::to_browser, string.toStdString().c_str());
        }

        if (!doc.isEmpty())
        {
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                auto file      = obj.take("file");
                auto file_obj  = file.toObject();
                auto file_keys = file_obj.keys();
                for (auto& key : file_keys)
                {
                    TRACE(Logger::info, "- %s: %s", key.toStdString().c_str(), file_obj.take(key).toString().toStdString().c_str());
                    if (key == "endian")
                    {
                        /// TODO: set endianess
                    }
                }

                file_parsing_ok = true;
                /// parse structs before content
                auto structs = obj.take("structs");
                if (structs.isObject())
                {
                    auto structs_obj = structs.toObject();
                    auto keys = structs_obj.keys();
                    for (const auto& key : keys)
                    {
                        DisplayValue display_value;
                        display_value.name = key;
                        auto structs_children = structs_obj.take(key);
                        if (structs_children.isArray())
                        {
                            auto structs_children_array = structs_children.toArray();
                            for (const auto& element : structs_children_array)
                            {
                                if (!themodel.insert_display_value(element, &display_value.member))
                                {
                                    file_parsing_ok = false;
                                    break;
                                }
                            }
                        }
                        if (!file_parsing_ok) break;
                        themodel.m_td_structs[key] = display_value;
                    }
                    if (file_parsing_ok)
                    {
                        /// check structs for sub content
                        for (auto& struct_obj : themodel.m_td_structs)
                        {
                            if (struct_obj.second.member.empty())
                            {
                                auto stored_struct = themodel.m_td_structs.find(struct_obj.second.name);
                                if (stored_struct != themodel.m_td_structs.end())
                                {                               // store struct members, if already there
                                    struct_obj.second.name = stored_struct->second.name;
                                    struct_obj.second.member.push_back(stored_struct->second.member.front());
                                }
                                else
                                {
                                    file_parsing_ok = false;
                                    TRACE(Logger::to_browser, "Error: struct %s is not defined or has wrong name", struct_obj.second.name.toStdString().c_str());
                                    break;
                                }
                            }
                        }
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

bool  BinaryTableModel::insert_display_value(const QJsonValue& jval, std::vector<DisplayValue>* dv)
{
    if (dv == 0) dv = &m_td_values;
    DisplayValue value;
    QString type_name;
    if (jval.isArray()) // array may have more parameter
    {                                     // name, type name, array length
        value.name = jval[0].toString();  // variable name
        type_name  = jval[1].toString();  // type name
        if (jval.toArray().count() > 2)
        {
            value.array_length = jval[2]; // optional array length
        }
    }
    else if (jval.isObject()) // object is a pair of variable name and type name
    {
        auto obj  = jval.toObject();
        auto keys = obj.keys();
        value.name  =  keys[0];           // variable name
        type_name = obj.take(value.name).toString(); // type name
        if (obj.count() > 1)              // this is a syntax error
        {
            TRACE(Logger::to_browser, "Error: more than one variable:");
            for (auto& key : keys)
            {
                TRACE(Logger::to_browser, "- ", key.toStdString().c_str());
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

            if (themodel.has_typed_display_values())
            {
                /// TODO: 5. determine update row for has_typed_display_values
            }
            else
            {
                int row = position / themodel.get_bytes_per_row();
                update_complete_row(row);
                Q_EMIT contentChanged();
            }
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
  , m_display_type(CDisplayType::HEX64)
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
    const int bytes_per_type = get_bytes_per_type();
    const bool is_hex_display = m_display_type >= CDisplayType::HEX16 && m_display_type <= CDisplayType::HEX128;
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

int BinaryTableModel::get_td_array_length(int itdv)
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    int length = 1;
    const auto& value = m_td_values[itdv];
    if (value.array_length.has_value())
    {
        QJsonValue jv = value.array_length.value();
        switch (jv.type())
        {
        case QJsonValue::Type::Double:
            length = static_cast<int>(jv.toDouble());
            break;
        case QJsonValue::String:
        {
            QString name = jv.toString();
            /// TODO search backward
            auto length_variable = std::find_if(m_td_values.rbegin(), m_td_values.rend(), [&](const DisplayValue& value)
            {
               return value.name.compare(name, Qt::CaseInsensitive) == 0;
            });
            if (length_variable != m_td_values.rend())
            {
                size_t value_index = std::distance(m_td_values.rend()-1, length_variable);
                if (static_cast<int>(value_index) < itdv && length_variable->display)
                {
                    bool ok = false;
                    /// TODO: evaluate length read from binary file (right offset value?)
                    auto found_value_index = std::find(m_td_index.begin(), m_td_index.end(), value_index);
                    if (found_value_index != m_td_index.end())
                    {
                        size_t index = std::distance(m_td_index.begin(), found_value_index);
                        length = length_variable->display->Display(&buffer_pointer[m_td_offset[index]]).toInt(&ok);
                        if (!ok) length = 0;
                    }
                }
            }
        }break;
        default:
            TRACE(Logger::to_browser, "Error: : not a valid QJsonValue::type %d", jv.type());
            break;
        }
    }
    return length;
}

void BinaryTableModel::update_typed_display_rows()
{
    m_td_index.clear();
    m_td_offset.clear();
    int offset = 0;
    for (size_t itdv = 0; itdv < m_td_values.size(); ++itdv)
    {
        int length = get_td_array_length(itdv);
        if (!length) break;
        const auto& value = m_td_values[itdv];
        update_typed_display_value(value, offset, length, itdv);
    }
    m_td_offset.push_back(offset);
    insertRows(0, m_td_index.size(), QModelIndex());
}

void BinaryTableModel::update_typed_display_value(const DisplayValue& value, int &offset, int length, int itdv)
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    if (value.display)
    {
        switch (value.display->getType())
        {
        case CDisplayType::Ascii:
            value.display->SetBytes(length > 1 ? length : 0);
            length = 1;
            break;
        case CDisplayType::Unicode:
            value.display->SetBytes(length > 1 ? length * 2 : 0);
            length = 1;
            break;
        case CDisplayType::Binary:
            value.display->SetBytes(length);
            length = 1;
            break;
        default:
            break;
        }
        while (length > 0 && offset < m_binary_content.size())
        {
            int byte_length = value.display->GetByteLength(&buffer_pointer[offset]) * std::min(length, m_columns_per_row);
            m_td_index.push_back(itdv);
            m_td_offset.push_back(offset);
            offset += byte_length;
            length -= m_columns_per_row;
        }
    }
    else
    {
        const auto struct_iter = m_td_structs.find(value.name);
        if (struct_iter != m_td_structs.end())
        {
            DisplayValue& structure = struct_iter->second;
            while (length > 0 && offset < m_binary_content.size())
            {
                for (std::uint32_t member = 0; member < structure.member.size(); ++member)
                {
                    if (member)
                    {
                        structure.m_td_row_to_member[m_td_index.size()] = member;
                    }
                    update_typed_display_value(structure.member[member], offset, 1, itdv);
                }
                --length;
            }
        }
        /// TODO: 1. update rows and offsets for strings
    }

}

bool BinaryTableModel::has_typed_display_values() const
{
    return m_display_type == CDisplayType::FormatFile && m_td_values.size() > 0;
}

QString  BinaryTableModel::display_typed_value(const DisplayValue& value, int row) const
{
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(m_binary_content.data());
    QString display_value;
    if (value.display)
    {
        display_value = value.name + ": " + value.display->Display(&buffer_pointer[m_td_offset[row]]);
        if (value.array_length.has_value())
        {
            const int next_offset = m_td_offset[row+1];
            const int value_bytes = value.display->GetByteLength();
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
            const auto member_index =  structure.m_td_row_to_member.find(row);
            int index = 0;
            if (member_index != structure.m_td_row_to_member.end())
            {
                index = member_index->second;
            }
            if (index < static_cast<int>(structure.member.size()))
            {
                display_value = structure.name + "::" + display_typed_value(structure.member[index], row);
            }
        }
        /// TODO: 2. get display values for strings
    }
    return display_value;
}


QVariant BinaryTableModel::get_typed_display_values(int row) const
{
    QString display_value {"n/a"};
    if (row < static_cast<int>(m_td_index.size()) && m_td_offset[row] <= m_binary_content.size())
    {
        const auto& value = m_td_values[m_td_index[row]];
        display_value = display_typed_value(value, row);
    }
    return QVariant(display_value);
}

QString  BinaryTableModel::display_type(const DisplayValue& value, int row) const
{
    if (value.display)
    {
        return value.display->Type();
    }
    else
    {
        const auto struct_iter = m_td_structs.find(value.name);
        if (struct_iter != m_td_structs.end())
        {
            const DisplayValue& structure = struct_iter->second;
            const auto member_index = structure. m_td_row_to_member.find(row);
            int index = 0;
            if (member_index != structure.m_td_row_to_member.end())
            {
                index = member_index->second;
            }
            if (index < static_cast<int>(structure.member.size()))
            {
                return display_type(structure.member[index], row);
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
        return get_bytes_per_type() * m_columns_per_row;
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
