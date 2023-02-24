
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

	QCommonStyle* style = (QCommonStyle*) QStyleFactory::create("fusion");
	qApp->setStyle(style);

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
	qApp->setPalette(palette);

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
