#pragma once

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>
#include "PCEvent.h"
#include "JJ2Event.h"
#include <functional>
#include <array>

enum JJ2EventParamType {
    NONE,
    BOOL,
    UINT,
    INT
};

typedef std::array<quint16, 8> ParamArray;
typedef std::array<QPair<JJ2EventParamType, quint8>, 5> JJ2EventParamListDefinition;

struct ConversionResult {
    PCEvent event;
    ParamArray parameters;
};

typedef std::function<ConversionResult(quint32)> ConversionFunction;


class EventConverter {
public:
    static ConversionResult convert(JJ2Event old, quint32 params);
    static ConversionResult convert(quint16 old, quint32 params);
    static QString getJazz2EventName(quint16 num);
    static QString getPCEventName(PCEvent ev);
private:
    static void init();
    static QMap<JJ2Event, ConversionFunction> assigned;
    static ConversionFunction getTrivialConverter(PCEvent ev);
    static ConversionFunction getConstantParameterListConverter(PCEvent ev, ParamArray params);
    static ConversionFunction getTrivialParameterListConverter(PCEvent ev, JJ2EventParamListDefinition paramDefs);
    static ConversionFunction getSpringConverter(PCEvent ev, bool horizontal);
    static ConversionFunction getPlatformConverter(quint8 type);
    static ConversionFunction getAmmoCrateConverter(quint8 type);

    static ParamArray convertParamInt(quint32 paramInt, JJ2EventParamListDefinition paramTypes);

    const static QStringList JJ2EventNames;
    const static QMap<PCEvent, QString> PCEventNames;
};
