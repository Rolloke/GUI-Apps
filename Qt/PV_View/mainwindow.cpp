#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yaml_structs.h"
#include "qprogressbarfloat.h"
#include "helper.h"
#include "characteristicsdlg.h"

#include <string>
#include <QFileDialog>
#include <QSettings>
#include <QUrl>
#include <QLoggingCategory>
#include <QSerialPort>
#include <QProgressBar>
#include <QProgressDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QTimer>
#include <QFile>
#include <QMessageBox>
#include <fstream>

#if SERIALBUS == 1

#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>

#else

#include <boost/bind/bind.hpp>
#include <boost/chrono.hpp>
using boost::asio::ip::tcp;

#endif

/// TODO: RW Values (scheduled values for storing scheduled parameters)
/// [x] read from file and write to file
/// [x] show in table on control tab
/// [x] checkbox to select values to write back to PV
/// [x] resize elements in control tab
/// [x] show and access control buttons defined in yaml
/// [x] show array parameters defined in "choice" parameter description
/// [ ] - show characteristic curve
/// [ ] - show info
/// [x] show checkboxes on same heigth of progress bars

namespace config
{
constexpr char sGroupMeter[]  = "Meter";
constexpr char sMeasurement[] = "Measurement";

constexpr char s_modbus[]                = "modbus";
constexpr char s_tcpip[]                 = "tcpip";
constexpr char s_timeout[]               = "timeout";
constexpr char s_store_value_section[]   = "StoreValuesSection";
constexpr char s_control_section[]       = "ControlSection";

constexpr char s_property_association[]  = "associated";
}

#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STRF(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), FUNC(ITEM))

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), QVariant(ITEM)).CONVERT()
#define LOAD_STRF(SETTING, ITEM, FUNC_OUT, FUNC_IN, CONVERT) ITEM = FUNC_OUT(fSettings.value(getSettingsName(#ITEM), QVariant(FUNC_IN(ITEM))).CONVERT());

QString getSettingsName(const QString& aItemName);
int     to_parity(const QString& parity);

#if SERIALBUS == 1
QString to_string(QModbusPdu::ExceptionCode code);
#endif

struct Register
{
    enum Table
    {
        eName, eAddress, eType, eDecode, eScale, eValue, eModel, eLast
    };
};

struct Schedule
{
    enum Table
    {
        eName, eValue, eLast
    };
};

enum ModbusConnection
{
    Serial,
    Tcp
};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  #if SERIALBUS == 0
  , m_socket(m_io_service)
  #endif
  , ui(new Ui::MainWindow)
    , mDocumentFile("/home/rolf/projects/Qt/PV_View/resource/huawei-sun2000-dongle-powersensor.yaml")
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/48x48/go-home.png"));

    QStringList fSectionNames = { tr("Name"), tr("Address"), tr("Type"), tr("Decode"), tr("Scale"), tr("Value"), tr("Unit/Type")};
    mListModel = new QStandardItemModel(0, Register::eLast, this);
    for (int fSection = 0; fSection < Register::eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
    }

    ui->tableView->setModel(mListModel);
    for (int fSection = 0; fSection < Register::eLast; ++fSection)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(fSection , fSection == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
    }
    ui->tableView->horizontalHeader()->setStretchLastSection(false);

    QStringList fSectionNamesSchedule = { tr("Name"), tr("Data") };
    mListModelSchedule = new CheckboxItemModel(0, Schedule::eLast, this);
    mListModelSchedule->setCheckedColumn(Schedule::eName);

    for (int fSection = 0; fSection < Schedule::eLast; ++fSection)
    {
        mListModelSchedule->setHeaderData(fSection, Qt::Horizontal, fSectionNamesSchedule[fSection]);
    }

    ui->tableViewSchedule->setModel(mListModelSchedule);
    for (int fSection = 0; fSection < Schedule::eLast; ++fSection)
    {
        ui->tableViewSchedule->horizontalHeader()->setSectionResizeMode(fSection , fSection == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
    }
    ui->tableViewSchedule->horizontalHeader()->setStretchLastSection(false);
    connect(mListModelSchedule, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(schedule_table_list_item_changed(QStandardItem*)));

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    LOAD_STR(fSettings, mDocumentFile, toString);
    LOAD_STR(fSettings, mConfigurationFileName, toString);
    LOAD_PTR(fSettings, ui->edtAddress, setText, text, toString);
    LOAD_PTR(fSettings, ui->edtServerAddress, setValue, value, toInt);
    LOAD_PTR(fSettings, ui->spinControlColumns, setValue, value, toInt);
    fSettings.beginGroup(config::sGroupMeter);
    {
        int fItemCount = fSettings.beginReadArray(config::sMeasurement);
        {
            for (int fItem = 0; fItem < fItemCount; ++fItem)
            {
                fSettings.setArrayIndex(fItem);
                QString fName;
                LOAD_STR(fSettings, fName, toString);
                QString fPrettyName;
                LOAD_STR(fSettings, fPrettyName, toString);
                QString fValues = "";
                LOAD_STR(fSettings, fValues, toString);

                double fScale = 0;
                LOAD_STR(fSettings, fScale, toDouble);
                double fMinimum = 0;
                LOAD_STR(fSettings, fMinimum, toDouble);
                double fMaximum = 0;
                LOAD_STR(fSettings, fMaximum, toDouble);
                QString fUnit;
                LOAD_STR(fSettings, fUnit, toString);

                add_meter_widgets(fName, fPrettyName, fValues.toInt(), fScale, fMinimum, fMaximum, 0, fUnit);
            }
        }
    }

    load_yaml(mDocumentFile);
    update_buttons();
#if 1
    startTimer(10);
#else
    connect( &m_thread, &QThread::started, this, &MainWindow::process);
    m_thread.start();
#endif
}

