#include "palettecolorselector.h"
#include "ui_palettecolorselector.h"

#include <QPalette>
#include <QColorDialog>
#include <QMessageBox>

QPalette PaletteColorSelector::m_dark_palette;
QPalette PaletteColorSelector::m_default_palette;

PaletteColorSelector::PaletteColorSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaletteColorSelector)
{
    ui->setupUi(this);
    m_palette = m_dark_palette;
    setPalette(m_palette);

    for (int color_role = 0; color_role < QPalette::NColorRoles; ++color_role)
    {
        QString text;
        QString tooltip;
        int color_background = QPalette::Button;
        int color_text       = QPalette::WindowText;
        switch (color_role)
        {
        case QPalette::Button:          text = "Button";
            tooltip = "The general button background color.\n"
                      "This background can be different from Window as some styles\n"
                      "require a different background color for buttons."; break;
        case QPalette::Text:            text = "Text";
            tooltip = "The foreground color used with Base.\n"
                      "This is usually the same as the WindowText,"
                      "in which case it must provide good contrast with Window and Base."; break;
        case QPalette::WindowText:      text = "Window Text";        color_text       = color_role;
            tooltip = "A general foreground color"; break;
        case QPalette::Light:           text = "Light";              color_background = color_role;
            tooltip = "Lighter than Button color.";  break;
        case QPalette::Midlight:        text = "Midlight";           color_background = color_role;
            tooltip = "Between Button and Light."; break;
        case QPalette::Dark:            text = "Dark";               color_background = color_role;
            tooltip = "Darker than Button."; break;
        case QPalette::Mid:             text = "Mid";                color_background = color_role;
            tooltip = "Between Button and Dark."; break;
        case QPalette::BrightText:      text = "Bright Text";        color_text       = color_role; color_background = QPalette::Shadow;
            m_other_color[color_role] = color_background;
            tooltip = "A text color that is very different from WindowText,\n"
                      "and contrasts well with e.g. Dark. Typically used for\n"
                      "text that needs to be drawn where Text or WindowText\n"
                      "would give poor contrast, such as on pressed push buttons.\n"
                      "Note that text colors can be used for things other than just words;\n"
                      "text colors are usually used for text, but it's quite common to\n"
                      "use the text color roles for lines, icons, etc."; break;
        case QPalette::ButtonText:      text = "Button Text";        color_text       = color_role;
            tooltip = "A foreground color used with the Button color."; break;
        case QPalette::Base:            text = "Base";               color_background = color_role;
            tooltip = "Used mostly as the background color for text entry widgets,\n"
                      "but can also be used for other painting such as the background of\n"
                      "- combobox\n"
                      "- drop down lists\n"
                      "- and toolbar handles."
                      "It is usually white or another light color."; break;
        case QPalette::Window:          text = "Window";             color_background = color_role;
            tooltip = "A general background color"; break;
        case QPalette::Shadow:          text = "Shadow";             color_background = color_role; color_text       = QPalette::Shadow;
            m_other_color[color_role] = color_text;
             tooltip = "A very dark color. By default, the shadow color is Qt::black."; break;
        case QPalette::Highlight:       text = "Highlight";          color_background = color_role; color_text       = QPalette::HighlightedText;
             m_other_color[color_role] = color_text;
             tooltip = "A color to indicate a selected item or the current item.\n"
                       "By default, the highlight color is Qt::darkBlue."; break;
        case QPalette::HighlightedText: text = "Highlighted Text";   color_text       = color_role; color_background = QPalette::Highlight;
             m_other_color[color_role] = color_background;
             tooltip = "A text color that contrasts with Highlight.\n"
                       "By default, the highlighted text color is Qt::white."; break;
        case QPalette::Link:            text = "Link";               color_text       = color_role;
             tooltip = "A text color used for unvisited hyperlinks.\n"
                       "By default, the link color is Qt::blue."; break;
        case QPalette::LinkVisited:     text = "LinkVisited";        color_text       = color_role;
             tooltip = "A text color used for already visited hyperlinks.\n"
                       "By default, the linkvisited color is Qt::magenta. "; break;
        case QPalette::AlternateBase:   text = "Alternate Base";     color_background = color_role;
             tooltip = "Used as the alternate background color in views with alternating row colors\n"
                       "(see QAbstractItemView::setAlternatingRowColors())."; break;
        case QPalette::ToolTipBase:     text = "ToolTip Base";       color_background = color_role; color_text       = QPalette::ToolTipText;
             m_other_color[color_role] = color_text;
             tooltip = "Used as the background color for QToolTip and QWhatsThis.\n"
                      "Tool tips use the Inactive color group of QPalette, because tool tips are not active windows."; break;
        case QPalette::ToolTipText:     text = "ToolTip Text";       color_text       = color_role; color_background = QPalette::ToolTipBase;
             m_other_color[color_role] = color_background;
             tooltip = "Used as the foreground color for QToolTip and QWhatsThis.\n"
                      "Tool tips use the Inactive color group of QPalette, because tool tips are not active windows."; break;
        case QPalette::PlaceholderText: text = "Placeholder Text";   color_text       = color_role;
            tooltip = "Used as the placeholder color for various text input widgets.\n"
                      "This enum value has been introduced in Qt 5.12"; break;
        case QPalette::NoRole: continue; break;
        }

        auto* color_btn = new QPushButton(tr("%1").arg(text), this);
        if (color_background == color_role)
        {
            m_background[color_role] = true;
        }
        //m_text_color[color_text] = color_text;
        //m_background_color[color_text] = color_background;

        color_btn->setStyleSheet(tr("QPushButton {background-color: %1; color: %2; }")
            .arg(m_palette.color(static_cast<QPalette::ColorRole>(color_background)).name(),
                 m_palette.color(static_cast<QPalette::ColorRole>(color_text)).name()));
        color_btn->setToolTip(tooltip);
        connect(color_btn, SIGNAL(clicked()), this, SLOT(color_btn_clicked()));
        ui->colorButtons->addWidget(color_btn);
    }
    ui->textBrowser->setText("Text in edit field");
}

