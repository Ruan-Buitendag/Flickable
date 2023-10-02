cmake_minimum_required(VERSION 3.16)

project(Flickable VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appFlickable
    main.cpp
    activations.c CMakeLists.txt dictionary.c dynamicarray.c evaluation.c full_transcription.c matrix.c note_transcription.c spectrogram.c stft.c wav.c
    activations.h dictionary.h dynamicarray.h evaluation.h full_transcription.h main.cpp matrix.h note_transcription.h spectrogram.h stft.h transcription.cpp transcription.h wav.h
)

qt_add_qml_module(appFlickable
    URI Flickable
    VERSION 1.0
SOURCES
RESOURCES
QML_FILES Keyboard.qml KeyboardNote.qml Main.qml NoteActivation.qml Octave.qml GridBlock.qml CustomGrid.qml

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appFlickable PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appFlickable
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_package(HDF5 COMPONENTS C HL)


include_directories(${HDF5_INCLUDE_DIRS} code/headers)


target_link_libraries(appFlickable
    PRIVATE Qt6::Quick         ${HDF5_LIBRARIES} # This should list all libraries.
)




include(GNUInstallDirs)
install(TARGETS appFlickable
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
