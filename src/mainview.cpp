#include "mainview.h"

#include "mapuploader.h"

#include <QCheckBox>
#include <QDir>
#include <QEventLoop>
#include <QGridLayout>
#include <QImageReader>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTimeZone>

using namespace ui;

CMainView::CMainView( QWidget *pParent ) :
	QDialog( pParent )
{
	this->setWindowTitle( "Portal 2 Map Uploader" );

	auto pMainViewLayout = new QGridLayout( this );

	m_pWorkshopItemTree = new QTreeWidget( this );

	auto workshopTreeListTitles = QStringList();
	workshopTreeListTitles.append( "Title" );
	workshopTreeListTitles.append( "File" );
	workshopTreeListTitles.append( "Last Updated" );

	m_pWorkshopItemTree->setHeaderLabels( workshopTreeListTitles );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 0, Qt::AlignCenter );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 1, Qt::AlignCenter );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 2, Qt::AlignCenter );
	m_pWorkshopItemTree->setColumnWidth( 0, 600 / 3 );
	m_pWorkshopItemTree->setColumnWidth( 1, 600 / 3 );
	m_pWorkshopItemTree->setColumnWidth( 2, 600 / 3 );
	m_pWorkshopItemTree->setMinimumSize( 640, 200 );

	pMainViewLayout->addWidget( m_pWorkshopItemTree, 0, 0, 4, 6 );

	auto pTimezoneLabel = new QLabel( "Timezone:", this );

	pMainViewLayout->addWidget( pTimezoneLabel, 5, 0, Qt::AlignLeft | Qt::AlignBottom );

	m_pTimezoneComboBox = new QComboBox( this );
	foreach( auto timezoneID, QTimeZone::availableTimeZoneIds() )
		m_pTimezoneComboBox->addItem( timezoneID );

	QDateTime time = QDateTime::currentDateTime();
	m_pTimezoneComboBox->setCurrentIndex( QTimeZone::availableTimeZoneIds().indexOf( time.timeZone().id() ) );

	pMainViewLayout->addWidget( m_pTimezoneComboBox, 5, 1, Qt::AlignLeft | Qt::AlignBottom );

	auto pButtonLayout = new QVBoxLayout();

	auto pAddButton = new QPushButton( "Add", this );
	pButtonLayout->addWidget( pAddButton );

	auto pDeleteButton = new QPushButton( "Delete", this );
	pDeleteButton->setEnabled( false );
	pButtonLayout->addWidget( pDeleteButton );

	auto pEditButton = new QPushButton( "Edit", this );
	pEditButton->setEnabled( false );
	pButtonLayout->addWidget( pEditButton );

	auto pRefreshButton = new QPushButton( "Refresh", this );
	pButtonLayout->addWidget( pRefreshButton );

	pMainViewLayout->addLayout( pButtonLayout, 0, 6, Qt::AlignLeft );

	m_CallbackTimer.setSingleShot( false );
	connect( &m_CallbackTimer, &QTimer::timeout, this, SteamAPI_RunCallbacks );

	m_CallbackTimer.start( 100 );

	this->populateWorkshopList();

	connect( m_pTimezoneComboBox, &QComboBox::currentTextChanged, this, [this]( const QString &index )
			 {
				 QTimeZone zone = QTimeZone( QByteArray::fromStdString( index.toStdString() ) );
				 QTreeWidgetItemIterator workshopListIterator( m_pWorkshopItemTree );
				 for ( ; *workshopListIterator; ++workshopListIterator )
				 {
					 QDateTime time = QDateTime::fromSecsSinceEpoch( ( *workshopListIterator )->data( 0, Qt::UserRole ).toInt(),
																	 zone );
					 ( *workshopListIterator )->setText( 2, time.toString() );
				 }
			 } );

	connect( pAddButton, &QPushButton::pressed, this, [this]
			 {
				 auto pMapUploader = CMapUploader( this );
				 pMapUploader.exec();
			 } );

	connect( pDeleteButton, &QPushButton::pressed, this, &CMainView::onDeletePressed );

	connect( pEditButton, &QPushButton::pressed, this, [this]
			 {
				 if ( !m_pWorkshopItemTree->currentItem() )
					 return;

				 auto pMapUploader = CMapUploader( this );
				 pMapUploader.setEditItem( m_SteamUGCDetailsList[m_pWorkshopItemTree->currentItem()->data( 1, Qt::UserRole ).toInt()] );
				 pMapUploader.exec();
			 } );

	connect( pRefreshButton, &QPushButton::pressed, this, &CMainView::populateWorkshopList );

	connect( m_pWorkshopItemTree, &QTreeWidget::itemSelectionChanged, this, [pEditButton, pDeleteButton, this]
			 {
				 pEditButton->setEnabled( m_pWorkshopItemTree->selectedItems().length() > 0 );
				 pDeleteButton->setEnabled( m_pWorkshopItemTree->selectedItems().length() > 0 );
			 } );
}

