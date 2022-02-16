#ifndef BINARY_VALUES_VIEW_H
#define BINARY_VALUES_VIEW_H

#include <QDialog>
#include <QVector>
#include <QCheckBox>
#include <QLineEdit>

#include <map>
#include <memory>

#include "DisplayType.h"

namespace Ui {
class binary_values_view;
}



class binary_values_view : public QDialog
{
    Q_OBJECT

public:
    explicit binary_values_view(QWidget *parent = nullptr);
    ~binary_values_view();

Q_SIGNALS:
    void set_value(const QByteArray& array, int position);
    void status_message(const QString&, int);

public Q_SLOTS:
    void receive_value(const QByteArray& array, int position);
    void receive_external_data(bool external);


private slots:
    void on_checkSignedByte_clicked(bool checked);
    void on_checkUnsignedByte_clicked(bool checked);
    void on_checkHexByte_clicked(bool checked);
    void on_checkSignedShort_clicked(bool checked);
    void on_checkUnsignedShort_clicked(bool checked);
    void on_checkHexShort_clicked(bool checked);
    void on_checkSignedLong_clicked(bool checked);
    void on_checkUnsignedLong_clicked(bool checked);
    void on_checkHexLong_clicked(bool checked);
    void on_checkSignedLongLong_clicked(bool checked);
    void on_checkUnsignedLongLong_clicked(bool checked);
    void on_checkHexLongLong_clicked(bool checked);
    void on_checkFloat_clicked(bool checked);
    void on_checkDouble_clicked(bool checked);
    void on_btnReadValue_clicked();
    void on_btnWriteValue_clicked();
    void on_radioEndian1_clicked();
    void on_radioEndian2_clicked();
    void on_checkStandAlone_clicked(bool checked);

    void editing_finished();

private:
    void uncheck_all(bool checked);
    Ui::binary_values_view *ui;
    CDisplayType::type_map& m_display;
    QVector<QCheckBox*> m_Checkboxes;
    QVector<QLineEdit*> m_Edit;
    int                 m_current_position;
    QByteArray          m_array;
};

#endif // BINARY_VALUES_VIEW_H