#if SERIALBUS == 0
void MainWindow::timerEvent(QTimerEvent *)
{
    m_io_service.run_one();
}
void MainWindow::process()
{
    m_io_service.run();
}
#endif

void MainWindow::set_no_gui()
{
    m_gui_mode = false;
}

bool MainWindow::display_gui()
{
    return m_gui_mode;
}


MainWindow::~MainWindow()
{
    disconnect_modbus_device();
    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    STORE_STR(fSettings, mDocumentFile);
    STORE_STR(fSettings, mConfigurationFileName);
    STORE_PTR(fSettings, ui->edtAddress, text);
    STORE_PTR(fSettings, ui->edtServerAddress, value);
    STORE_PTR(fSettings, ui->spinControlColumns, value);

    fSettings.beginGroup(config::sGroupMeter);
    {
        fSettings.beginWriteArray(config::sMeasurement);
        {
            for (int i = 0; i < ui->verticalNames->count(); ++i)
            {
                fSettings.setArrayIndex(i);
                auto namewidget = dynamic_cast<QLabel*>(ui->verticalNames->itemAt(i)->widget());
                if (namewidget)
                {
                    QString fName       = namewidget->toolTip();
                    STORE_STR(fSettings, fName);
                    QString fPrettyName = namewidget->text();
                    STORE_STR(fSettings, fPrettyName);
                    QString fValues     = namewidget->whatsThis();
                    STORE_STR(fSettings, fValues);
                }
                auto progressbar = dynamic_cast<QProgressBarFloat*>(ui->verticalMeter->itemAt(i)->widget());
                if (progressbar)
                {
                    double fScale   = progressbar->scale();
                    STORE_STR(fSettings, fScale);
                    double fMinimum = progressbar->minimum();
                    STORE_STR(fSettings, fMinimum);
                    double fMaximum = progressbar->maximum();
                    STORE_STR(fSettings, fMaximum);
                    QString fUnit   = progressbar->unit();
                    STORE_STR(fSettings, fUnit);
                }
            }
        }
        fSettings.endArray();
    }
    fSettings.endGroup();

//    m_thread.quit();
//    m_thread.wait();
    delete ui;
}

QString MainWindow::getConfigName() const
{
    QString sConfigFileName = windowTitle();
    sConfigFileName.replace(" ", "");
#ifdef __linux__
    return QDir::homePath() + "/.config/" + sConfigFileName + ".ini";
#else
    return "HKEY_CURRENT_USER\\Software\\KESoft\\" + sConfigFileName;
#endif
}

bool MainWindow::load_yaml(const QString &filename)
{
    try
    {
        YAML::Node yaml_document = YAML::LoadFile(filename.toStdString());
        m_meter = std::make_unique<meter>();
        yaml_document >> *m_meter;
        QString render_file = m_meter->m_render_source;
        bool render_file_exitst = QFileInfo(render_file).exists();
        if (!render_file_exitst)
        {
            QFileInfo info(filename);
            render_file = info.absolutePath() + "/" + render_file;
            render_file_exitst = QFileInfo(render_file).exists();
        }
        if (m_meter->m_render_source.contains("{{-"))
        {
            m_meter->m_render_source = m_meter->m_render_source.replace("{{-", "command: ");
            YAML::Node rendered = YAML::Load(m_meter->m_render_source.toStdString());
            rendered >> m_meter->m_rendered;
            m_request_name_index = 1;
        }
        else if (render_file_exitst)
        {
            YAML::Node rendered = YAML::LoadFile(render_file.toStdString());
            rendered >> m_meter->m_rendered;
            m_request_name_index    = 2;
            m_request_section_index = 1;
        }
        else
        {
            yaml_document >> m_meter->m_rendered;
        }

        ui->statusbar->showMessage(tr("read document \"%1\" successfully").arg(QFileInfo(filename).baseName()));

        init_table_and_controls();

        return true;
    }
    catch (const YAML::Exception& e)
    {
        ui->statusbar->showMessage(e.what());
        return false;
    }
}


void MainWindow::on_btnLoadYamlFile_clicked()
{
    QFileInfo info(mDocumentFile);
    QString fileName = QFileDialog::getOpenFileName(this, "Select template file for meter", info.absolutePath());
    if (fileName.size())
    {
        mDocumentFile = fileName;
    }

    load_yaml(mDocumentFile);
}

