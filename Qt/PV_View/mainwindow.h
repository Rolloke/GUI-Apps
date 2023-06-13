#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yaml-cpp/yaml.h>
#include <memory>
#include <QModbusDataUnit>
#include <QFile>

class QModelIndex;
class QStandardItemModel;
class QModbusClient;
class QModbusReply;

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

private slots:
    void on_btnLoadYamlFile_clicked();
    void readReady();
    void on_btnConnect_clicked();
    void onStateChanged(int state);
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

private:
    QString getConfigName() const;
    bool load_yaml(const QString &filename);
    void create_modbus_device();
    void readValue(const QString& name, int values=1);
    void disconnect_modbus_device();
    void add_meter_widgets(const QString &name, const QString &pretty_name, int values,
                           double scale, double minimum, double maximum, double value,
                           const QString &unit);
    void read_meter_value();
    void updateButtons();

    enum class read { off, table, meter, control };

    Ui::MainWindow *ui = nullptr;
    QStandardItemModel*  mListModel = nullptr;
    QList<int>           m_hidden_columns;

    QModbusReply  *lastRequest   = nullptr;
    QModbusClient *modbusDevice = nullptr;
    QString        m_pending_request;
    read           m_read_permanent = read::off;
    int            m_read_index = 0;

    QString        mDocumentFile;
    std::unique_ptr<meter> m_meter;
    bool           m_gui_mode = true;

    QString        mConfigurationFileName;
    QFile          m_configuration_file;


};
#endif // MAINWINDOW_H
