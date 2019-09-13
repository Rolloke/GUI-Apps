#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
#include "carithmetic.h"


using namespace std;

int main(int argc, char *argv[])
{
    string sArgument   = "";
    bool fShow         = false;
    bool fScientific   = false;
    bool fFixed        = false;
    bool fHelp         = false;
    bool fShowEquation = false;
    int  fError        = IDE_AR_NOEQUATION;

    for (int i=1; i< argc; ++i)
    {
        if (strcmp(argv[i], "--show") == 0)
        {
            fShow = true;
        }
        else if (strcmp(argv[i], "--grad") == 0)
        {
            CArithmetic::gm_AngleTo   = M_PI / 180.0;
            CArithmetic::gm_AngleFrom = 1.0 / CArithmetic::gm_AngleTo;
        }
        else if (strcmp(argv[i], "--gon") == 0)
        {
            CArithmetic::gm_AngleTo   = M_PI / 200.0;
            CArithmetic::gm_AngleFrom = 1.0 / CArithmetic::gm_AngleTo;
        }
        else if (strcmp(argv[i], "--scientific") == 0)
        {
            fScientific = true;
        }
        else if (strcmp(argv[i], "--fixed") == 0)
        {
            fFixed = true;
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            fHelp = true;
        }
        else if (strcmp(argv[i], "--equation") == 0)
        {
            fShowEquation = true;
        }
        else
        {
            sArgument += argv[i];
        }
    }

    if (sArgument.size())
    {
        if (fShowEquation)
        {
            cout << sArgument << " = ";
        }
        CArithmetic ar;
        sError fErrorPos = ar.setEquation(sArgument.c_str());
        fError = ar.getError();
        if (fError == 0)
        {
            complex<double> fResult = ar.calculate();
            fError = ar.getError();
            if (fError == 0)
            {
                cout << setprecision(13);
                if (fScientific)
                {
                    cout << scientific;
                }
                else if (fFixed)
                {
                    cout << fixed;
                }
                if (fResult.imag() != 0) cout << "complex" << fResult << endl;
                else                     cout << fResult.real() << endl;
            }
            if (fShow)
            {
                ar.show();
            }
        }

        if (fError)
        {
            if (fErrorPos.valid())
            {
                cout << "Fehler an der Position " << fErrorPos.mStart << " -> " << fErrorPos.mStop << endl;
            }
            switch (fError)
            {
            case IDE_AR_BRACE:            cout << "Klammerfehler"; break;
            case IDE_AR_OUTOFRANGE:       cout << "Zahlenbereich verlassen"; break;
            case IDE_AR_OPERATOR:         cout << "aufeinanderfolgende Operatoren sind selten erlaubt"; break;
            case IDE_AR_NOEQUATION:       cout << "Keine Gleichung vorhanden"; break;
            case IDE_AR_ZERO:             cout << "durch 0 geteilt"; break;
            case IDE_AR_NONUMBER:         cout << "Zahl kann nicht konvertiert werden"; break;
            case IDE_AR_NOFUNCTION:       cout << "Ungültige Funktion"; break;
            case IDE_AR_INVALIDCHARACTER: cout << "ungültiges Zeichen"; break;
            case IDE_AR_NO_COMPLEX_IMPL:  cout << "Komplexe Funktion nicht implementiert"; break;
            case IDE_AR_DECIMAL_POINT:    cout << "ungültiges Zeichen"; break;
            default:                      cout << "ups"; break;
                break;
            }
            cout << endl;
        }
    }
    else
    {
        cout << "Aufruf: " << argv[0] << ":" << endl;
        cout << argv[0] << " \"Ausdruck\", z.B.: " << argv[0] << " \"2 * (3 + 4)\"" << endl;
        cout << "Parameter:" << endl;
        cout << "--scientific : Zeigt das Ergebnis mit exponenten Darstellung" << endl;
        cout << "--fixed      : Zeigt das Ergebnis, wenn möglich ohne exponentielle Darstellung" << endl;
        cout << "--show       : Zeigt den Berechnungsbaum" << endl;
        cout << "--help       : Zeigt alle verfügbaren Funktionen und Konstanten an" << endl;
        cout << "--equation   : Zeigt die Gleichung vor dem Ergebnis an"<< endl;
        cout << "--grad       : Argument für trigonometrische Funktionen in Grad"<< endl;
        cout << "--gon        : Argument für trigonometrische Funktionen in Neugrad"<< endl;
        if (fHelp)
        {
            cout << endl;
            cout << "Unäre Funktionen:" << endl;
            for (int i=0; CArithmetic::gm_UnaryFkt[i] != 0; ++i)
            {
                cout << CArithmetic::gm_UnaryFkt[i] << "(arg), " << endl;
            }
            cout << endl;

            cout << "Binäre Functionen:" << endl;
            for (int i=0; CArithmetic::gm_BinaryFkt[i] != 0; ++i)
            {
                cout << CArithmetic::gm_BinaryFkt[i] << "(arg1, arg2), " << endl;
            }
            cout << endl;

            cout << setprecision(13);
            cout << "Konstanten:" << endl;
            for (int i=0; CArithmetic::gm_constants[i].name != 0; ++i)
            {
                cout << CArithmetic::gm_constants[i].name << ", " << CArithmetic::gm_constants[i].Description << ": " << CArithmetic::gm_constants[i].value << endl;
            }
        }
    }

    return fError;
}
