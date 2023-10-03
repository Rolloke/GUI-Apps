#include "helper.h"
#include "logger.h"
#include "actions.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpression>
#include <QRandomGenerator>
#else
#include <QRegExp>
#endif

#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QAction>
#include <QEvent>
#include <QWhatsThisClickedEvent>
#include <QThread>

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <map>

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <stdlib.h>
#endif


namespace txt
{
   const char git[]             = "git";
   const char New[]             = "new";
   const char currentpath_id[]  = "#2";
   const char recursivecmd_id[] = "#r";
   const char invalid[]         = "invalid";
}

QPoint menu_offset(5, 0);

QString getSettingsName(const QString& aItemName)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QRegularExpression fRegEx("([A-Z][A-Za-z0-9:\[]+)");
    auto match = fRegEx.match(aItemName);
    if (match.isValid())
    {
        const auto captured = match.capturedTexts();
        QString fTemp = captured[0];
        fTemp = fTemp.replace(":", "_");
        fTemp = fTemp.replace("[", "_");
        return fTemp;
    }
    else return aItemName;
#else
    QRegExp fRegEx("([A-Z][A-Za-z0-9:\[]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        const auto captured = fRegEx.capturedTexts();
        QString fTemp = captured[0];
        fTemp = fTemp.replace(":", "_");
        fTemp = fTemp.replace("[", "_");
        return fTemp;
    }
    else return aItemName;
#endif
}


QString formatFileSize(quint64 aSize)
{
    using namespace std;

    static map<quint64, QString> exponents;
    const quint64 tera_exponent = 40;
    const quint64 giga_exponent = 30;
    const quint64 mega_exponent = 20;
    const quint64 kilo_exponent = 10;
    const quint64 one = 1;
    const qint32 thousand_digit = 3;
    if (exponents.empty())
    {
        exponents[one << tera_exponent] = "TB";
        exponents[one << giga_exponent] = "GB";
        exponents[one << mega_exponent] = "MB";
        exponents[one << kilo_exponent] = "KB";
    }

    quint64 exponent = tera_exponent;
    for (auto fIt = exponents.rbegin(); fIt != exponents.rend(); ++fIt)
    {
        if (aSize > fIt->first)
        {
            QString thousands = QString::number(static_cast<quint64>((aSize & (fIt->first-1))) >> (exponent - kilo_exponent));
            while (thousands.size() < thousand_digit) thousands = "0" + thousands;
            QString number = QString::number(static_cast<quint64>(aSize >> exponent)) + "."
                      + thousands + " " + fIt->second;
            return number;
        }
        exponent -= kilo_exponent;
    }
    return QString::number(aSize) + " B";
}

QString formatPermissions(const QFile::Permissions& permissions)
{
    QString text = "Owner: ";
    if (permissions.testFlag(QFileDevice::ReadOwner))  text += "R";
    if (permissions.testFlag(QFileDevice::WriteOwner)) text += "W";
    if (permissions.testFlag(QFileDevice::ExeOwner))   text += "X";
    text += ", User: ";
    if (permissions.testFlag(QFileDevice::ReadUser))   text += "R";
    if (permissions.testFlag(QFileDevice::WriteUser))  text += "W";
    if (permissions.testFlag(QFileDevice::ExeUser))    text += "X";
    text += ", Other: ";
    if (permissions.testFlag(QFileDevice::ReadOther))  text += "R";
    if (permissions.testFlag(QFileDevice::WriteOther)) text += "W";
    if (permissions.testFlag(QFileDevice::ExeOther))   text += "X";

    return text;
}

QString get_word_at_position(const QString& sentence, int pos)
{
    for (; pos > 0; --pos)
    {
        if (sentence[pos] == ' ')
        {
            ++pos;
            break;
        }
    }
    int end = sentence.indexOf(' ', pos);
    if (end == -1) end = sentence.size();
    if (pos >= 0 && (end - pos) > 1)
    {
        return sentence.mid(pos, end-pos);
    }
    return sentence;
}

