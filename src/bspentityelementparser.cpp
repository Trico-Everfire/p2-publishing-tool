#include "bspentityelementparser.h"

#include "mainview.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

using namespace ui;

CElementList::ListInitResponse CElementList::initialiseElementList()
{
	QFile keyvalueFile = QFile( QDir::currentPath() + "/elements.kv" );

	auto mainRoot = new KeyValueRoot();

	if ( !CMainView::isFileWritable(QDir::currentPath() + "/elements.kv") )
	{
		mainRoot->Parse( defaultElementList.toStdString().c_str() );

		elementList = mainRoot;
		initialised = true;

		return ListInitResponse::FILEINVALID;
	}

	if ( !keyvalueFile.exists() )
	{
		keyvalueFile.open( QIODevice::ReadWrite );

		QTextStream stream( &keyvalueFile );
		stream << defaultElementList << Qt::endl;

		keyvalueFile.close();

		mainRoot->Parse( defaultElementList.toStdString().c_str() );

		elementList = mainRoot;
		initialised = true;

		return ListInitResponse::FILENOTFOUND;
	};

	keyvalueFile.open( QIODevice::ReadOnly );
	KeyValueErrorCode code = mainRoot->Parse( keyvalueFile.readAll().constData() );

	if ( code != KeyValueErrorCode::NO_ERROR )
	{
		mainRoot->Parse( defaultElementList.toStdString().c_str() );

		keyvalueFile.close();

		elementList = mainRoot;
		initialised = true;

		return ListInitResponse::FILEINVALID;
	}

	keyvalueFile.close();

	elementList = mainRoot;
	initialised = true;

	return ListInitResponse::FILEINITIALISED;
}
bool CElementList::isInitialised()
{
	return initialised;
}

KeyValueRoot *CElementList::getElementList()
{
	return elementList;
}