void CMainView::onSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure )
{
	if ( bFailure || pQuery->m_eResult != 1 )
	{
		QMessageBox::critical( this, "Fatal Error", "Failed retrieve Query, Error code: " + QString::number( pQuery->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult" );
		return;
	}

	m_SteamUGCDetailsList.clear();
	m_pWorkshopItemTree->clear();

	auto fullUGCDetails = FullUGCDetails {};

	for ( int index = 0; index < pQuery->m_unNumResultsReturned; index++ )
	{
		SteamUGCDetails_t pDetails {};

		SteamUGC()->GetQueryUGCResult( pQuery->m_handle, index, &pDetails );

		char previewURL[MAX_URL_SIZE];
		SteamUGC()->GetQueryUGCPreviewURL( pQuery->m_handle, index, previewURL, MAX_URL_SIZE );

		if ( QString( previewURL ).isEmpty() )
		{
			QMessageBox::critical( this, "Fatal Error", "Unable to fetch image URL. (Missing or broken)" );
			continue;
		}

		QByteArray imageData {};
		auto fileName = this->downloadImageFromURL( QString( previewURL ), imageData );

		auto fileDirectory = QDir::tempPath() + "/" + QString::number( pDetails.m_nPublishedFileId );

		if ( !QDir( fileDirectory ).exists() )
			if ( !QDir().mkpath( fileDirectory ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + fileDirectory );
				return;
			}

		auto filePath = fileDirectory + "/" + fileName;

		if ( !CMainView::isFileWritable( filePath ) )
		{
			QMessageBox::critical( this, "Fatal Error", "Unable to download thumbnail. (Permission Denied)\n" + filePath );
			continue;
		}

		auto thumbnailFile = QFile( filePath );
		thumbnailFile.open( QFile::WriteOnly );

		if ( !thumbnailFile.write( imageData ) )
		{
			QMessageBox::critical( this, "Fatal Error", "Unable to download thumbnail. (Permission Denied)\n" + filePath );
			continue;
		}

		thumbnailFile.close();

		fullUGCDetails.standardDetails = pDetails;
		fullUGCDetails.thumbnailDetails = filePath;

		auto count = SteamUGC()->GetQueryUGCNumAdditionalPreviews( pQuery->m_handle, index );

		fullUGCDetails.additionalDetails = getAdditionalUGCPreviews( pQuery->m_handle, count, index, pDetails.m_nPublishedFileId );

		QDateTime time = QDateTime::fromSecsSinceEpoch( pDetails.m_rtimeUpdated );
		time.setTimeZone( QTimeZone( QByteArray( m_pTimezoneComboBox->currentText().toStdString().c_str() ) ) );

		auto pWorkshopitem = new QTreeWidgetItem( 0 );
		pWorkshopitem->setText( 0, pDetails.m_rgchTitle );
		pWorkshopitem->setText( 1, pDetails.m_pchFileName );
		pWorkshopitem->setText( 2, time.toString() );

		pWorkshopitem->setData( 0, Qt::UserRole, pDetails.m_rtimeUpdated );
		pWorkshopitem->setData( 1, Qt::UserRole, index );

		pWorkshopitem->setTextAlignment( 0, Qt::AlignCenter );
		pWorkshopitem->setTextAlignment( 1, Qt::AlignCenter );
		pWorkshopitem->setTextAlignment( 2, Qt::AlignCenter );
		m_pWorkshopItemTree->addTopLevelItem( pWorkshopitem );

		m_SteamUGCDetailsList.insert( std::make_pair( index, fullUGCDetails ) );
	}
}

bool CMainView::isFileWritable( const QString &fullPath )
{
	auto fileInfo = QFileInfo( fullPath );
	auto filePathInfo = QFileInfo( fileInfo.path() );

	return ( filePathInfo.isWritable() || fileInfo.isWritable() );
}

CMainView::AdditionalUGCDetails CMainView::getAdditionalUGCPreviews( UGCQueryHandle_t queryHandle, int count, int itemIndex, PublishedFileId_t fileID )
{
	auto additionalDetails = AdditionalUGCDetails {};

	additionalDetails.amount = count;

	for ( uint32 i = 0; i < count; i++ )
	{
		char pchUrl[MAX_URL_SIZE];
		char pchFileName[MAX_URL_SIZE];
		EItemPreviewType pType;

		SteamUGC()->GetQueryUGCAdditionalPreview( queryHandle, itemIndex, i,
												  pchUrl, MAX_URL_SIZE, pchFileName,
												  MAX_URL_SIZE, &pType );

		if ( pType == k_EItemPreviewType_Image )
		{
			QByteArray imageData {};

			auto fileName = this->downloadImageFromURL( pchUrl, imageData );

			auto fileDirectory = QDir::tempPath() + "/" + QString::number( fileID );

			if ( !QDir( fileDirectory ).exists() )
				if ( !QDir().mkpath( fileDirectory ) )
				{
					QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + fileDirectory );
					return AdditionalUGCDetails {};
				}

			fileDirectory += "/additional/";

			if ( !QDir( fileDirectory ).exists() )
				if ( !QDir().mkpath( fileDirectory ) )
				{
					QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + fileDirectory );
					return AdditionalUGCDetails {};
				}

			quint32 uniqueFolderName = QRandomGenerator::global()->generate();
			fileDirectory += QString::number( uniqueFolderName ) + "/";

			if ( !QDir( fileDirectory ).exists() )
				if ( !QDir().mkpath( fileDirectory ) )
				{
					QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + fileDirectory );
					return AdditionalUGCDetails {};
				}

			auto filePath = fileDirectory + "/" + fileName;

			if ( !CMainView::isFileWritable( filePath ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to download additional images. (Permission Denied)\n" + filePath );
				return AdditionalUGCDetails {};
			}

			auto additionalImageFile = QFile( filePath );
			additionalImageFile.open( QFile::WriteOnly );

			if(!additionalImageFile.write( imageData ))
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to download additional images. (Permission Denied)\n" + filePath );
				return AdditionalUGCDetails {};
			}
			additionalImageFile.close();

			auto fileInfoList = QStringList {};
			fileInfoList.append( pchFileName );
			fileInfoList.append( filePath );

			additionalDetails.imagePaths.append( fileInfoList );
		}
		if ( pType == k_EItemPreviewType_YouTubeVideo )
		{
			additionalDetails.videoURLs.append( pchUrl );
			break;
		}
	}

	return additionalDetails;
}

