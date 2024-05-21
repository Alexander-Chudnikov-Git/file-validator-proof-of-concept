#ifndef FILE_VALIDATOR_HPP
#define FILE_VALIDATOR_HPP

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
        InvalidSignature,
        WrongHeaderHash,
        MalformedWaveformPacket,
        WrongWaveformPacket,
        ReadError
    };
    Q_DECLARE_FLAGS(ValidationErrors, ValidationError)

    explicit FileValidator(QObject *parent = nullptr);
    explicit FileValidator(const QString &filename, QObject *parent = nullptr);

    ~FileValidator();

    void initialize(const QString &filename);

    ValidationError validateFile();

    ValidationError errors() const;
    uint32_t settingsNumber() const;
    uint32_t validPacketNumber() const;

    void close();

private:
    bool validateSignature();
    bool validateSettings();
    bool validateWaveformPackets();

public:
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
        case FileValidator::ValidationError::InvalidSignature:
            os << "InvalidSignature";
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
        case FileValidator::ValidationError::ReadError:
            os << "ReadError";
            break;
        default:
            os << "Unknown";
            break;
        }
        os << "\n";
        return os;
    }


private:
    QFile *file;
    ValidationError error{ValidationError::None};

    uint16_t settings_number;
    uint32_t valid_packets;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FileValidator::ValidationErrors)

#endif // FILE_VALIDATOR_HPP
