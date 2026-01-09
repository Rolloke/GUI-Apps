#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yaml-cpp/yaml.h>
#include <memory>
#include <QModbusDataUnit>
#include <QFile>
#include <QThread>
#include <QStandardItemModel>

#if SERIALBUS == 1
class QModelIndex;
class QModbusClient;
class QModbusReply;
#else
#include <boost/asio.hpp>
#include <mothbus/adu/tcp.h>
#endif


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct meter;
struct measured_value;
typedef QMap<QString, QString> stored_value_list;


class CheckboxItemModel : public QStandardItemModel
{
public:
    CheckboxItemModel(int rows, int columns, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setCheckedColumn(int checked);
private:
    int mChecked = 0;
};

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
    void on_btnStoreValues_clicked();
    void on_btnLoadValues_clicked();
    void on_btnSendValueToPv_clicked();
    void on_btnUpdataList_clicked();
    void on_tableViewSchedule_clicked(const QModelIndex &index);
    void on_btn_clicked();

private:
    QString getConfigName() const;
    bool load_yaml(const QString &filename);

    void readValue(const QString& name, int values=1);
    void add_meter_widgets(const QString &name, const QString &pretty_name, int values,
                           double scale, double minimum, double maximum, double value,
                           const QString &unit);
    void read_meter_value();
    void update_buttons();
    void init_table_and_controls();

    void create_modbus_device();
    void disconnect_modbus_device();
    void update_schedule_value_list();

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

    CheckboxItemModel*  mListModelSchedule = nullptr;

    QString        m_pending_request;
    read           m_read_permanent = read::off;
    int            m_read_index = 0;
    int            m_request_name_index = 0;
    int            m_request_section_index = 0;

    QString        mDocumentFile;
    std::unique_ptr<meter> m_meter;
    bool           m_gui_mode = true;

    QString        m_store_value_section;
    stored_value_list m_values;

    QString        m_control_filter_section;
    QString        mConfigurationFileName;

};


#endif // MAINWINDOW_H
