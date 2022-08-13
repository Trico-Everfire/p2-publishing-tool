//
// Created by trico on 19-6-22.
//

#include "P2ElementParser.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

ListInitResponse P2ElementParser::initialiseElementList()
{ // returns true if the file has been read correctly
	initialised = false;
	QFile keyvalueFile = QFile( QDir::currentPath() + "/elements.kv" );
	KeyValueRoot *mainRoot = new KeyValueRoot();
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
	//	qInfo() << keyvalueFile.readAll().constData();
	KeyValueErrorCode code = mainRoot->Parse( keyvalueFile.readAll().constData() );
	//	qInfo() << (code == KeyValueErrorCode::NO_ERROR);
	if ( code != KeyValueErrorCode::NO_ERROR )
	{
		mainRoot->Parse( defaultElementList.toStdString().c_str() );
		elementList = mainRoot;
		initialised = true;
		keyvalueFile.close();
		return ListInitResponse::FILEINVALID;
	}

	elementList = mainRoot;
	initialised = true;
	keyvalueFile.close();
	return ListInitResponse::FILEINITIALISED;
}
bool P2ElementParser::isInitialised()
{
	return initialised;
}

KeyValueRoot *P2ElementParser::getElementList()
{
	return elementList;
}