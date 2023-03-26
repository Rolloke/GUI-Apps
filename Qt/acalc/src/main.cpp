#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>

#include "carithmetic.h"

using namespace std;

int    calculate(const string& sArgument);
void   print_help(const char *app_name);
string parse_cmd_line(int argc, char *argv[]);
bool   compare_equation(const std::string& a1, const std::string& a2);

/// raw mode functions
void   enable_raw_mode();
string get_string();
bool   get_cursor_position(int *rows, int *cols);
void   set_cursor_position(int row, int col);

bool g_show          = false;
bool g_scientific    = false;
bool g_fixed         = false;
bool g_help          = false;
bool g_show_equation = false;
bool g_console_mode  = false;
bool g_xconsole_mode = false;

vector<string> g_history;

/// TODO: evaluate delete, insert
/// TODO: console mode with user equations
/// - e.g.: deg2rad(x) = x*p/180
/// TODO: console mode with variables?
/// - m*x+b

int main(int argc, char *argv[])
{
    string argument = parse_cmd_line(argc, argv);

    int error_code = 0;
    if (g_console_mode)
    {
        cout << "Gleichung eingeben" << endl;
        do
        {
            cout << ">> ";
            cin >> argument;
            calculate(argument);
        }
        while (argument != "q");
    }
    else if (g_xconsole_mode)
    {
        enable_raw_mode();
        cout << "Gleichung eingeben\r\n";
        do
        {
            cout << ">> ";
            argument = get_string();
            cout << "\r\n>> ";
            if (   calculate(argument) == IDE_AR_OK
                && find_if(g_history.begin(), g_history.end(), [argument](const auto& element)
                   { return compare_equation(argument, element); }) == g_history.end())
            {
                g_history.push_back(argument);
            }
            cout << "\r";
        }
        while (argument != "q");
    }
    else
    {
        error_code = calculate(argument);
        if (error_code == IDE_AR_NOEQUATION)
        {
            print_help(argv[0]);
        }
    }

    return error_code;
}

string parse_cmd_line(int argc, char *argv[])
{
    string argument   = "";

    for (int i=1; i< argc; ++i)
    {
        if (strcmp(argv[i], "--show") == 0)
        {
            g_show = true;
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
            g_scientific = true;
        }
        else if (strcmp(argv[i], "--fixed") == 0)
        {
            g_fixed = true;
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            g_help = true;
        }
        else if (strcmp(argv[i], "--equation") == 0)
        {
            g_show_equation = true;
        }
        else if (strcmp(argv[i], "--console") == 0)
        {
            g_console_mode = true;
        }
        else if (strcmp(argv[i], "--xconsole") == 0)
        {
            g_xconsole_mode = true;
        }
        else
        {
            argument += argv[i];
        }
    }
    return argument;
}

int calculate(const string& sArgument)
{
    int  fError        = IDE_AR_NOEQUATION;

    if (sArgument.size())
    {
        if (g_show_equation)
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
                if (g_scientific)
                {
                    cout << scientific;
                }
                else if (g_fixed)
                {
                    cout << fixed;
                }
                if (fResult.imag() != 0) cout << "complex" << fResult << endl;
                else                     cout << fResult.real() << endl;
            }
            if (g_show)
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
    cout << "--xconsole   : Konsolenmodus mit verbesserter Eingabe"<< endl;
    if (g_help)
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

bool allowed(int a)
{
    if (isdigit(a)) return false;
    if (isspace(a)) return false;
    return true;
}

bool compare_equation(const std::string& a1, const std::string& a2)
{
    string b1;
    copy_if(a1.begin(), a1.end(),  back_inserter(b1), [](const auto e) { return allowed(e); });
    string b2;
    copy_if(a2.begin(), a2.end(),  back_inserter(b2), [](const auto e) { return allowed(e); });
    return b1 == b2;
}


// Functions for raw input mode

#define KB_UP 65
#define KB_DOWN 66
#define KB_LEFT 68
#define KB_RIGHT 67
#define KB_ESCAPE 27
#define KB_ENTER 13
#define KB_BACK 127
#define KB_INSERT 50
#define KB_DELETE 51
#define KB_HOME 72
#define KB_END 70
#define KB_PAGE_UP 53
#define KB_PAGE_DOWN 54

struct editorConfig {
  struct termios orig_termios;
};

struct editorConfig E;


void die(const char*s)
{
    cout << s << "\r\n";
}
void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
  {
      die("tcsetattr");
  }
}

void enable_raw_mode()
{
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
  {
      die("tcgetattr");
  }

  atexit(disableRawMode);
  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
  {
      die("tcsetattr");
  }
}

#define CTRL_KEY(k) ((k) & 0x1f)

