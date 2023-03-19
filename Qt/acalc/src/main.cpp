#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>

#include "carithmetic.h"

#define KB_UP 65
#define KB_DOWN 66
#define KB_LEFT 68
#define KB_RIGHT 67
#define KB_ESCAPE 27
#define KB_ENTER 10

using namespace std;

int    calulate(const string& sArgument);
void   print_help(const char *app_name);
string parse_cmd_line(int argc, char *argv[]);
void   start_filter_keys();
int    kbhit();

bool fShow         = false;
bool fScientific   = false;
bool fFixed        = false;
bool fHelp         = false;
bool fShowEquation = false;
bool fConsoleMode  = false;

/// TODO: create history of successfull inputs
/// TODO: evaluate cursor up down
/// TODO: console mode with user equations
/// - e.g.: deg2rad(x) = x*p/180
/// TODO: console mode with variables?
/// - m*x+b

string get_string()
{
    std::string line;
    int c = 0;
    do
    {
        int character = kbhit();
        if (character == 1)
        {
            c = getc(stdin);
            if (c != KB_ENTER)
            {
                line += c;
            }
            //cout << "c : " << c << endl;
            character = 0;
        }
        while (character)
        {
            c = getc(stdin);
            //cout << "c = " << c << endl;
            --character;
        }
    } while(c != KB_ENTER);

    return line;
}

struct editorConfig {
  struct termios orig_termios;
};

struct editorConfig E;

#include <unistd.h>

void die(const char*s)
{
    cout << s << endl;
}
void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

#define CTRL_KEY(k) ((k) & 0x1f)

int main(int argc, char *argv[])
{
    string sArgument   = parse_cmd_line(argc, argv);
    int fError = 0;
    if (fConsoleMode)
    {
        enableRawMode();
//        start_filter_keys();
        cout << "Gleichung eingeben" << endl;
        do
        {
            cout << ">> ";
            fflush(stdout);
            //sArgument = get_string();
            while (1) {
              char c = '\0';
              if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
              if (iscntrl(c)) {
                printf("%d\r\n", c);
              } else {
                printf("%d ('%c')\r\n", c, c);
              }
              if (c == CTRL_KEY('q')) break;
            }
            //cin >> sArgument;
            cout << ">> ";
            calulate(sArgument);
        }
        while (sArgument != "q");
    }
    else
    {
        fError = calulate(sArgument);
        if (fError == IDE_AR_NOEQUATION)
        {
            print_help(argv[0]);
        }
    }

    return fError;
}

string parse_cmd_line(int argc, char *argv[])
{
    string sArgument   = "";

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
        else if (strcmp(argv[i], "--console") == 0)
        {
            fConsoleMode = true;
        }
        else
        {
            sArgument += argv[i];
        }
    }
    return sArgument;
}

int calulate(const string& sArgument)
{
    int  fError        = IDE_AR_NOEQUATION;

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
    return fError;
}

void print_help(const char* app_name)
{
    cout << "Aufruf: " << app_name << ":" << endl;
    cout << app_name << " \"Ausdruck\", z.B.: " << app_name << " \"2 * (3 + 4)\"" << endl;
    cout << "Parameter:" << endl;
    cout << "--scientific : Zeigt das Ergebnis mit exponenten Darstellung" << endl;
    cout << "--fixed      : Zeigt das Ergebnis, wenn möglich ohne exponentielle Darstellung" << endl;
    cout << "--show       : Zeigt den Berechnungsbaum" << endl;
    cout << "--help       : Zeigt alle verfügbaren Funktionen und Konstanten an" << endl;
    cout << "--equation   : Zeigt die Gleichung vor dem Ergebnis an"<< endl;
    cout << "--grad       : Argument für trigonometrische Funktionen in Grad"<< endl;
    cout << "--gon        : Argument für trigonometrische Funktionen in Neugrad"<< endl;
    cout << "--console    : Konsolenmodus"<< endl;
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


int kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting;
}

void* filter_keys(void*)
{
    char KB_code = 0;
    while(KB_code != KB_ESCAPE)
    {
        if (kbhit())
        {
            KB_code = getc(stdin);
            printf("KB_code = %i ",KB_code);

            switch (KB_code)
            {
            case KB_LEFT:
                cout << "left";
                //Do something
                break;

            case KB_RIGHT:
                cout << "right";
                //Do something
                break;

            case KB_UP:
                cout << "up";
                //Do something
                break;

            case KB_DOWN:
                cout << "down";
                //Do something
                break;

            }
        }
    }
    return 0;
}

void start_filter_keys()
{
    pthread_attr_t attr;
    int result = pthread_attr_init(&attr);
    if (result == 0)
    {
        pthread_t id = 0;
        result = pthread_create(&id, &attr, &filter_keys, 0);
        if (result == 0)
        {
            cout << "thread started " << id << endl;
        }
    }
}
