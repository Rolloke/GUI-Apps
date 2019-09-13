#ifndef GCODECONTROLDLG_H
#define GCODECONTROLDLG_H

#include <QDialog>

namespace Ui {
class CodeTestDlg;
}


class CodeTestDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CodeTestDlg(QWidget *parent = 0);
    ~CodeTestDlg();

    void setText(const QString& aText);
    QString getText() const;

signals:
    void testParser(int aParser, const QString &aText);

private slots:
    void on_BtnDisplay_clicked();

private:
    Ui::CodeTestDlg *ui;
};

#endif // GCODECONTROLDLG_H
