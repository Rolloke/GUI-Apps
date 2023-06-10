#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yaml_structs.h"
#include "qprogressbarfloat.h"

#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <QFileDialog>
#include <QSettings>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QUrl>
#include <QLoggingCategory>
#include <QSerialPort>
#include <QStandardItemModel>
#include <QProgressBar>
#include <QProgressDialog>
#include <QCheckBox>
#include <QTimer>

/// TODO: RW Values
/// store in file
namespace config
{
constexpr char sGroupMeter[]  = "Meter";
constexpr char sMeasurement[] = "Measurement";
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
QString to_string(QModbusPdu::ExceptionCode code);
int     to_parity(const QString& parity);

enum Table
{
    eName, eAddress, eType, eDecode, eScale, eValue, eModel, eLast
};

enum ModbusConnection
{
    Serial,
    Tcp
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mDocumentFile("/home/rolf/.config/huawei-sun2000-dongle-powersensor.yaml")
    , m_gui_mode(true)
{
    ui->setupUi(this);

    QStringList fSectionNames = { tr("Name"), tr("Address"), tr("Type"), tr("Decode"), tr("Scale"), tr("Value"), tr("Model")};
    mListModel = new QStandardItemModel(0, eLast, this);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        mListModel->setHeaderData(fSection, Qt::Horizontal, fSectionNames[fSection]);
        if (!m_hidden_columns.contains(fSection))
        {
        }
    }

    ui->tableView->setModel(mListModel);
    for (int fSection = 0; fSection < eLast; ++fSection)
    {
        if (!m_hidden_columns.contains(fSection))
        {
            ui->tableView->horizontalHeader()->setSectionResizeMode(fSection , fSection == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
        }
    }
    ui->tableView->horizontalHeader()->setStretchLastSection(false);

    QSettings fSettings(getConfigName(), QSettings::NativeFormat);

    LOAD_STR(fSettings, mDocumentFile, toString);
    LOAD_PTR(fSettings, ui->edtAddress, setText, text, toString);
    LOAD_PTR(fSettings, ui->edtServerAddress, setValue, value, toInt);
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
}

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
    STORE_PTR(fSettings, ui->edtAddress, text);
    STORE_PTR(fSettings, ui->edtServerAddress, value);

