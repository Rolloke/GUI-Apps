#include "binary_values_view.h"
#include "ui_binary_values_view.h"
#include "DisplayType.h"
#include "helper.h"
#include <QFileDialog>
#include <QClipboard>
#include <QScrollBar>
#include <QTextStream>

/// TODO: [x] Suche nach Tabellenwerten mit CheckBox aktivierbar
/// TODO: [x] Suche nach Tabellenwerten implementieren
/// TODO: [ ] Binär Tabelle als Tree
/// TODO: [ ] JSON Strukturen mit ID (integer, string) wählbar
/// TODO: [x] String Datentyp mit Größe
/// TODO: [x] enable disable buttons by context

binary_values_view::binary_values_view(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::binary_values_view)
  , m_display(CDisplayType::get_type_map())
  , m_current_position(0)
  , m_setting_table_property(false)
{
    ui->setupUi(this);

    notify_endian_changed();

    QString tooltip = ui->checkUnsignedByte->toolTip();
    /// NOTE: insert the datatype checkboxes in the same order as datatypes in the m_display map
    m_Checkboxes.push_back(ui->checkSignedByte);
    m_Checkboxes.push_back(ui->checkUnsignedByte);
    m_Checkboxes.push_back(ui->checkSignedShort);
    m_Checkboxes.push_back(ui->checkUnsignedShort);
    m_Checkboxes.push_back(ui->checkSignedLong);
    m_Checkboxes.push_back(ui->checkUnsignedLong);
    m_Checkboxes.push_back(ui->checkSignedLongLong);
    m_Checkboxes.push_back(ui->checkUnsignedLongLong);
    m_Checkboxes.push_back(ui->checkFloat);
    m_Checkboxes.push_back(ui->checkDouble);
    m_Checkboxes.push_back(ui->checkHexByte);
    m_Checkboxes.push_back(ui->checkHexShort);
    m_Checkboxes.push_back(ui->checkHexLong);
    m_Checkboxes.push_back(ui->checkHexLongLong);
    m_Checkboxes.push_back(ui->checkBinary);
    m_Checkboxes.push_back(ui->checkChar);

    for (auto& checkbox : m_Checkboxes)
    {
        checkbox->setToolTip(tooltip);
        connect(checkbox, SIGNAL(clicked(bool)), this, SLOT(check_box_clicked(bool)));
    }

    tooltip = ui->edtUnsignedByte->toolTip();
    /// NOTE: insert the datatype edit fields in the same order as the datatype in the m_display map
    m_Edit.push_back(ui->edtSignedByte);
    m_Edit.push_back(ui->edtUnsignedByte);
    m_Edit.push_back(ui->edtSignedShort);
    m_Edit.push_back(ui->edtUnsignedShort);
    m_Edit.push_back(ui->edtSignedLong);
    m_Edit.push_back(ui->edtUnsignedLong);
    m_Edit.push_back(ui->edtSignedLongLong);
    m_Edit.push_back(ui->edtUnsignedLongLong);
    m_Edit.push_back(ui->edtFloat);
    m_Edit.push_back(ui->edtDouble);
    m_Edit.push_back(ui->edtHexByte);
    m_Edit.push_back(ui->edtHexShort);
    m_Edit.push_back(ui->edtHexLong);
    m_Edit.push_back(ui->edtHexLongLong);
    m_Edit.push_back(ui->edtBinary);
    m_Edit.push_back(ui->edtChar);

    for (auto& edit : m_Edit)
    {
        edit->setToolTip(tooltip);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(editing_finished()));
    }
    for (const auto& type : std::as_const(m_display))
    {
        if (type.first == CDisplayType::Binary) continue;
        if (type.first == CDisplayType::Ascii) continue;
        if (type.first == CDisplayType::Unicode) continue;
        ui->comboType->addItem(CDisplayType::getNameOfType(type.first));
    }
    ui->comboType->addItem(CDisplayType::getNameOfType(CDisplayType::FormatFile));

    connect(ui->comboType, SIGNAL(currentTextChanged(QString)), this, SLOT(table_type_changed(QString)));
    connect(ui->spinColumns, SIGNAL(valueChanged(int)), this, SLOT(table_columns_changed(int)));
    connect(ui->spinOffset, SIGNAL(valueChanged(int)), this, SLOT(table_offset_changed(int)));

    LinkFilter * filter = new LinkFilter(this);
    installEventFilter(filter);
    connect(filter, SIGNAL(linkClicked(QString)), this, SLOT(showLink(QString)));

    ui->btnOpenTypeFile->setWhatsThis(""
       "<pre>{\n"
       "\"file\":{ // optional file parameter section\n"
       "   \"description\": \"file description\",\n"
       "      \"mime\"       : \"mymime\",\n"
       "      \"extension\"  : \"ext\",\n"
       "      \"endian\"     : \"big\"\n"
       "    },\n"
       "    \"content\":[ // content of file in an array\n"
       "       {\"name\": \"long\"},                   // simple name and type\n"
       "       [\"name\", \"float\", 8],               // float array with size 8\n"
       "       {\"sizedouble\", \"int32\"},            // size variable for double array\n"
       "       [\"darray\", \"double\", \"sizedouble\"], // variable double array\n"
       "       {\"text string\": \"String\"},          // struct \"String\" see below\n"
       "       {\"bytes\": \"uint32\"},                // simple name and type\n"
       "       [\"some strings\", \"String\", \"#bytes\"]// strings with max bytes to read\n"
       "    ],\n"
       "    \"structs\":{ // optional structs section\n"
       "       \"String\": [  // structure for strings, name is used like a type\n"
       "          [\"length\", \"uint32\" ],           // length of string\n"
       "          [\"text\", \"string\", \"length\"]     // variable length string type\n"
       "       ]\n"
       "    }\n"
       "}\n"
       "</pre>"
       "<a href='btnOpenTypeFile'>Copy text</a>");

    for (const auto& type : std::as_const(m_display))
    {
        if (type.first == CDisplayType::Binary) continue;
        ui->comboDataType2insert->addItem(CDisplayType::getNameOfType(type.first));
    }

    mListModel = new CheckboxItemModel(0, 4, this);
    ui->tableView->setModel(mListModel);
    mListModel->setCheckedColumn(Table::checkbox);
    mListModel->setHeaderData(Table::checkbox,  Qt::Horizontal, tr("Select"), Qt::CheckStateRole);
    mListModel->setHeaderData(Table::name,  Qt::Horizontal, tr("Name"), Qt::DisplayRole);
    mListModel->setHeaderData(Table::type,  Qt::Horizontal, tr("Type"), Qt::DisplayRole);
    mListModel->setHeaderData(Table::value, Qt::Horizontal, tr("Value"), Qt::DisplayRole);

    const int column_width = 200;

    ui->tableView->setColumnWidth(Table::checkbox, 30);
    ui->tableView->horizontalHeader()->setSectionResizeMode(Table::checkbox, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(Table::name    , column_width);
    ui->tableView->horizontalHeader()->setSectionResizeMode(Table::name    , QHeaderView::ResizeToContents);
    ui->tableView->setColumnWidth(Table::type    , column_width);
    ui->tableView->horizontalHeader()->setSectionResizeMode(Table::type    , QHeaderView::Fixed);
    ui->tableView->setColumnWidth(Table::value   , column_width);
    ui->tableView->horizontalHeader()->setSectionResizeMode(Table::value   , QHeaderView::Stretch);
}