QString CMainView::downloadImageFromURL( const QString &url, QByteArray &imageData )
{
	QNetworkAccessManager manager;
	QNetworkReply *reply = manager.get( QNetworkRequest( QUrl( url ) ) );

	QEventLoop wait;
	connect( &manager, SIGNAL( finished( QNetworkReply * ) ), &wait, SLOT( quit() ) );
	connect( &manager, SIGNAL( finished( QNetworkReply * ) ), &manager, SLOT( deleteLater() ) );

	QTimer oneTake;
	oneTake.start( 10000 );
	connect( &oneTake, SIGNAL( timeout() ), &wait, SLOT( quit() ) );
	wait.exec();

	QRegularExpression filenameRegex( R"(filename[^;=\n]*=(?<fileName>(['"]).*?\2|[^;\n]*))" );
	auto filenameMatch = filenameRegex.match( reply->rawHeader( "Content-Disposition" ) );
	auto fileName = filenameMatch.captured( "fileName" ).replace( "UTF-8''", "" ).replace( R"(")", "" );

	QImageReader imageReader( reply );
	if ( reply->error() != QNetworkReply::NetworkError::NoError || !imageReader.canRead() )
	{
		QMessageBox::critical( this, "Failed Download", ( QString( "Failed retrieve image : " ) + url ) );
		return "";
	}

	imageData = reply->readAll();

	reply->deleteLater();

	return fileName;
}

