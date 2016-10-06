#include "Jazz2Level.h"

#include "Jazz2FormatParseException.h"

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <exception>

Jazz2Level::Jazz2Level() {
}

Jazz2Level* Jazz2Level::fromFile(const QString& filename, bool strictParser) {
    QFile fh(filename);
    if (!(fh.exists())) {
        throw Jazz2FormatParseException(FILE_NOT_FOUND);
    }

    if (!(fh.open(QIODevice::ReadOnly))) {
        throw Jazz2FormatParseException(FILE_CANNOT_BE_OPENED);
    }

    Jazz2FormatDataBlock headerBlock(fh.read(262), false, 262);
    auto level = new Jazz2Level(headerBlock, fh.size(), false);

    // Read the lengths, uncompress the blocks and bail if any block could not be uncompressed
    // This could look better without all the copy-paste, but meh.
    quint32 infoBlockPackedSize,   eventBlockPackedSize,   dictBlockPackedSize,   layoutBlockPackedSize,
            infoBlockUnpackedSize, eventBlockUnpackedSize, dictBlockUnpackedSize, layoutBlockUnpackedSize;
    infoBlockPackedSize     = headerBlock.readUInt();
    infoBlockUnpackedSize   = headerBlock.readUInt();
    eventBlockPackedSize    = headerBlock.readUInt();
    eventBlockUnpackedSize  = headerBlock.readUInt();
    dictBlockPackedSize     = headerBlock.readUInt();
    dictBlockUnpackedSize   = headerBlock.readUInt();
    layoutBlockPackedSize   = headerBlock.readUInt();
    layoutBlockUnpackedSize = headerBlock.readUInt();

    Jazz2FormatDataBlock infoBlock  (fh.read(infoBlockPackedSize  ), true, infoBlockUnpackedSize  , 0);
    Jazz2FormatDataBlock eventBlock (fh.read(eventBlockPackedSize ), true, eventBlockUnpackedSize , 1);
    Jazz2FormatDataBlock dictBlock  (fh.read(dictBlockPackedSize  ), true, dictBlockUnpackedSize  , 2);
    Jazz2FormatDataBlock layoutBlock(fh.read(layoutBlockPackedSize), true, layoutBlockUnpackedSize, 3);

    // All data is in the memory, so the file can be closed.
    fh.close();

    level->loadMetadata(infoBlock, strictParser);
    level->loadEvents(eventBlock, strictParser);
    level->loadLayers(dictBlock, dictBlockUnpackedSize / 8, layoutBlock, strictParser);

    return level;
}

void Jazz2Level::printData(std::ostream& target) {
    target << "  Level name: " << name.toStdString()        << "\n"
           << " JCS version: " << (version == BASE_GAME ? "Base game" : "TSF/CC") << "\n\n"
           << "     Tileset: " << tileset.toStdString()     << "\n"
           << "  Music file: " << music.toStdString()       << "\n"
           << "  Next level: " << nextLevel.toStdString()   << "\n"
           << " Bonus level: " << bonusLevel.toStdString()  << "\n"
           << "Secret level: " << secretLevel.toStdString() << "\n\n";

    if (nullPasswordDetected) {
        target << "Empty password was detected on this level.\n\n";
    }

    target << "Text event strings:\n";
    for (unsigned i = 0; i < textEventStrings.length(); ++i) {
        target << "  " << QString("#" + QString::number(i + 1)).rightJustified(4).toStdString()
               << ": " << textEventStrings.at(i).toStdString() << "\n";
    }
    target << "\n";

    QString flagNames[32] = { "Tile X", "Tile Y", "Limit Visible Area", "Textured Mode", "Parallax Stars" };
    for (unsigned i = 0; i < layers.length(); ++i) {
        target << "Layer #" << (i + 1) << " (" << layers[i].width   << " x "   << layers[i].height  << ")\n"
               << "  Speeds: X "      << layers[i].speed_x << " / Y " << layers[i].speed_y << "\n"
               << "  Auto speeds: X " << layers[i].auto_x  << " / Y " << layers[i].auto_y  << "\n";

        QStringList appliedFlags;
        for (unsigned j = 0; j < 32; ++j) {
            if ((layers[i].flags >> j) % 2 != 0) {
                appliedFlags.append(flagNames[j]);
            }
        }

        if (appliedFlags.length() == 0) {
            appliedFlags.append("None");
        }
        target << "  Flags: " << appliedFlags.join(", ").toStdString() << "\n";

        if ((layers[i].flags & 0x00000008) != 0) {
            target << "  Textured Mode colour:" 
                   << " R" << (int)layers[i].tex_param[0] 
                   << " G" << (int)layers[i].tex_param[1]
                   << " B" << (int)layers[i].tex_param[2] << "\n";
        }

        target << "\n";
    }
}