void MainWindow::create_modbus_device()
{
#if SERIALBUS == 1
    if (modbusDevice)
    {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    auto type = m_meter->m_parameters.get_choice(config::s_modbus) == config::s_tcpip ? Tcp : Serial;
    if (type == Serial)
    {
        modbusDevice = new QModbusRtuSerialMaster(this);
    }
    else if (type == Tcp)
    {
        modbusDevice = new QModbusTcpClient(this);
    }

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error)
    {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });

    if (!modbusDevice)
    {
        ui->btnConnect->setDisabled(true);
        statusBar()->showMessage(type == Serial ? tr("Could not create Modbus master.") : tr("Could not create Modbus client."), 5000);
    }
    else
    {
        connect(modbusDevice, &QModbusClient::stateChanged, this, &MainWindow::onStateChanged);
    }
#else
    QStringList host = ui->edtAddress->text().split(":");
    if (host.size() > 1)
    {
        using namespace  boost::asio::ip;

        tcp::endpoint end_point(address::from_string(host[0].toStdString()), std::atoi(host[1].toStdString().c_str()));
        boost::system::error_code ec;
        m_socket.connect(end_point, ec);
        if (ec)
        {
            ui->btnConnect->setText(tr("Connect"));
            ui->statusbar->showMessage(tr("Connection error %1").arg(ec.message().c_str()));
        }
        else
        {
            ui->btnConnect->setText(tr("Disconnect"));
            ui->statusbar->showMessage(tr("Connected to %1").arg(ui->edtAddress->text()));
        }
    }

#endif
}

void MainWindow::disconnect_modbus_device()
{
#if SERIALBUS == 1
    if (modbusDevice && modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        modbusDevice->disconnectDevice();
    }
#else
    ui->btnConnect->setText(tr("Connect"));
    m_socket.close();
#endif
}

void MainWindow::update_schedule_value_list()
{
    m_initialize = true;
    mListModelSchedule->removeRows(0, mListModelSchedule->rowCount());

    int current_row = 0;
    QMapIterator<QString, QString> entry(m_values);
    while (entry.hasNext())
    {
        entry.next();
        if (entry.key().contains(m_store_value_section))
        {
            mListModelSchedule->insertRows(current_row, 1);
            mListModelSchedule->setData(mListModelSchedule->index(current_row, Schedule::eName , QModelIndex()), entry.key());
            mListModelSchedule->setData(mListModelSchedule->index(current_row, Schedule::eName , QModelIndex()), Qt::Checked, Qt::CheckStateRole);
            mListModelSchedule->setData(mListModelSchedule->index(current_row, Schedule::eValue, QModelIndex()), entry.value());

            current_row++;
        }
        if (entry.key().contains(m_control_filter_section))
        {
            set_control_combo_index(entry.key(), entry.value());
        }
    }
    m_initialize = false;
}

void MainWindow::set_control_combo_index(const QString &request, const QString& string_value)
{
    QString name = get_request(request, m_request_name_index);
    QPushButton* button = dynamic_cast<QPushButton*>(find_widget(ui->gridLayoutControls, name));
    if (button)
    {
        QComboBox *combo = button->property(config::s_property_association).value<QComboBox*>();
        if (combo)
        {
            combo->setCurrentIndex(get_request(string_value, 0).toInt());
        }
    }
}

void MainWindow::add_meter_widgets(const QString& name, const QString& pretty_name, int values,
                                   double scale, double minimum, double maximum, double value,
                                   const QString& unit)
{
    QLabel* label_name = new QLabel(pretty_name, this);

    label_name->setToolTip(name);
    if (values > 1)
    {
        label_name->setWhatsThis(tr("%1").arg(values));
    }
    if (label_name->text().isEmpty())
    {
        label_name->setText(name);
    }


    QProgressBarFloat* meter = new QProgressBarFloat(this);
    /// TODO: set stylesheet for Progressbar

    meter->setScale(scale);
    meter->setRange(minimum, maximum);
    meter->setValue(value);
    meter->setUnit(unit);
    meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    meter->setSizeIncrement(1, 0);
    meter->setMinimumHeight(label_name->height());
    meter->setMaximumHeight(label_name->height());
//    "{ border: 1px solid black; text-align: top; padding: 1px; border-top-left-radius: 7px; border-bottom-left-radius: 7px; "
//    "background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fff, stop: 0.4999 #eee, stop: 0.5 #ddd, stop: 1 #eee ); width: 15px;}"
//        "QProgressBar::chunk { background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #0000ff, stop: 1 #ff0000 ); border-top-left-radius: 7px;border-bottom-left-radius: 7px; border: 1px solid black;}");

    ui->verticalMeter->addWidget(meter, 1);

    label_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->verticalNames->addWidget(label_name);

    auto checkbox = new QCheckBox(this);
    connect(checkbox, SIGNAL(clicked()), this, SLOT(btnCheckboxClicked()));
    checkbox->setMinimumHeight(label_name->height());
    checkbox->setMaximumHeight(label_name->height());
    ui->verticalCheck->addWidget(checkbox);
}

