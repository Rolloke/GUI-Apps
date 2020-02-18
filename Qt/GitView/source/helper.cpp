#include "helper.h"
#include "logger.h"

#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <map>

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        return fRegEx.capturedTexts()[0];
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
    for (map<quint64, QString>::reverse_iterator fIt = fMap.rbegin(); fIt != fMap.rend(); ++fIt)
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

void deleteSelectedTreeWidgetItem(QTreeWidget& aTree)
{
    for (auto fSelected : aTree.selectedItems())
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

int execute(const QString& command, QString& aResultText)
{
    QDir fTemp = QDir::tempPath() + "/cmd_" + QString::number(qrand()) + "_result.tmp";
    QString fTempResultFileNameAndPath = fTemp.path();
    auto fResult = system((command + " > " + fTempResultFileNameAndPath).toStdString().c_str());

    std::ostringstream fStringStream;
    std::ifstream fFile(fTempResultFileNameAndPath.toStdString());
    fStringStream << fFile.rdbuf();
    std::string fStreamString = fStringStream.str();
    boost::algorithm::trim_right(fStreamString);
    aResultText = fStreamString.c_str();

    if (!fTemp.remove(fTemp.path()))
    {
        TRACE(Logger::error, "FanControl", "Could not delete temporary file %s", fTempResultFileNameAndPath.toStdString().c_str());
    }

    return fResult;

}
