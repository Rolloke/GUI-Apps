#ifndef FUNCTIONGENERATOR_H
#define FUNCTIONGENERATOR_H

#include <QDialog>
class GenerateFunction;
class QDomDocument;
class QDomElement;
class QDomNode;

namespace Ui {
class FunctionGenerator;
}

class FunctionGenerator : public QDialog
{
    Q_OBJECT

    enum ePage
    {
        SinePage,
        RectanglePage,
        TrianglePage,
        NoisePage
    };

    enum class Burst
    {
        Off,
        Samples,
        Period,
        Time,
        LastItem
    };
    enum eConst
    {
        NoBursts = -1
    };

public:
    explicit FunctionGenerator(QWidget *parent = 0);
    ~FunctionGenerator();

    void setChannels(int aChannels);
    void setBuffer(int aChannel, std::vector<float>* aBuffer);
    void saveParameter(QDomDocument& aDoc, QDomElement& aDE);
    void loadParameter(const QDomNode& aDE);
    static const char* getName(Burst aBurstType);

Q_SIGNALS:
    void start_stream(bool, qint32);

public Q_SLOTS:
    void get_stream_state(bool );

private Q_SLOTS:
    void on_btnStartAudio_clicked(bool checked);
    void on_comboChannel_currentIndexChanged(int index);
    void on_btnApply_clicked();
    void on_groupWaveform_clicked(int index);
    void currentTabPagesChanged(int index);
    void on_checkSineActive_clicked(bool checked);
    void on_checkRectangleActive_clicked(bool checked);
    void on_checkTriangleActive_clicked(bool checked);
    void on_checkNoiseActive_clicked(bool checked);
    void on_checkSineSweepActive_clicked(bool checked);
    void on_checkNoiseFitered_clicked(bool checked);
    void on_comboLowPassType_currentIndexChanged(int index);
    void on_comboHighPassType_currentIndexChanged(int index);
    void on_checkPinkNoise_clicked(bool checked);
    void on_comboBurstLengthType_currentIndexChanged(int index);
    void on_editBurstLength_textChanged(const QString &arg1);

private:
    void showEvent (QShowEvent * event);

    void updateBurstParameters();
    void updateNoiseFilterParameters();
    void updateFunctionType();
    void updateChannelParameter();
    void getChannelParameter();
    quint32 getPeriodSamples();

    double calcPulseWidthRectangleFromPercent(double aPercent);
    void   saveChannel(QDomDocument& aDoc, QDomElement& aDE, GenerateFunction& aChannel);
    void   loadChannel(const QDomNode& aDE, GenerateFunction& aChannel);

    Ui::FunctionGenerator *ui;
    int                 mChannels;
    int                 mChannel;
    GenerateFunction*   mFunction;
    Burst               mBurstType;
    qint32              mBurstCycles;
};

#endif // FUNCTIONGENERATOR_H