void deleteTopLevelItemOfSelectedTreeWidgetItem(QTreeWidget& aTree)
{
    const auto fList = aTree.selectedItems();
    for (const auto& fSelected : fList)
    {
        auto top_level_item = getTopLevelItem(aTree, fSelected);
        if (top_level_item)
        {
            if (callMessageBox(QObject::tr("Delete %1 entry;%1"), QObject::tr("Tree"), top_level_item->text(0), true) & QMessageBox::Yes)
            {
                aTree.removeItemWidget(top_level_item, 0);
                delete top_level_item;
            }
        }
    }
}

void deleteSelectedTreeWidgetItem(QTreeWidget& aTree)
{
    const auto fList = aTree.selectedItems();
    for (const auto& fSelected : fList)
    {
        auto fParent = fSelected->parent();
        if (fParent)
        {
            fParent->removeChild(fSelected);
            delete fSelected;
        }
        else if (callMessageBox(QObject::tr("Delete %1 entry;%1"), QObject::tr("Tree"), fSelected->text(0), true) & QMessageBox::Yes)
        {
            aTree.removeItemWidget(fSelected, 0);
            delete fSelected;
        }
    }
}

void deleteAllTreeWidgetItem(QTreeWidget& aTree)
{
    for (int i=0; i<aTree.topLevelItemCount(); ++i)
    {
        aTree.takeTopLevelItem((i));
    }
}

int getItemLevel(QTreeWidgetItem* aItem)
{
    int fLevel = 1;
    while (aItem)
    {
        aItem = aItem->parent();
        --fLevel;
    }
    return -fLevel;
}

QTreeWidgetItem* getTopLevelItem(QTreeWidget& aTree, QTreeWidgetItem* aItem, const tGTLIFunction& function)
{
    while (aItem && aTree.indexOfTopLevelItem(aItem) == -1)
    {
        if (function)
        {
            function(aItem);
        }
        aItem = aItem->parent();
    }
    return aItem;
}

void do_with_item_and_children(QTreeWidgetItem* aItem, const tGTLIFunction& function, bool also_leaf)
{
    auto count = aItem->childCount();
    if (also_leaf || count)
    {
        function(aItem);
    }

    for (int i=0; i<count; ++i)
    {
        if (!also_leaf && aItem->child(i)->childCount() == 0)
        {
            continue;
        }
        do_with_item_and_children(aItem->child(i), function, also_leaf);
    }
}

void toggle_expand_item(QTreeWidgetItem* item)
{
    bool expand = !item->isExpanded();
    auto expand_item = [&](QTreeWidgetItem*the_item)
    {
        the_item->setExpanded(expand);
    };
    do_with_item_and_children(item, expand_item, false);
    item->setExpanded(!expand);
}

QTreeWidgetItem* find_child_item(QTreeWidgetItem*parent_item, int column, const QString& name)
{
    for (int i=0; i<parent_item->childCount(); ++i)
    {
        if (parent_item->child(i)->text(column) == name)
        {
            return parent_item->child(i);
        }
    }
    return nullptr;
}

bool containsPathAsChildren(QTreeWidgetItem*parent_item, int column, const QString& right_path)
{
    const QStringList right_parts = right_path.split('/');
    for (const auto&file_name : right_parts)
    {
        parent_item = find_child_item(parent_item, column, file_name);
        if (!parent_item)
        {
            break;
        }
    }
    return parent_item != nullptr;
}

QTreeWidgetItem* insert_as_tree(QTreeWidgetItem* parent_item, int column, const QStringList& path, int level)
{
    QTreeWidgetItem* new_child_item = find_child_item(parent_item, column, path[level]);
    if (!new_child_item || level == path.size() -1)
    {
        new_child_item = new QTreeWidgetItem();
        parent_item->addChild(new_child_item);
        new_child_item->setText(column, path[level]);
    }
    if (++level < path.size())
    {
        return insert_as_tree(new_child_item, column, path, level);
    }
    return new_child_item;
}

