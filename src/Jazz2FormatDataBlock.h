#pragma once

#include <QByteArray>
#include <QDataStream>
#include <memory>

class Jazz2FormatDataBlock {
public:
    Jazz2FormatDataBlock(QByteArray originalData, bool compressed, quint32 len, quint8 idx = 0);

    bool readBool();
    char readChar();
    quint16 readUShort();
    quint32 readUInt();
    float readFloat();
    QByteArray readRawBytes(int n);
    void discardBytes(int n);
    void clear();
    int length();

private:
    QByteArray data;
    std::unique_ptr<QDataStream> stream;

    QByteArray convertQUintToByteArray(quint32 val);

    template<typename T>
    T readValue();
};
