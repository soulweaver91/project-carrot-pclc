#pragma once

#include <iostream>
#include <QDir>
#include <QString>
#include <QVector>
#include <QSettings>
#include <QSet>
#include "JJ2Event.h"
#include "PCEvent.h"
#include "Jazz2FormatDataBlock.h"
#include "EventConverter.h"

enum Jazz2LevelVersion {
    BASE_GAME,
    TSF
};

enum Jazz2EventDifficulty {
    DIFFICULTY_ALL = 0,
    DIFFICULTY_EASY = 1,
    DIFFICULTY_HARD = 2,
    DIFFICULTY_MULTIPLAYER = 3
};

struct Jazz2Layer {
    quint32 flags;              // all except Parallax Stars supported
    quint8 type;                // ignored
    bool used;                  // supported
    quint32 width;              // supported
    quint32 internalWidth;      // supported
    quint32 height;             // supported
    qint32 depth;               // written, but not used
    quint8 detailLevel;         // written, but not used
    double waveX;               // ignored
    double waveY;               // ignored
    double speedX;              // supported
    double speedY;              // supported
    double autoSpeedX;          // supported
    double autoSpeedY;          // supported
    quint8 texturedType;        // supported
    quint8 texturedParams[3];   // supported
    QList<QList<quint16>> tiles;
};

struct Jazz2TileEvent {
    quint8 eventType;                 // subset of events supported
    Jazz2EventDifficulty difficulty;  // supported
    bool illuminate;                  // not yet supported
    quint32 params;                   // support varies by event
};

struct Jazz2TileProperty {
    Jazz2TileEvent event;       // supported
    quint8 flipped;             // supported
    quint8 type;                // translucent: supported, caption: won't be supported
};

struct Jazz2AniTile {
    quint16 delay;
    quint16 delayJitter;
    quint16 reverseDelay;
    bool isReverse;
    quint8 speed;
    quint8 frameCount;
    quint16 frames[64];
};

struct Jazz2DictionaryEntry {
    quint16 tiles[4];
};

struct PCLevelEventConversionStatistics {
    struct PCEventAppearance {
        quint16 x;
        quint16 y;
        JJ2Event originalEventType;
        QString originalEventName;
        PCEvent eventType;
        QString eventName;
        ParamArray params;
        Jazz2EventDifficulty difficulty;
        bool illuminate;
    };

    struct UnsupportedEvent {
        quint16 count;
        QString eventName;
    };

    QVector<PCEventAppearance> eventAppearances;
    QMap<JJ2Event, std::shared_ptr<UnsupportedEvent>> unsupportedEvents;
};

struct PCLevelConversionStatistics {
    PCLevelEventConversionStatistics events;
};

class Jazz2Level {
public:
    Jazz2Level();
    static Jazz2Level* fromFile(const QString& filename, bool strictParser = false);
    PCLevelConversionStatistics saveAsProjectCarrotLevel(const QDir& directory, const QString& uniqueID);
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

    // PC file writing functions
    void writePCConfigFile(const QString& filename, const QString& uniqueID);
    void writePCConfigFileLayerSection(QSettings& file, const QString& sectionName, const Jazz2Layer& layer, bool addBackgroundFields);
    void writePCLayer(const QString& filename, const Jazz2Layer& layer);
    PCLevelEventConversionStatistics writePCEvents(const QString& filename, quint32 width, quint32 height);
    void writePCAnimatedTiles(const QString& filename);

    int maxSupportedTiles();
    int maxSupportedAnims();

    const static quint8 JAZZ2_LAYER_COUNT;
};
