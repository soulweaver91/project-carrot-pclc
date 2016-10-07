// J2L to native Project Carrot level file converter
// Written in 2013 by Soulweaver

#define CONVERTERVERSION "0.5.13"
#define LAYERFORMATVERSION 5
#define EVENTSETVERSION 11

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QSettings>
#include <iostream>
#include "PCLevelConverter.h"
#include "EventConverter.h"

#include "Jazz2Level.h"
#include "Jazz2FormatParseException.h"

void writeLayout(const QString& target, const Jazz2Layer& layer,
                 const QString& file_descriptive, uint flipbyte, uint last_tile = 0xFFFFFFFF) {
    QFile outfile(target);
    if (!(outfile.open(QIODevice::WriteOnly))) {
        std::cerr << "ERROR: Cannot seem to be able to write to the sprite layer file!\n";
        throw 0;
    }
    QByteArray* out_bytes = new QByteArray();
    QDataStream outstr(out_bytes, QIODevice::WriteOnly);
    std::cout << "Writing " << file_descriptive.toLatin1().data() << " layout to " << outfile.fileName().toLatin1().data() << ".\n";
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            quint16 type = layer.tiles.at(y).at(x);
            bool flip_x = ((type & flipbyte) > 0);
            bool flip_y = false;

            if (flip_x) { 
                type -= flipbyte;
            }
            bool anim = (last_tile <= type);
            if (anim) {
                type -= last_tile;
            }
            quint8 tile_flags = (flip_x ? 0x01 : 0) + (flip_y ? 0x02 : 0) + (anim ? 0x04 : 0);
            outstr << type;
            outstr << tile_flags;
        }
        outstr << (quint16)(0xFFFF);
    }
    QByteArray compressed = qCompress(*out_bytes);
    delete out_bytes;
    outfile.write(compressed);
    outfile.close();
}

void writeEventLayout(const QString& target, const Jazz2Layer& layer,
                      const QVector<Jazz2TileEvent>& event_table) {
    QFile outfile(target);
    if (!(outfile.open(QIODevice::WriteOnly))) {
        std::cerr << "ERROR: Cannot seem to be able to write to the event layer file!\n";
        throw 0;
    }
    QByteArray* out_bytes = new QByteArray();
    QDataStream outstr(out_bytes, QIODevice::WriteOnly);
    std::cout << "Writing event layer layout to " << outfile.fileName().toLatin1().data() << ".\n";
    for (int y = 0; y < layer.height; ++y) {
        for (int x = 0; x < layer.width; ++x) {
            int idx = y * layer.width + x;
            quint8 flags = event_table.at(idx).illuminate +
                          (event_table.at(idx).difficulty * 2);
            auto converted = EventConverter::convert(event_table.at(idx).event_type, event_table.at(idx).params);
            quint16 type = converted.event;
            outstr << type << flags;
            for (int i = 0; i < 8; ++i) {
                outstr << converted.parameters.at(i);
            }
        }
        outstr << (quint16)(0xFFFF);
    }
    QByteArray compressed = qCompress(*out_bytes);
    delete out_bytes;
    outfile.write(compressed);
    outfile.close();
}

