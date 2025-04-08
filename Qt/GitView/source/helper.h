#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QTreeWidget>
#include <string>
#include <functional>
#include <QFile>
#include <QObject>
#ifdef USE_BOOST
#include <boost/function.hpp>
#else
#include <functional>
#endif

namespace txt
{
   extern const char git[];
   extern const char New[];
   extern const char invalid[];

   extern const char set_currentpath[];
   extern const char cmd_recursive[];
   extern const char root_path[];
   extern const char relative_path[];
   extern const char file_name[];
   extern const char file_ext[];
   extern const char file_basename[];

   extern const char no_double_entries[];
}


extern QPoint menu_offset;

#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STR_IF(SETTING, ITEM) if (ITEM.size())    SETTING.setValue(getSettingsName(#ITEM), ITEM);
#define STORE_STRFN(SETTING, ITEM, NAME, FUNC) SETTING.setValue(NAME, FUNC(ITEM))
#define STORE_STRF(SETTING, ITEM, FUNC) STORE_STRFN(SETTING, ITEM, getSettingsName(#ITEM), FUNC)

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()
#define LOAD_STRFN(SETTING, ITEM, NAME, FUNC_OUT, FUNC_IN, CONVERT) ITEM = FUNC_OUT(fSettings.value(NAME, QVariant(FUNC_IN(ITEM))).CONVERT());
#define LOAD_STRF(SETTING, ITEM, FUNC_OUT, FUNC_IN, CONVERT) ITEM = LOAD_STRFN(SETTING, ITEM, getSettingsName(#ITEM), FUNC_OUT, FUNC_IN, CONVERT)

#define INT(n) static_cast<qint32>(n)

typedef std::function<void(QTreeWidgetItem*&)> tGTLIFunction;

QString getSettingsName(const QString& aItemName);

QString formatFileSize(quint64 aSize);
QString formatPermissions(const QFile::Permissions& permissions);

QString get_word_at_position(const QString& sentence, int pos);

bool is_whole_word(const QString& text);

bool get_pid_list(const QString& name, QStringList& pid_list);


void deleteTopLevelItemOfSelectedTreeWidgetItem(QTreeWidget& aTree, const tGTLIFunction& function = {});
void deleteSelectedTreeWidgetItem(QTreeWidget& aTree);
void deleteAllTreeWidgetItem(QTreeWidget& aTree);

QTreeWidgetItem* getTopLevelItem(QTreeWidget& aTree, QTreeWidgetItem* aItem, const tGTLIFunction& function = {});
void do_with_item_and_children(QTreeWidgetItem* aItem, const tGTLIFunction& function, bool also_leaf = true);
void toggle_expand_item(QTreeWidgetItem* item);
int getItemLevel(QTreeWidgetItem* aItem);
QTreeWidgetItem* find_child_item(QTreeWidgetItem*parent_item, int column, const QString& name);
bool containsPathAsChildren(QTreeWidgetItem*, int, const QString& );
QTreeWidgetItem* insert_as_tree(QTreeWidgetItem* parent_item, int column, const QStringList&path, int level=0);

enum error
{
    ErrorNumberInErrno = -1,
    NoError = 0
};

#ifdef USE_BOOST
extern boost::function<void (const QString &)> g_test_command_only;
int execute(const QString& command, QString& aResultText, bool hide=true, boost::function<void (const QString &)> emit_file_path = {});
#else
extern std::function<void (const QString &)> g_test_command_only;
int execute(const QString& command, QString& aResultText, bool hide=true, std::function<void (const QString &)> emit_file_path = {});
#endif



enum msg_box_buttons
{
    to_all = false,
    to_one = true,
    to_all_or_one = 2
};

int callMessageBox(const QString& fMessageBoxText, const QString& fFileTypeName="", const QString& fFileName="", int aIsFile = true, bool aEditText=true);
int callMessageBox(const QString& fMessageBoxText, const QString& fFileTypeName, QString& fFileName, int aIsFile = true, bool aEditText=true);
bool isMessageBoxNo(int msg);

const char* getLineFeed();

class QTreeWidgetHook: public QTreeWidget
{
public:
    QTreeWidgetHook();

