#ifndef TRANSCRIPTION_H
#define TRANSCRIPTION_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantList>
#include <QVariantMap>
#include <QUrl>
#include <QtConcurrent>
#include <QAudioDevice>
//#include <QAudioSink>
#include <QAudioSource>
#include <QBuffer>
#include <QDataStream>
#include <QMediaDevices>

extern "C" {
#include "matrix.h"
#include "dictionary.h"
}

class Transcription : public QObject
{
    Q_OBJECT
public:
    Transcription(QObject *parent = nullptr);

    Q_INVOKABLE void startRecordedTranscription();
    Q_INVOKABLE void startLiveTranscription();
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();


    void liveTranscription();
    void recordedTranscription();

    Q_PROPERTY(QVariantList notes READ notes WRITE setNotes NOTIFY notesChanged FINAL);
    Q_PROPERTY(QUrl songName READ songName WRITE setSongName NOTIFY songNameChanged FINAL);
    Q_PROPERTY(QUrl transcriptionFile READ transcriptionFile WRITE setTranscriptionFile NOTIFY transcriptionFileChanged FINAL);
    Q_PROPERTY(QAudioDevice audioInputDevice READ audioInputDevice WRITE setAudioInputDevice NOTIFY audioInputDeviceChanged FINAL);

    QUrl songName() const;
    void setSongName(const QUrl &songName);


    QVariantList notes() const;
    void setNotes(const QVariantList &notes);

    QUrl transcriptionFile() const;
    void setTranscriptionFile(const QUrl &transcriptionFile);

    QAudioDevice audioInputDevice() const;
    void setAudioInputDevice(const QAudioDevice &audioInputDevice);

signals:
    void songNameChanged(QUrl songName);
    void notesChanged(QVariantList notes);
    void transcriptionFileChanged(QUrl transcriptionFile);
    void audioInputDeviceChanged(QAudioDevice audioInputDevice);

private:
    QUrl m_songName;
    QVariantList m_notes;
    QUrl m_transcriptionFile;

    QAudioDevice m_audioInputDevice;
    QAudioSource* m_audioInputSource;
    QBuffer m_audioInputBuffer;

    Dictionary m_dictionary;

    QVariantList notesToVariantList(Matrix estimated_notes);




};





#endif // TRANSCRIPTION_H