Jazz2Level::Jazz2Level(Jazz2FormatDataBlock& header, quint32 fileLength, bool strictParser) {
    // Skip the copyright info at the beginning of the file
    header.discardBytes(180);

    // Read the next four bytes; should spell out "LEVL"
    qint32 id = header.readUInt();
    if (id != 0x4C56454C) {
        throw Jazz2FormatParseException(INVALID_MAGIC);
    }

    nullPasswordDetected = false;
    quint32 hash = header.readUInt();
    if (hash != 0x00BEBA00) {
        // This seems to be a password protected file
        if (hash != 0x00000000) {
            throw Jazz2FormatParseException(PASSWORD_PROTECTED);
        } else {
            nullPasswordDetected = true;
        }
    }

    name = header.readRawBytes(32);

    quint16 versionNum = header.readUShort();
    version = (versionNum <= 514 ? BASE_GAME : TSF);

    quint32 recordedSize = header.readUInt();
    if (strictParser && fileLength != recordedSize) {
        throw Jazz2FormatParseException(UNEXPECTED_FILE_LENGTH);
    }

    // Get the CRC; would check here if it matches if we knew what variant it is AND what it applies to
    // Test file across all CRC32 variants + Adler had no matches to the value obtained from the file
    // so either the variant is something else or the CRC is not applied to the whole file but on a part
    quint32 recordedCRC = header.readUInt();
}

void Jazz2Level::loadMetadata(Jazz2FormatDataBlock& block, bool strictParser) {
    // First 9 bytes are JCS coordinates on last save.
    block.discardBytes(9);

    lightingMin = block.readChar();
    lightingStart = block.readChar();

    animCount = block.readUShort();

    verticalMPSplitscreen = block.readBool();
    isMPLevel = block.readBool();

    // this should be the same as size of block in the start?
    quint32 headerSize = block.readUInt();

    QString secondLevelName = block.readRawBytes(32);
    if (strictParser && name != secondLevelName) {
        throw Jazz2FormatParseException(LEVEL_NAME_MISMATCH);
    }

    tileset     = block.readRawBytes(32);
    bonusLevel  = block.readRawBytes(32);
    nextLevel   = block.readRawBytes(32);
    secretLevel = block.readRawBytes(32);
    music       = block.readRawBytes(32);

    for (unsigned i = 0; i < 16; ++i) {
        textEventStrings << block.readRawBytes(512);
    }

    loadLayerMetadata(block, strictParser);

    qint16 staticTilesCount = block.readUShort();
    if (strictParser && maxSupportedTiles() - animCount != staticTilesCount) {
        throw Jazz2FormatParseException(TILE_COUNT_MISMATCH);
    }

    loadStaticTileData(block, strictParser);

    // the unused XMask field
    block.discardBytes(maxSupportedTiles());

    loadAnimatedTiles(block, strictParser);

    // The rest is just buffer data, free memory associated to that
    block.clear();
}

void Jazz2Level::loadStaticTileData(Jazz2FormatDataBlock& block, bool strictParser) {
    int tileCount = maxSupportedTiles();
    for (unsigned i = 0; i < tileCount; ++i) {
        Jazz2TileProperty tileProperties;
        qint32 tileEvent = block.readUInt();
        tileProperties.event.event_type = tileEvent & 0x000000FF;
        tileProperties.event.difficulty = (tileEvent & 0x0000C000) >> 14;
        tileProperties.event.illuminate = ((tileEvent & 0x00002000) >> 13 == 1);
        // TODO: test this
        tileProperties.event.params = ((tileEvent & 0xFFFF0000 >> 12) | (tileEvent & 0x00000F00 >> 8));
        staticTiles << tileProperties;
    }
    for (unsigned i = 0; i < tileCount; ++i) {
        staticTiles[i].flipped = block.readBool();
    }

    for (unsigned i = 0; i < tileCount; ++i) {
        staticTiles[i].type = block.readChar();
    }
}

