#include "characteristicsdlg.h"
#include "ui_characteristicsdlg.h"

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
    /// TODO: edit values and modify curves
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


    QVector<QVector<QPointF>> data;

    int rows = lists.begin()->second.values.size();
    data.resize(lists.size());
    for (int current_row = 0; current_row < rows; ++current_row)
    {
        mListModel->insertRows(current_row, 1);
        int column = 0;
        mListModel->setData(mListModel->index(current_row, column++, QModelIndex()), lists.begin()->second.index[current_row]);
        for (const auto& list : lists)
        {
            data[column-1].push_back(QPointF(current_row, list.second.values[current_row]));
            mListModel->setData(mListModel->index(current_row, column++, QModelIndex()), list.second.values[current_row]);
        }
    }

    QStringList curve_names;
    for (int i=1; i<fSectionNames.size(); ++i)
    {
        curve_names.append(fSectionNames[i]);
    }
    ui->graphicsView->setCurves(data, curve_names);
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
    /// TODO: store also graphics vew settings
    /// TODO: store values, if changed
}

void CharacteristicsDlg::on_ckShowHoverValues_clicked(bool checked)
{
    ui->graphicsView->set_show_hover_values(checked);
}


void CharacteristicsDlg::on_ckLegend_clicked(bool checked)
{
    ui->graphicsView->set_show_legend(checked);
}


void CharacteristicsDlg::on_ckTicks_clicked(bool checked)
{
    ui->graphicsView->set_show_ticks(checked);
}


void CharacteristicsDlg::on_ckAxis_clicked(bool checked)
{
    ui->graphicsView->set_show_axis(checked);
}


void CharacteristicsDlg::on_ckGrid_clicked(bool checked)
{
    ui->graphicsView->set_show_grid(checked);
}