void MainWindow::read_meter_value()
{
    switch (m_read_permanent)
    {
    case read::meter:
        if (m_read_index < ui->verticalNames->count())
        {
            auto namewidget = dynamic_cast<QLabel*>(ui->verticalNames->itemAt(m_read_index)->widget());
            if (namewidget)
            {
                bool ok = false;
                int n = namewidget->whatsThis().toInt(&ok);
                if (!ok) n = 1;
                readValue(namewidget->toolTip(), n);
            }
        } break;
    case read::table:
        while (m_read_index < mListModel->rowCount())
        {
            auto current = mListModel->index(m_read_index, Register::eName);
            const auto& param = m_meter->m_rendered.m_measurements[mListModel->data(current).toString()];
            if (!param.m_address_valid)
            {
                ++m_read_index;
                continue;
            }
            ui->tableView->selectionModel()->select(current, QItemSelectionModel::SelectCurrent);
            ui->tableView->selectionModel()->setCurrentIndex(current, QItemSelectionModel::SelectCurrent);
            readValue(mListModel->data(current).toString());
            break;
        } break;
    case read::control:
        {
            QStringList keys = m_meter->m_rendered.m_measurements.keys();
            QString filter = ui->edtFilterForSchedule->text();
            for (; m_read_index < keys.size(); ++m_read_index)
            {
                const auto& param = m_meter->m_rendered.m_measurements[keys[m_read_index]];
                if (param.m_address_valid)
                {
                    if (filter.size())
                    {
                        if (keys[m_read_index].contains(filter))
                        {
                            break;
                        }
                    }
                }
            }
            if (m_read_index < m_meter->m_rendered.m_measurements.count())
            {
                readValue(keys[m_read_index]);
            }
            else
            {
                m_read_permanent = read::off;
                update_buttons();
            }
        } break;
    default: break;
    }

}

void MainWindow::on_btnAddMeter_clicked()
{
    add_meter_widgets(ui->textSelected->text(), ui->edtPrettyName->text(), ui->edtMultipleValues->text().toInt(),
        ui->textScale->text().toDouble(),
        ui->edtMinimum->text().toDouble(), ui->edtMaximum->text().toDouble(),
        ui->textValue->text().toDouble() ,ui->edtUnit->text());
}

#if SERIALBUS == 1
void MainWindow::onStateChanged(int state)
{
    ui->btnConnect->setText(state == QModbusDevice::UnconnectedState ? tr("Connect") : tr("Disconnect"));
    ui->btnStartRead->setEnabled(state == QModbusDevice::ConnectedState);
    ui->btnStopRead->setEnabled(state == QModbusDevice::ConnectedState);

    ui->btnStart->setEnabled(state == QModbusDevice::ConnectedState);
    ui->btnStop->setEnabled(state == QModbusDevice::ConnectedState);

    ui->btnReadConfig->setEnabled(state == QModbusDevice::ConnectedState);
    ui->btnSendValueToPv->setEnabled(state == QModbusDevice::ConnectedState);
}
#endif

void MainWindow::on_btnConnect_clicked()
{
    QLoggingCategory::setFilterRules("pv-module");

#if SERIALBUS == 1
    if (!modbusDevice)
    {
        create_modbus_device();
    }

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        auto type = m_meter->m_parameters.get_choice(config::s_modbus) == config::s_tcpip ? Tcp : Serial;
        if (type == Serial)
        {
            QString port = ui->edtAddress->text();
            QStringList part_parameter = port.split(",");
            if (part_parameter.size() >= 5)
            {
                modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, part_parameter[0].toInt());
                modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, part_parameter[1].toInt());
                modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, part_parameter[2].toInt());
                modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, to_parity(part_parameter[3]));
                modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, part_parameter[4].toInt());
            }
        }
        else
        {
            const QUrl url = QUrl::fromUserInput(ui->edtAddress->text());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        }
        modbusDevice->setTimeout(m_meter->m_parameters.get_default(config::s_timeout).toInt());
        modbusDevice->setNumberOfRetries(3);
        if (!modbusDevice->connectDevice())
        {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
        else
        {
        }
    }
    else
    {
        modbusDevice->disconnectDevice();
    }
#else
    if (m_socket.is_open())
    {
        disconnect_modbus_device();
    }
    else
    {
        create_modbus_device();
    }

#endif
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    readValue(mListModel->data(mListModel->index(index.row(), static_cast<int>(Register::eName))).toString(), ui->edtMultipleValues->text().toInt());
}

void MainWindow::readValue(const QString& name, int values)
{
#if SERIALBUS == 1
    if (modbusDevice)
    {
        auto measurement = m_meter->m_rendered.m_measurements[name];
        statusBar()->clearMessage();
        auto readRequest = QModbusDataUnit(get_type(measurement.m_register.m_type),
                                           get_address(measurement.m_register.m_address, measurement.m_register.m_address.contains(";") ? 0 : -1),
                                           get_entries(measurement.m_register.m_decode)*values);

        if (auto *reply = modbusDevice->sendReadRequest(readRequest, ui->edtServerAddress->value()))
        {
            if (!reply->isFinished())
            {
                statusBar()->showMessage(tr("Reading %2 of: %1").arg(name).arg(values));
                m_pending_request = name;
                connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
            }
            else
            {
                delete reply; // broadcast replies return immediately
            }
        }
        else
        {
            statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
        }
    }
#else
    if (m_socket.is_open())
    {
        auto measurement = m_meter->m_rendered.m_measurements[name];
        statusBar()->clearMessage();
        uint8_t slave = 1;
        uint16_t register_address = get_address(measurement.m_register.m_address, measurement.m_register.m_address.contains(";") ? 0 : -1);
        mothbus::tcp_master<boost::asio::ip::tcp::socket> client(m_socket);
        int bytes = get_entries(measurement.m_register.m_decode)*2*values;
        std::vector<mothbus::byte> singleRegister(bytes);
        boost::system::error_code ec = client.read_registers(slave, register_address, singleRegister);
        if (ec.failed())
        {
            ui->statusbar->showMessage(tr("read form modbus failed: %1").arg(ec.message().c_str()));
        }
        else
        {
            m_pending_request = name;
            QVector<quint16> values;
            values.resize(singleRegister.size()/2);
            for (int i=0, j=0; i<values.size(); ++i, j+=2)
            {
                values[i] = (gsl::to_integer<uint16_t>(singleRegister[j]) << 8) + gsl::to_integer<uint16_t>(singleRegister[j+1]);
            }
            readReady(values);
        }

        singleRegister.clear();
    }
#endif
}

