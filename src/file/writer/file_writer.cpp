#include "file_writer.hpp"
#include "validation_defines.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QCryptographicHash>


FileWriter::FileWriter(QObject *parent) : QObject(parent)
{
    initialize();
}

FileWriter::FileWriter(const QString &filename, QObject *parent) : QObject(parent)
{
    initialize(filename);
}


FileWriter::~FileWriter()
{
    close();
}

void FileWriter::write(const QVector<device::DevicePSDSettings> &settings_array)
{
    if (!file->isOpen())
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

    qDebug() << hash;
    qDebug() << file->size();
}


void FileWriter::write(const QVector<device::WaveformPacket> &waveform_array)
{
    if (!file->isOpen())
    {
        qWarning() << "File is not open for writing.";
        return;
    }
    qDebug() << file->size();

    for (const auto& waveform : waveform_array)
    {
        QByteArray buffer;
        QDataStream out(&buffer, QIODevice::WriteOnly);

        if (waveform.values.size() > std::numeric_limits<quint32>::max())
        {
            qWarning() << "Invalid settings array size.";
            return;
        }
        out.writeRawData(default_body_prefix, 4);

        out << waveform;

        out.writeRawData(default_body_prefix, 4);

        //QByteArray hash = QCryptographicHash::hash(buffer, QCryptographicHash::Sha256);

        file->write(buffer);
        //file->write(hash);
    }
}

void FileWriter::close()
{
    if (file->isOpen())
    {
        file->close();
    }
}

QString FileWriter::filename()
{
    if (file->exists())
    {
        return file->fileName();
    }

    return "";
}

bool FileWriter::initialize(QString filename)
{
    if (filename == "")
    {
        filename = default_filename.arg(QDateTime::currentDateTime()
                                            .toString(default_datetime),
                                        "test");
    }

    file = new QFile(filename);

    if (!file->open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open file for writing:" << file->errorString();
        return false;
    }
    QString signatureString = default_signature.arg(version_major,
                                                    version_minor,
                                                    version_patch);

    QByteArray signature = QByteArray::fromHex(signatureString.toUtf8());
    file->write(signature);

    return true;
}


