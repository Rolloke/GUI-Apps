#include "helper.h"
#include "logger.h"
#include "actions.h"

#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMessageBox>
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

    static map<quint64, QString> fMap;
    const quint64 fTeraExponent = 40;
    const quint64 fGigaExponent = 30;
    const quint64 fMegaExponent = 20;
    const quint64 fKiloExponent = 10;
    const quint64 fOne = 1;
    const qint32 fThousandDigit = 3;
    if (fMap.empty())
    {
        fMap[fOne << fTeraExponent] = "TB";
        fMap[fOne << fGigaExponent] = "GB";
        fMap[fOne << fMegaExponent] = "MB";
        fMap[fOne << fKiloExponent] = "KB";
    }

    quint64 fExp = fTeraExponent;
    for (auto fIt = fMap.rbegin(); fIt != fMap.rend(); ++fIt)
    {
        if (aSize > fIt->first)
        {
            QString fNumber;
            QString fThousands = QString::number(static_cast<quint64>((aSize & (fIt->first-1))) >> (fExp - fKiloExponent));
            while (fThousands.size() < fThousandDigit) fThousands = "0" + fThousands;
            fNumber = QString::number(static_cast<quint64>(aSize >> fExp)) + "."
                      + fThousands + " " + fIt->second;
            return fNumber;
        }
        fExp -= fKiloExponent;
    }
    return QString::number(aSize) + " B";
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
        if (fParent == 0) aTree.removeItemWidget(fSelected, 0);
        else              fParent->removeChild(fSelected);
        delete fSelected;
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

QTreeWidgetItem* getTopLevelItem(QTreeWidget& aTree, QTreeWidgetItem* aItem)
{
    while (aItem && aTree.indexOfTopLevelItem(aItem) == -1)
    {
        aItem = aItem->parent();
    }
    return aItem;
}

bool containsPathAsChildren(QTreeWidgetItem*parent_item, int column, const QString& right_path)
{
    QStringList right_parts = right_path.split('/');
    for (const auto&file_name : right_parts)
    {
        bool found = false;
        for (int i=0; i<parent_item->childCount(); ++i)
        {
            if (parent_item->child(i)->text(column) == file_name)
            {
                found = true;
                parent_item = parent_item->child(i);
                break;
            }
        }
        if (!found)
        {
            parent_item = nullptr;
            break;
        }
    }
    return parent_item != nullptr;
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

int callMessageBox(const QString& aMessageBoxText, const QString& aFileTypeName, const QString& aFileName, bool aIsFile)
{
    if (aMessageBoxText != ActionList::sNoCustomCommandMessageBox)
    {
        QStringList fTextList = aMessageBoxText.split(";");
        QMessageBox fRequestMessage;

        std::string fText1   = fTextList[0].toStdString().c_str();
        switch (fTextList.size())
        {
            case 1:
                fRequestMessage.setText(QObject::tr(fText1.c_str()).arg(aFileTypeName));
                fRequestMessage.setInformativeText(aFileName);
                break;
            case 2:
                fRequestMessage.setText(QObject::tr(fText1.c_str()).arg(aFileTypeName));
                fRequestMessage.setInformativeText(QObject::tr(fTextList[1].toStdString().c_str()).arg(aFileName, aFileTypeName));
                break;
        }

        fRequestMessage.setStandardButtons(aIsFile ? QMessageBox::Yes | QMessageBox::No : QMessageBox::YesToAll | QMessageBox::NoToAll);
        fRequestMessage.setDefaultButton(  aIsFile ? QMessageBox::Yes : QMessageBox::YesToAll);

        return fRequestMessage.exec();
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