#ifdef _WIN32
int win_system(const char *command, bool hide)
{
    // Windows has a system() function which works, but it opens a command prompt window.
    unsigned long       ret_val {0};
    PROCESS_INFORMATION process_info = {0};
    STARTUPINFOA        startup_info = {0};

    std::string tmp_command = "/c ";
    tmp_command += command;

    startup_info.cb = sizeof(STARTUPINFOA);
    size_t cmd_exe_path_size = 0;
    char* cmd_exe_path_ptr =nullptr;
    _dupenv_s(&cmd_exe_path_ptr, &cmd_exe_path_size, "COMSPEC");
    _flushall();  // required for Windows system() calls, probably a good idea here too

    if (CreateProcessA(cmd_exe_path_ptr, (LPSTR)tmp_command.c_str(), NULL, NULL, 0, hide ? CREATE_NO_WINDOW:0, NULL, NULL, &startup_info, &process_info))
    {
        WaitForSingleObject(process_info.hProcess, INFINITE);
        GetExitCodeProcess(process_info.hProcess, &ret_val);
        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);
    }
    else
    {
        ret_val = GetLastError();
    }

    free(cmd_exe_path_ptr);
    return(ret_val);
}
#endif

/// @brief executes a system command and returns result string
/// @param command the command to be executed
/// @param aResultText result of the command
/// @param hide hides the command console (only windows)
/// @param emit_file_path emits the temp file path to gather result asynchroneously
/// @returns success of command execution (0 = success, !0 = error)
int execute(const QString& command, QString& aResultText, bool hide, boost::function<void (const QString &)> emit_file_path)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    static QRandomGenerator rg(123);
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(rg.generate()) + "_result.tmp";
#else
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(qrand()) + "_result.tmp";
#endif
    QString fTempResultFileNameAndPath = fTemp.path();
    QString system_cmd = command + " > " + fTempResultFileNameAndPath;
#ifdef __linux__
    system_cmd += " 2>&1 ";
#endif

    if (emit_file_path)
    {
        emit_file_path(fTempResultFileNameAndPath);
    }

#ifdef _WIN32
    auto fResult = win_system(system_cmd.toStdString().c_str(), hide);
#else
    auto fResult = system(system_cmd.toStdString().c_str());

    UNUSED(hide);

#ifndef __linux__
    if (fResult != NoError)
    {
        system_cmd = command + " 2>> " + fTempResultFileNameAndPath;
        fResult = system(system_cmd.toStdString().c_str());
    }
#endif
#endif

    if (emit_file_path)
    {
        QThread::msleep(150);
        emit_file_path("");
        return fResult;
    }

    std::ostringstream fStringStream;
    std::ifstream fFile(fTempResultFileNameAndPath.toStdString());
    fStringStream << fFile.rdbuf();
    std::string fStreamString = fStringStream.str();
    if (fResult != NoError)
    {
        fStreamString += QObject::tr("\nError occurred executing command: %1").arg(fResult).toStdString();
        if (fResult == ErrorNumberInErrno)
        {
            fStreamString += QObject::tr("\nError number : %1").arg((int)errno).toStdString();
        }
    }
    boost::algorithm::trim_right(fStreamString);
    aResultText = fStreamString.c_str();


    if (!fTemp.remove(fTemp.path()))
    {
        TRACEX(Logger::error, "Could not delete temporary file " << fTempResultFileNameAndPath);
    }

    return fResult;
}

int callMessageBox(const QString& fMessageBoxText, const QString& fFileTypeName, const QString& fFileName, int aIsFile, bool aEditText)
{
    QString file_name(fFileName);
    return callMessageBox(fMessageBoxText, fFileTypeName, file_name, aIsFile, aEditText);
}