    fSettings.beginGroup(config::sGroupMeter);
    {
        fSettings.beginWriteArray(config::sMeasurement);
        {
            for (int i = 1; i < ui->labelNames->count(); ++i)
            {
                fSettings.setArrayIndex(i-1);
                auto namewidget = dynamic_cast<QLabel*>(ui->labelNames->itemAt(i)->widget());
                if (namewidget)
                {
                    QString fName       = namewidget->toolTip();
                    STORE_STR(fSettings, fName);
                    QString fPrettyName = namewidget->text();
                    STORE_STR(fSettings, fPrettyName);
                    QString fValues     = namewidget->whatsThis();
                    STORE_STR(fSettings, fValues);
                }
                auto progressbar = dynamic_cast<QProgressBarFloat*>(ui->labelMeter->itemAt(i)->widget());
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
        YAML::Node           yaml_document;
        yaml_document = YAML::LoadFile(filename.toStdString());

        m_meter = std::make_unique<meter>();
        yaml_document >> *m_meter;
        m_meter->m_render_source = m_meter->m_render_source.replace("{{-", "command: ");

        YAML::Node rendered = YAML::Load(m_meter->m_render_source.toStdString());
        rendered >> m_meter->m_rendered;

        ui->statusbar->showMessage(tr("read document \"%1\" successfully").arg(QFileInfo(filename).baseName()));

        mListModel->removeRows(0, mListModel->rowCount());
        int current_row = 0;

        for (auto measurement= m_meter->m_rendered.m_measurements.constBegin();
             measurement != m_meter->m_rendered.m_measurements.constEnd(); ++measurement)
        {
            mListModel->insertRows(current_row, 1);
            mListModel->setData(mListModel->index(current_row, eName, QModelIndex()), measurement.key());
            mListModel->setData(mListModel->index(current_row, eAddress, QModelIndex()), measurement.value().m_register.m_address);
            mListModel->setData(mListModel->index(current_row, eType, QModelIndex()), measurement.value().m_register.m_type);
            mListModel->setData(mListModel->index(current_row, eDecode, QModelIndex()), measurement.value().m_register.m_decode);
            mListModel->setData(mListModel->index(current_row, eScale, QModelIndex()), measurement.value().m_scale);
            mListModel->setData(mListModel->index(current_row, eValue, QModelIndex()), "n/a");
            mListModel->setData(mListModel->index(current_row, eModel, QModelIndex()), measurement.value().m_model);

            current_row++;
        }

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
    if (modbusDevice)
    {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    auto type = m_meter->m_parameters.get_choice("modbus") == "tcpip" ? Tcp : Serial;
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
}

void MainWindow::disconnect_modbus_device()
{
    if (modbusDevice && modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        modbusDevice->disconnectDevice();
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

    ui->labelMeter->addWidget(meter);

    label_name->setMinimumHeight(meter->height()-5);
    ui->labelNames->addWidget(label_name);

    auto checkbox = new QCheckBox(this);
    checkbox->setMinimumHeight(meter->height()-5);
    ui->labelCheck->addWidget(checkbox);
}

void MainWindow::read_meter_value()
{
    if (m_read_permanent)
    {
        if (ui->tabWidget->currentWidget() == ui->tabMeter && m_read_index < ui->labelNames->count())
        {
            auto namewidget = dynamic_cast<QLabel*>(ui->labelNames->itemAt(m_read_index)->widget());
            if (namewidget)
            {
                bool ok = false;
                int n = namewidget->whatsThis().toInt(&ok);
                if (!ok) n = 1;
                readValue(namewidget->toolTip(), n);
            }
        }
        if (ui->tabWidget->currentWidget() == ui->tabTable && m_read_index < mListModel->rowCount())
        {
            auto current = mListModel->index(m_read_index, eName);
            ui->tableView->selectionModel()->select(current, QItemSelectionModel::SelectCurrent);
            ui->tableView->selectionModel()->setCurrentIndex(current, QItemSelectionModel::SelectCurrent);
            readValue(mListModel->data(current).toString());
        }
    }
}

void MainWindow::on_btnAddMeter_clicked()
{
    add_meter_widgets(ui->textSelected->text(), ui->edtPrettyName->text(), ui->edtMultipleValues->text().toInt(),
        ui->textScale->text().toDouble(),
        ui->edtMinimum->text().toDouble(), ui->edtMaximum->text().toDouble(),
        ui->textValue->text().toDouble() ,ui->edtUnit->text());
}


void MainWindow::onStateChanged(int state)
{
    ui->btnConnect->setText(state == QModbusDevice::UnconnectedState ? tr("Connect") : tr("Disconnect"));
}


void MainWindow::on_btnConnect_clicked()
{
    if (!modbusDevice)
    {
        create_modbus_device();
    }

    QLoggingCategory::setFilterRules("battery");

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        auto type = m_meter->m_parameters.get_choice("modbus") == "tcpip" ? Tcp : Serial;
        if (type == Serial)
        {
            QString port = ui->edtAddress->text();
            QStringList part_parameter = port.split(",");
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, part_parameter[0].toInt());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, part_parameter[1].toInt());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, part_parameter[2].toInt());
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, to_parity(part_parameter[3]));
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, part_parameter[4].toInt());
        }
        else
        {
            const QUrl url = QUrl::fromUserInput(ui->edtAddress->text());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        }
        modbusDevice->setTimeout(m_meter->m_parameters.get_default("timeout").toInt());
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
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    readValue(mListModel->data(mListModel->index(index.row(), static_cast<int>(eName))).toString(), ui->edtMultipleValues->text().toInt());
}

void MainWindow::readValue(const QString& name, int values)
{
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
}

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
            auto measurement = m_meter->m_rendered.m_measurements[m_pending_request];
            for (uint i = 0; i < unit.valueCount(); i++)
            {
                values[i] = unit.value(i);
            }
            if (measurement.m_register.m_decode.contains("char"))
            {
                char *value = reinterpret_cast<char*>(&values[0]);
                mListModel->setData(mListModel->index(ui->tableView->selectionModel()->currentIndex().row(), static_cast<int>(eValue)), value);
                ++m_read_index;
            }
            else if (measurement.m_register.m_decode.contains("array"))
            {
                /// TODO: read array
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
                    if (measurement.m_scale == 1)
                    {
                        /// TODO: evaluate also bits
                        QString choice = m_meter->m_parameters.get_choice(get_request(m_pending_request, 1), static_cast<int>(value));
                        if (!choice.isEmpty())
                        {
                            statusBar()->showMessage(tr("%1 is %2").arg(m_pending_request).arg(choice));
                        }
                    }
                    if (ui->tabWidget->currentWidget() == ui->tabTable)
                    {
                        for (int row = 0; row < mListModel->rowCount(); ++row)
                        {
                            auto i_name   = mListModel->index(row, static_cast<int>(eName));
                            if (mListModel->data(i_name).toString() == m_pending_request)
                            {
                                auto i_value  = mListModel->index(row, static_cast<int>(eValue));
                                mListModel->setData(i_value, value);
                                if (i_v == 0)
                                {
                                    ui->textSelected->setText(m_pending_request);
                                    ui->textValue->setText(tr("%1").arg(value));
                                    ui->textScale->setText(tr("%1").arg(measurement.m_scale));
                                    ui->edtUnit->setText(tr("%1").arg(measurement.m_unit));
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (int i = 1; i < ui->labelNames->count(); ++i)
                        {
                            auto name = dynamic_cast<QLabel*>(ui->labelNames->itemAt(i)->widget());
                            if (name && name->toolTip() == m_pending_request)
                            {
                                auto meter = dynamic_cast<QProgressBarFloat*>(ui->labelMeter->itemAt(i)->widget());
                                if (meter)
                                {
                                    meter->setValue(value);
                                }
                                break;
                            }
                        }
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
            if (ui->tabWidget->currentWidget() == ui->tabMeter)
            {
                if (m_read_index >= ui->labelNames->count())
                {
                    m_read_index = 1;
                }
            }
            else
            {
                if (m_read_index >= mListModel->rowCount())
                {
                    m_read_index = 0;
                }
            }

            QTimer::singleShot(200, [&] () { read_meter_value(); });
        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2: %3)").
                                     arg(reply->errorString()).
                                     arg(reply->rawResult().exceptionCode(), -1, 16).arg(to_string(reply->rawResult().exceptionCode())), 5000);

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
    }
}

void MainWindow::on_btnStart_clicked()
{
    if (ui->labelNames->count() > 1)
    {
        m_read_permanent = true;
        m_read_index     = 1;
        read_meter_value();
    }
}

void MainWindow::on_btnStop_clicked()
{
    m_read_permanent = false;
    statusBar()->showMessage(tr("Stopped reading"));
}

void MainWindow::on_btnRemove_clicked()
{
    for (int i = 1; i < ui->labelCheck->count(); ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->labelCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            ui->labelCheck->removeWidget(check);
            delete check;
            auto meter = ui->labelMeter->itemAt(i)->widget();
            ui->labelMeter->removeWidget(meter);
            delete meter;
            auto name = ui->labelNames->itemAt(i)->widget();
            ui->labelNames->removeWidget(name);
            delete name;
            break;
        }
    }
}

void MainWindow::on_btnUp_clicked()
{
    for (int i = 2; i < ui->labelCheck->count(); ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->labelCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            QWidget* check = ui->labelCheck->itemAt(i)->widget();
            ui->labelCheck->removeWidget(check);
            ui->labelCheck->insertWidget(i-1, check);

            QWidget* name = ui->labelNames->itemAt(i)->widget();
            ui->labelNames->removeWidget(name);
            ui->labelNames->insertWidget(i-1, name);

            QWidget* meter = ui->labelMeter->itemAt(i)->widget();
            ui->labelMeter->removeWidget(meter);
            ui->labelMeter->insertWidget(i-1, meter);
            break;
        }
    }
}

void MainWindow::on_btnDown_clicked()
{
    for (int i = 1; i < ui->labelCheck->count()-1; ++i)
    {
        auto check = dynamic_cast<QCheckBox*>(ui->labelCheck->itemAt(i)->widget());
        if (check &&check->isChecked())
        {
            QWidget* check = ui->labelCheck->itemAt(i)->widget();
            ui->labelCheck->removeWidget(check);
            ui->labelCheck->insertWidget(i+1, check);

            QWidget* name = ui->labelNames->itemAt(i)->widget();
            ui->labelNames->removeWidget(name);
            ui->labelNames->insertWidget(i+1, name);

            QWidget* meter = ui->labelMeter->itemAt(i)->widget();
            ui->labelMeter->removeWidget(meter);
            ui->labelMeter->insertWidget(i+1, meter);
            break;
        }
    }
}

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
