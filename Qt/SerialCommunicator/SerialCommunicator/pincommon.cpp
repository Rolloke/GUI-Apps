#include "pincommon.h"
#include <map>
#include <string>

PinCommon::PinCommon()
    : mIndex(0)
    , mPinNumber(0)
    , mPinType(Type::Unknown)
    , mEdge(Edge::Unknown)
    , mValue(-1)
    , mPulse(-1)
    , mDelay(-1)
    , mRepeat(-1)
{

}

const char* PinCommon::name_of (PinCommon::Type::eType aType)
{
    switch (aType)
    {
    case Type::Binary:    return "BINARY";
    case Type::Analog:    return "ANALOG";
    case Type::Tone:      return "TONE";
    case Type::Switch:    return "SWITCH";
    case Type::Interrupt: return "INTERRUPT";
    case Type::Unknown:  default:
        return "Unknown";
    }
}

PinCommon::Type::eType PinCommon::type_name(const char* aName)
{
    static std::map<std::string, int> fMap;
    if (not fMap.size())
    {
        for (int fType = Type::Unknown; fType < Type::Last; ++fType)
        {
            fMap[name_of(static_cast<Type::eType>(fType))] = fType;
        }
    }
    auto fType = fMap.find(aName);
    if (fType != fMap.end())
    {
        return static_cast<Type::eType>(fType->second);
    }
    return Type::Unknown;
}

const char* PinCommon::name_of (PinCommon::Edge::eEdge aEdge)
{
    switch (aEdge)
    {
    case Edge::Change:  return "CHANGE";
    case Edge::Rising:  return "RISING";
    case Edge::Falling: return "FALLING";
    case Edge::High:    return "HIGH";
    case Edge::Low:     return "LOW";
    case Edge::Unknown:
    default:            return "DEFAULT";
    }
}

PinCommon::Edge::eEdge PinCommon::edge_name(const char* aName)
{
    static std::map<std::string, int> fMap;
    if (not fMap.size())
    {
        for (int fType = Edge::Unknown; fType < Edge::Last; ++fType)
        {
            fMap[name_of(static_cast<Edge::eEdge>(fType))] = fType;
        }
    }
    auto fType = fMap.find(aName);
    if (fType != fMap.end())
    {
        return static_cast<Edge::eEdge>(fType->second);
    }
    return Edge::Unknown;
}
