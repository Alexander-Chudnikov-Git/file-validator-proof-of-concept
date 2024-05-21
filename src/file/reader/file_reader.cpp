#include "file_reader.hpp"
#include "validation_defines.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>

#include <iostream>

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
    std::cout << "Validation start" << std::endl;
    validator = new FileValidator(filename);

    if (validator->validateFile() != FileValidator::ValidationError::None)
    {
        qWarning() << "Failed to validate:" << file->errorString();
        validator->close();
        return false;
    }
    validator->close();

    std::cout << "Validation done" << std::endl;

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
    quint16 settingsSize;
    in >> settingsSize;

    for (quint16 i = 0; i < settingsSize; ++i)
    {
        device::DevicePSDSettings settings_temp;
        in >> settings;
        settings.append(settings_temp);
    }

    return true;
}

bool FileReader::readWaveforms(QVector<device::WaveformPacket> &waveform)
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

    while (!file->atEnd())
    {
        // Add packet validation
        QByteArray buffer;
        QDataStream in(file);

        QByteArray prefix;
        in >> prefix;

        device::WaveformPacket waveform_temp;
        in >> waveform_temp;

        QByteArray postfix;
        in >> postfix;

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
