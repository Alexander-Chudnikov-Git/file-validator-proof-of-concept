#include "file_validator.hpp"
#include "validation_defines.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QCryptographicHash>


FileValidator::FileValidator(QObject *parent) : QObject(parent)
{

}

FileValidator::FileValidator(const QString &filename, QObject *parent) : QObject(parent)
{
    initialize(filename);
}

FileValidator::~FileValidator()
{
    close();
}

void FileValidator::initialize(const QString &filename)
{
    file = new QFile(filename);

    if (!file->open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open file for reading:" << file->errorString();
        error = ValidationError::UnableToOpen;
        close();
        return;
    }
}

FileValidator::ValidationError FileValidator::validateFile()
{
    if (!file->isOpen())
    {
        qWarning() << "File is not loaded:" << file->errorString();
        error = ValidationError::UnableToOpen;
        return error;
    }

    auto signature = file->read(8);

    const auto expected_signature = QByteArray::fromHex(default_signature
            .arg(static_cast<uchar>(signature[4]), 2, 16, QChar('0'))
            .arg(static_cast<uchar>(signature[5]), 2, 16, QChar('0'))
            .arg(static_cast<uchar>(signature[6]), 2, 16, QChar('0'))
            .toUtf8()
        );

    if (signature != expected_signature)
    {
        qWarning() << "Wrong signature: \nexpected: " << expected_signature << " in file: " << signature;
        error = ValidationError::UnableToOpen;
        close();
        return error;
    }

    auto settings_bytes = file->read(2);
    auto settings_number = settings_bytes.toHex().toUInt(nullptr, 16);
    auto settings = file->read(settings_number * 46); // 368 bits X size of the settngs
    auto expected_hash = QCryptographicHash::hash(settings_bytes + settings, QCryptographicHash::Md5);
    auto settings_hash = file->read(16);

    if (settings_hash != expected_hash)
    {
        qWarning() << "Wrong setting hash: \nexpected: " << expected_hash.toHex() << " in file: " << settings_hash.toHex();
        error = ValidationError::WrongHeaderHash;
        close();
        return error;
    }

    uint32_t number_of_waveform_packets = 0;

    do
    {
        auto waveform_prefix = file->read(4);

        if (waveform_prefix != default_body_prefix)
        {
            qWarning() << "Wrong waveform prefix: \nexpected: " << default_body_prefix.toHex() << " in file: " << waveform_prefix.toHex();
            qWarning() << "Found " << number_of_waveform_packets << " valid packets.";
            error = ValidationError::MalformedWaveformPacket;
            valid_packets = number_of_waveform_packets;
            break;
        }

        auto waveform_values_bytes = file->read(4);
        auto waveform_number_of_values = waveform_values_bytes.toHex().toUInt(nullptr, 16);

        qDebug() << waveform_number_of_values << " " << waveform_values_bytes;
        file->seek(file->pos() + 4 + (waveform_number_of_values * 2));

        auto waveform_postfix = file->read(4);
        if (waveform_postfix != default_body_prefix)
        {
            qWarning() << "Wrong waveform postfix: \nexpected: " << default_body_prefix.toHex() << " in file: " << waveform_postfix.toHex();
            qWarning() << "Found " << number_of_waveform_packets << " valid packets.";
            error = ValidationError::MalformedWaveformPacket;
            valid_packets = number_of_waveform_packets;
            break;
        }
    }
    while (file->atEnd());

    close();
    return error;
}


FileValidator::ValidationError FileValidator::errors() const
{
    return error;
}


uint32_t FileValidator::validPacketNumber() const
{
    return valid_packets;
}

void FileValidator::close()
{
    if (file->isOpen())
    {
        file->close();
    }
}
