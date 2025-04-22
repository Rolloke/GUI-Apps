#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <vector>
#include <string>
#include <string.h>

QDomNode findElement(const QDomNode& aParent, const QString & aName);
std::vector<std::string> SplitPath(const std::string &path);
QString toDate(const QString &aDate);

enum eTable
{
    eFile, ePath, eExtension, eAdded, eModified, eVisited, eID, eLast
};

static const QString gRecentFilePath("/.local/share/recently-used.xbel");

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
, ui(new Ui::MainWindow)
, mDoc("Recent")
{
    ui->setupUi(this);

    QFile file(QDir::homePath() + gRecentFilePath);

    bool result = false;
    if (file.open(QIODevice::ReadOnly))
    {
        result = mDoc.setContent(&file);
        file.close();
    }
    QStringList section_names = { tr("File"), tr("Path"), tr("Type"), tr("added"), tr("modified"), tr("visited"), tr("ID")};
    mListModel = new QStandardItemModel(0, eLast, this);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, section_names[fSection]);
        ui->comboBoxSearchColumn->addItem(section_names[fSection], fSection);
    }
    ui->comboBoxSearchColumn->setCurrentIndex(ePath);
    ui->tableView->setModel(mListModel);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        QHeaderView::ResizeMode resize;
        switch (fSection)
        {
        case eFile: resize = QHeaderView::Stretch;          break;
        //case ePath: resize = QHeaderView::Fixed;        break;
        default:    resize = QHeaderView::ResizeToContents; break;
        }
        ui->tableView->horizontalHeader()->setSectionResizeMode(fSection, resize);
    }
    ui->tableView->horizontalHeader()->setStretchLastSection(false);

    int some_files_do_not_exist = 0;
    if (result)
    {
        int row = 0;
        QDomNode node = mDoc.firstChild();
        for (; !node.isNull(); node = node.nextSibling())
        {
            QDomNode element = findElement(node, "bookmark");
            for (; !element.isNull(); element = element.nextSibling())
            {
                std::vector<std::string> file_parts = SplitPath(element.attributes().namedItem("href").nodeValue().toStdString());
                QFileInfo info((file_parts[ePath] + "/" + file_parts[eFile]).c_str());
                mListModel->insertRows(row, 1, QModelIndex());
                mListModel->setData(mListModel->index(row, eFile     ), QString(file_parts[eFile].c_str()));
                mListModel->setData(mListModel->index(row, ePath     ), QString(file_parts[ePath].c_str()));
                mListModel->setData(mListModel->index(row, eExtension), QString(file_parts[eExtension].c_str()));
                mListModel->setData(mListModel->index(row, eAdded    ), toDate(element.attributes().namedItem("added").nodeValue()));
                mListModel->setData(mListModel->index(row, eModified ), toDate(element.attributes().namedItem("modified").nodeValue()));
                mListModel->setData(mListModel->index(row, eVisited  ), toDate(element.attributes().namedItem("visited").nodeValue()));
                mListModel->setData(mListModel->index(row, eID       ), QVariant(row));
                mListModel->setData(mListModel->index(row, eLast     ), element.attributes().namedItem("href").nodeValue());
                element.setNodeValue(QString::number(row));
                if (!info.exists())
                {
                    ui->tableView->selectionModel()->select(mListModel->index(row, eFile), QItemSelectionModel::Select);
                    ++some_files_do_not_exist;
                }
                ++row;
            }
        }
    }
    if (some_files_do_not_exist)
    {
        ui->statusBar->showMessage(tr("%1 file of the do not exist and have been removed, save list").arg(some_files_do_not_exist));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


QDomNode findElement(const QDomNode& aParent, const QString & aName)
{
    QDomNode fReturn;
    QStringList fNameList = aName.split('/');
    QDomNode fParent = aParent;
    bool bFound = false;
    for (int i=0; i<fNameList.size(); ++i)
    {
        QString sattribute, svalue, ssearch(fNameList.at(i));
        int nAttribute  = (int)aName.indexOf("#"); // attribute is recognized by '#'
        if (nAttribute != -1)
        {
            int nEqual = (int)aName.indexOf("=");  // the value is separated by '='
            if (nEqual != -1)
            {
                sattribute = aName.mid(nAttribute+1, nEqual-nAttribute-1);
                svalue     = aName.mid(nEqual+1, aName.length()-nEqual);
                ssearch    = aName.mid(0, nAttribute);
                nAttribute = 1;                 // use also attribute
            }
            else
            {
                nAttribute = 0;
            }
        }
        else
        {
            nAttribute = 0;
        }

        bFound = false;
        for(QDomNode n = fParent.firstChild(); !bFound && !n.isNull(); n = n.nextSibling())
        {
            if (n.nodeName() == ssearch)          // found name
            {
                if (nAttribute == 1)            // use attribute?
                {                               // compare the attributes value
                    QDomNode fattr = n.attributes().namedItem(sattribute);
                    if (!fattr.isNull() && svalue == fattr.nodeValue())
                    {
                        fParent = n;
                        bFound = true;
                    }
                }
                else
                {
                    fParent = n;
                    bFound = true;
                }
            }
            else                                // search recursively
            {
                QDomNode node = findElement(n, aName);
                if (!node.isNull())
                {
                    fParent = node;
                    bFound = true;
                }
            }
        }

        if (bFound && i == fNameList.size()-1)
        {
            fReturn = fParent;
        }
    }
    return fReturn;
}



std::vector<std::string> SplitPath(const std::string &path)
{
    std::string delims("/\\");
    std::vector<std::string> elements;
    int pos = path.find_last_of(delims);
    int posfile = path.find("file://");
    posfile = (posfile != -1) ? posfile + 7 : 0;
    if (pos != -1)
    {
        elements.push_back(path.substr(pos + 1));
        elements.push_back(path.substr(posfile, pos-posfile));
        pos = elements[0].find_last_of('.');
        if (pos != -1)
        {
            elements.push_back(elements[0].substr(pos+1));
        }
    }
    while (elements.size() < 3)
    {
        elements.push_back("");
    }
    return elements;
}

QString toDate(const QString &aDate)
{
    // source:      2017-03-29T19:55:26Z
    // destination:19:55:26,29.03.2017
    int year(0), month(0), day(0), hour(0), minute(0), second(0);
    sscanf(aDate.toStdString().c_str(), "%04d-%02d-%02dT%02d:%02d:%02dZ", &year, &month, &day, &hour, &minute, &second);
    return QString("%1:%2:%3 %4.%5.%6").arg(hour, 2).arg(minute, 2).arg(second, 2).arg(day, 2).arg(month, 2).arg(year, 4);
}

void MainWindow::on_pushButtonSave_clicked()
{
    QString xml = mDoc.toString();
    QFile file(QDir::homePath() + gRecentFilePath);
    QDir::setCurrent(QDir::homePath());
    file.open(QFile::WriteOnly|QFile::Text);
    file.write(xml.toStdString().c_str());
    file.close();
}

void MainWindow::on_pushButtonDeleteSelected_clicked()
{
    QDomNode element;
    QDomNode node = mDoc.firstChild();
    for (; !node.isNull(); node = node.nextSibling())
    {
        element = findElement(node, "bookmark");
        if (!element.isNull())
        {
            break;
        }
    }
    std::vector<QDomNode> node_list;
    std::vector<int>      row_list;
    int row = 0;
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    do
    {
        row = element.nodeValue().toInt();
        for (auto index : indexes)
        {
            auto data = mListModel->data(mListModel->index(index.row(), static_cast<int>(eID)));
            if (data.isValid() && data.toInt() == row)
            {
                node_list.push_back(element);
                row_list.push_back(index.row());
                break;
            }
        }
        if (indexes.size() == static_cast<int>(row_list.size())) break;
    }
    while (!(element = element.nextSibling()).isNull());

    ui->tableView->clearSelection();

    for (const auto& item : node_list)
    {
        item.parentNode().removeChild(item);
    }

    for (auto row = row_list.rbegin(); row != row_list.rend(); ++row)
    {
        mListModel->removeRow(*row);
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString fFile = "xdg-open \"" + mListModel->data(mListModel->index(index.row(), ePath)).toString() + "/" +
            mListModel->data(mListModel->index(index.row(), eFile)).toString() + "\"";

    QUrl fUrl(fFile);
    fFile = fUrl.path();

    int fError = ::system(fFile.toStdString().c_str());
    if (fError)
    {
        mErrorMsgDlg.showMessage(QString(strerror(fError)));
        mErrorMsgDlg.show();
    }
}

void MainWindow::on_pushButtonSelect_clicked()
{
    QString fSelect = ui->lineEditSelection->text();
    int fRows = mListModel->rowCount();
    int fSection = ui->comboBoxSearchColumn->currentIndex();
    for (int fRow=0; fRow < fRows; ++fRow)
    {
        if (mListModel->data(mListModel->index(fRow, fSection)).toString().indexOf(fSelect) != -1)
        {
            ui->tableView->selectionModel()->select(mListModel->index(fRow, fSection), QItemSelectionModel::Select);
        }
    }
}
