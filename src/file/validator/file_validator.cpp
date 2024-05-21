#include "file_validator.hpp"
#include "validation_defines.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QCryptographicHash>
#include <QDebug>


FileValidator::FileValidator(QObject *parent)
    : QObject(parent), file(nullptr), error(ValidationError::None), settings_number(0), valid_packets(0)
{}

FileValidator::FileValidator(const QString &filename, QObject *parent)
    : QObject(parent), file(nullptr), error(ValidationError::None), settings_number(0), valid_packets(0)
{
    initialize(filename);
}

FileValidator::~FileValidator()
{
    close();
    delete file;
}

void FileValidator::initialize(const QString &filename)
{
    file = new QFile(filename);

    if (!file->open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open file for reading:" << file->errorString();
        error = ValidationError::UnableToOpen;
        close();
    }
}

FileValidator::ValidationError FileValidator::validateFile()
{
    if (!file || !file->isOpen())
    {
        qWarning() << "File is not loaded or open:" << (file ? file->errorString() : "File is null");
        error = ValidationError::UnableToOpen;
        return error;
    }

    if (!validateSignature())
    {
        close();
        return error;
    }

    if (!validateSettings())
    {
        close();
        return error;
    }

    if (!validateWaveformPackets())
    {
        close();
        return error;
    }

    close();
    return ValidationError::None;
}

FileValidator::ValidationError FileValidator::errors() const
{
    return error;
}

uint32_t FileValidator::settingsNumber() const
{
    return settings_number;
}

uint32_t FileValidator::validPacketNumber() const
{
    return valid_packets;
}

void FileValidator::close()
{
    if (file && file->isOpen())
    {
        file->close();
    }
}

bool FileValidator::validateSignature()
{
    if (file->bytesAvailable() < 8)
    {
        qWarning() << "File is too small to contain a valid signature";
        error = ValidationError::InvalidSignature;
        return false;
    }

    auto signature = file->read(8);

    if (signature.size() != 8)
    {
        qWarning() << "Failed to read the full signature";
        error = ValidationError::ReadError;
        return false;
    }

    const auto expected_signature = QByteArray::fromHex(default_signature
                                                            .arg(static_cast<uchar>(signature[4]), 2, 16, QChar('0'))
                                                            .arg(static_cast<uchar>(signature[5]), 2, 16, QChar('0'))
                                                            .arg(static_cast<uchar>(signature[6]), 2, 16, QChar('0'))
                                                            .toUtf8()
                                                        );

    if (signature != expected_signature)
    {
        qWarning() << "Wrong signature: \nexpected:" << expected_signature << "in file:" << signature;
        error = ValidationError::InvalidSignature;
        return false;
    }

    return true;
}

bool FileValidator::validateSettings()
{
    if (file->bytesAvailable() < 2)
    {
        qWarning() << "File is too small to contain settings bytes";
        error = ValidationError::ReadError;
        return false;
    }

    auto settings_bytes = file->read(2);
    if (settings_bytes.size() != 2)
    {
        qWarning() << "Failed to read settings bytes";
        error = ValidationError::ReadError;
        return false;
    }

    auto number_of_settings = settings_bytes.toHex().toUInt(nullptr, 16);
    settings_number = number_of_settings;

    qint64 expected_settings_size = number_of_settings * 46;
    if (file->bytesAvailable() < expected_settings_size + 16)
    {
        qWarning() << "File is too small to contain all settings and hash";
        error = ValidationError::ReadError;
        return false;
    }

    auto settings = file->read(expected_settings_size);
    if (settings.size() != expected_settings_size)
    {
        qWarning() << "Failed to read all settings";
        error = ValidationError::ReadError;
        return false;
    }

    auto expected_hash = QCryptographicHash::hash(settings_bytes + settings, QCryptographicHash::Md5);
    auto settings_hash = file->read(16);
    if (settings_hash.size() != 16)
    {
        qWarning() << "Failed to read settings hash";
        error = ValidationError::ReadError;
        return false;
    }

    if (settings_hash != expected_hash)
    {
        qWarning() << "Wrong setting hash: \nexpected:" << expected_hash.toHex() << "in file:" << settings_hash.toHex();
        error = ValidationError::WrongHeaderHash;
        return false;
    }

    return true;
}

bool FileValidator::validateWaveformPackets()
{
    uint32_t number_of_waveform_packets = 0;

    while (!file->atEnd())
    {
        if (file->bytesAvailable() < 8)
        {
            qWarning() << "File is too small to contain a valid waveform packet";
            error = ValidationError::MalformedWaveformPacket;
            return false;
        }

        auto waveform_prefix = file->read(4);
        if (waveform_prefix.size() != 4)
        {
            qWarning() << "Failed to read waveform prefix";
            error = ValidationError::ReadError;
            return false;
        }

        if (waveform_prefix != default_body_prefix)
        {
            qWarning() << "Wrong waveform prefix: \nexpected:" << default_body_prefix.toHex() << "in file:" << waveform_prefix.toHex();
            qWarning() << "Found" << number_of_waveform_packets << "valid packets.";
            error = ValidationError::MalformedWaveformPacket;
            return false;
        }

        auto waveform_values_bytes = file->read(4);
        if (waveform_values_bytes.size() != 4)
        {
            qWarning() << "Failed to read waveform values bytes";
            error = ValidationError::ReadError;
            return false;
        }

        auto waveform_number_of_values = waveform_values_bytes.toHex().toUInt(nullptr, 16);
        qint64 waveform_data_size = (waveform_number_of_values * 2);

        if (file->bytesAvailable() < waveform_data_size)
        {
            qWarning() << "File is too small to contain full waveform data";
            error = ValidationError::MalformedWaveformPacket;
            return false;
        }

        file->seek(file->pos() + waveform_data_size + 4);

        auto waveform_postfix = file->read(4);
        if (waveform_postfix.size() != 4)
        {
            qWarning() << "Failed to read waveform postfix";
            error = ValidationError::ReadError;
            return false;
        }

        if (waveform_postfix != default_body_prefix)
        {
            qWarning() << "Wrong waveform postfix: \nexpected:" << default_body_prefix.toHex() << "in file:" << waveform_postfix.toHex();
            qWarning() << "Found" << number_of_waveform_packets << "valid packets.";
            error = ValidationError::MalformedWaveformPacket;
            return false;
        }
        number_of_waveform_packets++;
    }

    valid_packets = number_of_waveform_packets;
    return true;
}
