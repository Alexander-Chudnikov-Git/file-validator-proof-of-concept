#ifndef FILE_VALIDATOR_HPP
#define FILE_VALIDATOR_HPP

#include "header_structure.hpp"
#include "packet_structure.hpp"

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QFile>
#include <QDateTime>


class FileValidator : public QObject
{
    Q_OBJECT
public:
    enum class ValidationError
    {
        None,
        UnableToOpen,
        MissingHeaderSignature,
        WrongHeaderHash,
        MalformedWaveformPacket,
        WrongWaveformPacket,
    };
    Q_DECLARE_FLAGS(ValidationErrors, ValidationError)

    explicit FileValidator(QObject *parent = nullptr);
    explicit FileValidator(const QString &filename, QObject *parent = nullptr);
    ~FileValidator();

    ValidationError errors() const;

    ValidationError validateFile();

    uint32_t validPacketNumber() const;

    friend std::ostream& operator<<(std::ostream& os, const FileValidator::ValidationError& error)
    {
        switch (error)
        {
        case FileValidator::ValidationError::None:
            os << "None";
            break;
        case FileValidator::ValidationError::UnableToOpen:
            os << "UnableToOpen";
            break;
        case FileValidator::ValidationError::MissingHeaderSignature:
            os << "MissingHeaderSignature";
            break;
        case FileValidator::ValidationError::WrongHeaderHash:
            os << "WrongHeaderHash";
            break;
        case FileValidator::ValidationError::MalformedWaveformPacket:
            os << "MalformedWaveformPacket";
            break;
        case FileValidator::ValidationError::WrongWaveformPacket:
            os << "WrongWaveformPacket";
            break;
        default:
            os << "Unknown";
            break;
        }
        return os;
    }

    void close();

private:
    void initialize(const QString &filename);

private:
    QFile *file;
    ValidationError error{ValidationError::None};
    uint32_t valid_packets;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FileValidator::ValidationErrors)

#endif // FILE_VALIDATOR_HPP
