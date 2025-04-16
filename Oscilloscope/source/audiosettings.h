#ifndef AUDIO_SETTINGS_H
#define AUDIO_SETTINGS_H

#include <QDialog>

class QAudioRecord;
class QAudioOutStream;
class QAudioParameter;
class QComboBox;

namespace Ui {
class Settings;
}

class AudioSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit AudioSettings(QWidget *parent, QAudioRecord& aAudioInput, QAudioOutStream& aAudioOutput);
    ~AudioSettings();
    enum eReturnValue { Restart = 2 };
    
private Q_SLOTS:
    void on_btnApply_clicked();
    void on_btnOk_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_checkShowFGinScope_clicked(bool checked);
    void on_comboDeviceID_currentIndexChanged(int index);

    void on_comboDeviceID_2_currentIndexChanged(int index);

    void on_comboDeviceID_highlighted(int index);

public:
    bool mShowFGinScope;
    int  mAudioInputType;
private:
    void showEvent (QShowEvent * event);

    void updateParameters();
    void initComboDeviceID(QComboBox& aCombo, const QAudioParameter& aParam, int aID);
    void initComboFrequencies(QComboBox& aCombo, const QAudioParameter& aParam, const QString& aCurrentText);
    void initComboFormats(QComboBox& aCombo, const QAudioParameter& aParam, int aFormat);
    void initComboChannels(QComboBox& aCombo, const QAudioParameter& aParam, const QString& aCurrentText);

    Ui::Settings *ui;
    QAudioRecord& mAudioInput;
    QAudioOutStream& mAudioOutput;
    bool mInitializingComboDeviceID;
};

#endif // AUDIO_SETTINGS_H