int callMessageBox(const QString& aMessageBoxText, const QString& aFileTypeName, QString& aFileName, int aIsFile, bool aEditText)
{
    if (aMessageBoxText != ActionList::sNoCustomCommandMessageBox)
    {
        QStringList fTextList = aMessageBoxText.split(";");
        std::string fText1   = fTextList[0].toStdString().c_str();
        QString message_text;
        QString informative_text;
        switch (fTextList.size())
        {
        case 1:
            message_text = QObject::tr(fText1.c_str()).arg(aFileTypeName);
            informative_text = aFileName;
            break;
        case 2: case 3:
            message_text = QObject::tr(fText1.c_str()).arg(aFileTypeName);
            informative_text = QObject::tr(fTextList[1].toStdString().c_str()).arg(aFileName, aFileTypeName);
            break;
        }

        if (aEditText && fTextList.size() == 3)
        {
            bool ok;
            QString text = QInputDialog::getText(0, message_text, informative_text, QLineEdit::Normal, aFileName, &ok);
            if (ok && !text.isEmpty())
            {
                aFileName = text;
                return QMessageBox::Apply|QMessageBox::Yes;
            }
            return QMessageBox::No;
        }
        else
        {
            QMessageBox fRequestMessage;
            fRequestMessage.setText(message_text);
            fRequestMessage.setInformativeText(informative_text);
            QMessageBox::StandardButtons standard_buttons = QMessageBox::NoButton;
            QMessageBox::StandardButton  default_button   = QMessageBox::NoButton;
            switch (aIsFile)
            {
            case to_one:
                standard_buttons = QMessageBox::Yes | QMessageBox::No;
                default_button   = QMessageBox::Yes;
                break;
            case to_all:
                standard_buttons = QMessageBox::YesToAll | QMessageBox::NoToAll;
                default_button   = QMessageBox::YesToAll;
                break;
            case to_all_or_one:
                standard_buttons = QMessageBox::YesToAll |QMessageBox::Yes | QMessageBox::No;
                default_button   = QMessageBox::Yes;
                break;
            }

            fRequestMessage.setStandardButtons(standard_buttons);
            fRequestMessage.setDefaultButton(default_button);

            return fRequestMessage.exec();
        }
    }
    return QMessageBox::Yes;
}

bool isMessageBoxNo(int msg)
{
    return msg == QMessageBox::No || msg == QMessageBox::NoToAll;
}

const char* getLineFeed()
{
#ifdef __linux__
    return  "\n";
#else
    return "\r\n";
#endif
}


QTreeWidgetHook::QTreeWidgetHook()
{

}

QModelIndex QTreeWidgetHook::indexFromItem(QTreeWidgetItem *item, int column) const
{
    return QTreeWidget::indexFromItem(item, column);
}

QTreeWidgetItem * QTreeWidgetHook::itemFromIndex(const QModelIndex &index) const
{
    return QTreeWidget::itemFromIndex(index);
}

void set_widget_and_action_enabled(QWidget* widget, bool enabled, int action_index)
{
    auto actions = widget->actions();
    if (action_index < actions.size())
    {
        actions[action_index]->setEnabled(enabled);
    }
    widget->setEnabled(enabled);
}


ColorSelector::ColorSelector(Qt::GlobalColor first_color ) :
    m_first_color(first_color),  m_current_color(first_color)
{
}

QColor ColorSelector::get_color_and_increment()
{
    QColor color(m_current_color);
    m_current_color = static_cast<Qt::GlobalColor>(m_current_color + 1);
    while (m_unapplied_color.contains(m_current_color))
    {
        m_current_color = static_cast<Qt::GlobalColor>(m_current_color + 1);
    }
    if (m_current_color == Qt::transparent)
    {
        m_current_color = m_first_color;
    }
    return color;
}

void   ColorSelector::unapply_color(Qt::GlobalColor not_wanted)
{
    m_unapplied_color.append(not_wanted);
}

LinkFilter::LinkFilter(QObject *parent) : QObject(parent)
{

}

bool LinkFilter::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::WhatsThisClicked)
    {
        QWhatsThisClickedEvent* wtcEvent = static_cast<QWhatsThisClickedEvent*>(event);
        Q_EMIT linkClicked(wtcEvent->href());
        return true;
    }
    return false;
}


