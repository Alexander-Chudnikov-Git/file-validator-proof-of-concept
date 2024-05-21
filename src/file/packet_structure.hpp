#ifndef WAVEFORM_STRUCTURE_HPP
#define WAVEFORM_STRUCTURE_HPP

#include <QObject>
#include <QDataStream>


namespace device
{
struct WaveformPacket
{
    uint32_t nubmerOfValues;

    uint16_t baseline;
    uint16_t chanelId;

    QVector<uint16_t> values;

    friend QDataStream &operator<<(QDataStream &s, const WaveformPacket &value)
    {
        s << value.nubmerOfValues;
        s << value.baseline;
        s << value.chanelId;

        for (auto val : value.values)
        {
            s << val;
        }

        return s;
    }

    friend QDataStream &operator>>(QDataStream &s, WaveformPacket &value)
    {
        s >> value.nubmerOfValues;
        s >> value.baseline;
        s >> value.chanelId;

        for (auto val : value.values)
        {
            s >> val;
        }

        return s;
    }

    bool operator==(const WaveformPacket &other) const
    {
        bool result;

        result = nubmerOfValues == other.nubmerOfValues;
        result &= baseline == other.baseline;
        result &= chanelId == other.chanelId;
        result &= values.size() == other.values.size();

        if (result)
        {
            for (uint32_t index = 0; index < values.size(); ++index)
            {
                result &= values.at(index) == other.values.at(index);
            }
        }
        return result;
    }

    bool operator!=(const WaveformPacket &other) const
    {
        return !(*this == other);
    }
};

} // namespace device

#endif // WAVEFORM_STRUCTURE_HPP
