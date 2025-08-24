
#include "mainview.h"
#include "steam_api.h"

#include <QApplication>
#include <QCommonStyle>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStyleFactory>

using namespace ui;

//static const constexpr int APP_ID = 620;

void shutdown_steam()
{
	SteamAPI_Shutdown();
}

int main( int argc, char **argv )
{
	auto app = new QApplication( argc, argv );
	QString APP_ID = "644";

	for (int i = 0; i < argc; i++)
	{
		auto str = QString(argv[i]);
		if(str.toCaseFolded() == "--appid")
		{
			if(i == argc-1)
			{
				QMessageBox::warning( nullptr, "No appid provided.", "You did not provide an appid.\n Defaulting to appid: "+APP_ID);
				break;
			}

			auto possibleAppid = QString(argv[i + 1]);
			bool possible;
			possibleAppid.toInt(&possible);
			if(!possible)
			{
				QMessageBox::warning( nullptr, "Invalid appid provided.", "You provided a non-numerical ID.\nIf Steam supports non-numerical IDs in the future. God help you.\nDefaulting to appid: "+APP_ID);
				break;
			}

			APP_ID = possibleAppid;

			qInfo() << "Appid override accepted. Now running appid:" +APP_ID;
		}
 	}

	qputenv( "SteamAppId",  APP_ID.toLocal8Bit() );
	qputenv( "SteamGameId",  APP_ID.toLocal8Bit() );

	QCommonStyle *style = (QCommonStyle *)QStyleFactory::create( "fusion" );
	qApp->setStyle( style );

	QPalette palette {};
	palette.setColor( QPalette::Window, QColor( 49, 54, 59 ) );
	palette.setColor( QPalette::WindowText, Qt::white );
	palette.setColor( QPalette::Base, QColor( 27, 30, 32 ) );
	palette.setColor( QPalette::AlternateBase, QColor( 49, 54, 59 ) );
	palette.setColor( QPalette::ToolTipBase, Qt::black );
	palette.setColor( QPalette::ToolTipText, Qt::white );
	palette.setColor( QPalette::Text, Qt::white );
	palette.setColor( QPalette::Button, QColor( 49, 54, 59 ) );
	palette.setColor( QPalette::ButtonText, Qt::white );
	palette.setColor( QPalette::BrightText, Qt::red );
	palette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );
	palette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
	palette.setColor( QPalette::HighlightedText, Qt::black );
	palette.setColor( QPalette::Active, QPalette::Button, QColor( 49, 54, 59 ) );
	palette.setColor( QPalette::Disabled, QPalette::ButtonText, Qt::darkGray );
	palette.setColor( QPalette::Disabled, QPalette::WindowText, Qt::darkGray );
	palette.setColor( QPalette::Disabled, QPalette::Text, Qt::darkGray );
	palette.setColor( QPalette::Disabled, QPalette::Light, QColor( 49, 54, 59 ) );
	qApp->setPalette( palette );

	if ( !SteamAPI_Init() )
	{
		QMessageBox::critical( nullptr, "Fatal Error", "Steam must be running to use this tool (SteamAPI_Init() failed)." );
		return 1;
	}

	if ( !QFileInfo( QDir::tempPath() ).isWritable() )
	{
		QMessageBox::critical( nullptr, "Fatal Error", "We detected that we are unable to write to your computer's temporary files folder.\nTherefore the application cannot write to it. please check your firewall and maybe run the application in administrator." );
		return 1;
	}

	auto pMainWindow = new CMainView();
	pMainWindow->setAttribute( Qt::WA_DeleteOnClose );
	pMainWindow->open();

	atexit( shutdown_steam );

	return QApplication::exec();
}
