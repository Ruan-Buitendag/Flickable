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
#include <QAudioSink>

extern "C" {
#include "matrix.h"
#include "dictionary.h"
#include "dynamicarray.h"
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

    Q_INVOKABLE void playMidiFile();
    Q_INVOKABLE void playRecording();

    void pauseRecording();


    void liveTranscription();
    void recordedTranscription();

    Q_PROPERTY(QVariantList notes READ notes WRITE setNotes NOTIFY notesChanged FINAL);
    Q_PROPERTY(QUrl songName READ songName WRITE setSongName NOTIFY songNameChanged FINAL);
    Q_PROPERTY(QUrl transcriptionFile READ transcriptionFile WRITE setTranscriptionFile NOTIFY transcriptionFileChanged FINAL);
    Q_PROPERTY(QAudioDevice audioInputDevice READ audioInputDevice WRITE setAudioInputDevice NOTIFY audioInputDeviceChanged FINAL);
    Q_PROPERTY(int transcriptionIterations READ transcriptionIterations WRITE setTranscriptionIterations NOTIFY transcriptionIterationsChanged FINAL);

    QUrl songName() const;
    void setSongName(const QUrl &songName);


    QVariantList notes() const;
    void setNotes(const QVariantList &notes);

    QUrl transcriptionFile() const;
    void setTranscriptionFile(const QUrl &transcriptionFile);

    QAudioDevice audioInputDevice() const;
    void setAudioInputDevice(const QAudioDevice &audioInputDevice);

    int transcriptionIterations() const;
    void setTranscriptionIterations(const int &transcriptionIterations);

signals:
    void songNameChanged(QUrl songName);
    void notesChanged(QVariantList notes);
    void transcriptionFileChanged(QUrl transcriptionFile);
    void audioInputDeviceChanged(QAudioDevice audioInputDevice);
    void transcriptionIterationsChanged(int transcriptionIterations);

private:
    QUrl m_songName;
    QVariantList m_notes;
    QUrl m_transcriptionFile;

    QAudioDevice m_audioInputDevice;
    QAudioSource* m_audioInputSource;

    QAudioDevice m_audioOutputDevice;
    QAudioSink* m_audioOutputSink;

    QBuffer m_audioInputBuffer;

    int m_transcriptionIterations;


    DynamicArray GetAudioFromBuffer();


    void notesToMidiFile(Matrix estimated_notes, const char* midiFile);
    QVariantList notesToVariantList(Matrix estimated_notes);


    const char * path_fluidsynth_exe = "C:/tools/fluidsynth/bin/fluidsynth.exe";
    const char * path_soundfont = "C:/Users/ruanb/OneDrive/Documents/Flickable/soundfonts/yamaha_piano.sf2";


};





#endif // TRANSCRIPTION_H
