#ifndef QRADIOBUTTONGROUP_H
#define QRADIOBUTTONGROUP_H

#include <QGroupBox>

class QRadioButtonGroup : public QGroupBox
{
    Q_OBJECT
public:
    enum eType { click_once, click_always, click_uncheck };
    explicit QRadioButtonGroup(QWidget *parent = 0);

    void setCurrentIndex(int anIndex);
    int  getCurrentIndex();
    void setClickSignal(eType aAlways);

private:
    void childEvent ( QChildEvent * c );

Q_SIGNALS:
    void clicked(int index);

private Q_SLOTS:
    void on_radio_button_clicked(bool checked);

private:
    int  mIndex;
    eType mClickSignal;
};

#endif // QRADIOBUTTONGROUP_H
