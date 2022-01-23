#include "binary_values_view.h"
#include "ui_binary_values_view.h"

binary_values_view::binary_values_view(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::binary_values_view)
  , m_current_position(0)
{
    ui->setupUi(this);

    if (CDisplayType::getDifferentEndian()) ui->radioEndian2->setChecked(true);
    else                                    ui->radioEndian1->setChecked(true);

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

    m_display[CDisplayType::Char]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayChar));
    m_display[CDisplayType::UChar]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayUChar));
    m_display[CDisplayType::Short]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayShort));
    m_display[CDisplayType::UShort]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayUShort));
    m_display[CDisplayType::Long]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayLong));
    m_display[CDisplayType::ULong]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayULong));
    m_display[CDisplayType::LongLong]  = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayLongLong));
    m_display[CDisplayType::ULongLong] = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayULongLong));
    m_display[CDisplayType::Float]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayFloat));
    m_display[CDisplayType::Double]    = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayDouble));
    m_display[CDisplayType::HEX2]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX2));
    m_display[CDisplayType::HEX4]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX4));
    m_display[CDisplayType::HEX8]      = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX8));
    m_display[CDisplayType::HEX16]     = std::unique_ptr<CDisplayType>(reinterpret_cast<CDisplayType*>(new CDisplayHEX16));

    m_array.resize(sizeof(std::uint64_t));
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
        if (position + display.second->GetByteLength() <= array.size())
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

void binary_values_view::on_btnReadValue_clicked()
{

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
    CDisplayType::setDifferentEndian(false);
}

void binary_values_view::on_radioEndian2_clicked()
{
    CDisplayType::setDifferentEndian(true);
}

void binary_values_view::uncheck_all()
{
    ui->btnWriteValue->setEnabled(true);
    for (auto& checkbox : m_Checkboxes)
    {
        checkbox->setChecked(false);
    }
}

void binary_values_view::on_checkSignedByte_clicked(bool checked)
{
    uncheck_all();
    ui->checkSignedByte->setChecked(checked);
}

void binary_values_view::on_checkUnsignedByte_clicked(bool checked)
{
    uncheck_all();
    ui->checkUnsignedByte->setChecked(checked);
}

void binary_values_view::on_checkHexByte_clicked(bool checked)
{
    uncheck_all();
    ui->checkHexByte->setChecked(checked);
}

void binary_values_view::on_checkSignedShort_clicked(bool checked)
{
    uncheck_all();
    ui->checkSignedShort->setChecked(checked);
}

void binary_values_view::on_checkUnsignedShort_clicked(bool checked)
{
    uncheck_all();
    ui->checkUnsignedShort->setChecked(checked);
}

void binary_values_view::on_checkHexShort_clicked(bool checked)
{
    uncheck_all();
    ui->checkHexShort->setChecked(checked);
}

void binary_values_view::on_checkSignedLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkSignedLong->setChecked(checked);
}

void binary_values_view::on_checkUnsignedLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkUnsignedLong->setChecked(checked);
}

void binary_values_view::on_checkHexLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkHexLong->setChecked(checked);
}

void binary_values_view::on_checkSignedLongLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkSignedLongLong->setChecked(checked);
}

void binary_values_view::on_checkUnsignedLongLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkUnsignedLongLong->setChecked(checked);
}

void binary_values_view::on_checkHexLongLong_clicked(bool checked)
{
    uncheck_all();
    ui->checkHexLongLong->setChecked(checked);
}

void binary_values_view::on_checkFloat_clicked(bool checked)
{
    uncheck_all();
    ui->checkFloat->setChecked(checked);
}

void binary_values_view::on_checkDouble_clicked(bool checked)
{
    uncheck_all();
    ui->checkDouble->setChecked(checked);
}