#if SERIALBUS == 1
void MainWindow::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (reply && !m_pending_request.isEmpty())
    {
        if (reply->error() == QModbusDevice::NoError)
        {
            const QModbusDataUnit unit = reply->result();
            QVector<quint16> values;
            values.resize(unit.valueCount());
            for (uint i = 0; i < unit.valueCount(); i++)
            {
                values[i] = unit.value(i);
            }
#else
void MainWindow::readReady(QVector<quint16>& values)
{
    if (1)
    {
        if (1)
        {
#endif
            measured_value measurement = m_meter->m_rendered.m_measurements[m_pending_request];
            QString string_value;
            if (measurement.m_register.m_decode.contains("char"))
            {
                char *value = reinterpret_cast<char*>(&values[0]);
                mListModel->setData(mListModel->index(ui->tableView->selectionModel()->currentIndex().row(), static_cast<int>(Register::eValue)), value);
                string_value = value;
                ++m_read_index;
            }
            else if (measurement.m_register.m_decode.contains("array"))
            {
                std::stringstream stream;
                for (auto& v : values)
                {
                    stream << v << ",";
                }
                string_value = QString::fromStdString(stream.str());
                ++m_read_index;
            }
            else
            {
                const int value_size = get_value_size(measurement);
                int       no_of_values = values.size() / value_size;
                int address = get_address(measurement.m_register.m_address);
                for (int i_v=0, v=0; i_v < no_of_values; v += value_size, ++i_v)
                {
                    if (value_size > 1)
                    {
                        std::swap(values[v], values[v+1]);
                    }
                    double value = get_value(measurement, &values[v]);
                    string_value = tr("%1").arg(value);
                    if (measurement.m_scale == 1)
                    {
                        const QString request = get_request(m_pending_request, m_request_name_index);
                        const QString type    = m_meter->m_parameters.get_type(request);
                        /// TODO: get parameter values "id" "bit"
                        if (type.contains("id") || type.contains("bit"))
                        {
                            statusBar()->showMessage(tr("%1 is %2").arg(m_pending_request).arg(m_meter->m_parameters.get_value(request, string_value)));
                        }
                        else
                        {
                            const QStringList& choices = m_meter->m_parameters.get_choices(request);
                            int no_of_choices = choices.size();
                            if (no_of_choices)
                            {
                                bool hex = -1 != choices[0].indexOf("0x");
                                int ivalue = static_cast<int>(value);
                                auto found =std::find_if(choices.begin(), choices.end(), [hex, ivalue](const QString&str)
                                {
                                    bool ok = false;
                                    bool equal = ivalue == str.split(":")[0].toInt(&ok, hex? 16 : 10);
                                    return ok && equal;
                                });
                                if (found != choices.end())
                                {
                                    string_value = *found;
                                }
                                else if (ivalue < 0)
                                {
                                    string_value = choices[0];
                                }
                                else if (ivalue < static_cast<int>(choices.size()))
                                {
                                    string_value = choices[ivalue];
                                }
                                statusBar()->showMessage(tr("%1 is %2").arg(m_pending_request).arg(string_value));
                            }
                        }
                    }
                    switch (m_read_permanent)
                    {
                    case read::table: default:
                    {
                        for (int row = 0; row < mListModel->rowCount(); ++row)
                        {
                            auto i_name   = mListModel->index(row, static_cast<int>(Register::eName));
                            if (mListModel->data(i_name).toString() == m_pending_request)
                            {
                                auto i_value  = mListModel->index(row, static_cast<int>(Register::eValue));
                                mListModel->setData(i_value, value);
                                if (i_v == 0)
                                {
                                    ui->textSelected->setText(m_pending_request);
                                    ui->edtPrettyName->setText(get_request(m_pending_request, m_request_name_index));
                                    ui->textValue->setText(tr("%1").arg(value));
                                    ui->textScale->setText(tr("%1").arg(measurement.m_scale));
                                    ui->edtUnit->setText(tr("%1").arg(measurement.m_unit));
                                }
                                break;
                            }
                        }
                    } break;
                    case read::meter:
                    {
                        for (int i = 0; i < ui->verticalNames->count(); ++i)
                        {
                            auto name = dynamic_cast<QLabel*>(ui->verticalNames->itemAt(i)->widget());
                            if (name && name->toolTip() == m_pending_request)
                            {
                                auto meter = dynamic_cast<QProgressBarFloat*>(ui->verticalMeter->itemAt(i)->widget());
                                if (meter)
                                {
                                    meter->setValue(value);
                                }
                                break;
                            }
                        }
                    } break;
                    case read::control:
                    {
                        /// NOTE: do nothing here
                    } break;
                    }
                    ++m_read_index;
                    if (i_v < no_of_values)
                    {
                        address += value_size;
                        auto found = std::find_if(m_meter->m_rendered.m_measurements.begin(), m_meter->m_rendered.m_measurements.end(), [address](const auto& item)
                        {
                            return get_address(item.m_register.m_address) == address;
                        });
                        if (found != m_meter->m_rendered.m_measurements.end())
                        {
                            m_pending_request = found.key();
                            measurement = found.value();
                        }
                    }
                }
            }
            QString request_section = get_request(m_pending_request, m_request_section_index);
            if (request_section == m_store_value_section)
            {
                m_values[m_pending_request] = string_value;
            }
            else if (request_section == m_control_filter_section)
            {
                m_values[m_pending_request] = string_value;
                set_control_combo_index(m_pending_request, string_value);
            }

            switch (m_read_permanent)
            {
            case read::meter:
                if (m_read_index >= ui->verticalNames->count())
                {
                    m_read_index = 0;
                } break;
            case read::table:
                if (m_read_index >= mListModel->rowCount())
                {
                    m_read_permanent = read::off;
                    update_buttons();
                } break;
            case read::control:
            {
                if (m_read_index >= m_meter->m_rendered.m_measurements.count())
                {
                    m_read_permanent = read::off;
                    update_buttons();
                }
            }  break;
            default: break;
            }

            QTimer::singleShot(200, [&] () { read_meter_value(); });
        }
#if SERIALBUS == 1
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
            measured_value& measurement = m_meter->m_rendered.m_measurements[m_pending_request];

            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2: %3)").
                                     arg(reply->errorString()).
                                     arg(reply->rawResult().exceptionCode(), -1, 16).arg(to_string(reply->rawResult().exceptionCode())), 5000);

            switch (reply->rawResult().exceptionCode())
            {
            case QModbusPdu::IllegalFunction:
            case QModbusPdu::IllegalDataAddress:
            //case QModbusPdu::IllegalDataValue:
                measurement.m_address_valid = false;
                break;
            default: break;
            }

            QTimer::singleShot(1000, [&] () { read_meter_value(); });
        }
        else
        {
            statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                     arg(reply->errorString()).
                                     arg(reply->error(), -1, 16), 5000);

            QTimer::singleShot(1000, [&] () { read_meter_value(); });
        }
        reply->deleteLater();
