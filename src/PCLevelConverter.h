#pragma once

#include <QString>
#include <QDir>

class PCLevelConverter {
public:
    static void convert(const QString& filename, const QString& id, const QDir& outputDir);
};
