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
    void color_btn_clicked();
    void italic_btn_clicked(bool checked);
    void weigth_index_changed(int weight);
    void on_btnOK_clicked();
    void on_btnSelectedLineBackgroundColor_clicked();
    void on_btnCancel_clicked();

private:
    Ui::HighlighterDialog *ui;
    TextCharFormatMap mList;
    QColor mSelectedLineBackground;
};

#endif // HIGHLIGHTERDIALOG_H