void draw_line(int start, const string& line)
{
    for (unsigned int i=start; i<line.size(); ++i)
    {
        printf("%c", line[i]);
    }
    if (start == 0) fflush(stdout);
}

void clear_line(int start_row, int start_col, int length)
{
    set_cursor_position(start_row, start_col);
    for (int i=0; i<length; ++i)
    {
        printf(" ");
    }
    fflush(stdout);
}


string get_string()
{
    fflush(stdout);
    string line;
    std::string escape;
    int          start_row = 0;
    int          start_col = 0;
    unsigned int cursor    = 0;
    int          history   = -1;

    get_cursor_position(&start_row, &start_col);
    escape.clear();
    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
        {
            die("read");
        }
        if (c)
        {
            if (iscntrl(c))
            {
                if (c == KB_ENTER) break;
                switch(c)
                {
                case KB_BACK:
                if (cursor)
                {
                    escape += static_cast<char>(KB_ESCAPE);
                    escape += '[';
                    line.erase(--cursor, 1);
                } break;
                case KB_ESCAPE:
                    escape += static_cast<char>(c);
                    continue;
                default: printf("%d\r\n", c); break;
                }
            }
            if (escape.size())
            {
                bool write_escape          = true;
                int  additional_escape_cmd = 0;
                switch (c)
                {
                case '[': escape += c;
                    continue;
                case KB_UP:
                    if (history < static_cast<int>(g_history.size()) - 1)
                    {
                        ++history;
                        clear_line(start_row, start_col, line.size());
                        line = g_history[history];
                        set_cursor_position(start_row, start_col);
                        draw_line(0, line);
                        cursor = line.size();
                        set_cursor_position(start_row, start_col+cursor);
                    }
                    write_escape = false;
                    break;
                case KB_DOWN:
                    if (history > 0)
                    {
                        --history;
                        clear_line(start_row, start_col, line.size());
                        line = g_history[history];
                        set_cursor_position(start_row, start_col);
                        draw_line(0, line);
                        cursor = line.size();
                        set_cursor_position(start_row, start_col+cursor);
                    }
                    else
                    {
                        history = -1;
                        set_cursor_position(start_row, start_col);
                        clear_line(start_row, start_col, line.size());
                        line.clear();
                        cursor = 0;
                        set_cursor_position(start_row, start_col);
                    }
                    write_escape = false;
                    break;
                case KB_LEFT:
                    if (cursor) --cursor;
                    else write_escape = false;
                    break;
                case KB_RIGHT:
                    if (cursor<line.size()) ++cursor;
                    else write_escape = false;
                    break;
                case KB_HOME:
                {
                    cursor = 0;
                    int row = 0, col = 0;
                    if (get_cursor_position(&row, &col))
                    {
                        set_cursor_position(row, start_col);
                    }
                    write_escape = false;
                }   break;
                case KB_END:
                {
                    cursor = line.size();
                    int row = 0, col = 0;
                    if (get_cursor_position(&row, &col))
                    {
                        set_cursor_position(row, start_col+cursor);
                    }
                    write_escape = false;
                }   break;
                case KB_INSERT: /// TODO: KB_INSERT
                    write_escape = false;
                    break;
                case KB_DELETE: /// TODO: KB_DELETE
                    write_escape = false;
                    break;
                case KB_BACK:
                    c = KB_LEFT;
                    additional_escape_cmd = 'K';
                    break;
                case KB_PAGE_UP: case KB_PAGE_DOWN: case 126: /// NOTE: ignore
                    write_escape = false;
                    break;
                default:  /// NOTE: ignore, but show number
                    printf("e:%d\r\n", c);
                    write_escape = false;
                    break;
                }
                if (write_escape)
                {
                    escape += c;
                    ssize_t n = write(STDOUT_FILENO, escape.c_str(), escape.size());
                    if (additional_escape_cmd)
                    {
                        escape.back() = additional_escape_cmd;
                        n = write(STDOUT_FILENO, escape.c_str(), escape.size());
                    }
                    (void)(n);
                }
                escape.clear();
            }
            else
            {
                int row = 0, col = 0;
                line.insert(cursor, 1, c);
                ++cursor;

                printf("%c", c);
                if (cursor != line.size())
                {
                    draw_line(cursor, line);
                    get_cursor_position(&row, &col);
                }
                fflush(stdout);
                if (row && col)
                {
                    set_cursor_position(row, col+1);
                }
            }
        }
        else
        {
            escape.clear();
        }
    }
    return line;
}

void set_cursor_position(int row, int col)
{
    char sz[32];
    sprintf(sz, "\x1b[%d;%dH", row, col);
    ssize_t n =write(STDOUT_FILENO, sz, strlen(sz));
    (void)(n);
}

bool get_cursor_position(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return false;
    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[') return false;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return false;
    return true;
}