void binary_values_view::showLink(const QString& link_name)
{
    if (link_name == "btnOpenTypeFile")
    {
        QString text = ui->btnOpenTypeFile->whatsThis();
        int start = text.indexOf("{");
        int end   = text.lastIndexOf("}");
        QApplication::clipboard()->setText(text.mid(start, end-start + 1));
    }
}

binary_values_view::~binary_values_view()
{
    delete ui;
}

std::uint32_t binary_values_view::type2index(CDisplayType::eType type)
{
    auto found = std::find_if(m_display.begin(), m_display.end(), [type](const auto& display) { return display.first == type; });
    return std::distance(m_display.begin(), found);
}

CDisplayType::eType binary_values_view::index2type(std::uint32_t index)
{
    if (index < m_display.size())
    {
        auto display = m_display.begin();
        std::advance(display, index);
        return display->first;
    }
    return CDisplayType::Unknown;
}

void binary_values_view::receive_value(const QByteArray &array, int position)
{
    QString text;
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(array.data());
    m_current_position = position;
    const bool stand_alone_view = ui->checkStandAlone->isChecked();
    const auto checked_box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox) { return checkbox->isChecked();} );
    const bool any_checked = (checked_box != m_Checkboxes.end());

    if (!stand_alone_view)
    {
        ui->btnWriteValue->setEnabled(any_checked);
    }
    int binary_length_index = ui->comboType->currentIndex();
    if (binary_length_index < 0)
    {
        ui->comboType->setCurrentText(CDisplayType::getNameOfType(CDisplayType::HEX32));
        binary_length_index = ui->comboType->currentIndex();
    }
    if (any_checked)
    {
        binary_length_index = std::distance(m_Checkboxes.begin(), checked_box);
    }
    m_display[CDisplayType::Binary]->SetBytes(static_cast<int>(m_display[index2type(binary_length_index)]->GetByteLength()));

    for (auto& display : m_display )
    {
        auto index = type2index(display.first);
        if (index >= static_cast<std::uint32_t>(m_Checkboxes.size())) continue;

        if (stand_alone_view)
        {
            if (m_Checkboxes[index]->isChecked())
            {
                continue;
            }
            if (static_cast<int>(display.second->GetByteLength()) == array.size())
            {
                text = display.second->Display(buffer_pointer);
            }
            else
            {
                text.clear();
            }
        }
        else if (position + static_cast<int>(display.second->GetByteLength()) <= array.size())
        {
            text = display.second->Display(&buffer_pointer[position]);
        }
        else
        {
            text.clear();
            m_Checkboxes[index]->setChecked(false);
        }
        switch (display.first)
        {
        case CDisplayType::Char: ui->edtSignedByte->setText(text); break;
        case CDisplayType::UChar: ui->edtUnsignedByte->setText(text); break;
        case CDisplayType::Short: ui->edtSignedShort->setText(text); break;
        case CDisplayType::UShort: ui->edtUnsignedShort->setText(text); break;
        case CDisplayType::Long: ui->edtSignedLong->setText(text); break;
        case CDisplayType::ULong: ui->edtUnsignedLong->setText(text); break;
        case CDisplayType::LongLong: ui->edtSignedLongLong->setText(text); break;
        case CDisplayType::ULongLong: ui->edtUnsignedLongLong->setText(text); break;
        case CDisplayType::HEX8: ui->edtHexByte->setText(text); break;
        case CDisplayType::HEX16: ui->edtHexShort->setText(text); break;
        case CDisplayType::HEX32: ui->edtHexLong->setText(text); break;
        case CDisplayType::HEX64: ui->edtHexLongLong->setText(text); break;
        case CDisplayType::Float: ui->edtFloat->setText(text); break;
        case CDisplayType::Double: ui->edtDouble->setText(text); break;
        case CDisplayType::Binary: ui->edtBinary->setText(text); break;
        case CDisplayType::Ascii: ui->edtChar->setText(text); break;
        default:
            break;
        }
    }

    const int rows = mListModel->rowCount();
    for (int row = 0; row < rows; ++row)
    {
        const QString type_name = mListModel->data(mListModel->index(row, Table::type)).toString();
        QStringList type_name_parts = type_name.split("[");
        const auto type_id = CDisplayType::getTypeOfName(type_name_parts[0]);
        auto &display_type = m_display[type_id];
        if (type_name_parts.length() > 1)
        {
            type_name_parts[1].remove("]");
            display_type->SetBytes(type_id == CDisplayType::Ascii ? type_name_parts[1].toInt() : type_name_parts[1].toInt()*2);
        }
        const auto length = display_type->GetByteLength();
        if (position + static_cast<int>(length) <= array.size())
        {
            if (!mListModel->data(mListModel->index(row, Table::checkbox), Qt::CheckStateRole).toBool())
            {
                text = display_type->Display(&buffer_pointer[position]);
                mListModel->setData(mListModel->index(row, Table::value), text, Qt::DisplayRole);
            }
            position += length;
        }
        else
        {
            mListModel->setData(mListModel->index(row, Table::value), "n/a", Qt::DisplayRole);
        }
    }
}

