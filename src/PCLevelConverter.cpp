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
#include "EventIDConverter.h"

#include "Jazz2Level.h"
#include "Jazz2FormatParseException.h"

bool convertParams(quint8 event_type, quint16& newType, quint32 old_params, QList< quint16 >& result) {
    // 8 parameter slots per event
    for (unsigned i = 0; i < 8; ++i) {
        result << 0;
    }
    // Converts parameters to Project Carrot format, removing unused ones if necessary and adding new defaults when applicable
    switch(event_type) {
        case JJ2_WARP_ORIGIN:
            {
                quint16 coins = (old_params >> 8) % 256;

                if (coins > 0) {
                    newType = (quint16)PC_WARP_COIN_BONUS;
                    result[3] = coins;
                }
            }

            result[0] =  old_params        % 256;  // Warp ID
            result[1] = (old_params >> 16) % 2;    // Set Lap
            result[2] = (old_params >> 17) % 2;    // Show Anim
            return true;
        case JJ2_WARP_TARGET:
            result[0] =  old_params        % 256;  // Warp ID
            return true;
        case JJ2_TRIGGER_AREA:
            result[0] =  old_params        % 32;   // Trigger ID
            return true;
        case JJ2_TRIGGER_CRATE:
            result[0] =  old_params        % 32;   // Trigger ID
            return true;
        case JJ2_LIGHT_SET:
            result[0] =  old_params        % 128;  // Intensity
            result[1] = (old_params >> 7)  % 16;   // Red
            result[2] = (old_params >> 11) % 16;   // Green
            result[3] = (old_params >> 15) % 16;   // Blue
            result[4] = (old_params >> 16) % 2;    // Flicker
            return true;
        case JJ2_PUSHABLE_BOX:
            result[0] = 1; // Solid Object, type 1
            return true;
        case JJ2_PUSHABLE_ROCK:
            result[0] = 0; // Solid Object, type 0
            return true;
        case JJ2_BRIDGE:
            result[0] =  old_params        % 16 * 2;  // Width
            result[1] = (old_params >> 4)  % 8;   // Type
            result[2] = (old_params >> 7)  % 16;  // Toughness
            return true;
        case JJ2_SCENERY_DESTRUCT:
            if ((old_params >> 10) % 32 > 0) {
                newType = (quint16)PC_SCENERY_DESTRUCT_SPD;
                result[0] = (old_params >> 10) % 32; // Weapon
            } else {
                result[0] = (old_params >> 15) % 16; // Weapon
            }
            return true;
        case JJ2_SPRING_RED:
        case JJ2_SPRING_GREEN:
        case JJ2_SPRING_BLUE:
            result[0] =  old_params        % 2 * 2;  // Orientation
            result[1] = (old_params >> 1)  % 2;  // Keep Xspeed
            result[2] = (old_params >> 2)  % 2;  // Keey Yspeed
            result[3] = (old_params >> 3)  % 16; // Delay
            return true;
        case JJ2_SPRING_RED_HOR:
        case JJ2_SPRING_GREEN_HOR:
        case JJ2_SPRING_BLUE_HOR:
            result[0] = 5;  // Orientation
            result[1] = (old_params >> 1)  % 2;  // Keep Xspeed
            result[2] = (old_params >> 2)  % 2;  // Keey Yspeed
            result[3] = (old_params >> 3)  % 16; // Delay
            return true;
        case JJ2_PLATFORM_BOLL:
        case JJ2_PLATFORM_FRUIT:
        case JJ2_PLATFORM_GRASS:
        case JJ2_PLATFORM_PINK:
        case JJ2_PLATFORM_SONIC:
        case JJ2_PLATFORM_SPIKE:
            result[0] = (event_type - JJ2_PLATFORM_FRUIT + 1);  // Type
            result[1] =  old_params        % 4;  // Sync

            {
                qint16 speed = (old_params >> 2) % 64;
                if (speed > 31) {
                    speed = -32 + (speed - 32);
                }

                result[2] = (quint16)speed; // Speed
            }

            result[3] = (old_params >> 8)  % 16; // Length
            result[4] = (old_params >> 12) % 2;  // Swing
            return true;
        case JJ2_MODIFIER_TUBE:
            {
                qint16 speed = (old_params) % 64;
                if (speed > 31) {
                    speed = -32 + (speed - 32);
                }
                result[0] = (quint16)speed; // X Speed

                speed = (old_params >> 7) % 64;
                if (speed > 31) {
                    speed = -32 + (speed - 32);
                }
                result[1] = (quint16)speed; // Y Speed
            }

            result[2] = (old_params >> 17) % 8;   // Wait Time
            result[7] = (old_params >> 14) % 8;   // Trig Sample (unknown)
            return true;
        case JJ2_SCENERY_COLLAPSE:
            result[0] = (old_params      ) % 1024; // Wait Time
            result[1] = (old_params >> 10) % 32;   // FPS
            return true;
        case JJ2_LIGHT_PULSE:
            result[0] = 0;                       // Alpha
            result[1] = (old_params)      % 256; // Speed
            result[2] = (old_params >> 8) % 16;  // Sync
            return true;
        case JJ2_LIGHT_STEADY:
            result[0] = 0; // Alpha
            return true;
        case JJ2_LIGHT_DIM:
            result[0] = 127; // Alpha
            return true;
        case JJ2_AREA_TEXT:
            result[0] = (old_params      ) % 256;  // Text
            result[1] = (old_params >> 8 ) % 2;    // Vanish
            return true;
        default:
            return false;
    }
}

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
            quint16 type = layer.tiles.at(y).at(x);
            quint8 flags = event_table.at(idx).illuminate +
                          (event_table.at(idx).difficulty * 2);
            QList< quint16 > params;
            convertParams(event_table.at(idx).event_type, type, event_table.at(idx).params, params);
            outstr << type << flags;
            for (int i = 0; i < 8; ++i) {
                outstr << params.at(i);
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
            new_row.push_back(EventIDConverter::convert(level->events.at(y * level->layers.at(3).width + x).event_type));
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