void PaletteColorSelector::init_dark_palette()
{
    const auto button_color = QColor(53, 53, 53);
    m_default_palette = QGuiApplication::palette();

    m_dark_palette = m_default_palette;
    m_dark_palette.setColor(QPalette::Window, button_color);
    m_dark_palette.setColor(QPalette::WindowText, Qt::white);
    m_dark_palette.setColor(QPalette::Base, QColor(25, 25, 25));
    m_dark_palette.setColor(QPalette::AlternateBase, button_color);
    m_dark_palette.setColor(QPalette::ToolTipBase, Qt::black);
    m_dark_palette.setColor(QPalette::ToolTipText, Qt::white);
    m_dark_palette.setColor(QPalette::Text, Qt::white);
    m_dark_palette.setColor(QPalette::ButtonText, Qt::white);
    m_dark_palette.setColor(QPalette::BrightText, Qt::red);
    m_dark_palette.setColor(QPalette::Link, QColor(42, 130, 218));
    m_dark_palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_dark_palette.setColor(QPalette::HighlightedText, Qt::black);
    m_dark_palette.setColor(QPalette::Button, button_color);              // invert normal behavour
    m_dark_palette.setColor(QPalette::Light, button_color.darker(100));    // Lighter than Button color
    m_dark_palette.setColor(QPalette::Midlight, button_color.lighter(50));// Between Button and Light.
    m_dark_palette.setColor(QPalette::Mid, button_color.lighter(100));    // Between Button and Dark.
    m_dark_palette.setColor(QPalette::Dark, button_color.lighter(150));   // Darker than Button.
}

const QPalette& PaletteColorSelector::get_default_palette()
{
    return m_default_palette;
}

void PaletteColorSelector::set_dark_palette_colors(const QString& colors)
{
    QStringList palette_colors = colors.split(",");
    int color_role = 0;
    for (const auto& color_name : palette_colors)
    {
        m_dark_palette.setColor(static_cast<QPalette::ColorRole>(color_role), QColor(color_name));
    }
}

QString PaletteColorSelector::get_dark_palette_colors()
{
    QString colors;
    for (int color_role=0; color_role < QPalette::NColorRoles; ++color_role)
    {
        colors += m_dark_palette.color(static_cast<QPalette::ColorRole>(color_role)).name();
        if (color_role < QPalette::NColorRoles-1)
        {
            colors += ",";
        }
    }
    return colors;
}

PaletteColorSelector::~PaletteColorSelector()
{
    delete ui;
}

void PaletteColorSelector::color_btn_clicked()
{
    auto *button = dynamic_cast<QPushButton*>(sender());
    int index = ui->colorButtons->indexOf(button);
    if (index != -1)
    {
        QColorDialog dlg;
        auto palette_color = static_cast<QPalette::ColorRole>(index);
        dlg.setWindowTitle(tr("Color of %1").arg(button->text()));
        dlg.setCurrentColor(m_palette.color(palette_color));
        if (dlg.exec() == QDialog::Accepted)
        {
            QString other_color;
            if (m_other_color.contains(index))
            {
                other_color  = m_palette.color(static_cast<QPalette::ColorRole>(m_other_color[index])).name();
            }
            auto color = dlg.selectedColor();
            m_palette.setColor(palette_color, color);
            setPalette(m_palette);

            if (m_background.contains(index))
            {
                if (!other_color.isEmpty())
                {
                    button->setStyleSheet(tr("QPushButton {background-color: %1; color: %2 }").arg(color.name()).arg(other_color));
                }
                else
                {
                    button->setStyleSheet(tr("QPushButton {background-color: %1; }").arg(color.name()));
                }
            }
            else
            {
                if (!other_color.isEmpty())
                {
                    button->setStyleSheet(tr("QPushButton {background-color: %2; color: %1 }").arg(color.name()).arg(other_color));
                }
                else
                {
                    button->setStyleSheet(tr("QPushButton {color: %1; }").arg(color.name()));
                }
            }
        }
    }
}


void PaletteColorSelector::on_btnCancel_clicked()
{
    reject();
}

void PaletteColorSelector::on_btnApply_clicked()
{
    /// TODO: request for restart application
    accept();
}

const QPalette& PaletteColorSelector::get_palette()
{
    return m_palette;
}

void PaletteColorSelector::on_btnSavePalette_clicked()
{
    m_dark_palette = m_palette;
}

