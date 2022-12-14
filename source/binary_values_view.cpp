#include "binary_values_view.h"
#include "ui_binary_values_view.h"
#include "DisplayType.h"

#include <QFileDialog>


binary_values_view::binary_values_view(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::binary_values_view)
  , m_display(CDisplayType::get_type_map())
  , m_current_position(0)
  , m_setting_table_property(false)
{
    ui->setupUi(this);

    if (CDisplayType::getDifferentEndian()) ui->radioEndian2->setChecked(true);
    else                                    ui->radioEndian1->setChecked(true);

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

    for (auto& edit : m_Edit)
    {
        edit->setToolTip(tooltip);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(editing_finished()));
    }
    for (const auto& type : m_display)
    {
        if (type.first == CDisplayType::Binary) continue;
        if (type.first == CDisplayType::Ascii) continue;
        if (type.first == CDisplayType::Unicode) continue;
        ui->comboType->addItem(CDisplayType::getNameOfType(type.first));
    }
    ui->comboType->addItem(CDisplayType::getNameOfType(CDisplayType::FormatFile));

    connect(ui->comboType, SIGNAL(currentIndexChanged(QString)), this, SLOT(table_type_changed(QString)));
    connect(ui->spinColumns, SIGNAL(valueChanged(int)), this, SLOT(table_columns_changed(int)));
    connect(ui->spinOffset, SIGNAL(valueChanged(int)), this, SLOT(table_offset_changed(int)));
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
        for (std::uint32_t i=0; i<index; ++i, ++display) ;
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
        int index = type2index(display.first);
        if (!(index >=0 && index < m_Checkboxes.size())) break;

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
        default:
            break;
        }
    }
}

void binary_values_view::receive_external_data(bool external_data)
{
    ui->checkStandAlone->setChecked(!external_data);
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
}

void binary_values_view::on_radioEndian2_clicked()
{
# if __BYTE_ORDER == __LITTLE_ENDIAN
    CDisplayType::setDifferentEndian(true);
#else
    CDisplayType::setDifferentEndian(false);
#endif
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
    { return checkbox == sender(); } );

    if (found_box != m_Checkboxes.end())
    {
        ui->btnWriteValue->setEnabled(checked);
        int index = std::distance(m_Checkboxes.begin(), found_box);
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
    ui->comboType->setEnabled(!checked);
    ui->spinColumns->setEnabled(!checked);
    ui->spinOffset->setEnabled(!checked);
}

void binary_values_view::editing_finished()
{
    if (ui->checkStandAlone->isChecked())
    {
        auto checked_box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox) { return checkbox->isChecked();} );
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
    bool opened = false;
    Q_EMIT open_binary_format_file(file_name, opened);
    if (opened)
    {
        m_current_combo_index = ui->comboType->currentIndex();
        ui->comboType->setCurrentText(CDisplayType::getNameOfType(CDisplayType::FormatFile));
    }
    else if (m_current_combo_index)
    {
        ui->comboType->setCurrentIndex(m_current_combo_index.value());
        m_current_combo_index.reset();
    }
}

