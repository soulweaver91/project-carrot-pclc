#pragma once

#include <QString>
#include <QDir>
#include "Jazz2Level.h"

class PCLevelConverter {
public:
    static void convert(const QString& filename, const QString& id, const QDir& outputDir);
    static void printDetailedEventStats(const PCLevelConversionStatistics& stats);
};
