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
    void on_btnReadValue_clicked();
    void readReady();
    void on_btnConnect_clicked();
    void onStateChanged(int state);

    void on_tableView_clicked(const QModelIndex &index);

private:
    QString getConfigName() const;
    bool load_yaml(const QString &filename);
    void create_modbus_device();
    void readValue(const QString& name);
    QModbusDataUnit::RegisterType get_type(const QString& name);
    int                           get_address(const QString& address, int n=-1);
    int                           get_entries(const QString& decode);
    Ui::MainWindow *ui = nullptr;
    QStandardItemModel*  mListModel = nullptr;
    QList<int>           m_hidden_columns;

    QModbusReply *lastRequest   = nullptr;
    QModbusClient *modbusDevice = nullptr;
    QString m_pending_request;

    QString              mDocumentFile;
    std::unique_ptr<meter> m_meter;
};
#endif // MAINWINDOW_H
