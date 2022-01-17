
#include "dialogs/P2MapMainWindow.h"

#include <QApplication>
#include <QIcon>
#include <QSharedMemory>

// static bool m_setLoaded = false;

int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	app.setWindowIcon( QIcon( ":/zoo_textures/P2 Publisher Icon2.png" ) );

	QFile file(":/dark/stylesheet.qss");
	file.open(QFile::ReadOnly | QFile::Text);
	QTextStream stream(&file);
	app.setStyleSheet(stream.readAll());
	// QSharedMemory shared( "qt_Portal_2_Map_Publisher" );
	// if ( !shared.create(512, QSharedMemory::ReadWrite) )
	// {
	// 	QMessageBox::warning(nullptr,"Multiple Instances","A instance of Portal 2 Map Publisher is already running.",QMessageBox::Ok);
	// 	app.exit( 0 );
	//     exit( 0 );
	// }

	auto pZoo = new ui::CP2MapMainMenu( nullptr );
	pZoo->setAttribute( Qt::WA_DeleteOnClose );
	pZoo->show();

	return QApplication::exec();
}