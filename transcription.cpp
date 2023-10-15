#include "transcription.h"

extern "C" {
#include "full_transcription.h"
#include "domain_adaptation.h"
}

#include "midifile.h"
#include "stdlib.h"

#include <Windows.h>



Transcription::Transcription(QObject *parent)
    : QObject{parent}
{
    qDebug() << "Transcription obect created";

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

    m_audioOutputDevice =  QMediaDevices::defaultAudioOutput();

    if (!m_audioOutputDevice.isFormatSupported(format)) {
        qWarning() << "Default format not supported ny output device, trying to use the nearest.";
    }

    m_audioOutputSink = new QAudioSink(m_audioOutputDevice, format);

//    m_dictionary = LoadTranscriptionDictionary();

//    DomainAdaptationTest();

//    CalculateTemplateWeights();

//    qDebug() << cache_line_size();

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
//    m_audioInputBuffer.close();
//    m_audioInputBuffer.open(QIODevice::ReadWrite);



    pauseRecording();
    m_audioInputBuffer.buffer().clear();
    m_audioInputBuffer.seek(0);

//    m_audioInputBuffer.setData(QByteArray());

    qDebug() << "Starting recording";
    m_audioInputSource->start(&m_audioInputBuffer);
}

void Transcription::stopRecording()
{
    qDebug() << "Stopping recording";

    m_audioInputSource->stop();

    m_audioInputBuffer.seek(0);

}

void Transcription::playMidiFile()
{
    char command[256];

    sprintf(command, "%s -i -n %s %s", path_fluidsynth_exe, path_soundfont, "transcription_output.mid");

    printf(command);

    //    system(command);
    WinExec(command, SW_HIDE);

}



void Transcription::playRecording()
{
    pauseRecording();
    m_audioInputBuffer.seek(0);

    qDebug() << "Playing recording on " << m_audioOutputDevice.description();

    m_audioOutputSink->start(&m_audioInputBuffer);
}

void Transcription::pauseRecording()
{

    qDebug() << "Stopping recording";

    m_audioOutputSink->stop();

}

void Transcription::liveTranscription()
{
    qDebug() << "Starting live transcription";

    m_audioInputBuffer.seek(0);

    QDataStream stream(&m_audioInputBuffer);

    int counter = 0;

    double max = 0;

    while(!stream.atEnd()) {
        qint16 value;
        stream >> value;

        if(value > max)
        {
            max = value;
        }

        counter++;
    }

    DynamicArray array = CreateDynamicArray(counter);

    m_audioInputBuffer.seek(0);

    counter = 0;

    if(max == 0)
    {
        max = 32768;
    }

    printf("Max: %f\n", max);
    fflush(stdout);

    while(!stream.atEnd()) {
        qint16 value;
        stream >> value;
//        array.array[counter] = value/32768.0;
        array.array[counter] = value/max;
        counter++;
    }

    std::string qTranscriptionFile = m_transcriptionFile.toLocalFile().toStdString();
    const char* transcriptionFilename = qTranscriptionFile.c_str();


//    qDebug() << "Dictionary dims: " << m_dictionary.shape[0] << ", " << m_dictionary.shape[1] << ", " << m_dictionary.shape[2];

    // print array
    for(int i = 0; i < 10; i++)
    {
        printf("%f\n", array.array[i]);
    }

    fflush(stdout);


//    Matrix estimated_notes = full_transcription_from_array(&array, transcriptionFilename, &m_dictionary);

    Matrix estimated_notes = transcribe_array(&array, transcriptionFilename);

    QVariantList list = notesToVariantList(estimated_notes);

    notesToMidiFile(estimated_notes, "transcription_output.mid");
    playMidiFile();


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

//    printf("Dictionary dims: %d, %d, %d\n", m_dictionary.shape[0], m_dictionary.shape[1], m_dictionary.shape[2]);
//    fflush(stdout);


    std::string qSongName = m_songName.toLocalFile().toStdString();
    std::string qTranscriptionFile = m_transcriptionFile.toLocalFile().toStdString();

    const char* waveFilename = qSongName.c_str();
    const char* transcriptionFilename = qTranscriptionFile.c_str();

//    qDebug() << waveFilename;
//    qDebug() << transcriptionFilename;

//    Dictionary tempdict = GetBestDictionaryForRecording(waveFilename);

//    Dictionary tempdict = GetDictionary("AkPnBcht");

//    tempdict = HardFilterSpectrograms(&tempdict, 1500);

//    NormaliseDictionary(&tempdict);

//    m_dictionary = tempdict;

//    Matrix estimated_notes = full_transcription_from_wav(waveFilename, transcriptionFilename, &m_dictionary);

    Matrix estimated_notes = transcribe_wav(waveFilename, transcriptionFilename);

    notesToMidiFile(estimated_notes, "transcription_output.mid");
    playMidiFile();

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

void Transcription::notesToMidiFile(Matrix estimated_notes, const char *midiFilename)
{
    int ticks_per_beat = 960;

    MidiFile_t midifile = MidiFile_new(1, MIDI_FILE_DIVISION_TYPE_PPQ, ticks_per_beat);

    double noteDuration = 0.25; // in seconds
    int noteTickDuration = MidiFile_getTickFromTime(midifile, noteDuration);


    MidiFileTrack_t tempoTrack = MidiFile_createTrack(midifile);
    MidiFileTrack_createTempoEvent(tempoTrack, 0, 60.0f);

    MidiFileTrack_t noteTrack = MidiFile_createTrack(midifile);

    for(int note = 0; note < estimated_notes.rows; note++){
        long onset_tick = MidiFile_getTickFromTime(midifile, estimated_notes.array[note][0]);

        MidiFileTrack_createNoteOnEvent(noteTrack, onset_tick, 0, estimated_notes.array[note][1], 100);
        MidiFileTrack_createNoteOffEvent(noteTrack, onset_tick + noteTickDuration, 0, estimated_notes.array[note][1], 100);
    }

    MidiFile_save(midifile, midiFilename);

}
