#include "helper.h"
#include "logger.h"
#include "actions.h"

#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QAction>

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <map>

#ifndef __linux__
//#define USE_ShellExecute 1
#include <windows.h>

#include <shellapi.h>
#endif

QString getSettingsName(const QString& aItemName)
{
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

void deleteTopLevelItemOfSelectedTreeWidgetItem(QTreeWidget& aTree)
{
    auto fList = aTree.selectedItems();
    for (auto fSelected : fList)
    {
        fSelected = getTopLevelItem(aTree, fSelected);
        if (fSelected)
        {
            if (callMessageBox(QObject::tr("Delete %1 entry;%1"), QObject::tr("Tree"), fSelected->text(0), true) == QMessageBox::Yes)
            {
                aTree.removeItemWidget(fSelected, 0);
                delete fSelected;
            }
        }
    }
}

void deleteSelectedTreeWidgetItem(QTreeWidget& aTree)
{
    auto fList = aTree.selectedItems();
    for (const auto& fSelected : fList)
    {
        auto fParent = fSelected->parent();
        if (fParent)
        {
            fParent->removeChild(fSelected);
            delete fSelected;
        }
        else if (callMessageBox(QObject::tr("Delete %1 entry;%1"), QObject::tr("Tree"), fSelected->text(0), true) == QMessageBox::Yes)
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
    QStringList right_parts = right_path.split('/');
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

int execute(const QString& command, QString& aResultText, bool hide)
{
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(qrand()) + "_result.tmp";
    QString fTempResultFileNameAndPath = fTemp.path();
    QString system_cmd = command + " > " + fTempResultFileNameAndPath;
#ifdef __linux__
    system_cmd += " 2>&1 ";
#endif

#ifdef USE_ShellExecute
    system_cmd = "/C " + system_cmd;
    int instance = reinterpret_cast<std::uint64_t>(ShellExecuteA(0, "open", "cmd.exe", system_cmd.toStdString().c_str(), 0, hide ? SW_HIDE: SW_SHOWNORMAL));
    TRACE(Logger::error, "GitView", "Execute result %d", instance);
    int fResult = NoError;
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

    std::ostringstream fStringStream;
    std::ifstream fFile(fTempResultFileNameAndPath.toStdString());
    fStringStream << fFile.rdbuf();
    std::string fStreamString = fStringStream.str();
    boost::algorithm::trim_right(fStreamString);
    if (fResult != NoError)
    {
        fStringStream << QObject::tr("Error occurred executing command: ").toStdString() << fResult;
        if (fResult == ErrorNumberInErrno)
        {
            fStringStream << QObject::tr("Error number : ").toStdString() << errno;
        }
    }
    aResultText = fStreamString.c_str();

    if (!fTemp.remove(fTemp.path()))
    {
        TRACE(Logger::error, "Could not delete temporary file %s", fTempResultFileNameAndPath.toStdString().c_str());
    }

    return fResult;
}

int callMessageBox(const QString& fMessageBoxText, const QString& fFileTypeName, const QString& fFileName, bool aIsFile)
{
    QString file_name(fFileName);
    return callMessageBox(fMessageBoxText, fFileTypeName, file_name, aIsFile, false);
}

int callMessageBox(const QString& aMessageBoxText, const QString& aFileTypeName, QString& aFileName, bool aIsFile, bool aEditText)
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
                return QMessageBox::Yes;
            }
            return QMessageBox::No;
        }
        else
        {
            QMessageBox fRequestMessage;
            fRequestMessage.setText(message_text);
            fRequestMessage.setInformativeText(informative_text);

            fRequestMessage.setStandardButtons(aIsFile ? QMessageBox::Yes | QMessageBox::No : QMessageBox::YesToAll | QMessageBox::NoToAll);
            fRequestMessage.setDefaultButton(  aIsFile ? QMessageBox::Yes : QMessageBox::YesToAll);

            return fRequestMessage.exec();
        }
    }
    return QMessageBox::Yes;
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
