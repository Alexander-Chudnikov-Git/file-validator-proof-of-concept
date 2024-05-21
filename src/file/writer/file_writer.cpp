#include "file_writer.hpp"
#include "validation_defines.hpp"

#include <QDataStream>
#include <QCryptographicHash>
#include <QDebug>
#include <limits>


FileWriter::FileWriter(QObject *parent) : QObject(parent), file(nullptr)
{
    initialize();
}

FileWriter::FileWriter(const QString &filename, QObject *parent) : QObject(parent), file(nullptr)
{
    initialize(filename);
}

FileWriter::~FileWriter()
{
    close();
    delete file;
}

void FileWriter::write(const QVector<device::DevicePSDSettings> &settings_array)
{
    if (!file || !file->isOpen())
    {
        qWarning() << "File is not open for writing.";
        return;
    }

    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);

    if (settings_array.size() > std::numeric_limits<uint16_t>::max())
    {
        qWarning() << "Invalid settings array size.";
        return;
    }
    out << static_cast<uint16_t>(settings_array.size());

    for (const auto& settings : settings_array)
    {
        out << settings;
    }

    QByteArray hash = QCryptographicHash::hash(buffer, QCryptographicHash::Md5);

    file->write(buffer);
    file->write(hash);
}

void FileWriter::write(const QVector<device::WaveformPacket> &waveform_array)
{
    if (!file || !file->isOpen())
    {
        qWarning() << "File is not open for writing.";
        return;
    }

    for (const auto& waveform : waveform_array)
    {
        QByteArray buffer;
        QDataStream out(&buffer, QIODevice::WriteOnly);

        if (waveform.values.size() > std::numeric_limits<quint32>::max())
        {
            qWarning() << "Invalid waveform packet size.";
            return;
        }
        out.writeRawData(default_body_prefix, 4);

        out << waveform;

        out.writeRawData(default_body_prefix, 4);

        file->write(buffer);
    }
}

void FileWriter::close()
{
    if (file && file->isOpen())
    {
        file->close();
    }
}

QString FileWriter::filename()
{
    if (file && file->exists())
    {
        return file->fileName();
    }

    return "";
}

bool FileWriter::initialize(QString filename)
{
    if (filename.isEmpty())
    {
        filename = default_filename.arg(QDateTime::currentDateTime().toString(default_datetime), "test");
    }

    file = new (std::nothrow) QFile(filename);
    if (!file)
    {
        qWarning() << "Failed to allocate memory for QFile.";
        return false;
    }

    if (!file->open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open file for writing:" << file->errorString();
        delete file;
        file = nullptr;
        return false;
    }

    QString signatureString = default_signature.arg(version_major, version_minor, version_patch);
    QByteArray signature = QByteArray::fromHex(signatureString.toUtf8());
    file->write(signature);

    return true;
}