void PCLevelConverter::convert(const QString& filename, const QString& id, const QDir& outputDir) {
    Jazz2Level* level = Jazz2Level::fromFile(filename);
    level->printData(std::cout);

    std::cout << "\nWriting new format files...\n";

    QSettings inifile(outputDir.absolutePath() + "/config.ini", QSettings::IniFormat);
    if (!inifile.isWritable()) {
        std::cerr << "ERROR: Cannot seem to be able to write to the output folder!\n";
        throw 0;
    } else {
        std::cout << "Writing common configuration to " << inifile.fileName().toLatin1().data() << ".\n";
    }

    inifile.beginGroup("Version");
    inifile.setValue("WritingApp", "PCLC-" + QString(CONVERTERVERSION));
    inifile.setValue("LayerFormat", LAYERFORMATVERSION);
    inifile.setValue("EventSet", EVENTSETVERSION);
    inifile.endGroup();

    inifile.beginGroup("Level");
    inifile.setValue("LevelToken", id);
    inifile.setValue("FormalName", level->name);
    inifile.setValue("Next", level->nextLevel);
    if (level->tileset.endsWith(".j2t", Qt::CaseInsensitive)) {
        level->tileset.chop(4);
        std::cout << "Found a .j2t extension in the tileset name, removed it automatically.\n";
    }
    inifile.setValue("Tileset", level->tileset);
    if (!((level->music.endsWith(".it")) || (level->music.endsWith(".s3m")) || (level->music.endsWith(".mod")))) {
        level->music.append(".it");
        std::cout << "Music name didn't have an extension .it, .s3m or .mod, added .it automatically.\n";
    }
    inifile.setValue("MusicDefault", level->music);
    inifile.setValue("LightInit", qRound(level->lightingStart * 100.0 / 64.0));
    inifile.endGroup();


    inifile.beginGroup("TextEvent");
    for (unsigned i = 0; i < 16; ++i) {
        inifile.setValue("Str" + QString::number(i), level->textEventStrings.at(i));
    }
    inifile.endGroup();

    inifile.beginGroup("sky.layer");
    inifile.setValue("XSpeed", level->layers[7].speed_x);
    inifile.setValue("YSpeed", level->layers[7].speed_y);
    inifile.setValue("XAutoSpeed", level->layers[7].auto_x);
    inifile.setValue("YAutoSpeed", level->layers[7].auto_y);
    inifile.setValue("XRepeat", (level->layers[7].flags % 2 > 0));
    inifile.setValue("YRepeat", ((level->layers[7].flags >> 1) % 2 > 0));
    inifile.setValue("J2L.Depth", level->layers[7].depth);
    inifile.setValue("J2L.DetailLevel", level->layers[7].detail_level);
    inifile.setValue("TexturedMode", level->layers[7].tex_type);
    inifile.setValue("TexturedModeColor", '#' + QString::number(level->layers[7].tex_param[0], 16).rightJustified(2, '0')
                     + QString::number(level->layers[7].tex_param[1], 16).rightJustified(2, '0')
                     + QString::number(level->layers[7].tex_param[2], 16).rightJustified(2, '0'));
    inifile.setValue("InherentOffset", ((level->layers[7].flags >> 2) % 2 > 0)); // "Limit Visible Region"
    inifile.setValue("TexturedModeEnabled", ((level->layers[7].flags >> 3) % 2 > 0));
    inifile.setValue("ParallaxStarsEnabled", ((level->layers[7].flags >> 4) % 2 > 0));
    inifile.endGroup();

    // Write foreground layer settings
    for (unsigned i = 0; i < 3; ++i) {
        if (level->layers.at(i).used) {
            inifile.beginGroup(QString::number(2 - i) + ".fg.layer");
            inifile.setValue("XSpeed", level->layers[i].speed_x);
            inifile.setValue("YSpeed", level->layers[i].speed_y);
            inifile.setValue("XAutoSpeed", level->layers[i].auto_x);
            inifile.setValue("YAutoSpeed", level->layers[i].auto_y);
            inifile.setValue("XRepeat", (level->layers[i].flags % 2 > 0));
            inifile.setValue("YRepeat", ((level->layers[i].flags >> 1) % 2 > 0));
            inifile.setValue("J2L.Depth", level->layers[i].depth);
            inifile.setValue("J2L.DetailLevel", level->layers[i].detail_level);
            //inifile.setValue("TexturedModeType",level->layers[i].tex_type);
            //inifile.setValue("TexturedModeColor",'#' + QString::number(level->layers[i].tex_param[0],16).rightJustified(2,'0')
            //                                         + QString::number(level->layers[i].tex_param[1],16).rightJustified(2,'0')
            //                                         + QString::number(level->layers[i].tex_param[2],16).rightJustified(2,'0'));
            inifile.setValue("InherentOffset", ((level->layers[i].flags >> 2) % 2 > 0)); // "Limit Visible Region"
            //inifile.setValue("TexturedModeEnabled", ((level->layers[i].flags >> 3) % 2 > 0));
            //inifile.setValue("ParallaxStarsEnabled", ((level->layers[i].flags >> 4) % 2 > 0));
            inifile.endGroup();
        }
    }

    // Write background layer settings
    for (unsigned i = 4; i < 7; ++i) {
        if (level->layers.at(i).used) {
            inifile.beginGroup(QString::number(6 - i) + ".bg.layer");
            inifile.setValue("XSpeed", level->layers[i].speed_x);
            inifile.setValue("YSpeed", level->layers[i].speed_y);
            inifile.setValue("XAutoSpeed", level->layers[i].auto_x);
            inifile.setValue("YAutoSpeed", level->layers[i].auto_y);
            inifile.setValue("XRepeat", (level->layers[i].flags % 2 > 0));
            inifile.setValue("YRepeat", ((level->layers[i].flags >> 1) % 2 > 0));
            inifile.setValue("J2L.Depth", level->layers[i].depth);
            inifile.setValue("J2L.DetailLevel", level->layers[i].detail_level);
            //inifile.setValue("TexturedModeType",level->layers[i].tex_type);
            //inifile.setValue("TexturedModeColor",'#' + QString::number(level->layers[i].tex_param[0],16).rightJustified(2,'0')
            //                                         + QString::number(level->layers[i].tex_param[1],16).rightJustified(2,'0')
            //                                         + QString::number(level->layers[i].tex_param[2],16).rightJustified(2,'0'));
            inifile.setValue("InherentOffset", ((level->layers[i].flags >> 2) % 2 > 0)); // "Limit Visible Region"
            //inifile.setValue("TexturedModeEnabled", ((level->layers[i].flags >> 3) % 2 > 0));
            //inifile.setValue("ParallaxStarsEnabled", ((level->layers[i].flags >> 4) % 2 > 0));
            inifile.endGroup();
        }
    }
    inifile.sync();

    const uint maxTiles = (level->version <= 514) ? 1024 : 4096;
    const uint maxAnimTiles = (level->version <= 514) ? 128 : 256;
    qint16 staticTilesCount = maxTiles - level->animCount;
    writeLayout(outputDir.absolutePath() + "/spr.layer", level->layers[3], "sprite layer (layer 4)", maxTiles, staticTilesCount);
    writeLayout(outputDir.absolutePath() + "/sky.layer", level->layers[7], "sky layer (layer 8)", maxTiles, staticTilesCount);
    for (unsigned i = 0; i < 3; ++i) {
        if (level->layers.at(i).used) {
            writeLayout(outputDir.absolutePath() + "/" + QString::number(2 - i) + ".fg.layer", level->layers[i], "foreground layer " + QString::number(3 - i), maxTiles, staticTilesCount);
        } else {

        }
    }
    for (unsigned i = 4; i < 7; ++i) {
        if (level->layers.at(i).used) {
            writeLayout(outputDir.absolutePath() + "/" + QString::number(6 - i) + ".bg.layer", level->layers[i], "background layer " + QString::number(6 - i), maxTiles, staticTilesCount);
        } else {

        }
    }

    // rewrite over the sprite layer with the event map
    level->layers[3].tiles.clear();
    for (unsigned y = 0; y < level->layers.at(3).height; ++y) {
        QList< quint16 > new_row;
        for (unsigned x = 0; x < level->layers.at(3).width; ++x) {
            new_row.push_back(level->events.at(y * level->layers.at(3).width + x).event_type);
        }
        level->layers[3].tiles.push_back(new_row);
    }
    writeEventLayout(outputDir.absolutePath() + "/event.layer", level->layers[3], level->events);

    // Parse animated tiles
    QFile outfile(outputDir.absolutePath() + "/animtiles.dat");
    if (!(outfile.open(QIODevice::WriteOnly))) {
        std::cerr << "ERROR: Cannot seem to be able to write to the animated tiles file!\n";
        throw 0;
    }
    QByteArray* out_bytes = new QByteArray();
    QDataStream outstr(out_bytes, QIODevice::WriteOnly);
    std::cout << "Writing animated tiles to " << (outputDir.absolutePath() + "/animtiles.dat").toStdString() << ".\n";
    for (Jazz2AniTile tile : level->animatedTiles) {
        for (int i = 0; i < 64; ++i) {
            if (i >= tile.frame_cnt) {
                outstr << (quint16)(0xFFFF);
            } else {
                outstr << tile.frames[i];
            }
        }
        quint8 rev = tile.is_reverse ? 1 : 0;
        outstr << tile.speed << tile.delay << tile.delay_jitter << rev << tile.reverse_delay;
    }
    QByteArray compressed = qCompress(*out_bytes);
    delete out_bytes;
    outfile.write(compressed);
    outfile.close();
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
                  << "NOTE: Conversion is not perfect nor complete at this stage. Unconverted\n"
                  << "aspects include textured background and some global settings.\n"
                  << "Not all events are converted yet either.\n"
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