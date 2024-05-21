#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

#include "header_structure.hpp"
#include "packet_structure.hpp"

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QFile>
#include <QDateTime>

class FileWriter : QObject
{
    Q_OBJECT
public:
    explicit FileWriter(QObject *parent = nullptr);
    explicit FileWriter(const QString &filename, QObject *parent = nullptr);
    ~FileWriter();

    void write(const QVector<device::DevicePSDSettings> &settings_array);
    void write(const QVector<device::WaveformPacket> &waveform_array);

    void close();

    QString filename();

private:
    bool initialize(QString filename = "");

private:
    QFile *file;
};

#endif // FILE_WRITER_HPP