#else

#endif
    }
}

void MainWindow::on_btnStart_clicked()
{
    if (ui->verticalNames->count() > 0)
    {
        m_read_permanent = read::meter;
        m_read_index     = 1;
        update_buttons();
        read_meter_value();
    }
}

void MainWindow::on_btnStop_clicked()
{
    m_read_permanent = read::off;
    update_buttons();
    statusBar()->showMessage(tr("Stopped reading"));
}

void MainWindow::on_btnRemove_clicked()
{
    for (int i = 0; i < ui->verticalCheck->count(); ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->verticalCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            ui->verticalCheck->removeWidget(check);
            delete check;
            auto meter = ui->verticalMeter->itemAt(i)->widget();
            ui->verticalMeter->removeWidget(meter);
            delete meter;
            auto name = ui->verticalNames->itemAt(i)->widget();
            ui->verticalNames->removeWidget(name);
            delete name;
            break;
        }
    }
}

void MainWindow::on_btnUp_clicked()
{
    for (int i = 1; i < ui->verticalCheck->count(); ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->verticalCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            QWidget* check = ui->verticalCheck->itemAt(i)->widget();
            ui->verticalCheck->removeWidget(check);
            ui->verticalCheck->insertWidget(i-1, check);

            QWidget* name = ui->verticalNames->itemAt(i)->widget();
            ui->verticalNames->removeWidget(name);
            ui->verticalNames->insertWidget(i-1, name);

            QWidget* meter = ui->verticalMeter->itemAt(i)->widget();
            ui->verticalMeter->removeWidget(meter);
            ui->verticalMeter->insertWidget(i-1, meter);
            break;
        }
    }
}

void MainWindow::on_btnDown_clicked()
{
    for (int i = 0; i < ui->verticalCheck->count()-1; ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->verticalCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            QWidget* check = ui->verticalCheck->itemAt(i)->widget();
            ui->verticalCheck->removeWidget(check);
            ui->verticalCheck->insertWidget(i+1, check);

            QWidget* name = ui->verticalNames->itemAt(i)->widget();
            ui->verticalNames->removeWidget(name);
            ui->verticalNames->insertWidget(i+1, name);

            QWidget* meter = ui->verticalMeter->itemAt(i)->widget();
            ui->verticalMeter->removeWidget(meter);
            ui->verticalMeter->insertWidget(i+1, meter);
            break;
        }
    }
}

void MainWindow::on_btnReadConfig_clicked()
{
    if (m_read_permanent != read::control)
    {
        m_read_permanent = read::control;
        m_read_index     = 0;
        update_buttons();
        read_meter_value();
    }
}

void MainWindow::on_btnStartRead_clicked()
{
    if (m_read_permanent == read::off)
    {
        m_read_permanent = read::table;
        m_read_index     = 0;
        read_meter_value();
        update_buttons();
    }
}