void binary_values_view::receive_external_data(bool external_data)
{
    ui->checkStandAlone->setChecked(!external_data);
    ui->btnOpenTypeFile->setEnabled(external_data);
}

void binary_values_view::on_btnReadValue_clicked()
{
    QByteArray array;
    Q_EMIT set_value(array, m_current_position);
}

void binary_values_view::on_btnWriteValue_clicked()
{
    auto checked_box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox) { return checkbox->isChecked();} );
    if (checked_box != m_Checkboxes.end())
    {
        int index = std::distance(m_Checkboxes.begin(), checked_box);
        QString text = m_Edit[index]->text();
        if (text.size())
        {
            QByteArray array;
            auto type = index2type(index);
            array.resize(static_cast<int>(m_display[type]->GetByteLength()));
            m_display[type]->Write(reinterpret_cast<std::uint8_t*>(array.data()), text);
            Q_EMIT set_value(array, m_current_position);
            receive_value(array, 0);
        }
    }
}

void binary_values_view::on_radioEndian1_clicked()
{
# if __BYTE_ORDER == __LITTLE_ENDIAN
    CDisplayType::setDifferentEndian(false);
#else
    CDisplayType::setDifferentEndian(true);
#endif
    Q_EMIT endian_changed();
}

void binary_values_view::on_radioEndian2_clicked()
{
# if __BYTE_ORDER == __LITTLE_ENDIAN
    CDisplayType::setDifferentEndian(true);
#else
    CDisplayType::setDifferentEndian(false);
#endif
    Q_EMIT endian_changed();
}

