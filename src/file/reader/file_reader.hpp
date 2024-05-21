#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include "header_structure.hpp"
#include "packet_structure.hpp"
#include "file_validator.hpp"

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QFile>
#include <QDateTime>

class FileReader : QObject
{
    Q_OBJECT
public:
    explicit FileReader(QObject *parent = nullptr);
    explicit FileReader(const QString &filename, QObject *parent = nullptr);
    ~FileReader();

    bool readSettings(QVector<device::DevicePSDSettings> &settings);
    bool readWaveforms(QVector<device::WaveformPacket> &waveform);

    FileValidator::ValidationError checkErrors();

    void close();

private:
    bool initialize(const QString &filename);

private:
    QFile *file;
    FileValidator *validator;
};

#endif // FILE_READER_HPP
