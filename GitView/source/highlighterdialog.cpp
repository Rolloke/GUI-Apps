#include "highlighterdialog.h"
#include "highlighter.h"
#include "ui_highlighterdialog.h"
#include "helper.h"

#include <QPushButton>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>

#include <QTextCharFormat>

HighlighterDialog::HighlighterDialog(QWidget *parent, const TextCharFormatMap& text_format_map) :
    QDialog(parent),
    ui(new Ui::HighlighterDialog)
  , mList(text_format_map)
{
    ui->setupUi(this);
    int height = 0;

    for (auto item = mList.begin(); item != mList.end(); ++item)
    {
        auto* color_btn = new QPushButton(item.key(), this);

        color_btn->setFont(item.value().font());
        color_btn->setStyleSheet(tr("QPushButton {background-color: %1; color: %2;}")
            .arg(palette().color(QPalette::Window).name(), item.value().foreground().color().name()));
        connect(color_btn, SIGNAL(clicked()), this, SLOT(color_btn_clicked()));

        auto* italic_check_btn = new QCheckBox(tr("italic"));
        connect(italic_check_btn, SIGNAL(clicked(bool)), this, SLOT(italic_btn_clicked(bool)));
        italic_check_btn->setChecked(item.value().font().italic());

        auto* weight_box = new QComboBox(this);
        connect(weight_box, SIGNAL(currentIndexChanged(int)), this, SLOT(weigth_index_changed(int)));
        weight_box->addItem(weight_name(QFont::Thin)      , QVariant(QFont::Thin));
        weight_box->addItem(weight_name(QFont::ExtraLight), QVariant(QFont::ExtraLight));
        weight_box->addItem(weight_name(QFont::Light)     , QVariant(QFont::Light));
        weight_box->addItem(weight_name(QFont::Normal)    , QVariant(QFont::Normal));
        weight_box->addItem(weight_name(QFont::Medium)    , QVariant(QFont::Medium));
        weight_box->addItem(weight_name(QFont::DemiBold)  , QVariant(QFont::DemiBold));
        weight_box->addItem(weight_name(QFont::Bold)      , QVariant(QFont::Bold));
        weight_box->addItem(weight_name(QFont::ExtraBold) , QVariant(QFont::ExtraBold));
        weight_box->addItem(weight_name(QFont::Black)     , QVariant(QFont::Black));

        for (int i=0;i<weight_box->count(); ++i)
        {
            if (weight_box->itemData(i).toInt() == item.value().font().weight())
            {
                weight_box->setCurrentIndex(i);
                break;
            }
        }
        if (height == 0)
        {
            std::vector<int> heights;
            heights.push_back(weight_box->size().height());
            heights.push_back(color_btn->size().height());
            height = *std::max_element(heights.begin(), heights.end());
        }

        color_btn->setMinimumHeight(height);
        italic_check_btn->setMinimumHeight(height);
        weight_box->setMinimumHeight(height);
        ui->color_buttons->addWidget(color_btn);
        ui->weight_boxes->addWidget(weight_box);
        ui->italic_buttons->addWidget(italic_check_btn);
    }
    mSelectedLineBackground = Highlighter::Language::mSelectedLineBackground;
    ui->btnSelectedLineBackgroundColor->setStyleSheet(tr("QPushButton {background-color: %1;}").
        arg(mSelectedLineBackground.name()));
}

HighlighterDialog::~HighlighterDialog()
{
    delete ui;
}

void HighlighterDialog::color_btn_clicked()
{
    auto *button = dynamic_cast<QWidget*>(sender());
    QColorDialog dlg(this);
    QTextCharFormat* text_char_format { nullptr };
    int index = ui->color_buttons->indexOf(button);
    if (index > 0) index--;
    if (index != -1)
    {
        auto item = std::next(mList.begin(), index);
        if (item != mList.end())
        {
            text_char_format = &item.value();
            dlg.setCurrentColor(text_char_format->foreground().color());
            ensure_dialog_on_same_screen(&dlg, this);
            if (dlg.exec() == QDialog::Accepted)
            {
                text_char_format->setForeground(QBrush(dlg.selectedColor()));
                button->setStyleSheet(tr("QPushButton {background-color: %1; color: %2;}").
                    arg(palette().color(QPalette::Normal, QPalette::Window).name(), dlg.selectedColor().name()));
            }
        }
    }
}

void HighlighterDialog::italic_btn_clicked(bool checked)
{
    auto *button = dynamic_cast<QWidget*>(sender());
    QTextCharFormat* text_char_format { nullptr };
    int index = ui->italic_buttons->indexOf(button);
    if (index != -1)
    {
        auto item = std::next(mList.begin(), index);
        if (item != mList.end())
        {
            text_char_format = &item.value();
            text_char_format->setFontItalic(checked);
            ui->color_buttons->itemAt(index)->widget()->setFont(text_char_format->font());
        }
    }
}

void HighlighterDialog::weigth_index_changed(int weight_index)
{
    auto *button = dynamic_cast<QWidget*>(sender());
    QTextCharFormat* text_char_format { nullptr };
    int index = ui->weight_boxes->indexOf(button);
    if (index != -1)
    {
        auto item = std::next(mList.begin(), index);
        if (item != mList.end())
        {
            text_char_format = &item.value();
            auto* weight_box = dynamic_cast<QComboBox*>(ui->weight_boxes->itemAt(index)->widget());
            if (weight_box)
            {
                text_char_format->setFontWeight(weight_box->itemData(weight_index).toInt());
            }
            ui->color_buttons->itemAt(index)->widget()->setFont(text_char_format->font());
        }
    }
}

void HighlighterDialog::on_btnOK_clicked()
{
    Highlighter::Language::mSelectedLineBackground = mSelectedLineBackground;
    accept();
}

void HighlighterDialog::on_btnCancel_clicked()
{
    close();
}

void HighlighterDialog::on_btnSelectedLineBackgroundColor_clicked()
{
    QColorDialog dlg;

    dlg.setCurrentColor(mSelectedLineBackground);
    ensure_dialog_on_same_screen(&dlg, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        mSelectedLineBackground = dlg.selectedColor();
        ui->btnSelectedLineBackgroundColor->setStyleSheet(tr("QPushButton {background-color: %1;}").
            arg(mSelectedLineBackground.name()));
    }
}


