#include "characteristicsdlg.h"
#include "ui_characteristicsdlg.h"

#include <QGraphicsItem>

struct s_entry
{
    std::vector<double> values;
    QStringList id;
    QStringList index;
    QString unit;
    QString type;
};

CharacteristicsDlg::CharacteristicsDlg(QString &characteristic, const QString& name, parameters &parameter, QWidget *parent)
    : QDialog(parent)
    , m_characteristic(characteristic)
    , ui(new Ui::CharacteristicsDlg)
{
    ui->setupUi(this);
    setWindowTitle(windowTitle() + name);
    QStringList values = m_characteristic.split(",");

    const QStringList& fmt = parameter.get_choices(name);
    const QString     type = parameter.get_type(name);
    const QString     def  = parameter.get_default(name);

    const QStringList& types = type.split(":");
    const int name_index  = types.indexOf("name");
    const int scale_index = types.indexOf("scale");
    const int id_index    = types.indexOf("id");
    const int unit_index  = types.indexOf("unit");
    const int index       = types.indexOf("index");
    const int type_index  = types.indexOf("type");

    const int size = std::min(values.size(), fmt.size());

    std::map<QString, s_entry> lists;
    for (int i=1; i< size; ++i)
    {
        QString item_name  = get_request(fmt[i], name_index);
        int pos = item_name.indexOf(def);
        if (pos != -1)
        {
            item_name = item_name.left(pos);
        }
        s_entry& entry = lists[item_name];
        entry.values.push_back(values[i].toInt() / get_request(fmt[i], scale_index).toDouble());
        entry.id.append(get_request(fmt[i], id_index));
        entry.index.append(get_request(fmt[i], index));
        entry.unit  = get_request(fmt[i], unit_index);
        entry.type  = get_request(fmt[i], type_index);
    }


    QStringList fSectionNames = { tr("Index") };
    for (const auto& list : lists)
    {
        fSectionNames.append(list.first + " [" + list.second.unit + "]");
    }
    mListModel = new QStandardItemModel(0, fSectionNames.size(), this);
    for (int fSection = 0; fSection < fSectionNames.size(); ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
    }

    ui->tableView->setModel(mListModel);
    for (int fSection = 0; fSection < fSectionNames.size(); ++fSection)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(fSection , fSection != 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
    }
    ui->tableView->horizontalHeader()->setStretchLastSection(false);


    int rows = lists.begin()->second.values.size();
    for (int current_row = 0; current_row < rows; ++current_row)
    {
        mListModel->insertRows(current_row, 1);
        int column = 0;
        mListModel->setData(mListModel->index(current_row, column++, QModelIndex()), lists.begin()->second.index[current_row]);
        for (const auto& list : lists)
        {
            mListModel->setData(mListModel->index(current_row, column++, QModelIndex()), list.second.values[current_row]);
        }
    }
    ui->graphicsView->setScene(new QGraphicsScene());
    // update_graphics();
    // fit_in_view();
}

CharacteristicsDlg::~CharacteristicsDlg()
{
    delete ui;
}

void CharacteristicsDlg::on_btnCancel_clicked()
{

}

void CharacteristicsDlg::on_btnApply_clicked()
{

}

void CharacteristicsDlg::update_graphics()
{
    ui->graphicsView->scene()->clear();

    std::vector<QPolygonF> polygons;
    std::vector<QPen>      pens;
    std::vector<QColor>    colors = { QColorConstants::Red, QColorConstants::Green, QColorConstants::Blue };
    int columns = mListModel->columnCount() - 1;
    polygons.resize(columns);
    pens.resize(columns);
    int rowcount = mListModel->rowCount();
    QPainterPath path;
    for (int column = 0; column < columns; ++column)
    {
        for (int row = 0; row < rowcount; ++row)
        {
            if (row == 0)
            {
                path.moveTo(row, mListModel->data(mListModel->index(row, column + 1, QModelIndex())).toDouble());
            }
            else
            {
                path.lineTo(row, mListModel->data(mListModel->index(row, column + 1, QModelIndex())).toDouble());
            }
            //polygons[column].append(QPointF(row, mListModel->data(mListModel->index(row, column + 1, QModelIndex())).toDouble()));
        }
        pens[column].setColor(colors[column]);

        QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
        pathItem->setPen(pens[column]);
        ui->graphicsView->scene()->addItem(pathItem);
    }

    // for (int column = 0; column < columns; ++column)
    // {
    //     auto item = new QGraphicsPolygonItem(polygons[column]);
    //     item->setPen(pens[column]);
    //     ui->graphicsView->scene()->addItem(item);
    //     //ui->graphicsView->scene()->addPolygon(polygons[column], );
    // }
}

void CharacteristicsDlg::fit_in_view()
{
    const auto& items = ui->graphicsView->scene()->items();
    QRectF bounding_rect;
    for (int i=0; i<items.size(); ++i)
    {
        bounding_rect = bounding_rect.united(items[i]->boundingRect());
    }
    //bounding_rect.marginsAdded(QMargins(20, 20, 20, 20));

    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//    ui->graphicsView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    ui->graphicsView->setSceneRect(bounding_rect);
    ui->graphicsView->scale(2, -2);
    //ui->graphicsView->fitInView(bounding_rect, Qt::KeepAspectRatio);



}

