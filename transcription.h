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

class Transcription : public QObject
{
    Q_OBJECT
public:
    Transcription(QObject *parent = nullptr);

    Q_INVOKABLE void bals();

    void startTranscription();

    Q_PROPERTY(QVariantList notes READ notes WRITE setNotes NOTIFY notesChanged FINAL);
    Q_PROPERTY(QUrl songName READ songName WRITE setSongName NOTIFY songNameChanged FINAL);
    Q_PROPERTY(QUrl transcriptionFile READ transcriptionFile WRITE setTranscriptionFile NOTIFY transcriptionFileChanged FINAL);
    Q_PROPERTY(QAudioDevice audioDevice READ audioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged FINAL);

    QUrl songName() const;
    void setSongName(const QUrl &songName);


    QVariantList notes() const;
    void setNotes(const QVariantList &notes);

    QUrl transcriptionFile() const;
    void setTranscriptionFile(const QUrl &transcriptionFile);

    QAudioDevice audioDevice() const;
    void setAudioDevice(const QAudioDevice &audioDevice);


signals:
    void songNameChanged(QUrl songName);
    void notesChanged(QVariantList notes);
    void transcriptionFileChanged(QUrl transcriptionFile);
    void audioDeviceChanged(QAudioDevice audioDevice);

private:
    QUrl m_songName;
    QVariantList m_notes;
    QUrl m_transcriptionFile;
    QAudioDevice m_audioDevice;

};





#endif // TRANSCRIPTION_H
