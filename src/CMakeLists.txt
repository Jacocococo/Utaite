add_executable(utaite
    songmodel.h songmodel.cpp
    songsindex.h songsindex.cpp
    playlistmanager.h playlistmanager.cpp
    songsentrymodel.h songsentrymodel.cpp
    mpris/mpris.h mpris/mpris.cpp
    mpris/mprisplayer.h mpris/mprisplayer.cpp
)

target_sources(utaite PRIVATE
    main.cpp
    resources.qrc
)

target_link_libraries(utaite
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Multimedia
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::I18n
    PkgConfig::LIBAV
)

install(TARGETS utaite ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
