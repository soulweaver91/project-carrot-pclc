// J2L to native Project Carrot level file converter
// Written in 2013 by Soulweaver

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QString>
#include <iostream>

#include "Jazz2FormatParseException.h"
#include "Jazz2Level.h"
#include "PCLevelConverter.h"
#include "Version.h"

void PCLevelConverter::convert(const QString& filename, const QString& id, const QDir& outputDir) {
    Jazz2Level* level = Jazz2Level::fromFile(filename);
    level->printData(std::cout);
    auto stats = level->saveAsProjectCarrotLevel(outputDir, id);
    printDetailedEventStats(stats);
}

void PCLevelConverter::printDetailedEventStats(const PCLevelConversionStatistics& stats) {
    QString diff[4] = { "ALL ", "EASY", "HARD", "MP  " };

    std::cout << "Event data\n";
    for (auto e : stats.events.eventAppearances) {
        std::cout << "("  << QString::number(e.x).rightJustified(3).toStdString()
                  << ", " << QString::number(e.y).rightJustified(3).toStdString() << ")  "
                  << "JJ2:   0x" << QString::number((quint8)e.originalEventType, 16).rightJustified(2, '0').toStdString()
                  << " / " << e.originalEventName.leftJustified(32, ' ', true).toStdString()
                  << "\n  " << diff[e.difficulty].toStdString() << "  " << (e.illuminate ? "IL" : "  ") << "  "
                  << "PC:  0x" << QString::number((quint16)e.eventType, 16).rightJustified(4, '0').toStdString()
                  << " / " << e.eventName.leftJustified(32, ' ', true).toStdString()
                  << "  ";

        for (auto i : e.params) {
            std::cout << QString::number((qint16)i).rightJustified(6).toStdString() << " ";
        }
        std::cout << "\n";
    }

    if (stats.events.unsupportedEvents.size() > 0) {
        std::cout << "\nUnsupported events:\n";
        for (auto key : stats.events.unsupportedEvents.keys()) {
            auto e = stats.events.unsupportedEvents.value(key);
            std::cout << "0x" << QString::number((quint8)key, 16).rightJustified(2, '0').toStdString()
                      << " / " << e->eventName.leftJustified(32, ' ', true).toStdString()
                      << " x" << e->count << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        std::cout << "PCLC version " << CONVERTERVERSION << "\n"
            << "Usage: pclc name inputfile\n\n"
            << "Converts a Jazz Jackrabbit 2 level file (.j2l) to the native, multiple file\n"
            << "based format recognized by Project Carrot.\n\n"
            << "Parameters:\n"
            << "  name         Optional. The unique identifying name of this level. Use only\n"
            << "               Latin letters, numbers and underscores. For the purposes of\n"
            << "               being cross platform compliant, identifiers are case\n"
            << "               insensitive.\n"
            << "               If omitted, the name of the input file is used.\n"
            << "  inputfile    Required. Complete or relative path to a J2L format file to be\n"
            << "               converted.\n";
        return EXIT_FAILURE;
    }

    QString uniqueID;
    QString filename;
    if (argc == 3) {
        uniqueID = argv[1];
        filename = argv[2];
    } else {
        filename = argv[1];
        uniqueID = QFileInfo(filename).baseName();
    }

    QDir outdir(QDir::current());
    if (!outdir.mkdir(uniqueID)) {
        int idx = 0;
        while (!outdir.mkdir(uniqueID + "_" + QString::number(idx))) {
            idx++;
        }
        outdir.cd(uniqueID + "_" + QString::number(idx));
    } else {
        outdir.cd(uniqueID);
    }

    try {
        std::cout << "Converting \"" << filename.toStdString() << "\" to Project Carrot level \"" << uniqueID.toStdString() << "\"...\n";
        PCLevelConverter::convert(filename, uniqueID, outdir);
        std::cout << "\nLevel converted successfully.\n"
                  << "NOTE: Conversion is not perfect nor complete at this stage.\n"
                  << "Press Enter to continue...\n";
    } catch (Jazz2FormatParseException e) {
        std::cout << "ERROR: " << e.friendlyText().toStdString() << "\n";
        getchar();
        return EXIT_FAILURE;
    } catch (...) {
        std::cout << "Unknown error. Aborting conversion...\n";
        getchar();
        return EXIT_FAILURE;
    }

    getchar();
    return EXIT_SUCCESS;
}