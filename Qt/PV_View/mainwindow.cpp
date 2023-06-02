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

/// TODO: update all values automatically
/// Combine readings of current

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
double  get_value(const measured_value& value_param, quint16* address);
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
    , mDocumentFile("/home/rolf/Downloads/evcc-master/templates/definition/meter/huawei-sun2000-dongle-powersensor.yaml")
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

                double fScale;
                LOAD_STR(fSettings, fScale, toDouble);
                double fMinimum;
                LOAD_STR(fSettings, fMinimum, toDouble);
                double fMaximum;
                LOAD_STR(fSettings, fMaximum, toDouble);
                QString fUnit;
                LOAD_STR(fSettings, fUnit, toString);

                add_meter_widgets(fName, fPrettyName, fScale, fMinimum, fMaximum, 0, fUnit);
            }
        }
    }


    load_yaml(mDocumentFile);
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

void MainWindow::add_meter_widgets(const QString& name, const QString& pretty_name,
                                   double scale, double minimum, double maximum, double value,
                                   const QString& unit)
{
    QLabel* label_name = new QLabel(pretty_name, this);

    label_name->setToolTip(name);
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

void MainWindow::on_btnAddMeter_clicked()
{
    add_meter_widgets(ui->textSelected->text(), ui->edtPrettyName->text(),
        ui->textScale->text().toDouble(),
        ui->edtMinimum->text().toDouble(), ui->edtMaximum->text().toDouble(),
        ui->textValue->text().toDouble(), ui->edtUnit->text());
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
    readValue(mListModel->data(mListModel->index(index.row(), static_cast<int>(eName))).toString());
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

QModbusDataUnit::RegisterType MainWindow::get_type(const QString& name)
{
    if (name.compare("holding", Qt::CaseInsensitive) == 0)
    {
        return QModbusDataUnit::HoldingRegisters;
    }
    else if (name.compare("discreteinputs", Qt::CaseInsensitive) == 0)
    {
        return QModbusDataUnit::DiscreteInputs;
    }
    else if (name.compare("coils", Qt::CaseInsensitive) == 0)
    {
        return QModbusDataUnit::Coils;
    }
    else if (name.compare("input", Qt::CaseInsensitive) == 0)
    {
        return QModbusDataUnit::InputRegisters;
    }
    return QModbusDataUnit::Invalid;
}

int MainWindow::get_address(const QString& address, int n)
{
    if (n >= 0)
    {
        QStringList list = address.split(";");
        if (n < list.size())
        {
            int naddress = list[n].split(" ")[0].toInt();
            return naddress;
        }
    }
    return address.toInt();
}

int MainWindow::get_entries(const QString& decode)
{
    if (decode.contains("char"))
    {
        return decode.mid(4).toInt()/sizeof(int16_t);
    }
    else if (decode.contains("32"))
    {
        return 2;
    }
    else
    {
        return 1;
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
            if (measurement.m_register.m_decode.contains("char"))
            {
                for (uint i = 0; i < unit.valueCount(); i++)
                {
                    values[i] = unit.value(i);
                }
                char *value = reinterpret_cast<char*>(&values[0]);
                mListModel->setData(mListModel->index(ui->tableView->selectionModel()->currentIndex().row(), static_cast<int>(eValue)), value);
            }
            else
            {
                for (uint i = 0, j=unit.valueCount()-1; i < unit.valueCount(); i++, j--)
                {
                    values[i] = unit.value(j);
                }
                double value = get_value(measurement, &values[0]);
                if (ui->tabWidget->currentIndex() == 0)
                {
                    mListModel->setData(mListModel->index(ui->tableView->selectionModel()->currentIndex().row(), static_cast<int>(eValue)), value);
                    ui->textSelected->setText(m_pending_request);
                    ui->textValue->setText(tr("%1").arg(value));
                    ui->textScale->setText(tr("%1").arg(measurement.m_scale));
                    ui->edtMinimum->setText(tr("%1").arg(value/2.5));
                    ui->edtMaximum->setText(tr("%1").arg(value*2.5));
                }
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
                    }
                }
            }
        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2: %3)").
                                     arg(reply->errorString()).
                                     arg(reply->rawResult().exceptionCode(), -1, 16).arg(to_string(reply->rawResult().exceptionCode())), 5000);
        }
        else
        {
            statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                     arg(reply->errorString()).
                                     arg(reply->error(), -1, 16), 5000);
        }

        reply->deleteLater();
    }
}


double get_value(const measured_value& value_param, quint16* address)
{
    if (value_param.m_register.m_decode.contains("32"))
    {
        if (value_param.m_register.m_decode.contains("u"))
        {
            return *reinterpret_cast<uint32_t*>(address) * value_param.m_scale;
        }
        else
        {
            return *reinterpret_cast<int32_t*>(address) * value_param.m_scale;
        }
    }
    else
    {
        if (value_param.m_register.m_decode.contains("u"))
        {
            return *reinterpret_cast<uint16_t*>(address) * value_param.m_scale;
        }
        else
        {
            return *reinterpret_cast<int16_t*>(address) * value_param.m_scale;
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

void MainWindow::on_btnStart_clicked()
{

}

void MainWindow::on_btnStop_clicked()
{

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
        }
    }
}


void MainWindow::on_btnUp_clicked()
{

}


void MainWindow::on_btnDown_clicked()
{

}

