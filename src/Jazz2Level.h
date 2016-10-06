#pragma once

#include <iostream>
#include <QString>
#include <QVector>
#include "PCLevelConverter.h"

#include "Jazz2FormatDataBlock.h"

struct Jazz2Layer {
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
    quint8 tex_type;            // covered
    quint8 tex_param[3];        // covered
    QList<QList<quint16>> tiles;
};

struct Jazz2TileEvent {
    quint8 event_type;          // defined events covered
    quint8 difficulty;          // not covered
    bool illuminate;            // not covered
    quint32 params;             // defined events covered
};

struct Jazz2TileProperty {
    Jazz2TileEvent event;       // covered
    quint8 flipped;             // covered
    quint8 type;                // not covered
};

struct Jazz2AniTile {
    quint16 delay;
    quint16 delay_jitter;
    quint16 reverse_delay;
    bool is_reverse;
    quint8 speed;
    quint8 frame_cnt;
    quint16 frames[64];
};

struct Jazz2DictionaryEntry {
    quint16 tiles[4];
};

enum Jazz2LevelVersion {
    BASE_GAME,
    TSF
};

class Jazz2Level {
public:
    Jazz2Level();
    static Jazz2Level* fromFile(const QString& filename, bool strictParser = false);
    void printData(std::ostream& target);

private:
    QString name;
    QString tileset;
    QString nextLevel;
    QString bonusLevel;
    QString secretLevel;
    QString music;

    QVector<Jazz2Layer> layers;
    QVector<Jazz2TileProperty> staticTiles;
    QVector<Jazz2AniTile> animatedTiles;
    QVector<Jazz2TileEvent> events;

    QVector<QString> textEventStrings;

    Jazz2LevelVersion version;
    bool nullPasswordDetected;
    quint16 lightingMin;
    quint16 lightingStart;
    quint16 animCount;
    bool verticalMPSplitscreen;
    bool isMPLevel;

    // Parser utility functions
    Jazz2Level(Jazz2FormatDataBlock& header, quint32 fileLength, bool strictParser);
    void loadMetadata(Jazz2FormatDataBlock& block, bool strictParser);
    void loadStaticTileData(Jazz2FormatDataBlock& block, bool strictParser);
    void loadAnimatedTiles(Jazz2FormatDataBlock& block, bool strictParser);
    void loadLayerMetadata(Jazz2FormatDataBlock& block, bool strictParser);
    void loadEvents(Jazz2FormatDataBlock& block, bool strictParser);
    void loadLayers(Jazz2FormatDataBlock& dictBlock, quint32 dictLength, Jazz2FormatDataBlock& layoutBlock, bool strictParser);

    int maxSupportedTiles();
    int maxSupportedAnims();

    const static quint8 JAZZ2_LAYER_COUNT;

    // Temporary, while the code is still in flux
    friend PCLevelConverter;
};
