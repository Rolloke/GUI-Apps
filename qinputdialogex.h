#ifndef QINPUTDIALOGEX_H
#define QINPUTDIALOGEX_H

#include <QInputDialog>

class QInputDialogEx : public QInputDialog
{
    Q_OBJECT
public:
    explicit QInputDialogEx(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

signals:

};

#endif // QINPUTDIALOGEX_H
