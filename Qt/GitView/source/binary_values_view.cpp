#include "binary_values_view.h"
#include "ui_binary_values_view.h"
#include "DisplayType.h"

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

    for (auto& checkbox : m_Checkboxes)
    {
        checkbox->setToolTip(tooltip);
        connect(checkbox, SIGNAL(clicked(bool)), this, SLOT(check_box_clicked(bool)));
    }

    tooltip = ui->edtUnsignedByte->toolTip();
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

    for (auto& edit : m_Edit)
    {
        edit->setToolTip(tooltip);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(editing_finished()));
    }
    for (const auto& type : m_display)
    {
        ui->comboType->addItem(CDisplayType::getNameOfType(type.second->getType()));
    }

    connect(ui->comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(table_type_changed(int)));
    connect(ui->spinColumns, SIGNAL(valueChanged(int)), this, SLOT(table_columns_changed(int)));
    connect(ui->spinOffset, SIGNAL(valueChanged(int)), this, SLOT(table_offset_changed(int)));
}

binary_values_view::~binary_values_view()
{
    delete ui;
}

void binary_values_view::receive_value(const QByteArray &array, int position)
{
    QString text;
    const std::uint8_t* buffer_pointer = reinterpret_cast<const std::uint8_t*>(array.data());
    m_current_position = position;

    for (auto& display : m_display )
    {
        if (ui->checkStandAlone->isChecked())
        {
            if (m_Checkboxes[display.first]->isChecked())
            {
                continue;
            }
            if (display.second->GetByteLength() == array.size())
            {
                text = display.second->Display(buffer_pointer);
            }
            else
            {
                text.clear();
            }
        }
        else if (position + display.second->GetByteLength() <= array.size())
        {
            text = display.second->Display(&buffer_pointer[position]);
        }
        else
        {
            text.clear();
            m_Checkboxes[display.first]->setChecked(false);
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
        case CDisplayType::HEX2: ui->edtHexByte->setText(text); break;
        case CDisplayType::HEX4: ui->edtHexShort->setText(text); break;
        case CDisplayType::HEX8: ui->edtHexLong->setText(text); break;
        case CDisplayType::HEX16: ui->edtHexLongLong->setText(text); break;
        case CDisplayType::Float: ui->edtFloat->setText(text); break;
        case CDisplayType::Double: ui->edtDouble->setText(text); break;
        default:
            break;
        }
    }
}

void binary_values_view::receive_external_data(bool external_data)
{
    ui->checkStandAlone->setChecked(!external_data);
    on_checkStandAlone_clicked(!external_data);
}

void binary_values_view::on_btnReadValue_clicked()
{
    QByteArray array;
    Q_EMIT set_value(array, m_current_position);
}

void binary_values_view::on_btnWriteValue_clicked()
{
    auto box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox) { return checkbox->isChecked();} );
    if (box != m_Checkboxes.end())
    {
        int index = std::distance(m_Checkboxes.begin(), box);
        QString text = m_Edit[index]->text();
        if (text.size())
        {
            QByteArray array;
            auto type = static_cast<CDisplayType::eType>(index);
            array.resize(m_display[type]->GetByteLength());
            m_display[type]->Write(reinterpret_cast<std::uint8_t*>(array.data()), text);
            Q_EMIT set_value(array, m_current_position);
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

void binary_values_view::table_type_changed(int type)
{
    if (!m_setting_table_property)
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
            }
            else
            {
                m_Edit[i]->setText("");
                m_Checkboxes[i]->setChecked(false);
                m_Edit[i]->setReadOnly(true);
            }
        }
    }
}

void binary_values_view::on_checkStandAlone_clicked(bool checked)
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
        auto box = std::find_if(m_Checkboxes.begin(), m_Checkboxes.end(), [] (auto &checkbox) { return checkbox->isChecked();} );
        if (box != m_Checkboxes.end())
        {
            int index = std::distance(m_Checkboxes.begin(), box);
            QString text = m_Edit[index]->text();
            if (text.size())
            {
                auto type = static_cast<CDisplayType::eType>(index);
                m_array.clear();
                m_array.resize(m_display[type]->GetByteLength());
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

