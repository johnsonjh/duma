
include( $${EFENCELIBPATH}EFenceLibHeaders.pro )
include( $${EFENCELIBPATH}EFenceLibSources.pro )

CONFIG += qt warn_on thread staticlib debug

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

win32 {
  QMAKE_CXXFLAGS += -GX
  QMAKE_CXXFLAGS_RELEASE += -MT
  QMAKE_CXXFLAGS_DEBUG += -MTd
  QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO /FORCE:MULTIPLE
}


TEMPLATE	=lib
DEFINES	+= QT_THREAD_SUPPORT
DBFILE	= EFenceLib.db
LANGUAGE	= C++
