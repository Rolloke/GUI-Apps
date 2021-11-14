#include "qradiobuttongroup.h"
#include <QChildEvent>
#include <QRadioButton>

QRadioButtonGroup::QRadioButtonGroup(QWidget *parent) :
    QGroupBox(parent)
  , mIndex(-1)
  , mClickSignal(click_once)
{
}

void QRadioButtonGroup::setClickSignal(QRadioButtonGroup::eType aClickType)
{
    mClickSignal = aClickType;
}

void QRadioButtonGroup::childEvent ( QChildEvent * c )
{
    QAbstractButton* fRB = qobject_cast<QAbstractButton*>(c->child());
    if (fRB)
    {
        QObject::connect(fRB, SIGNAL(clicked(bool)), this, SLOT(on_radio_button_clicked(bool)));
        fRB->setAutoExclusive(true);
    }
}

void QRadioButtonGroup::setCurrentIndex(int anIndex)
{
    if (anIndex >= 0 && anIndex < children().size())
    {
        QAbstractButton* fRB = qobject_cast<QAbstractButton*>(children().at(anIndex));
        if (fRB)
        {
            fRB->setChecked(true);
        }
    }
    mIndex = anIndex;
}

int QRadioButtonGroup::getCurrentIndex()
{
    int fIndex(-1);
    for (QObject* fObject : children())
    {
        QAbstractButton* fRB = qobject_cast<QAbstractButton*>(fObject);
        if (fRB)
        {
            ++fIndex;
            if (fRB->isChecked())
            {
                break;
            }
        }
    }
    return fIndex;
}

void QRadioButtonGroup::on_radio_button_clicked(bool checked)
{
    int fNewIndex = getCurrentIndex();
    if (fNewIndex != mIndex)
    {
        Q_EMIT clicked(fNewIndex);
        mIndex = fNewIndex;
    }
    else if (mClickSignal == click_always)
    {
        mIndex = -1;
    }
    else if (mClickSignal == click_uncheck && !checked)
    {

    }
}
