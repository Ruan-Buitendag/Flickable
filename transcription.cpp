#include "transcription.h"

extern "C" {
#include "full_transcription.h"
}

Transcription::Transcription(QObject *parent)
    : QObject{parent}
{
    m_songName = QUrl::fromLocalFile("C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS/AkPnBcht/MUS/MAPS_MUS-alb_se3_AkPnBcht.wav");
    m_transcriptionFile = QUrl::fromLocalFile("C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS/AkPnBcht/MUS/MAPS_MUS-alb_se3_AkPnBcht.txt");

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    m_audioInputDevice = QMediaDevices::defaultAudioInput();

    if (!m_audioInputDevice.isFormatSupported(format)) {
        qWarning() << "Default format not supported ny input device, trying to use the nearest.";
    }

    m_audioInputSource = new QAudioSource(m_audioInputDevice, format);

    m_audioInputBuffer.open(QIODevice::ReadWrite);


    m_dictionary = LoadTranscriptionDictionary();

}

void Transcription::startRecordedTranscription()
{
    auto function = &Transcription::recordedTranscription;
    auto temp = QtConcurrent::run(function, this);
}

void Transcription::startLiveTranscription()
{
    auto function = &Transcription::liveTranscription;
    auto temp = QtConcurrent::run(function, this);
}

void Transcription::startRecording()
{
    qDebug() << "Starting recording";
    m_audioInputSource->start(&m_audioInputBuffer);
}

void Transcription::stopRecording()
{
    qDebug() << "Stopping recording";

    m_audioInputSource->stop();

    m_audioInputBuffer.seek(0);

}

void Transcription::liveTranscription()
{
    qDebug() << "Starting live transcription";

    m_audioInputBuffer.seek(0);

    QDataStream stream(&m_audioInputBuffer);

    int counter = 0;

    while(!stream.atEnd()) {
        qint16 value;
        stream >> value;
        counter++;
    }

    DynamicArray array = CreateDynamicArray(counter);

    m_audioInputBuffer.seek(0);

    counter = 0;
    while(!stream.atEnd()) {
        qint16 value;
        stream >> value;
        array.array[counter] = value/32768.0;
        counter++;
    }

    std::string qTranscriptionFile = m_transcriptionFile.toLocalFile().toStdString();
    const char* transcriptionFilename = qTranscriptionFile.c_str();

    Matrix estimated_notes = full_transcription_from_array(&array, transcriptionFilename, &m_dictionary);

    QVariantList list = notesToVariantList(estimated_notes);

    m_notes = list;
    emit notesChanged(m_notes);
}

QVariantList Transcription::notesToVariantList(Matrix estimated_notes)
{
    QVariantList list;

    for(int i = 0; i < estimated_notes.rows; i++)
    {
        QVariantMap map;
        map.insert("onset", estimated_notes.array[i][0]);
        map.insert("midiValue", estimated_notes.array[i][1]);

        list << map;
    }

    return list;
}

void Transcription::recordedTranscription()
{

    std::string qSongName = m_songName.toLocalFile().toStdString();
    std::string qTranscriptionFile = m_transcriptionFile.toLocalFile().toStdString();

    const char* waveFilename = qSongName.c_str();
    const char* transcriptionFilename = qTranscriptionFile.c_str();

    qDebug() << waveFilename;
    qDebug() << transcriptionFilename;

    Matrix estimated_notes = full_transcription_from_wav(waveFilename, transcriptionFilename, &m_dictionary);

    QVariantList list = notesToVariantList(estimated_notes);

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

QAudioDevice Transcription::audioInputDevice() const
{
    return m_audioInputDevice;
}

void Transcription::setAudioInputDevice(const QAudioDevice &audioDevice)
{
    m_audioInputDevice = audioDevice;
    emit audioInputDeviceChanged(m_audioInputDevice);
}