void CMainView::populateWorkshopList()
{
	UGCQueryHandle_t hQueryResult = SteamUGC()->CreateQueryUserUGCRequest( SteamUser()->GetSteamID().GetAccountID(),
																		   k_EUserUGCList_Published,
																		   k_EUGCMatchingUGCType_Items_ReadyToUse,
																		   k_EUserUGCListSortOrder_CreationOrderDesc,
																		   SteamUtils()->GetAppID(), m_GameID, 1 );
	SteamUGC()->SetReturnAdditionalPreviews( hQueryResult, true );
	SteamUGC()->SetReturnLongDescription( hQueryResult, true );
	SteamUGC()->SetReturnMetadata( hQueryResult, true );
	SteamAPICall_t hApiQueryHandle = SteamUGC()->SendQueryUGCRequest( hQueryResult );
	m_SteamCallResultUGCRequest.Set( hApiQueryHandle, this, &CMainView::onSendQueryUGCRequest );
	SteamUGC()->ReleaseQueryUGCRequest( hQueryResult );
}

void CMainView::onDeletePressed()
{
	QMessageBox deleteWarningMessageBox( QMessageBox::Icon::Critical, "DELETING ITEM", "This action will delete this item permanently from the workshop!", QMessageBox::Ok | QMessageBox::Abort, this );
	auto deleteWarningMessageCheckBox = new QCheckBox( "I understand that this will delete this item from the workshop.", &deleteWarningMessageBox );
	deleteWarningMessageBox.button( QMessageBox::Ok )->setDisabled( true );
	connect( deleteWarningMessageCheckBox, &QCheckBox::stateChanged, this, [&]( int state )
			 {
				 if ( state == 0 )
				 {
					 deleteWarningMessageBox.button( QMessageBox::Ok )->setDisabled( true );
				 }
				 else
				 {
					 deleteWarningMessageBox.button( QMessageBox::Ok )->setDisabled( false );
				 }
			 } );

	deleteWarningMessageBox.setCheckBox( deleteWarningMessageCheckBox );
	int ret = deleteWarningMessageBox.exec();

	if ( ret == QMessageBox::Ok && deleteWarningMessageCheckBox->isChecked() )
	{
		QTreeWidgetItem *pWorkshopItem = this->m_pWorkshopItemTree->selectedItems()[0];
		int itemIndex = pWorkshopItem->data( 1, Qt::UserRole ).toInt();
		FullUGCDetails workshopItemDetails = m_SteamUGCDetailsList.at( itemIndex );
		SteamAPICall_t deleteItemCall = SteamUGC()->DeleteItem( workshopItemDetails.standardDetails.m_nPublishedFileId );
		m_CallResultDeleteItem.Set( deleteItemCall, this, &CMainView::onDeleteItem );
	}
}

void CMainView::onDeleteItem( DeleteItemResult_t *pItem, bool bFailure )
{
	if ( bFailure || pItem->m_eResult != 1 )
	{
		QMessageBox::critical( this, "Fatal Error", "Failed to delete item, Error code: " + QString::number( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult" );
		return;
	}

	this->populateWorkshopList();
}