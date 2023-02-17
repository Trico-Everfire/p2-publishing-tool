
#include "mainview.h"
#include "steam_api.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

using namespace ui;

constexpr int APP_ID = 644;

void shutdown_steam()
{
	SteamAPI_Shutdown();
}

int main( int argc, char **argv )
{
	qputenv( "SteamAppId", QString::number( APP_ID ).toLocal8Bit() );
	qputenv( "SteamGameId", QString::number( APP_ID ).toLocal8Bit() );

	auto app = new QApplication( argc, argv );

	QFile file( ":/resource/style.qss" );
	file.open( QFile::ReadOnly );
	QString styleSheet = QLatin1String( file.readAll() );
	qApp->setStyleSheet( styleSheet );

	if ( !SteamAPI_Init() )
	{
		QMessageBox::critical( nullptr, "Fatal Error", "Steam must be running to use this tool (SteamAPI_Init() failed)." );
		return 1;
	}

	if(!QFileInfo(QDir::tempPath()).isWritable())
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