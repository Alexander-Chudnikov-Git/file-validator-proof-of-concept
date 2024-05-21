#ifndef HEADER_STRUCTURE_HPP
#define HEADER_STRUCTURE_HPP

#include <QObject>
#include <QDataStream>


namespace device
{
enum class TTFilterType
{
    LED,
    CFD
};

struct DevicePSDSettings
{
    uint16_t channelId;
    uint32_t psdWaveLength;

    uint32_t psdPreTriggerLength;
    uint32_t triggerHoldOff;

    uint32_t psdPreGateLength;
    uint32_t psdShortGateLength;
    uint32_t psdLongGateLength;

    TTFilterType filterType;

    union {
        struct
        {
            uint32_t ledThresholdUp;
            uint32_t ledThresholdDown;
        } ledSettings;

        struct
        {
            uint32_t cfdDelay;
            uint32_t cfdThreshold;
            double cfdFraction;
        } cfdSettings;
    };

    friend QDataStream &operator<<(QDataStream &s, const DevicePSDSettings &value)
    {
        s << value.channelId;
        s << value.psdWaveLength;

        s << value.psdPreTriggerLength;
        s << value.triggerHoldOff;

        s << value.psdPreGateLength;
        s << value.psdShortGateLength;
        s << value.psdLongGateLength;

        s << value.filterType;

        if (value.filterType == TTFilterType::LED)
        {
            s << value.ledSettings.ledThresholdUp;
            s << value.ledSettings.ledThresholdDown;
            s << 0.;
        }
        else if (value.filterType == TTFilterType::CFD)
        {
            s << value.cfdSettings.cfdDelay;
            s << value.cfdSettings.cfdThreshold;
            s << value.cfdSettings.cfdFraction;
        }

        return s;
    }

    friend QDataStream &operator>>(QDataStream &s, DevicePSDSettings &value)
    {
        s >> value.channelId;
        s >> value.psdWaveLength;

        s >> value.psdPreTriggerLength;
        s >> value.triggerHoldOff;

        s >> value.psdPreGateLength;
        s >> value.psdShortGateLength;
        s >> value.psdLongGateLength;

        s >> value.filterType;

        if (value.filterType == TTFilterType::LED)
        {
            s >> value.ledSettings.ledThresholdUp;
            s >> value.ledSettings.ledThresholdDown;

            double padding;
            s >> padding;
        }
        else if (value.filterType == TTFilterType::CFD)
        {
            s >> value.cfdSettings.cfdDelay;
            s >> value.cfdSettings.cfdThreshold;
            s >> value.cfdSettings.cfdFraction;
        }

        return s;
    }

    bool operator==(const DevicePSDSettings &other) const
    {
        bool result;

        result = channelId == other.channelId;
        result &= psdWaveLength == other.psdWaveLength;
        result &= psdPreTriggerLength == other.psdPreTriggerLength;
        result &= triggerHoldOff == other.triggerHoldOff;
        result &= psdPreGateLength == other.psdPreGateLength;
        result &= psdShortGateLength == other.psdShortGateLength;
        result &= psdLongGateLength == other.psdLongGateLength;
        result &= filterType == other.filterType;
        result &= psdPreGateLength == other.psdPreGateLength;
        result &= filterType == other.filterType;

        if (filterType == TTFilterType::LED)
        {
            result &= ledSettings.ledThresholdUp == other.ledSettings.ledThresholdUp;
            result &= ledSettings.ledThresholdDown == other.ledSettings.ledThresholdDown;
        }
        else if (filterType == TTFilterType::CFD)
        {
            result &= cfdSettings.cfdDelay == other.cfdSettings.cfdDelay;
            result &= cfdSettings.cfdThreshold == other.cfdSettings.cfdThreshold;
            result &= cfdSettings.cfdFraction == other.cfdSettings.cfdFraction;
        }

        return result;
    }

    bool operator!=(const DevicePSDSettings &other) const
    {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const DevicePSDSettings& value)
    {
        os << "Channel ID: " << value.channelId << "\n";
        os << "PSD Wave Length: " << value.psdWaveLength << "\n";

        os << "PSD Pre Trigger: " << value.psdPreTriggerLength << "\n";
        os << "Trigger Hold Off: " << value.triggerHoldOff << "\n";

        os << "PSD Pre Gate: " << value.psdPreGateLength << "\n";
        os << "PSD Short Gate: " << value.psdShortGateLength << "\n";
        os << "PSD Long Gate: " << value.psdLongGateLength << "\n";

        os << "Filter Type: " << static_cast<int>(value.filterType) << "\n";

        if (value.filterType == TTFilterType::LED)
        {
            os << "Led Threshold Up: " << value.ledSettings.ledThresholdUp << "\n";
            os << "Led Threshold Down: " << value.ledSettings.ledThresholdDown << "\n";
        }
        else if (value.filterType == TTFilterType::CFD)
        {
            os << "CFD Delay: " << value.cfdSettings.cfdDelay << "\n";
            os << "CFD Threshold: " << value.cfdSettings.cfdThreshold << "\n";
            os << "CFD Fraction: " << value.cfdSettings.cfdFraction << "\n";
        }

        return os;
    }
};

} // namespace device

#endif // HEADER_STRUCTURE_HPP