void binary_values_view::set_table_type(int type)
{
    m_setting_table_property = true;
    ui->comboType->setCurrentIndex(type);
    m_setting_table_property = false;
}

void binary_values_view::table_type_changed(QString type_name)
{
    auto type = CDisplayType::getTypeOfName(type_name);
    if (!m_setting_table_property && type != CDisplayType::Unknown)
    {
        Q_EMIT change_table_type(type);
    }
}

void binary_values_view::set_table_columns(int columns)
{
    m_setting_table_property = true;
    ui->spinColumns->setValue(columns);
    m_setting_table_property = false;
}

void binary_values_view::table_columns_changed(int columns)
{
    if (!m_setting_table_property)
    {
        Q_EMIT change_table_columns(columns);
    }
}

void binary_values_view::set_table_offset(int offset)
{
    m_setting_table_property = true;
    ui->spinOffset->setValue(offset);
    m_setting_table_property = false;
}

void binary_values_view::notify_endian_changed()
{
    if (CDisplayType::getDifferentEndian()) ui->radioEndian2->setChecked(true);
    else                                    ui->radioEndian1->setChecked(true);
}

void binary_values_view::table_offset_changed(int offset)
{
    if (!m_setting_table_property)
    {
        Q_EMIT change_table_offset(offset);
    }
}

void binary_values_view::check_box_clicked(bool checked)
{
    auto found_box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [&] (auto &checkbox)
    {
        return checkbox == sender();
    } );

    if (found_box != m_Checkboxes.end())
    {
        ui->btnWriteValue->setEnabled(checked);
        int index = std::distance(m_Checkboxes.begin(), found_box);
        if (checked)
        {
            auto type = index2type(index);
            switch (type)
            {
            case CDisplayType::Ascii:
            case CDisplayType::Char:
            case CDisplayType::UChar:
            case CDisplayType::HEX8:
                m_display[CDisplayType::Ascii]->SetBytes(1);
                break;
            default: break;
            }
        }
        for (int i=0; i<m_Checkboxes.size(); ++i)
        {
            if (i == index)
            {
                m_Edit[i]->setReadOnly(!checked);
                on_btnReadValue_clicked();
            }
            else
            {
                m_Checkboxes[i]->setChecked(false);
                m_Edit[i]->setReadOnly(true);
            }
        }
    }

    if (!ui->checkStandAlone->isChecked() && !checked)
    {
        on_btnReadValue_clicked();
        ui->btnWriteValue->setEnabled(checked);
    }
}

