#include "transcription.h"

extern "C" {
#include "full_transcription.h"
}


Transcription::Transcription(QObject *parent)
    : QObject{parent}
{

}


QVariantList Transcription::getNotes()
{
//    test();

    Matrix notes = full_transcription();

    QVariantList list;

    for(int i = 0; i < notes.rows; i++)
    {
        QVariantMap map;
        map.insert("onset", notes.array[i][0]);
        map.insert("midiValue", notes.array[i][1]);

        list << map;
    }

//    QVariantList list;

    return list;
}
