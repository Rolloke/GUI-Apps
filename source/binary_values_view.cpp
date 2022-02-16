#include "binary_values_view.h"
#include "ui_binary_values_view.h"
#include "DisplayType.h"

binary_values_view::binary_values_view(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::binary_values_view)
  , m_display(CDisplayType::get_type_map())
  , m_current_position(0)
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

void binary_values_view::uncheck_all(bool checked)
{
    ui->btnWriteValue->setEnabled(checked);
    for (auto& checkbox : m_Checkboxes)
    {
        checkbox->setChecked(false);
    }
    for (auto& edit: m_Edit)
    {
        edit->setReadOnly(true);
    }
}

void binary_values_view::on_checkSignedByte_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkSignedByte->setChecked(checked);
    ui->edtSignedByte->setReadOnly(!checked);

}

void binary_values_view::on_checkUnsignedByte_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkUnsignedByte->setChecked(checked);
    ui->edtUnsignedByte->setReadOnly(!checked);
}

void binary_values_view::on_checkHexByte_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkHexByte->setChecked(checked);
    ui->edtHexByte->setReadOnly(!checked);
}

void binary_values_view::on_checkSignedShort_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkSignedShort->setChecked(checked);
    ui->edtSignedShort->setReadOnly(!checked);
}

void binary_values_view::on_checkUnsignedShort_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkUnsignedShort->setChecked(checked);
    ui->edtUnsignedShort->setReadOnly(!checked);
}

void binary_values_view::on_checkHexShort_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkHexShort->setChecked(checked);
    ui->edtHexShort->setReadOnly(!checked);
}

void binary_values_view::on_checkSignedLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkSignedLong->setChecked(checked);
    ui->edtSignedLong->setReadOnly(!checked);
}

void binary_values_view::on_checkUnsignedLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkUnsignedLong->setChecked(checked);
    ui->edtUnsignedLong->setReadOnly(!checked);
}

void binary_values_view::on_checkHexLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkHexLong->setChecked(checked);
    ui->edtHexLong->setReadOnly(!checked);
}

void binary_values_view::on_checkSignedLongLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkSignedLongLong->setChecked(checked);
    ui->edtSignedLongLong->setReadOnly(!checked);
}

void binary_values_view::on_checkUnsignedLongLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkUnsignedLongLong->setChecked(checked);
    ui->edtUnsignedLongLong->setReadOnly(!checked);
}

void binary_values_view::on_checkHexLongLong_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkHexLongLong->setChecked(checked);
    ui->edtHexLongLong->setReadOnly(!checked);
}

void binary_values_view::on_checkFloat_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkFloat->setChecked(checked);
    ui->edtFloat->setReadOnly(!checked);
}

void binary_values_view::on_checkDouble_clicked(bool checked)
{
    uncheck_all(checked);
    ui->checkDouble->setChecked(checked);
    ui->edtDouble->setReadOnly(!checked);
}

void binary_values_view::on_checkStandAlone_clicked(bool checked)
{
    ui->btnReadValue->setEnabled(!checked);
    ui->btnWriteValue->setEnabled(!checked);
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