void MainWindow::on_btnStopRead_clicked()
{
    m_read_permanent = read::off;
    update_buttons();
}

void MainWindow::update_buttons()
{
    ui->btnStop->setEnabled(m_read_permanent != read::off);
    ui->btnStart->setEnabled(m_read_permanent != read::meter);
    ui->btnStopRead->setEnabled(m_read_permanent != read::off);
    ui->btnStartRead->setEnabled(m_read_permanent != read::table);
    ui->btnReadConfig->setEnabled(m_read_permanent != read::control);
}

void MainWindow::init_table_and_controls()
{
    m_store_value_section = m_meter->m_parameters.get_default(config::s_store_value_section);
    ui->edtFilterForSchedule->setText(m_store_value_section);

    m_control_filter_section =   m_meter->m_parameters.get_default(config::s_control_section);
    ui->edtFilterForControls->setText(m_control_filter_section);

    m_initialize = true;
    clearLayout(ui->gridLayoutControls);
    mListModel->removeRows(0, mListModel->rowCount());
    int list_row = 0;
    int control_columns = ui->spinControlColumns->value() * 1;
    int control_row     = 0;
    int control_column  = 0;
    for (auto measurement= m_meter->m_rendered.m_measurements.constBegin();
         measurement != m_meter->m_rendered.m_measurements.constEnd(); ++measurement)
    {
        mListModel->insertRows(list_row, 1);
        mListModel->setData(mListModel->index(list_row, Register::eName   , QModelIndex()), measurement.key());
        mListModel->setData(mListModel->index(list_row, Register::eAddress, QModelIndex()), measurement.value().m_register.m_address);
        mListModel->setData(mListModel->index(list_row, Register::eType   , QModelIndex()), measurement.value().m_register.m_type);
        mListModel->setData(mListModel->index(list_row, Register::eDecode , QModelIndex()), measurement.value().m_register.m_decode);
        mListModel->setData(mListModel->index(list_row, Register::eScale  , QModelIndex()), measurement.value().m_scale);
        mListModel->setData(mListModel->index(list_row, Register::eValue  , QModelIndex()), "n/a");
        QString str = measurement.value().m_model;
        if (str.isEmpty())
        {
            str = measurement.value().m_unit;
        }
        if (str.isEmpty())
        {
            str = measurement.value().m_device_class;
        }
        mListModel->setData(mListModel->index(list_row, Register::eModel, QModelIndex()), str);

        list_row++;

        if (get_request(measurement.key(), m_request_section_index) == m_control_filter_section)
        {
            QString name = get_request(measurement.key(), m_request_name_index);
            const auto& choices = m_meter->m_parameters.get_choices(name);
            if (choices.size())
            {
                QPushButton *button = new QPushButton(tr("\"") + name + tr("\" send value: "), this);
                connect(button, SIGNAL(pressed()), this, SLOT(btn_clicked()));
                QComboBox   *combo  = new QComboBox(this);
                combo->addItems(choices);
                button->setProperty(config::s_property_association, QVariant::fromValue(combo));
                ui->gridLayoutControls->addWidget(button, control_row, control_column++);
                ui->gridLayoutControls->addWidget(combo, control_row, control_column++);
                if (control_column > control_columns)
                {
                    control_column = 0;
                    control_row++;
                }
            }
        }
    }
    m_initialize = false;
}

void MainWindow::btnCheckboxClicked()
{
    bool one_checked = false;
    for (int i = 1; i < ui->labelCheck->count()-1; ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->labelCheck->itemAt(i)->widget());
        if (check->isChecked())
        {
            one_checked = true;
            break;
        }
    }
    ui->btnRemove->setChecked(one_checked);
    ui->btnUp->setChecked(one_checked);
    ui->btnDown->setChecked(one_checked);
}


#if SERIALBUS == 1
#define CASE_RETURN(X) case X: return #X;
QString to_string(QModbusPdu::ExceptionCode code)
{
    switch (code)
    {
    CASE_RETURN(QModbusPdu::IllegalFunction)
    CASE_RETURN(QModbusPdu::IllegalDataAddress)
    CASE_RETURN(QModbusPdu::IllegalDataValue)
    CASE_RETURN(QModbusPdu::ServerDeviceFailure)
    CASE_RETURN(QModbusPdu::Acknowledge)
    CASE_RETURN(QModbusPdu::ServerDeviceBusy)
    CASE_RETURN(QModbusPdu::NegativeAcknowledge)
    CASE_RETURN(QModbusPdu::MemoryParityError)
    CASE_RETURN(QModbusPdu::GatewayPathUnavailable)
    CASE_RETURN(QModbusPdu::GatewayTargetDeviceFailedToRespond)
    CASE_RETURN(QModbusPdu::ExtendedException)
    }
    return "Invalid code";
}
#else

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

int to_parity(const QString& parity)
{
    if (parity == "n") return  QSerialPort::NoParity;
    else if (parity == "n") return  QSerialPort::NoParity;
    else if (parity == "e") return  QSerialPort::EvenParity;
    else if (parity == "o") return  QSerialPort::OddParity;
    else if (parity == "s") return  QSerialPort::SpaceParity;
    else if (parity == "m") return  QSerialPort::MarkParity;
    return QSerialPort::UnknownParity;
}


