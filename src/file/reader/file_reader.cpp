#include "file_reader.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>


FileReader::FileReader(QObject *parent) : QObject(parent)
{
}

FileReader::FileReader(const QString &filename, QObject *parent) : QObject(parent)
{
    initialize(filename);
}

FileReader::~FileReader()
{
    close();
    delete file;
}

bool FileReader::initialize(const QString &filename)
{
    validator = new FileValidator(filename);

    if (validator->validateFile() != FileValidator::ValidationError::None)
    {
        qWarning() << "Failed to validate:" << file->errorString();
        validator->close();
        return false;
    }
    validator->close();

    file = new QFile(filename);

    if (!file->open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open file for reading:" << file->errorString();
        return false;
    }

    return true;
}

bool FileReader::readSettings(QVector<device::DevicePSDSettings> &settings)
{
    if (checkErrors() != FileValidator::ValidationError::None)
    {
        return false;
    }

    if (!file->isOpen())
    {
        qWarning() << "File is not open for reading.";
        return false;
    }

    QDataStream in(file);
    in.skipRawData(10);

    uint16_t settings_size = validator->settingsNumber();

    for (uint16_t index = 0; index < settings_size; ++index)
    {
        device::DevicePSDSettings settings_temp;
        in >> settings_temp;
        settings.append(settings_temp);
    }

    file->seek(10 + (settings_size * 46) + 16); // 10 - 64 bits for signature + 16 bist for settings size
                                                // (settings_size * 46) settings offset
                                                // 16 - 128 bits md 5 hash

    return true;
}

bool FileReader::readWaveforms(QVector<device::WaveformPacket> &waveform)
{
    auto error = checkErrors();
    if (error != FileValidator::ValidationError::None && error != FileValidator::ValidationError::MalformedWaveformPacket)
    {
        return false;
    }

    if (!file->isOpen())
    {
        qWarning() << "File is not open for reading.";
        return false;
    }

    uint32_t valid_packages = validator->validPacketNumber();

    for (uint32_t index = 0; index < valid_packages; ++index)
    {
        QByteArray buffer;
        QDataStream in(file);

        buffer.resize(4);
        in.readRawData(buffer.data(), 4);

        device::WaveformPacket waveform_temp;
        in >> waveform_temp;

        buffer.resize(4);
        in.readRawData(buffer.data(), 4);

        waveform.append(waveform_temp);
    }

    return true;
}


FileValidator::ValidationError FileReader::checkErrors()
{
    if (validator != nullptr)
    {
        return validator->errors();
    }

    return FileValidator::ValidationError::None;
}

void FileReader::close()
{
    if (file->isOpen())
    {
        file->close();
    }
}
