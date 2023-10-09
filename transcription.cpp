#include "transcription.h"

extern "C" {
#include "full_transcription.h"
}


Transcription::Transcription(QObject *parent)
    : QObject{parent}
{

}

void Transcription::bals()
{
    auto asshole = &Transcription::startTranscription;
    QtConcurrent::run(asshole, this);
}


void Transcription::startTranscription()
{

    std::string qSongName = m_songName.toLocalFile().toStdString();
    std::string qTranscriptionFile = m_transcriptionFile.toLocalFile().toStdString();

    const char* waveFilename = qSongName.c_str();
    const char* transcriptionFilename = qTranscriptionFile.c_str();

    qDebug() << waveFilename;
    qDebug() << transcriptionFilename;

    Matrix estimated_notes = full_transcription(waveFilename, transcriptionFilename);

    QVariantList list;

    for(int i = 0; i < estimated_notes.rows; i++)
    {
        QVariantMap map;
        map.insert("onset", estimated_notes.array[i][0]);
        map.insert("midiValue", estimated_notes.array[i][1]);

        list << map;
    }

    m_notes = list;
    emit notesChanged(m_notes);
}

QUrl Transcription::songName() const
{
    return m_songName;
}

void Transcription::setSongName(const QUrl &songName)
{
    m_songName = songName;
    emit songNameChanged(m_songName);
}

QVariantList Transcription::notes() const
{
    return m_notes;
}

void Transcription::setNotes(const QVariantList &notes)
{
    m_notes = notes;
    emit notesChanged(m_notes);

}

QUrl Transcription::transcriptionFile() const
{
    return m_transcriptionFile;
}

void Transcription::setTranscriptionFile(const QUrl &transcriptionFile)
{
    m_transcriptionFile = transcriptionFile;

    emit transcriptionFileChanged(m_transcriptionFile);
}
