#ifndef DIRECTIODEVICE_H
#define DIRECTIODEVICE_H

#include <QIODevice>

class DirectIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit DirectIODevice(QObject *parent=nullptr);

    bool open(OpenMode mode);
    void close();
    bool isSequential() const;

Q_SIGNALS:
    void read_data(char *aData, qint64 aMaxBytes, qint64& aBytesRead);
    void write_data(const char *aData, qint64 aBytes, qint64& aBytesWritten);

private:
    virtual qint64 readData(char *aData, qint64 aMaxLen);
    virtual qint64 writeData(const char *aData, qint64 aBytes);

};

#endif // DIRECTIODEVICE_H
