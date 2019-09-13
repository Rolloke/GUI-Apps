#ifndef PINCOMMON_H
#define PINCOMMON_H

#include <QString>

class PinCommon
{
public:
    PinCommon();
    struct Type { enum eType { Unknown, Binary, Analog, Switch, Tone, Interrupt, Last }; };
    struct Edge { enum eEdge {Unknown, Change, Rising, Falling, High, Low, Last}; };

    int mIndex;

    int mPinNumber;
    Type::eType mPinType;
    Edge::eEdge mEdge;
    int mValue;

    int mPulse;
    int mDelay;
    int mRepeat;

    int mConnection;

    static const char* name_of (Edge::eEdge aEdge);
    static Edge::eEdge edge_name(const char*);

    static const char* name_of (Type::eType aType);
    static Type::eType type_name(const char*);
};

#endif // PINCOMMON_H
