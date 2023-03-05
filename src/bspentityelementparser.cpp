#include "bspentityelementparser.h"

#include "mainview.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

using namespace ui;

CElementList::ListInitResponse CElementList::initialiseElementList()
{
	QFile elementKeyValueFile = QFile( QDir::currentPath() + "/elements.kv" );

	std::unique_ptr<KeyValueRoot> elementKeyValueRoot = std::make_unique<KeyValueRoot>();

	if ( !CMainView::isFileWritable( QDir::currentPath() + "/elements.kv" ) )
	{
		elementKeyValueRoot->Parse( m_DefaultElementListString.toStdString().c_str() );

		m_ElementList = std::move( elementKeyValueRoot );
		m_Initialised = true;

		return ListInitResponse::FILE_INVALID;
	}

	if ( !elementKeyValueFile.exists() )
	{
		elementKeyValueFile.open( QIODevice::ReadWrite );

		QTextStream elementKeyValueFileStream( &elementKeyValueFile );
		elementKeyValueFileStream << m_DefaultElementListString << Qt::endl;

		elementKeyValueFile.close();

		elementKeyValueRoot->Parse( m_DefaultElementListString.toStdString().c_str() );

		m_ElementList = std::move( elementKeyValueRoot );
		m_Initialised = true;

		return ListInitResponse::FILE_NOT_FOUND;
	};

	elementKeyValueFile.open( QIODevice::ReadOnly );
	KeyValueErrorCode keyValueErrorCode = elementKeyValueRoot->Parse( elementKeyValueFile.readAll().constData() );

	if ( keyValueErrorCode != KeyValueErrorCode::NO_ERROR )
	{
		elementKeyValueRoot->Parse( m_DefaultElementListString.toStdString().c_str() );

		elementKeyValueFile.close();

		m_ElementList = std::move( elementKeyValueRoot );
		m_Initialised = true;

		return ListInitResponse::FILE_INVALID;
	}

	elementKeyValueFile.close();

	m_ElementList = std::move( elementKeyValueRoot );
	m_Initialised = true;

	return ListInitResponse::FILE_INITIALISED;
}

KeyValueRoot *CElementList::getElementList()
{
	return m_ElementList.get();
}

bool CElementList::isInitialised()
{
	return m_Initialised;
}
