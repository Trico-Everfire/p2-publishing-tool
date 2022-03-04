
#include "dialogs/P2MapMainWindow.h"

#include <QApplication>
#include <QIcon>
#include <QSharedMemory>
#include <QCommonStyle>
#include <QStyleFactory>

int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	app.setWindowIcon( QIcon( ":/zoo_textures/P2 Publisher Icon2.png" ) );
	QCommonStyle* style = (QCommonStyle*) QStyleFactory::create("fusion");
	app.setStyle(style);

	QPalette palette = QPalette();
	palette.setColor(QPalette::Window, QColor(49,54,59));
	palette.setColor(QPalette::WindowText, Qt::white);
	palette.setColor(QPalette::Base, QColor(27,30,32));
	palette.setColor(QPalette::AlternateBase, QColor(49,54,59));
	palette.setColor(QPalette::ToolTipBase, Qt::black);
	palette.setColor(QPalette::ToolTipText, Qt::white);
	palette.setColor(QPalette::Text, Qt::white);
	palette.setColor(QPalette::Button, QColor(49,54,59));
	palette.setColor(QPalette::ButtonText, Qt::white);;
	palette.setColor(QPalette::BrightText, Qt::red);
	palette.setColor(QPalette::Link, QColor(42, 130, 218));
	palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	palette.setColor(QPalette::HighlightedText, Qt::black);
	palette.setColor(QPalette::Active, QPalette::Button, QColor(49,54,59));
    	palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    	palette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
    	palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    	palette.setColor(QPalette::Disabled, QPalette::Light, QColor(49,54,59));
	app.setPalette(palette);

	// bool performSingleCheck = true;
	// for(int i = 0; i < argc; i++){
	// 	qInfo() << (QString(argv[i]));
	// 	if(QString(argv[i]) == "-overrideSingleOnly"){
	// 		performSingleCheck = false;
	// 	}
	// }
	
	// qInfo() << performSingleCheck;
	
		
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