void Jazz2Level::loadAnimatedTiles(Jazz2FormatDataBlock& block, bool strictParser) {
    int animCount = maxSupportedAnims();
    for (unsigned i = 0; i < animCount; ++i) {
        Jazz2AniTile animatedTile;
        animatedTile.delay = block.readUShort();
        animatedTile.delay_jitter = block.readUShort();
        animatedTile.reverse_delay = block.readUShort();
        animatedTile.is_reverse = block.readBool();
        animatedTile.speed = block.readChar(); // 0-70
        animatedTile.frame_cnt = block.readChar();

        for (unsigned j = 0; j < 64; ++j) {
            animatedTile.frames[j] = block.readUShort();
        }
        animatedTiles << animatedTile;
    }
}

void Jazz2Level::loadLayerMetadata(Jazz2FormatDataBlock& block, bool strictParser) {
    // What would I give for the format to be one layer at a time instead of property at a time...
    // TODO: Make this less copy-pastey, if you figure how.

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers << Jazz2Layer();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].flags = block.readUInt();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].type = block.readChar();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].used = block.readBool();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].width = block.readUInt();
    }

    // This is related to how data is presented in the file; the above is a WYSIWYG version, solely shown on the UI
    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].width_internal = block.readUInt();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].height = block.readUInt();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].depth = block.readUInt();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].detail_level = block.readChar();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].wave_x = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].wave_y = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].speed_x = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].speed_y = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].auto_x = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].auto_y = block.readFloat();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        layers[i].tex_type = block.readChar();
    }

    for (unsigned i = 0; i < JAZZ2_LAYER_COUNT; ++i) {
        for (unsigned j = 0; j < 3; ++j) {
            layers[i].tex_param[j] = block.readChar();
        }
    }
}

void Jazz2Level::loadEvents(Jazz2FormatDataBlock& block, bool strictParser) {
    if (strictParser && block.length() != (layers.at(3).width * layers.at(3).height * 4)) {
        throw Jazz2FormatParseException(EVENT_BLOCK_LENGTH_MISMATCH);
    }

    try {
        for (unsigned i = 0; i < layers.at(3).height; ++i) {
            for (unsigned j = 0; j < layers.at(3).width; ++j) {
                Jazz2TileEvent event;
                qint32 eventData = block.readUInt();
                event.event_type = (eventData & 0x000000FF);
                event.difficulty = ((eventData & 0x00000300) >> 8) % 4;
                event.illuminate = ((eventData & 0x00000400) >> 10 == 1);
                event.params = ((eventData & 0xFFFFF000) >> 12);
                events << event;
            }
        }
    } catch (...) {
        throw Jazz2FormatParseException(EVENT_BLOCK_LENGTH_MISMATCH);
    }
}

void Jazz2Level::loadLayers(Jazz2FormatDataBlock& dictBlock, quint32 dictLength, Jazz2FormatDataBlock& layoutBlock, bool strictParser) {
    QList<Jazz2DictionaryEntry> dictionary;
    for (unsigned i = 0; i < dictLength; ++i) {
        Jazz2DictionaryEntry entry;
        for (unsigned j = 0; j < 4; ++j) {
            entry.tiles[j] = dictBlock.readUShort();
        }
        dictionary << entry;
    }

    for (unsigned i = 0; i < layers.size(); ++i) {
        if (layers.at(i).used) {
            for (unsigned y = 0; y < layers.at(i).height; ++y) {
                QList< quint16 > new_row;
                for (unsigned x = 0; x < layers.at(i).width_internal; x += 4) {
                    quint16 s_dict = layoutBlock.readUShort();
                    for (unsigned j = 0; j < 4; j++) {
                        if (x + j >= layers.at(i).width) {
                            break;
                        }
                        new_row << dictionary.at(s_dict).tiles[j];
                    }
                }
                layers[i].tiles << new_row;
            }
        } else {
            for (unsigned y = 0; y < layers.at(i).height; ++y) {
                QList< quint16 > new_row;
                for (unsigned x = 0; x < layers.at(i).width; ++x) {
                    new_row.push_back(0);
                }
                layers[i].tiles << new_row;
            }

        }
    }
}

int Jazz2Level::maxSupportedTiles() {
    return (version == BASE_GAME) ? 1024 : 4096;
}

int Jazz2Level::maxSupportedAnims() {
    return (version == BASE_GAME) ? 128 : 256;
}

const quint8 Jazz2Level::JAZZ2_LAYER_COUNT = 8;