void binary_values_view::on_checkStandAlone_toggled(bool checked)
{
    ui->btnReadValue->setEnabled(!checked);
    ui->btnWriteValue->setEnabled(!checked);
    ui->btnOpenTypeFile->setEnabled(!checked);
    ui->btnEditJsonFile->setEnabled(!checked);
    ui->btnReloadJsonText->setEnabled(!checked);
    ui->comboType->setEnabled(!checked);
    ui->spinColumns->setEnabled(!checked);
    ui->spinOffset->setEnabled(!checked);
}

void binary_values_view::editing_finished()
{
    if (ui->checkStandAlone->isChecked())
    {
        auto checked_box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox)
        {
            return checkbox->isChecked();
        } );
        if (checked_box != m_Checkboxes.end())
        {
            int index = std::distance(m_Checkboxes.begin(), checked_box);
            QString text = m_Edit[index]->text();
            if (text.size())
            {
                auto type = index2type(index);
                m_array.clear();
                m_array.resize(static_cast<int>(m_display[type]->GetByteLength()));
                if (m_display[type]->Write(reinterpret_cast<std::uint8_t*>(m_array.data()), text))
                {
                    receive_value(m_array, 0);
                }
                else
                {
                    QByteArray empty;
                    receive_value(empty, 0);
                    Q_EMIT status_message(tr("Wrong input value: %1 for type %2").arg(text, m_display[type]->Type()), 10);
                }
            }
        }
    }
}

void binary_values_view::on_btnOpenTypeFile_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open type format file for binary content"), m_type_format_files_location, "Type definition file (*.json)");
    if (file_name.size())
    {
        QFileInfo file_info(file_name);
        m_type_format_files_location = file_info.absolutePath();
    }

    int  json_flag = Json::file_input;
    Q_EMIT open_binary_format_file(file_name, json_flag);

    if (Json::is(json_flag, Json::text_parsed) || Json::is(json_flag, Json::file_opened))
    {
        m_current_combo_index = ui->comboType->currentIndex();
        ui->edtOpenFilePath->setText(file_name);
        ui->comboType->setCurrentText(CDisplayType::getNameOfType(CDisplayType::FormatFile));
        ui->btnEditJsonFile->setEnabled(true);
        ui->btnReloadJsonText->setEnabled(true);
    }
    else if (m_current_combo_index)
    {
        ui->comboType->setCurrentIndex(m_current_combo_index.value());
        m_current_combo_index.reset();
    }
}

void binary_values_view::on_btnEditJsonFile_clicked()
{
    QString file_name = ui->edtOpenFilePath->text();
    if (QFileInfo(file_name).exists())
    {
        int  json_flag = Json::open_in_editor;
        Q_EMIT edit_binary_format_file(file_name, json_flag);
    }
}

void binary_values_view::on_btnReloadJsonText_clicked()
{
    QString file_name = ui->edtOpenFilePath->text();
    if (QFileInfo(file_name).exists())
    {
        int  json_flag = Json::get_text_from_editor;
        Q_EMIT edit_binary_format_file(file_name, json_flag);
        if (Json::is(json_flag, Json::text_input))
        {
            json_flag = Json::text_input;
            Q_EMIT open_binary_format_file(file_name, json_flag);
        }
    }
}

void binary_values_view::on_btnInsertType_clicked()
{
    const int row = mListModel->rowCount();
    mListModel->insertRows(row, 1, QModelIndex());
    QString type_name = ui->comboDataType2insert->currentText();
    auto type_id = CDisplayType::getTypeOfName(type_name);
    if (is_any_equal_to(type_id, CDisplayType::Ascii, CDisplayType::Unicode))
    {
        QString format = type_name + "[%1]";
        type_name = tr(format.toStdString().c_str()).arg(ui->spinTextLength->value());
    }

    mListModel->setData(mListModel->index(row, Table::checkbox), false, Qt::CheckStateRole);
    mListModel->setData(mListModel->index(row, Table::name), tr("name %1").arg(row + 1), Qt::EditRole);
    mListModel->setData(mListModel->index(row, Table::type), type_name, Qt::DisplayRole);
    mListModel->setData(mListModel->index(row, Table::value), "", Qt::EditRole);
    update_table_buttons();
}


void binary_values_view::on_btnRemoveType_clicked()
{
    auto index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        mListModel->removeRow(index.row());
        update_table_buttons();
    }
}

