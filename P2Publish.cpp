
#include "dialogs/P2MapMainWindow.h"

#include <QApplication>
#include <QIcon>
#include <QSharedMemory>
#include <QStyleFactory>

int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	app.setWindowIcon( QIcon( ":/zoo_textures/P2 Publisher Icon2.png" ) );
	// QFile file(":/dark/stylesheet.qss");
	// file.open(QFile::ReadOnly | QFile::Text);
	// QTextStream stream(&file);
	// app.setStyleSheet(stream.readAll());
	qInfo() << app.setStyle("Fusion");

	bool performSingleCheck = true;
	for(int i = 0; i < argc; i++){
		qInfo() << (QString(argv[i]));
		if(QString(argv[i]) == "-overrideSingleOnly"){
			performSingleCheck = false;
		}
	}
	
	qInfo() << performSingleCheck;
	
		
	// QSharedMemory shared( "qt_Portal_2_Map_Publisher" );
	// if ( !shared.create(512, QSharedMemory::ReadWrite) )
	// {
	// 	if(performSingleCheck){
	// 		QMessageBox::warning(nullptr,"Multiple Instances","A instance of Portal 2 Map Publisher is already running.",QMessageBox::Ok);
	// 		app.exit( 0 );
	// 		exit( 0 );
	// 	}
	// }


	auto pZoo = new ui::CP2MapMainMenu( nullptr );
	pZoo->setAttribute( Qt::WA_DeleteOnClose );
	pZoo->show();

	return QApplication::exec();
}