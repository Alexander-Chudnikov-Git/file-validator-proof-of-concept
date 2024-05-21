#include "file_writer.hpp"
#include "file_reader.hpp"
#include "file_validator.hpp"

#include <QDir>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>

#define WAVEFORM_MIN_VALUES 10000
#define WAVEFORM_MAX_VALUES 1000000

device::DevicePSDSettings generate_random_settings(int i)
{
    device::DevicePSDSettings settings_temp;

    settings_temp.channelId = i;
    settings_temp.psdWaveLength = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.psdPreTriggerLength = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.triggerHoldOff = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.psdPreGateLength = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.psdShortGateLength = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.psdLongGateLength = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    settings_temp.filterType = static_cast<device::TTFilterType>(QRandomGenerator::global()->bounded(0, 1));

    if (settings_temp.filterType == device::TTFilterType::LED)
    {
        settings_temp.ledSettings.ledThresholdUp = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
        settings_temp.ledSettings.ledThresholdDown = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
    }
    else
    {
        settings_temp.cfdSettings.cfdDelay = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
        settings_temp.cfdSettings.cfdThreshold = QRandomGenerator::global()->bounded(static_cast<quint32>(0), static_cast<quint32>(std::numeric_limits<uint32_t>::max()));
        settings_temp.cfdSettings.cfdFraction = static_cast<double>(QRandomGenerator::global()->bounded(static_cast<quint64>(0), static_cast<quint64>(std::numeric_limits<uint64_t>::max())));
    }
    return settings_temp;
}

device::WaveformPacket generate_random_weveforms()
{
    device::WaveformPacket weveform_temp;

    weveform_temp.nubmerOfValues = QRandomGenerator::global()->bounded(static_cast<quint32>(WAVEFORM_MIN_VALUES), static_cast<quint32>(WAVEFORM_MAX_VALUES));
    weveform_temp.baseline = QRandomGenerator::global()->bounded(static_cast<quint16>(0), static_cast<quint16>(std::numeric_limits<uint16_t>::max()));
    weveform_temp.chanelId = QRandomGenerator::global()->bounded(static_cast<quint16>(0), static_cast<quint16>(std::numeric_limits<uint16_t>::max()));

    weveform_temp.values.reserve(weveform_temp.nubmerOfValues);
    for (int i = 0; i < weveform_temp.nubmerOfValues; ++i)
    {
        weveform_temp.values.append(QRandomGenerator::global()->bounded(static_cast<quint16>(0), static_cast<quint16>(std::numeric_limits<uint16_t>::max())));
    }

    return weveform_temp;
}

void clear_directory()
{
    QString directoryPath = ".";
    QString filePattern = "output_*";
    QDir directory(directoryPath);
    QStringList fileList = directory.entryList(QStringList(filePattern), QDir::Files);

    for(const QString &fileName : fileList)
    {
        if(!directory.remove(fileName))
        {
            qDebug() << "Failed to remove file:" << fileName;
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("file_validator");
    QCoreApplication::setApplicationVersion("1.0.12");

    QCommandLineParser parser;
    parser.setApplicationDescription("File validation example.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption header_number_option(QStringList() << "s" << "settings-number",
                                            QCoreApplication::translate("main", "Number of DevicePSDSettings."),
                                            QCoreApplication::translate("main", "number"));
    QCommandLineOption body_number_option(QStringList() << "w" << "waveform-number",
                                            QCoreApplication::translate("main", "Number of WaveformPackets."),
                                            QCoreApplication::translate("main", "number"));
    QCommandLineOption deleteOption(QStringList() << "d" << "delete",
                                    QCoreApplication::translate("main", "Delete all output files."));

    parser.addOption(header_number_option);
    parser.addOption(body_number_option);
    parser.addOption(deleteOption);

    parser.process(app);

    uint32_t number_of_settings = parser.value(header_number_option).toUInt();
    uint32_t number_of_weveforms = parser.value(body_number_option).toUInt() ;

    std::cout << "Application received: " << argc << " arguments:" << std::endl;
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << std::endl;
    }

    if (parser.isSet(deleteOption))
    {
        std::cout << "Deleting output settings files" << std::endl;
        clear_directory();
    }
    else
    {
        if (number_of_settings == 0)
        {
            number_of_settings = QRandomGenerator::global()->bounded(1, 10);
        }

        if (number_of_weveforms == 0)
        {
            number_of_weveforms = QRandomGenerator::global()->bounded(1, 10000);
        }
    }

    FileWriter writer;
    QVector<device::DevicePSDSettings> settings;
    QVector<device::WaveformPacket> waveforms;
    QVector<device::DevicePSDSettings> settings_read;
    QVector<device::WaveformPacket> waveforms_read;

    if (number_of_settings != 0)
    {
        std::cout << "Generating " << number_of_settings << " psd settings" << std::endl;

        settings.reserve(number_of_settings);

        for (int i = 0; i < number_of_settings; ++i)
        {
            settings.append(generate_random_settings(i));
        }

        std::cout << "Writing psd settings" << std::endl;

        writer.write(settings);
    }

    if (number_of_weveforms != 0)
    {
        std::cout << "Generating " << number_of_weveforms << " waveform packets" << std::endl;

        waveforms.reserve(number_of_weveforms);

        for (int i = 0; i < number_of_settings; ++i)
        {
            waveforms.append(generate_random_weveforms());
        }

        std::cout << "Writing waveform packets" << std::endl;

        writer.write(waveforms);
    }
    writer.close();

    std::cout << "Validating file" << std::endl;
    FileReader reader(writer.filename());
    std::cout << reader.checkErrors();

    std::cout << "Validating settings" << std::endl;
    reader.readSettings(settings_read);

    std::cout << "Validating waveform" << std::endl;
    reader.readWaveforms(waveforms_read);

    if (settings_read == settings)
    {
        std::cout << "Header match" << std::endl;
    }
    else
    {
        std::cout << "Header differ" << std::endl;
    }

    if (waveforms_read == waveforms)
    {
        std::cout << "Waveform match" << std::endl;
    }
    else
    {
        std::cout << "Waveform differ" << std::endl;
    }
    reader.close();

    app.quit();
    return 0;
}

