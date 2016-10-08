#include "Jazz2FormatDataBlock.h"

#include "Jazz2FormatParseException.h"

Jazz2FormatDataBlock::Jazz2FormatDataBlock(QByteArray originalData, bool compressed, quint32 len, quint8 idx) {
    if (compressed) {
        data = qUncompress(convertQUintToByteArray(len) + originalData);

        if (data.size() == 0) {
            throw Jazz2FormatParseException(BLOCK_DECOMPRESSION_FAILED, { idx });
        }
    } else {
        data = originalData;
    }

    stream = std::make_unique<QDataStream>(&data, QIODevice::ReadOnly);
    stream->setByteOrder(QDataStream::LittleEndian);
}

bool Jazz2FormatDataBlock::readBool() {
    return readChar() != 0x00;
}

char Jazz2FormatDataBlock::readChar() {
    return readValue<quint8>();
}

quint16 Jazz2FormatDataBlock::readUShort() {
    return readValue<quint16>();
}

quint32 Jazz2FormatDataBlock::readUInt() {
    return readValue<quint32>();
}

float Jazz2FormatDataBlock::readFloat() {
    return readValue<qint32>() * 1.0 / 65536.0;
}

QByteArray Jazz2FormatDataBlock::readRawBytes(int n) {
    QByteArray buffer;

    char* raw = new char[n];
    stream->readRawData(raw, n);
    buffer.append(raw, n);
    delete[] raw;

    return buffer;
}

void Jazz2FormatDataBlock::discardBytes(int n) {
    stream->skipRawData(n);
}

void Jazz2FormatDataBlock::clear() {
    data.clear();
}

int Jazz2FormatDataBlock::length() {
    return data.length();
}

QByteArray Jazz2FormatDataBlock::convertQUintToByteArray(quint32 val) {
    QByteArray res;
    res.append((char)(val >> 24 & 0x000000FF))
       .append((char)(val >> 16 & 0x000000FF))
       .append((char)(val >>  8 & 0x000000FF))
       .append((char)(val       & 0x000000FF));

    return res;
}

template<typename T>
T Jazz2FormatDataBlock::readValue() {
    T val;
    *(stream.get()) >> val;
    return val;
}
