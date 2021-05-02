#ifndef HIGHLIGHTERDIALOG_H
#define HIGHLIGHTERDIALOG_H

#include "highlighter.h"

#include <QDialog>

namespace Ui {
class HighlighterDialog;
}

class HighlighterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HighlighterDialog(QWidget *parent, const TextCharFormatMap&);
    ~HighlighterDialog();

    const TextCharFormatMap& getTextFormatMap() { return mList; }

private slots:
    void on_color_btn_clicked(bool checked);
    void on_italic_btn_clicked(bool checked);
    void on_weigth_index_changed(int weight);
    void on_btnOK_clicked();

private:
    Ui::HighlighterDialog *ui;
    TextCharFormatMap mList;
};

#endif // HIGHLIGHTERDIALOG_H