QVariantList binary_values_view::get_search_list()
{
    QVariantList list;
    QByteArray array;
    const int rows = mListModel->rowCount();
    for (int row = 0; row < rows; ++row)
    {
        const QString type_name = mListModel->data(mListModel->index(row, Table::type)).toString();
        QStringList type_name_parts = type_name.split("[");
        const auto type_id = CDisplayType::getTypeOfName(type_name_parts[0]);
        auto &display_type = m_display[type_id];
        if (type_name_parts.length() > 1)
        {
            type_name_parts[1].remove("]");
            display_type->SetBytes(type_id == CDisplayType::Ascii ? type_name_parts[1].toInt() : type_name_parts[1].toInt()*2);
        }
        if (mListModel->data(mListModel->index(row, Table::checkbox), Qt::CheckStateRole).toBool())
        {
            QByteArray element(display_type->GetByteLength(), ' ');
            display_type->Write(reinterpret_cast<std::uint8_t*>(element.data()), mListModel->data(mListModel->index(row, Table::value)).toString());
            array.append(element);
        }
        else
        {
            if (array.length())
            {
                list.append(array);
                array.clear();
            }
            list.append(static_cast<int>(display_type->GetByteLength()));
        }
    }

    if (array.length())
    {
        list.append(array);
    }
    return list;
}


void binary_values_view::on_btnFindNext_clicked()
{
    QVariantList list = get_search_list();
    Q_EMIT search_expression(list, true);
}


void binary_values_view::on_btnFindPrevious_clicked()
{
    QVariantList list =get_search_list();
    Q_EMIT search_expression(list, false);
}


void binary_values_view::on_tableView_clicked(const QModelIndex &index)
{
    if (index.column() == mListModel->getCheckedColumn())
    {
        bool new_state = !mListModel->data(index, Qt::CheckStateRole).toBool();
        mListModel->setData(index, new_state, Qt::CheckStateRole);
        update_table_buttons();
    }
}

void binary_values_view::on_btnStoreTableContent_clicked()
{
    const QString file_name = QFileDialog::getSaveFileName(this, tr("Open table content file with data types"), m_type_format_files_location, "Name and Type definition file (*.txt)");
    QFile file(file_name);
    if (file.open( QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        const int rows = mListModel->rowCount();
        for (int row = 0; row < rows; ++row)
        {
            stream << mListModel->data(mListModel->index(row, Table::name)).toString() << ",";
            stream << mListModel->data(mListModel->index(row, Table::type)).toString() << ",";
            stream << mListModel->data(mListModel->index(row, Table::value)).toString() << ",";
            stream << mListModel->data(mListModel->index(row, Table::checkbox)).toInt() << "\n";
        }
    }
}

void binary_values_view::on_btnLoadTableContent_clicked()
{
    const QString file_name = QFileDialog::getOpenFileName(this, tr("Open table content file with data types"), m_type_format_files_location, "Name and Type definition file (*.txt)");
    QFile file(file_name);
    if (file.open( QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line;
        int row = mListModel->rowCount();
        while (!stream.atEnd())
        {
            line = stream.readLine();
            QStringList parts = line.split(",");
            if (parts.size() == 4)
            {
                mListModel->insertRows(row, 1, QModelIndex());
                mListModel->setData(mListModel->index(row, Table::name), parts[0], Qt::EditRole);
                mListModel->setData(mListModel->index(row, Table::type), parts[1], Qt::DisplayRole);
                mListModel->setData(mListModel->index(row, Table::value), parts[2], Qt::EditRole);
                mListModel->setData(mListModel->index(row, Table::checkbox), parts[3].toInt() != 0 ? true: false, Qt::CheckStateRole);
                ++row;
            }
        }
        update_table_buttons();
    }
}

void binary_values_view::update_table_buttons()
{
    const int count = mListModel->rowCount();
    bool check_state = false;
    for (int i = 0; i < count; ++i)
    {
        if (mListModel->data(mListModel->index(i, Table::checkbox), Qt::CheckStateRole).toBool())
        {
            check_state = true;
            break;
        }
    }

    ui->btnRemoveType->setEnabled(ui->tableView->currentIndex().isValid());
    ui->btnFindNext->setEnabled(check_state);
    ui->btnFindPrevious->setEnabled(check_state);
    ui->btnStoreTableContent->setEnabled(count > 0);
}
