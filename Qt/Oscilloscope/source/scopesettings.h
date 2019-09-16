#ifndef SCOPESETTINGS_H
#define SCOPESETTINGS_H

#include "cursors.h"
#include "Average.h"
#include <QDialog>

class Channel;
class TriggerParameter;
class CombineCurves;
class QStandardItemModel;
class QStandardItem;
class QComboBoxDelegate;
class QWidgetItemDelegate;

namespace Ui {
class ScopeSettings;
}

class ScopeSettings : public QDialog
{
    Q_OBJECT
public:
    enum eReceiveType
    {
        measured_period,
        calculated_avg,
        calculated_rms,
        measured_min,
        measured_max,
        measured_phase
    };

    enum eChannelListColumn
    {
        eName,
        eProbe,
        eScale,
        eOffset,
        eListSize
    };
    explicit ScopeSettings(QWidget *parent,
       std::vector<Channel>& aChannelParam,
       TriggerParameter& aTriggerpParam,
       CombineCurves& aCombineCurves);

    ~ScopeSettings();
    void  addCursors(Cursors& aCursors);
    void  updateParameters();
    void  updateColors();
    void  setFFT_FilterIndex(int anIndex);
    void  setFFT_OutputIndex(int anIndex);
    void  setActiveChannel(int aChannel);
    void  setTimePrefix(int aPrefix);
    void  setValue(double, int);
    int   getPhaseChannel() const;

Q_SIGNALS:
    void send_radio_filter(int index);
    void send_radio_output(int index);
    void send_floating_avg_values(int values);

public Q_SLOTS:
    void update_cursor(Cursors::eCursor, Cursors::eView);

private Q_SLOTS:
    void on_ckVertical_clicked(bool checked);
    void on_ckHorizontal_clicked(bool checked);
    void click_radio_filter(int index);
    void click_radio_output(int index);
    void click_radio_cursor_view(int index);

    void on_editVerticalCursor1_textEdited(const QString &arg1);
    void on_editVerticalCursor2_textEdited(const QString &arg1);
    void on_editHorizontalCursor1_textEdited(const QString &arg1);
    void on_editHorizontalCursor2_textEdited(const QString &arg1);


    void on_editHoldOffTime_textEdited(const QString &arg1);
    void on_checkHoldOffTimeActive_clicked(bool checked);
    void on_editHoldOffEvents_textEdited(const QString &arg1);
    void on_checkHoldOffEventActive_clicked(bool checked);

    void on_editTriggerDelay_textEdited(const QString &arg1);
    void on_checkTriggerDelayActive_clicked(bool checked);

    void on_editVerticalCursor1_editingFinished();
    void on_editVerticalCursor2_editingFinished();
    void on_editHorizontalCursor1_editingFinished();
    void on_editHorizontalCursor2_editingFinished();

    void on_clickedColorButtons(int index);
    void on_clickedMathCombine(int index);
    void on_spinChannel1_valueChanged(int arg1);
    void on_spinChannel2_valueChanged(int arg1);
    void on_spinChannelPhase_valueChanged(int arg1);

    void on_Tabulator_currentChanged(int index);
    void on_ChannelTableListItemChanged ( QStandardItem * item );
    void on_ProbeListItemChanged ( QStandardItem * item );

    void on_buttonAddProbe_clicked();
    void on_buttonRemoveProbe_clicked();
    void on_buttonBox_accepted();
    void on_checkSlopeTriggerActive_clicked(bool checked);
    void on_editSlopeTrigger_textEdited(const QString &arg1);
    void on_edtFloatingAverage_textEdited(const QString &arg1);
    void on_ckFloatingAverage_clicked(bool checked);

    void on_ckSyncCursors_clicked(bool checked);

private:
    void showEvent (QShowEvent * event);
    void timerEvent(QTimerEvent *event);
    void update_horizontal_cursor_unit();
    Cursors::eView getSynchedView(Cursors::eView aView);

    Ui::ScopeSettings *ui;
    std::vector<Cursors*> mCursors;
    std::vector<Channel>& mChannelParameter;
    TriggerParameter&     mTriggerParameter;
    CombineCurves&        mCombineCurves;
    int                   mActiveChannel;
    Cursors::eView        mActiveViewCursor;
    int                   mPhaseChannel;
    QStandardItemModel*   mChannelTableListModel;
    QStandardItemModel*   mProbeListModel;
    QComboBoxDelegate*    mProbeCombo;

    bool mInitialize;
    bool mProbesChanged;
    Average<double> mAverageFrequency;
    Average<double> mAverageAVGLevel;
    Average<double> mAverageRMSLevel;
};

#endif // SCOPESETTINGS_H
