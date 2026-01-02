#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yaml-cpp/yaml.h>
#include <memory>
#include <QModbusDataUnit>
#include <QFile>
#include <QThread>

#if SERIALBUS == 1
class QModelIndex;
class QModbusClient;
class QModbusReply;
#else
#include <boost/asio.hpp>
#include <mothbus/adu/tcp.h>
#endif

class QStandardItemModel;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct meter;
struct measured_value;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void set_no_gui();
    bool display_gui();
#if SERIALBUS == 0
    void timerEvent(QTimerEvent *event);
#endif

private slots:
    void on_btnLoadYamlFile_clicked();
    void on_btnConnect_clicked();
#if SERIALBUS == 1
    void readReady();
    void onStateChanged(int state);
#else
    void readReady(QVector<quint16> &values);
#endif
    void on_tableView_clicked(const QModelIndex &index);
    void on_btnAddMeter_clicked();
    void on_btnStartRead_clicked();
    void on_btnStopRead_clicked();

    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnUp_clicked();
    void on_btnDown_clicked();
    void on_btnRemove_clicked();
    void btnCheckboxClicked();
    void on_btnReadConfig_clicked();
    void on_btnTest_clicked();

private:
    QString getConfigName() const;
    bool load_yaml(const QString &filename);

    void readValue(const QString& name, int values=1);
    void add_meter_widgets(const QString &name, const QString &pretty_name, int values,
                           double scale, double minimum, double maximum, double value,
                           const QString &unit);
    void read_meter_value();
    void updateButtons();

    void create_modbus_device();
    void disconnect_modbus_device();

#if SERIALBUS == 1
    QModbusReply  *lastRequest   = nullptr;
    QModbusClient *modbusDevice = nullptr;
#else
    void process();
    QThread                 m_thread;
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::socket  m_socket;
#endif

    enum class read { off, table, meter, control };

    Ui::MainWindow *ui = nullptr;
    QStandardItemModel*  mListModel = nullptr;
    QList<int>           m_hidden_columns;

    QString        m_pending_request;
    read           m_read_permanent = read::off;
    int            m_read_index = 0;
    int            m_request_index = 0;

    QString        mDocumentFile;
    std::unique_ptr<meter> m_meter;
    bool           m_gui_mode = true;

    QString        mConfigurationFileName;
    QFile          m_configuration_file;


};
#endif // MAINWINDOW_H
