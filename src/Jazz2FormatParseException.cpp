#include "Jazz2FormatParseException.h"

Jazz2FormatParseException::Jazz2FormatParseException(Jazz2FormatParseExceptionType type, QVector<QVariant> info)
    : type(type), info(info) {

}

Jazz2FormatParseExceptionType Jazz2FormatParseException::getType() {
    return type;
}

const QVector<QVariant> Jazz2FormatParseException::getInfo() {
    return info;
}

QString Jazz2FormatParseException::friendlyText() {
    switch (type) {
        case FILE_NOT_FOUND:
            return "File " + getInfo().at(0).toString() + " was not found!";
            break;
        case FILE_CANNOT_BE_OPENED:
            return "File " + getInfo().at(0).toString() + " could not be opened!";
            break;
        case INVALID_MAGIC:
            return "File header not recognized as a .J2L file!";
            break;
        case PASSWORD_PROTECTED:
            return "File is password protected!\nAs a courtesy to level editors, by default only files with no or blank password can be converted.";
            break;
        case UNEXPECTED_FILE_LENGTH:
            return "Conflicting file length reported inside the file!";
            break;
        case BLOCK_DECOMPRESSION_FAILED:
            return "Data block " + getInfo().at(0).toString() + " could not be uncompressed!";
            break;
        case LEVEL_NAME_MISMATCH:
            return "Conflicting level names reported!";
            break;
        case TILE_COUNT_MISMATCH:
            return "Conflicting tile counts reported!";
            break;
        case EVENT_BLOCK_LENGTH_MISMATCH:
            return "Conflicting event counts reported!";
            break;
        default:
            return "Unknown error!";
    }
}
