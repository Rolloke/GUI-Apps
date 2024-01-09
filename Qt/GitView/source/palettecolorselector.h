#ifndef PALETTECOLORSELECTOR_H
#define PALETTECOLORSELECTOR_H

#include <QDialog>
#include <QMap>
namespace Ui {
class PaletteColorSelector;
}


class PaletteColorSelector : public QDialog
{
    Q_OBJECT

public:
    explicit PaletteColorSelector(QWidget *parent = nullptr);
    ~PaletteColorSelector();

    const QPalette& get_palette() ;

    static void            set_dark_palette_colors(const QString&colors);
    static QString         get_dark_palette_colors();
    static void            init_dark_palette();
    static const QPalette& get_default_palette();

private slots:
    void color_btn_clicked();
    void on_btnCancel_clicked();
    void on_btnApply_clicked();
    void on_btnSavePalette_clicked();

private:


    QMap<int, bool> m_background;
    QMap<int, int>  m_other_color;

    QPalette m_palette;
    Ui::PaletteColorSelector *ui;

    static QPalette m_dark_palette;
    static QPalette m_default_palette;
};

#endif // PALETTECOLORSELECTOR_H
