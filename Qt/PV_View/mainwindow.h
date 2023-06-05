#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yaml-cpp/yaml.h>
#include <memory>
#include <QModbusDataUnit>

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

private slots:
    void on_btnLoadYamlFile_clicked();
    void readReady();
    void on_btnConnect_clicked();
    void onStateChanged(int state);
    void on_tableView_clicked(const QModelIndex &index);
    void on_btnAddMeter_clicked();
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnRemove_clicked();
    void on_btnUp_clicked();
    void on_btnDown_clicked();

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

    Ui::MainWindow *ui = nullptr;
    QStandardItemModel*  mListModel = nullptr;
    QList<int>           m_hidden_columns;

    QModbusReply  *lastRequest   = nullptr;
    QModbusClient *modbusDevice = nullptr;
    QString        m_pending_request;
    bool           m_read_permanent = false;
    int            m_read_index = 0;

    QString        mDocumentFile;
    std::unique_ptr<meter> m_meter;
};
#endif // MAINWINDOW_H
