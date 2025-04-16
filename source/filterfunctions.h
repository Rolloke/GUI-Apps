#ifndef FILTERFUNCTIONS_H
#define FILTERFUNCTIONS_H
#include <vector>
#include <complex>

class FilterFunctions
{
public:
    enum eType
    {
        Butterworth,
        ButterworthVariableQ,
        Bessel,
        Tschebyscheff,
        LinkwitzRiley,
        LowPassHalfOrder,
        Pink        = LowPassHalfOrder,
        FirstFilter = Butterworth,
        LastFilter  = LowPassHalfOrder
    };
    enum eOrder
    {
        One   = 1,
        Two   = 2,
        Three = 3,
        Four  = 4,
        FirstOrder = One,
        LastOrder  = Four
    };

    FilterFunctions();

    void setHighPassType(eType aType);
    void setHighPassOrder(eOrder aOrder);
    void setHighPassQ(double aQ);
    void setHighPassCutOff(double aFrq);

    eType  getHighPassType() const;
    eOrder getHighPassOrder() const;
    double getHighPassQ() const;
    double getHighPassCutOff() const;

    void setLowPassType(eType aType);
    void setLowPassOrder(eOrder aOrder);
    void setLowPassQ(double aQ);
    void setLowPassCutOff(double aFrq);

    eType  getLowPassType() const;
    eOrder getLowPassOrder() const;
    double getLowPassQ() const;
    double getLowPassCutOff() const;

    const std::vector<double> & getFilterFrequenzcies() const { return mFilterFrq; }
    const std::vector<std::complex<double> >& getFilterFunction() const { return mFilterBox; }

    static const char* getNameOfType(eType aType);

    void calculateFilter(double aStep, std::vector<double>& aFourier, bool aGetFilterFunction = false);

private:
    void multiplyFilterExp(double real, double imag, double &a, double &b);
    void multiplyFilter(std::complex<double> cFK, double &a, double &b);
    void calculatePinkNoiseFilterExp(double dF0, double dFrequ, std::complex<double>&pErg);
    std::complex<double> calculatePinkNoiseFilter(double dF0, double dFrequ);


    eOrder       mHPOrder;        // Hochpass-Filterordnung: 1..4, 0->kein HP
    eType        mHPType;         // 0->Butterworth,
                                  // 1->ButterworthVariableQ (Q variabel, nur fuer 2./3. Ordnung)
                                  // 2->Bessel
                                  // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                  // 4->Linkwitz-Riley (nur f. 4. Ordnung)
    double       mHPQFaktor;      // gilt nur fuer nHPType=1 'entdaempft', sonst dHPQFaktor=0
    double       mHPCutOffFrequency; // ausgewaehlte Grenzfrequenz

    eOrder       mTPOrder;        // Tiefpass-Filterordnung: 1..4, 0->kein TP
    eType        mTPType;         // 0->Butterworth,
                                  // 1->ButterworthVariableQ (Q variabel, nur fuer 2./3. Ordnung)
                                  // 2->Bessel
                                  // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                  // 4->Linkwitz-Riley (nur f. 4. Ordnung)
    double       mTPQFaktor;      // gilt nur fuer nTPType=1 'entdaempft', sonst dTPQFaktor=0
    double       mTPCutOffFrequency; // ausgewaehlte Grenzfrequenz

    int          mNoOfFrequencies;

    std::vector<double> mFilterFrq;     // Filterfrequenzen
    std::vector< std::complex<double> > mFilterBox;     // komplexe Filterkoeffizienten, frequenzabh�ngig
};



#endif // FILTERFUNCTIONS_H