void MainWindow::on_btnStoreValues_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Schedule Values"), QFileInfo(mConfigurationFileName).absolutePath(), tr("*.yaml"));
    if (file.size())
    {
        mConfigurationFileName = file;
        if (m_values.size())
        {
            std::ofstream fout(mConfigurationFileName.toStdString());
            YAML::Node stored_values;
            stored_values << m_values;
            YAML::Node value_list;
            value_list["value_list"] = stored_values;
            fout << value_list;
            fout.close();
        }
    }
}


void MainWindow::on_btnLoadValues_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Read Schedule Values"), QFileInfo(mConfigurationFileName).absolutePath(), tr("*.yaml"));
    if (file.size())
    {
        mConfigurationFileName = file;
        YAML::Node value_list = YAML::LoadFile(mConfigurationFileName.toStdString());
        value_list["value_list"] >> m_values;
        update_schedule_value_list();
    }
}


void MainWindow::on_btnSendValueToPv_clicked()
{
    QMessageBox box(QMessageBox::Warning, tr("Send selected values to PV"), tr("Please be shure that the values are correct, otherwise they may cause damage"));
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if (box.exec() == QMessageBox::Yes)
    {
        /// TODO update value to PV
    }
}

void MainWindow::btn_clicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QComboBox *combo = button->property(config::s_property_association).value<QComboBox*>();
    QMessageBox box(QMessageBox::Warning, tr("Send selected values to PV"), tr("Please be shure that the values are correct, otherwise they may cause damage"));
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if (box.exec() == QMessageBox::Yes)
    {
        QString selection = combo->currentText();
        QString value = get_request(selection, 0);
        (void)(value);
        /// TODO update value to PV
    }
}

void MainWindow::on_btnUpdataList_clicked()
{
    update_schedule_value_list();
}

void MainWindow::on_tableViewSchedule_clicked(const QModelIndex &index)
{
    if (index.column() == Schedule::eName)
    {
        mListModelSchedule->setData(index, !mListModelSchedule->data(index, Qt::CheckStateRole).toBool(), Qt::CheckStateRole);
    }
    m_current_schedule_row = index.row();
    ui->btnEditCharacteristic->setEnabled(mListModelSchedule->data(mListModelSchedule->index(m_current_schedule_row, Schedule::eValue)).toString().contains(","));
}

void MainWindow::schedule_table_list_item_changed(QStandardItem *item)
{
    if (!m_initialize)
    {
        int column = item->column();
        if (column == Schedule::eValue)
        {
            QString name = mListModelSchedule->data(mListModelSchedule->index(item->row(), Schedule::eName)).toString();
            if (name.size())
            {
                m_values[name] = item->text();
            }
        }
    }
}

void MainWindow::on_btnEditCharacteristic_clicked()
{
    QString full_name = mListModelSchedule->data(mListModelSchedule->index(m_current_schedule_row, Schedule::eName)).toString();
    QString value     = mListModelSchedule->data(mListModelSchedule->index(m_current_schedule_row, Schedule::eValue)).toString();
    QString name      = get_request(full_name, m_request_name_index);
    CharacteristicsDlg dlg(value, name, m_meter->m_parameters);
    dlg.exec();
}

/*
void MainWindow::on_btnTest_clicked()
{
    //    using boost::asio::ip::tcp;
    //    std::string host = "192.168.2.113"; //ui->edtAddress->text().toStdString();
    //    boost::asio::io_service io_service;
    //    tcp::resolver resolver(io_service);
    //    tcp::socket socket(io_service);
    //    boost::asio::connect(socket, resolver.resolve(tcp::resolver::query{host, "502"}));

    //    // mothbus reads from server

    //    uint8_t slave = 1;
    //    uint16_t register_address = 37022;
    //    mothbus::tcp_master<tcp::socket> client(socket);
    //    std::array<mothbus::byte, 2> singleRegister;
    //    client.read_registers(slave, register_address, singleRegister);

    //    // output value
    //    uint16_t value = (gsl::to_integer<uint16_t>(singleRegister[0]) << 8) + gsl::to_integer<uint16_t>(singleRegister[1]);
    //    sleep(1);

    //    ui->statusbar->showMessage(tr("read valus: %1").arg(value));
}
*/
/*
void MainWindow::on_pushButtonAbout_clicked()
{
    QString message = tr("<h3>About Rescent Files list viewer and editor for linux</h3><br><br>"
                         "View, open recent files or remove files from recent files list.<br>"
                         "The program is provided AS IS with NO WARRANTY OF ANY KIND<br>"
                         "<table cellSpacing=\"0\" cellPadding=\"4\" >"
                         "<tr><td>Based on Qt</td><td>%1</td></tr>"
                         "<tr><td>Built on</td><td>%2, %3</td></tr>"
                         "<tr><td>Author</td><td>Rolf Kary Ehlers</td></tr>"
                         "<tr><td>Version</td><td>%4</td></tr>"
                         "<tr><td>License</td><td>GNU GPL Version 2</td></tr>"
                         "<tr><td>Email</td><td>rolf-kary-ehlers@t-online.de</td></tr>"
                         "</table>"
                         ).arg(qVersion(), __DATE__, __TIME__, txt::version);
    QMessageBox::about(this, windowTitle(), message);

}
*/