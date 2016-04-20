// J2L to native Project Carrot level file converter
// Written in 2013 by Soulweaver

#define CONVERTERVERSION "0.5.4"
#define LAYERFORMATVERSION 5
#define EVENTSETVERSION 4

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QString>
#include <QSettings>
#include <iostream>
#include "EventIDConverter.h"

struct J2Layer {
    quint32 flags;              // covered x and y tiling
    quint8 type;                // ???
    bool used;                  // covered as far as it seems to be used
    quint32 width;              // covered
    quint32 width_internal;     // covered?
    quint32 height;             // covered
    qint32 depth;               // ??? covered by PCLC, not covered by PC itself
    quint8 detail_level;        // ???
    double wave_x;              // not functional?
    double wave_y;              // not functional?
    double speed_x;             // covered
    double speed_y;             // covered
    double auto_x;              // covered
    double auto_y;              // covered
    quint8 tex_type;            // covered by PCLC, not covered by PC itself
    quint8 tex_param[3];        // covered by PCLC, not covered by PC itself
};

struct J2TileEvent {
    quint8 event_type;          // defined events covered
    quint8 difficulty;          // not covered
    bool illuminate;            // not covered
    quint32 params;             // defined events covered
};

struct J2TileProperty {
    J2TileEvent event;          // covered
    quint8 flipped;             // covered
    quint8 type;                // not covered
};

struct J2AniTile {
    quint16 delay;
    quint16 delay_jitter;
    quint16 reverse_delay;
    bool is_reverse;
    quint8 speed;
    quint8 frame_cnt;
    quint16 frames[64];
};

struct J2DictionaryEntry {
    quint16 tiles[4];
};

quint32 uintFromArray(const QByteArray& array) {
    if (array.size() != 4) {
        return 0;
    }
    QDataStream a(array);
    quint32 res = 0;
    quint8 byte;
    while (!a.atEnd()) {
        res *= 256;
        a >> byte;
        res += byte;
    }
    return res;
}

double speedFloatFromArray(const QByteArray& array) {
    if (array.size() != 4) {
        return 0;
    }
    QDataStream a(array);
    quint32 res = 0;
    quint8 byte;
    while (!a.atEnd()) {
        res *= 256.0;
        a >> byte;
        res += byte;
    }
    double fin = (static_cast<qint32>(res) * 1.0) / 65536.0;
    return fin;
}

quint16 ushortFromArray(const QByteArray& array) {
    if (array.size() != 2) {
        return 0;
    }
    QDataStream a(array);
    quint32 res = 0;
    quint8 byte;
    while (!a.atEnd()) {
        res *= 256;
        a >> byte;
        res += byte;
    }
    return res;
}

QByteArray BEfromLE(QByteArray le) {
    QDataStream a(le);
    QByteArray be;
    a.setByteOrder(QDataStream::BigEndian);
    qint8 byte;
    while (!a.atEnd()) {
        a >> byte;
        be.prepend(byte);
    }
    return be;
}

bool convertParams(quint8 event_type, quint32 old_params, QList< quint16 >& result) {
    // 8 parameter slots per event
    for (unsigned i = 0; i < 8; ++i) {
        result << 0;
    }
    // Converts parameters to Project Carrot format, removing unused ones if necessary and adding new defaults when applicable
    switch(event_type) {
        case JJ2_WARP_ORIGIN:
            result[0] =  old_params        % 256;  // Warp ID
            result[1] = (old_params >> 8)  % 256;  // Coins
            result[2] = (old_params >> 16) % 2;    // Set Lap
            result[3] = (old_params >> 17) % 2;    // Show Anim
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
            result[0] = (old_params >> 15) % 16; // Weapon
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
        default:
            return false;
    }
}

