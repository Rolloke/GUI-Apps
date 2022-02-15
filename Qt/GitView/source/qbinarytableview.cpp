#include "qbinarytableview.h"
#include "logger.h"

#define INT(X) static_cast<int>(X)

qbinarytableview::qbinarytableview(QWidget *parent) : QTableView(parent)
{
    BinaryTableModel* ListModel = new BinaryTableModel(0, INT(BinaryTableModel::Table::Last), this);
    QStringList fSectionNames = { tr("Pos"), tr("Typed"), tr("Character") };
    for (int fSection = 0; fSection < INT(BinaryTableModel::Table::Last); ++fSection)
    {
        ListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
    }
    setModel(ListModel);
}

void qbinarytableview::set_rows(int rows)
{
    model()->removeRows(0, model()->rowCount());
    model()->insertRows(0, rows, QModelIndex());
}

void qbinarytableview::set_content(const QByteArray &content)
{
    BinaryTableModel* themodel = dynamic_cast<BinaryTableModel*>(model());
    themodel->set_content(content);
    set_rows(10);
}


BinaryTableModel::BinaryTableModel(int rows, int columns, qbinarytableview *parent):
    QStandardItemModel(rows, columns, parent)
{

}

BinaryTableModel::~BinaryTableModel()
{
    TRACE(Logger::info, "BinaryTableModel::~BinaryTableModel()");
}

QVariant BinaryTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case INT(Table::Index):     return QVariant(tr("%1").arg(index.row())); break;
        case INT(Table::Character): return get_character_content(index.row()); break;
        case INT(Table::Typed):     return get_typed_content(index.row()); break;
        }
    }
    //  6: FontRole
    //  7: TextAlignmentRole
    //  9: ForegroundRole, TextColorRole
    // 10: CheckStateRole
    //  1: DecorationRole
    //  8: BackgroundRole
//    TRACE(Logger::info, "row %d, column %d, role %d", index.row(), index.column(), role);
    return QStandardItemModel::data(index, role);
}

void BinaryTableModel::set_content(const QByteArray &content)
{
    m_binary_content = content;
}

QVariant BinaryTableModel::get_typed_content(int row) const
{
    return QVariant(tr("Table::Typed"));
}

QVariant BinaryTableModel::get_character_content(int row) const
{
    return QVariant(tr("Table::Character"));
}
