#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml/QDomDocument>
#include <QErrorMessage>

class QAbstractItemModel;
class QModelIndex;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButtonSave_clicked();

    void on_pushButtonDeleteSelected_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButtonSelect_clicked();

private:
    Ui::MainWindow *ui;
    QErrorMessage mErrorMsgDlg;

    QDomDocument mDoc;
    QAbstractItemModel* mListModel;
};

#endif // MAINWINDOW_H