void writeLayout(const QString& target, const unsigned idx, const QList< J2Layer >& layers,
                 const QList< QList < quint16 >> layouts[8], const QString& file_descriptive, uint flipbyte, uint last_tile = 0xFFFFFFFF) {
    QFile outfile(target);
    if (!(outfile.open(QIODevice::WriteOnly))) {
        std::cerr << "ERROR: Cannot seem to be able to write to the sprite layer file!\n";
        throw 0;
    }
    QByteArray* out_bytes = new QByteArray();
    QDataStream outstr(out_bytes, QIODevice::WriteOnly);
    std::cout << "Writing " << file_descriptive.toLatin1().data() << " layout to " << outfile.fileName().toLatin1().data() << ".\n";
    for (int y = 0; y < layers.at(idx).height; ++y) {
        for (int x = 0; x < layers.at(idx).width; ++x) {
            quint16 type = layouts[idx].at(y).at(x);
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

void writeEventLayout(const QString& target, const QList< J2Layer >& layers, const QList< QList < quint16 >> layouts, const QList< J2TileEvent >& event_table) {
    QFile outfile(target);
    if (!(outfile.open(QIODevice::WriteOnly))) {
        std::cerr << "ERROR: Cannot seem to be able to write to the event layer file!\n";
        throw 0;
    }
    QByteArray* out_bytes = new QByteArray();
    QDataStream outstr(out_bytes, QIODevice::WriteOnly);
    std::cout << "Writing event layer layout to " << outfile.fileName().toLatin1().data() << ".\n";
    for (int y = 0; y < layers.at(3).height; ++y) {
        for (int x = 0; x < layers.at(3).width; ++x) {
            int idx = y * layers.at(3).width + x;
            quint16 type = layouts.at(y).at(x);
            quint8 flags = event_table.at(idx).illuminate +
                          (event_table.at(idx).difficulty * 2);
            QList< quint16 > params;
            convertParams(event_table.at(idx).event_type,event_table.at(idx).params, params);
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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "PCLC version " << CONVERTERVERSION << "\n"
                  << "Usage: pclc name inputfile\n\n"
                  << "Converts a Jazz Jackrabbit 2 level file (.j2l) to the native, multiple file\n"
                  << "based format recognized by Project Carrot.\n\n"
                  << "Parameters:\n"
                  << "  name         Required. The unique identifying name of this level. Use only\n"
                  << "               Latin letters, numbers and underscores. For the purposes of\n"
                  << "               being cross platform compliant, identifiers are case\n"
                  << "               insensitive.\n"
                  << "  inputfile    Required. Complete or relative path to a J2L format file to be\n"
                  << "               converted.\n";
        return EXIT_FAILURE;
    }

    QString unique_id(argv[1]);
    QString filename;
    for (int i = 2; i < argc; ++i) {
        filename += argv[i];
    }

    QFile fh(filename);
    if (!(fh.exists())) {
        std::cerr << "ERROR: The input file \"" << filename.toLocal8Bit().data() << "\" cannot be found!\n";
        return EXIT_FAILURE;
    }
    if (!(fh.open(QIODevice::ReadOnly))) {
        std::cerr << "ERROR: Cannot open input file \"" << filename.toLocal8Bit().data() << "\"!\n";
        return EXIT_FAILURE;
    }
    std::cout << "Opening input file \"" << filename.toLocal8Bit().data() << "\"...\n";

    qint32 filesize = fh.size();

    try {
        // Skip the copyright info at the beginning of the file
        QByteArray r = fh.read(180);

        // Read the next four bytes; should spell out "LEVL"
        qint32 id = uintFromArray(fh.read(4));
        if (id != 0x4C45564C) {
            std::cerr << "ERROR: This doesn't look like a J2L file! (Invalid header)\n";
            throw 0;
        }
        quint32 hash = uintFromArray(fh.read(4));
        if (hash != 0x00BABE00) {
            // This seems to be a password protected file
            if (hash != 0x00000000) {
                std::cout << "ERROR: This file seems to be password protected. To prove you are authorized\n"
                            << "to convert this level to a Project Carrot format (which does not support\n"
                            << "protected levels), please set the level password to an empty string before\n"
                            << "converting.\n";
                throw 0;
            } else {
                std::cout << "Notice: Password protected file with empty password (or other password\n"
                            << "resolving to empty).\n";
            }
        }

        // Read and display the level title
        QString levelname(fh.read(32));
        std::cout << "Level name: " << levelname.toLatin1().data() << "\n";

        // Check version and report the JCS version
        qint16 version(ushortFromArray(fh.read(2)));
        std::cout << "JCS version: " << (version % 256) << ".0" << (version / 256) << " (" << (version <= 514 ? "base game" : "TSF/CC") << ")\n";
            
        // Choose the tiles mode according to the said version
        const uint max_tiles = (version <= 514) ? 1024 : 4096;
        const uint max_anim_tiles = (version <= 514) ? 128 : 256;
            
        quint32 filesize_listed = uintFromArray(BEfromLE(fh.read(4)));

        if (filesize != filesize_listed) {
            std::cout << "WARNING: File size doesn't match the one saved inside the file!\n"
                        << "(expected " << filesize_listed << "bytes, length was " << filesize << ")\n";
        }
            
        // Get the CRC; would check here if it matches if we knew what variant it is AND what it applies to
        // Test file across all CRC32 variants + Adler had no matches to the value obtained from the file
        // so either the variant is something else or the CRC is not applied to the whole file but on a part
        quint32 crc_listed = uintFromArray(fh.read(4));
            
        // Read the lengths
        quint32 infoblock_packed_size        = uintFromArray(BEfromLE(fh.read(4)));
        QByteArray infoblock_unpacked_size   = BEfromLE(fh.read(4));
        quint32 eventblock_packed_size       = uintFromArray(BEfromLE(fh.read(4)));
        QByteArray eventblock_unpacked_size  = BEfromLE(fh.read(4));
        quint32 dictblock_packed_size        = uintFromArray(BEfromLE(fh.read(4)));
        QByteArray dictblock_unpacked_size   = BEfromLE(fh.read(4));
        quint32 layoutblock_packed_size      = uintFromArray(BEfromLE(fh.read(4)));
        QByteArray layoutblock_unpacked_size = BEfromLE(fh.read(4));

        // Let's unpack all the blocks so we can let go of the original file
        QByteArray infoblock   = qUncompress(  infoblock_unpacked_size + fh.read(  infoblock_packed_size));
        QByteArray eventblock  = qUncompress( eventblock_unpacked_size + fh.read( eventblock_packed_size));
        QByteArray dictblock   = qUncompress(  dictblock_unpacked_size + fh.read(  dictblock_packed_size));
        QByteArray layoutblock = qUncompress(layoutblock_unpacked_size + fh.read(layoutblock_packed_size));
            
        if (infoblock.size() == 0) {
            std::cerr << "ERROR: Unpacking the information block failed!\n";
            throw 0;
        }
        if (eventblock.size() == 0) {
            std::cerr << "ERROR: Unpacking the event block failed!\n";
            throw 0;
        }
        if (dictblock.size() == 0) {
            std::cerr << "ERROR: Unpacking the dictionary block failed!\n";
            throw 0;
        }
        if (layoutblock.size() == 0) {
            std::cerr << "ERROR: Unpacking the layer layout block failed!\n";
            throw 0;
        }

        // we can now let go of the file
        fh.close();

        // Parsing the info block
        std::cout << "\nParsing the information block...\n";

        // First 9 bytes are JCS coordinates on last save, we don't need those
        infoblock.remove(0,9);

        char ambient_min = infoblock.at(0);
        char ambient_start = infoblock.at(1);
        infoblock.remove(0,2);

        qint16 anim_num = ushortFromArray(infoblock.left(2));
            
        char splitscreen = infoblock.at(2);
        char is_multiplayer = infoblock.at(3);
        infoblock.remove(0,4);
            
        // this should be the same as size of infoblock in the start?
        qint16 header_size = ushortFromArray(infoblock.left(4));
        infoblock.remove(0,4);
            
        QString levelname_again = infoblock.mid(0,32);
        QString tileset = infoblock.mid(32,32);
        QString link_bonus = infoblock.mid(64,32);
        QString link_next = infoblock.mid(96,32);
        QString link_secret = infoblock.mid(128,32);
        QString music = infoblock.mid(160,32);
        std::cout << "  Next level: " << link_next.toLatin1().data() << "\n"
                    << " Bonus level: " << link_bonus.toLatin1().data() << "\n"
                    << "Secret level: " << link_secret.toLatin1().data() << "\n"
                    << "  Music file: " << music.toLatin1().data() << "\n"
                    << "     Tileset: " << tileset.toLatin1().data() << "\n";
        infoblock.remove(0,192);
            
        std::cout << "Defined text event strings:\n";
        QList< QString > text_events;
        for (unsigned i = 0; i < 16; ++i) {
            QString newtext = infoblock.left(512);
            if (newtext != "") {
                std::cout << "  Text #" << i << ": " << newtext.toLatin1().data() << "\n";
            }
            text_events << newtext;
            infoblock.remove(0,512);
        }
            
        // read a lot of layer related stuff
        QList< J2Layer > layers;
        for (unsigned i = 0; i < 8; ++i) {
            J2Layer a = {0,0,false,0,0,0,0,0,0,0,0,0,0,0,0,0};
            layers << a;
        }

        for (unsigned i = 0; i < 8; ++i) {
            layers[i].flags = uintFromArray(BEfromLE(infoblock.left(4)));
            std::cout << "Layer " << i + 1 << " flags raw: 0x" << QString(infoblock.left(4).toHex()).rightJustified(8,'0').toLatin1().data()
                        << ", after: 0x" << QString::number(layers[i].flags,16).rightJustified(8,'0').toLatin1().data() << "\n";
            infoblock.remove(0,4);
            QString names[32] = {"Tile X", "Tile Y", "Limit Visible", "Textured mode on", "Stars background"};
            for (int j = 0; j < 32; ++j) {
                std::cout << "Layer " << i + 1 << " flag " << j << " (" << names[j].toLatin1().data() << "): " << ((((layers[i].flags >> j) % 2) > 0) ? "true" : "false") << "\n";
            }
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].type = infoblock.at(i);
        }
        infoblock.remove(0,8);
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].used = (infoblock.at(i) != 0x00);
        }
        infoblock.remove(0,8);
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].width = uintFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        // This is related to how data is presented in the file; the above is a WYSIWYG version, solely shown on the UI
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].width_internal = uintFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }

        for (unsigned i = 0; i < 8; ++i) {
            layers[i].height = uintFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].depth = uintFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].detail_level = infoblock.at(i);
        }
        infoblock.remove(0,8);
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].wave_x = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].wave_y = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].speed_x = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].speed_y = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].auto_x = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].auto_y = speedFloatFromArray(BEfromLE(infoblock.left(4)));
            infoblock.remove(0,4);
        }
            
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].tex_type = infoblock.at(i);
        }
        infoblock.remove(0,8);
        for (unsigned i = 0; i < 8; ++i) {
            layers[i].tex_param[0] = infoblock.at(i*3);
            layers[i].tex_param[0] = infoblock.at(i*3+1);
            layers[i].tex_param[0] = infoblock.at(i*3+2);
        }
        infoblock.remove(0,24);

        // Let's report some basics on the found layer data
        for (unsigned i = 0; i < 8; ++i) {
            std::cout << "Layer " << (i + 1) << ": " << layers[i].width << " x " << layers[i].height << " tiles, speed "
                        << layers[i].speed_x << "X/" << layers[i].speed_y << "Y, auto speed " << layers[i].auto_x << "X/"
                        << layers[i].auto_y << "Y\n";
        }

        // should be max_tiles - number of animated tiles
        qint16 static_tiles_cnt = ushortFromArray(BEfromLE(infoblock.left(2)));
        infoblock.remove(0,2);

        QList< J2TileProperty > static_tiles;
        for (unsigned i = 0; i < max_tiles; ++i) {
            J2TileProperty t_props;
            qint32 e_type = uintFromArray(BEfromLE(infoblock.left(4)));
            t_props.event.event_type = (e_type & 0xFF000000) >> 24;
            t_props.event.difficulty = (e_type & 0x00C00000) >> 22;
            t_props.event.illuminate = ((e_type & 0x00200000) >> 21 == 1);
            t_props.event.params = (e_type & 0x000FFFFF);
            infoblock.remove(0,4);
            static_tiles << t_props;
        }
        for (unsigned i = 0; i < max_tiles; ++i) {
            static_tiles[i].flipped = (infoblock.at(i) > 0);
        }
        infoblock.remove(0,max_tiles);
        for (unsigned i = 0; i < max_tiles; ++i) {
            static_tiles[i].type = infoblock.at(i);
        }
        infoblock.remove(0,max_tiles);
            
        // the unused XMask field
        infoblock.remove(0,max_tiles);

        QList< J2AniTile > anim_tiles;
        for (unsigned i = 0; i < max_anim_tiles; ++i) {
            J2AniTile new_ani;
            new_ani.delay = ushortFromArray(BEfromLE(infoblock.mid(0,2)));
            new_ani.delay_jitter = ushortFromArray(BEfromLE(infoblock.mid(2,2)));
            new_ani.reverse_delay = ushortFromArray(BEfromLE(infoblock.mid(4,2)));
            new_ani.is_reverse = (infoblock.at(6) > 0);
            new_ani.speed = infoblock.at(7); // 0-70
            new_ani.frame_cnt = infoblock.at(8);
            infoblock.remove(0,9);
            for (unsigned j = 0; j < 64; ++j) {
                new_ani.frames[j] = ushortFromArray(BEfromLE(infoblock.mid(0,2)));
                infoblock.remove(0,2);
            }
            anim_tiles << new_ani;
        }
            
        // Parsing the info block
        std::cout << "Animated tiles parsed. In total, found " << (max_tiles - static_tiles_cnt) << ".\n";

        // The rest is just buffer data, free memory associated to that
        infoblock = "";


        // Parsing the event block
        std::cout << "\nParsing the event block...\n";

        QList< J2TileEvent > layer_events;
        if (eventblock.size() != (layers.at(3).width * layers.at(3).height * 4)) {
            std::cout << "WARNING: Unexpected event block length!\n"
                        << "(expected " << (layers.at(3).width * layers.at(3).height * 4) << "bytes, length was " << eventblock.size() << ")\n";
        }

        unsigned count = 0;
        unsigned unknownCount = 0;
        for (unsigned i = 0; i < layers.at(3).height; ++i) {
            for (unsigned j = 0; j < layers.at(3).width; ++j) {
                J2TileEvent t_event;
                qint32 e_type = uintFromArray(eventblock.left(4));
                t_event.event_type = (e_type & 0xFF000000) >> 24;
                t_event.difficulty = ((e_type & 0x00030000) >> 16) % 4;
                t_event.illuminate = ((e_type & 0x00040000) >> 18 == 1);
                t_event.params = ((e_type & 0x00F00000) >> 20)
                                | ((e_type & 0x0000FF00) >> 4)
                                | ((e_type & 0x000000FF) << 12);
                eventblock.remove(0,4);
                layer_events << t_event;
                if (t_event.event_type != 0) {
                    count++;
                    QString diff[4] = {"all difficulties", "Easy difficulty", "Hard difficulty", "multiplayer levels"};
                    std::cout << "At (" << j << ", " << i << "), found event " << static_cast<uint>(t_event.event_type)
                                << " (" << EventIDConverter::eventName(t_event.event_type).toLatin1().data()
                                << ") for " << diff[t_event.difficulty].toLatin1().data()
                                << (t_event.illuminate ? " (illuminated)" : "")
                                << " with parameter combination " << t_event.params << ".\n";
                    if (EventIDConverter::convert(static_cast<uint>(t_event.event_type)) == PC_EMPTY) {
                        std::cout << "Warning: NOT added to event map, no assignment in Project Carrot event mapping found!\n";
                        unknownCount++;
                    }
                }
            }
        }
        std::cout << "Event layer parsed. In total, found " << count << " non-empty events.\n";
        if (unknownCount > 0) {
            std::cout << unknownCount << " events were not added due to missing mapping.\n";
        }
            
        // Parsing the dict block
        std::cout << "\nParsing the dictionary block...\n";

        qint32 block3size = (uintFromArray(dictblock_unpacked_size) / 8);
        QList< J2DictionaryEntry > dictionary;
        count = 0;
        for (unsigned i = 0; i < block3size; ++i) {
            J2DictionaryEntry d_new;
            for (unsigned j = 0; j < 4; ++j) {
                d_new.tiles[j] = ushortFromArray(BEfromLE(dictblock.mid(2*j,2)));
            }
            dictblock.remove(0,8);
            dictionary << d_new;
            count++;
        }
        std::cout << "Dictionary parsed. In total, found " << count << " entries.\n";
            
        // Parsing the layout block
        std::cout << "\nParsing the layout block...\n";

        QList< QList< quint16 > > layouts[8];
        for (unsigned i = 0; i < layers.size(); ++i) {
            if (layers.at(i).used) {
                for (unsigned y = 0; y < layers.at(i).height; ++y) {
                    QList< quint16 > new_row;
                    for (unsigned x = 0; x < layers.at(i).width_internal; x += 4) {
                        quint16 s_dict = ushortFromArray(BEfromLE(layoutblock.mid(0,2)));
                        layoutblock.remove(0,2);
                        for (unsigned j = 0; j < 4; j++) {
                            if (x + j >= layers.at(i).width) {
                                break;
                            }
                            new_row.push_back(dictionary.at(s_dict).tiles[j]);
                        }
                    }
                    layouts[i].push_back(new_row);
                }
            } else {
                for (unsigned y = 0; y < layers.at(i).height; ++y) {
                    QList< quint16 > new_row;
                    for (unsigned x = 0; x < layers.at(i).width; ++x) {
                        new_row.push_back(0);
                    }
                    layouts[i].push_back(new_row);
                }

            }
        }
        std::cout << "Layouts parsed.\n";
            
        std::cout << "\nWriting new format files...\n";

        QDir outdir(QDir::current());
        if (!outdir.mkdir(unique_id)) {
            int idx = 0;
            while (!outdir.mkdir(unique_id + "_" + QString::number(idx))) {
                idx++;
            }
            outdir.cd(unique_id + "_" + QString::number(idx));
        } else {
            outdir.cd(unique_id);
        }

        QSettings inifile(outdir.absolutePath() + "/config.ini",QSettings::IniFormat);
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
        inifile.setValue("LevelToken", unique_id);
        inifile.setValue("FormalName", levelname);
        inifile.setValue("Next", link_next);
        if (tileset.endsWith(".j2t",Qt::CaseInsensitive)) {
            tileset.chop(4);
            std::cout << "Found a .j2t extension in the tileset name, removed it automatically.\n";
        }
        inifile.setValue("Tileset", tileset);
        if (!((music.endsWith(".it")) || (music.endsWith(".s3m")) || (music.endsWith(".mod")))) {
            music.append(".it");
            std::cout << "Music name didn't have an extension .it, .s3m or .mod, added .it automatically.\n";
        }
        inifile.setValue("MusicDefault", music);
        inifile.setValue("LightInit", ambient_start);
        inifile.endGroup();

            
        inifile.beginGroup("TextEvent");
        for (unsigned i = 0; i < 16; ++i) {
            inifile.setValue("Str" + QString::number(i),text_events.at(i));
        }
        inifile.endGroup();

        inifile.beginGroup("sky.layer");
        inifile.setValue("XSpeed", layers[7].speed_x);
        inifile.setValue("YSpeed", layers[7].speed_y);
        inifile.setValue("XAutoSpeed", layers[7].auto_x);
        inifile.setValue("YAutoSpeed", layers[7].auto_y);
        inifile.setValue("XRepeat", (layers[7].flags % 2 > 0));
        inifile.setValue("YRepeat", ((layers[7].flags >> 1) % 2 > 0));
        inifile.setValue("J2L.Depth",layers[7].depth);
        inifile.setValue("J2L.DetailLevel",layers[7].detail_level);
        inifile.setValue("TexturedMode",layers[7].tex_type);
        inifile.setValue("TexturedModeColor",'#' + QString::number(layers[7].tex_param[0],16).rightJustified(2,'0')
                                                    + QString::number(layers[7].tex_param[1],16).rightJustified(2,'0')
                                                    + QString::number(layers[7].tex_param[2],16).rightJustified(2,'0'));
        inifile.setValue("InherentOffset", ((layers[7].flags >> 2) % 2 > 0)); // "Limit Visible Region"
        inifile.setValue("TexturedModeEnabled", ((layers[7].flags >> 3) % 2 > 0));
        inifile.setValue("ParallaxStarsEnabled", ((layers[7].flags >> 4) % 2 > 0));
        inifile.endGroup();
            
        // Write foreground layer settings
        for (unsigned i = 0; i < 3; ++i) {
            if (layers.at(i).used) {
                inifile.beginGroup(QString::number(2-i) + ".fg.layer");
                inifile.setValue("XSpeed", layers[i].speed_x);
                inifile.setValue("YSpeed", layers[i].speed_y);
                inifile.setValue("XAutoSpeed", layers[i].auto_x);
                inifile.setValue("YAutoSpeed", layers[i].auto_y);
                inifile.setValue("XRepeat", (layers[i].flags % 2 > 0));
                inifile.setValue("YRepeat", ((layers[i].flags >> 1) % 2 > 0));
                inifile.setValue("J2L.Depth",layers[i].depth);
                inifile.setValue("J2L.DetailLevel",layers[i].detail_level);
                //inifile.setValue("TexturedModeType",layers[i].tex_type);
                //inifile.setValue("TexturedModeColor",'#' + QString::number(layers[i].tex_param[0],16).rightJustified(2,'0')
                //                                         + QString::number(layers[i].tex_param[1],16).rightJustified(2,'0')
                //                                         + QString::number(layers[i].tex_param[2],16).rightJustified(2,'0'));
                inifile.setValue("InherentOffset", ((layers[i].flags >> 2) % 2 > 0)); // "Limit Visible Region"
                //inifile.setValue("TexturedModeEnabled", ((layers[i].flags >> 3) % 2 > 0));
                //inifile.setValue("ParallaxStarsEnabled", ((layers[i].flags >> 4) % 2 > 0));
                inifile.endGroup();
            }
        }
            
        // Write background layer settings
        for (unsigned i = 4; i < 7; ++i) {
            if (layers.at(i).used) {
                inifile.beginGroup(QString::number(6 - i) + ".bg.layer");
                inifile.setValue("XSpeed", layers[i].speed_x);
                inifile.setValue("YSpeed", layers[i].speed_y);
                inifile.setValue("XAutoSpeed", layers[i].auto_x);
                inifile.setValue("YAutoSpeed", layers[i].auto_y);
                inifile.setValue("XRepeat", (layers[i].flags % 2 > 0));
                inifile.setValue("YRepeat", ((layers[i].flags >> 1) % 2 > 0));
                inifile.setValue("J2L.Depth",layers[i].depth);
                inifile.setValue("J2L.DetailLevel",layers[i].detail_level);
                //inifile.setValue("TexturedModeType",layers[i].tex_type);
                //inifile.setValue("TexturedModeColor",'#' + QString::number(layers[i].tex_param[0],16).rightJustified(2,'0')
                //                                         + QString::number(layers[i].tex_param[1],16).rightJustified(2,'0')
                //                                         + QString::number(layers[i].tex_param[2],16).rightJustified(2,'0'));
                inifile.setValue("InherentOffset", ((layers[i].flags >> 2) % 2 > 0)); // "Limit Visible Region"
                //inifile.setValue("TexturedModeEnabled", ((layers[i].flags >> 3) % 2 > 0));
                //inifile.setValue("ParallaxStarsEnabled", ((layers[i].flags >> 4) % 2 > 0));
                inifile.endGroup();
            }
        }
        inifile.sync();
            
        writeLayout(outdir.absolutePath() + "/spr.layer", 3, layers, layouts, "sprite layer (layer 4)", max_tiles, static_tiles_cnt);
        writeLayout(outdir.absolutePath() + "/sky.layer", 7, layers, layouts, "sky layer (layer 8)", max_tiles, static_tiles_cnt);
        for (unsigned i = 0; i < 3; ++i) {
            if (layers.at(i).used) {
                writeLayout(outdir.absolutePath() + "/" + QString::number(2 - i) + ".fg.layer", i, layers, layouts, "foreground layer " + QString::number(3 - i), max_tiles, static_tiles_cnt);
            } else {

            }
        }
        for (unsigned i = 4; i < 7; ++i) {
            if (layers.at(i).used) {
                writeLayout(outdir.absolutePath() + "/" + QString::number(6 - i) + ".bg.layer", i, layers, layouts, "background layer " + QString::number(6 - i), max_tiles, static_tiles_cnt);
            } else {

            }
        }

        // rewrite over the sprite layer with the event map
        layouts[3].clear();
        for (unsigned y = 0; y < layers.at(3).height; ++y) {
            QList< quint16 > new_row;
            for (unsigned x = 0; x < layers.at(3).width; ++x) {
                new_row.push_back(EventIDConverter::convert(layer_events.at(y * layers.at(3).width + x).event_type));
            }
            layouts[3].push_back(new_row);
        }
        writeEventLayout(outdir.absolutePath() + "/event.layer", layers, layouts[3], layer_events);

        // Parse animated tiles
        QFile outfile(outdir.absolutePath() + "/animtiles.dat");
        if (!(outfile.open(QIODevice::WriteOnly))) {
            std::cerr << "ERROR: Cannot seem to be able to write to the animated tiles file!\n";
            throw 0;
        }
        QByteArray* out_bytes = new QByteArray();
        QDataStream outstr(out_bytes, QIODevice::WriteOnly);
        std::cout << "Writing animated tiles to " << (outdir.absolutePath() + "/animtiles.dat").toStdString() << ".\n";
        for (J2AniTile tile : anim_tiles) {
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
            
        std::cout << "\nLevel converted successfully.\n"
            << "NOTE: Conversion is not perfect nor complete at this stage. Unconverted\n"
            << "aspects include textured background and some global settings.\n"
            << "Not all events are converted yet either.\nPress Enter to continue...\n";
    } catch(...) {
        std::cout << "Aborting conversion...\n";
        if (fh.isOpen()) {
            fh.close();
        }
        return EXIT_FAILURE;
    }
    getchar();
    return EXIT_SUCCESS;
}
