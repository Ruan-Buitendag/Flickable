#ifndef TRANSCRIPTION_H
#define TRANSCRIPTION_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantList>
#include <QVariantMap>



class Transcription : public QObject
{
    Q_OBJECT
public:
    Transcription(QObject *parent = nullptr);



    Q_INVOKABLE QVariantList getNotes();

};



#endif // TRANSCRIPTION_H