    QModelIndex indexFromItem(QTreeWidgetItem *item, int column = 0) const;
    QTreeWidgetItem *itemFromIndex(const QModelIndex &index) const;

private:
};

void set_widget_and_action_enabled(QWidget* widget, bool enabled, int action_index=0);


class ColorSelector
{
public:
    ColorSelector(Qt::GlobalColor first_color = Qt::red);
    QColor get_color_and_increment();
    void   unapply_color(Qt::GlobalColor not_wanted);

    static void   set_dark_mode(bool dark);
    static bool   is_dark_mode();

private:
    Qt::GlobalColor          m_first_color;
    Qt::GlobalColor          m_current_color;
    QVector<Qt::GlobalColor> m_unapplied_color;
    static bool              m_dark_mode;
};

class LinkFilter :public QObject
{
    Q_OBJECT
public:
    explicit LinkFilter(QObject* parent=0);

Q_SIGNALS:
    void linkClicked(const QString&);

public Q_SLOTS:

protected:
    bool eventFilter(QObject *watched, QEvent *event);

};

#if 0
/// \brief variadic count_equal
/// \param a comparison parameter
/// \param b variadic parameter(s)
/// \return number of b(s) equal two a
template <typename T, typename ... Tn>
constexpr int count_equal(T a, Tn... b)
{
    return ( ( a == b ) + ... );
}

/// \brief variadic is_exclusive_one_equal_to
/// \note use as xor for all b(s) XOR(b1, ..., bn) = is_exclusive_one_equal_to(true, b1, ..., bn);
/// \param a comparison parameter
/// \param b variadic parameter(s)
/// \return true, if only one of the b(s) is equal to a
template <typename T, typename ... Tn>
constexpr bool is_exclusive_one_equal_to(T a, Tn... b)
{
    return ( ( a == b ) + ... ) == 1;
}

/// \brief variadic is_any_equal_to
/// \param a comparison parameter
/// \param b variadic parameter(s)
/// \return true, if any of b(s) are equal two a
template <typename T, typename ... Tn>
constexpr bool is_any_equal_to(T a, Tn... b)
{
    return ( ( a == b ) || ... );
}

/// \brief variadic are_all_equal_to
/// \param a comparison parameter
/// \param b variadic parameter(s)
/// \return true, if all of b(s) are equal two a
template <typename T, typename ... Tn>
constexpr bool are_all_equal_to(T a, Tn... b)
{
    return ( ( a == b ) && ... );
}

/// \brief variadic is_in_range
/// \param min minimum for all parameters
/// \param max maximum for all parameters
/// \param num variadic parameter(s)
/// \return true, if all of num(s) are within range
template <typename T, typename ... Tn>
constexpr bool is_in_range(T min, T max, Tn... num)
{
    return ( ( min <= num && num <= max)  && ... );
}

template <typename T>
constexpr T vmin(T a, T b)
{
    return std::min(a, b);
}

template <typename T, typename ... Tn>
constexpr T vmin(T a, T b, Tn&&... args)
{
    return (a < b) ? vmin(a, std::forward<Tn>(args)...) : vmin(b, std::forward<Tn>(args)...);
}

template <typename T, typename _Compare>
constexpr T value_of(_Compare comp, T a, T b)
{
    return comp(a, b) ? a : b;
}

template <typename T, typename _Compare, typename ... Tn>
constexpr T value_of(_Compare comp, T a, T b, Tn&&... args)
{
    return (comp(a, b)) ? value_of(comp, a, std::forward<Tn>(args)...) : value_of(comp, b, std::forward<Tn>(args)...);
}
#else

template <typename T, typename  Tn>
constexpr bool is_any_equal_to(T a, Tn b)
{
    return ( ( a == b ) );
}

template <typename T, typename  T1, typename  T2>
constexpr bool is_any_equal_to(T a, T1 b, T2 c)
{
    return ( ( a == b ) || (a == c) );
}

template <typename T, typename  T1, typename  T2, typename  T3>
constexpr bool is_any_equal_to(T a, T1 b, T2 c, T3 d)
{
    return ( ( a == b ) || (a == c) || (a == d) );
}

template <typename T, typename  Tn>
constexpr bool is_in_range(T min, T max, Tn num)
{
    return ( ( min <= num && num <= max)  );
}

#endif

#endif // HELPER_H
