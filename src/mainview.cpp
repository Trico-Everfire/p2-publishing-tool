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

	for ( int itemIndex = 0; itemIndex < pQuery->m_unNumResultsReturned; itemIndex++ )
	{
		SteamUGCDetails_t pDetails {};

		SteamUGC()->GetQueryUGCResult( pQuery->m_handle, itemIndex, &pDetails );

		char previewURL[MAX_URL_SIZE];
		SteamUGC()->GetQueryUGCPreviewURL( pQuery->m_handle, itemIndex, previewURL, MAX_URL_SIZE );

		if ( QString( previewURL ).isEmpty() )
		{
			QMessageBox::critical( this, "Fatal Error", "Unable to fetch image URL. (Missing or broken)" );
			continue;
		}

		QByteArray imageData {};
		auto fileName = this->downloadImageFromURL( QString( previewURL ), imageData );

		if ( fileName.isEmpty() )
			continue;

		auto tempThumbnailFileDirecotry = QDir::tempPath() + "/" + QString::number( pDetails.m_nPublishedFileId );

		if ( !QDir( tempThumbnailFileDirecotry ).exists() && !QDir().mkpath( tempThumbnailFileDirecotry ) )
		{
			QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + tempThumbnailFileDirecotry );
			continue;
		}

		auto filePath = tempThumbnailFileDirecotry + "/" + fileName;

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

		auto additionalPreviewsCount = SteamUGC()->GetQueryUGCNumAdditionalPreviews( pQuery->m_handle, itemIndex );

		fullUGCDetails.additionalDetails = getAdditionalUGCPreviews( pQuery->m_handle, additionalPreviewsCount, itemIndex, pDetails.m_nPublishedFileId );

		QDateTime itemLastUpdatedTimeStamp = QDateTime::fromSecsSinceEpoch( pDetails.m_rtimeUpdated );
		itemLastUpdatedTimeStamp.setTimeZone( QTimeZone( QByteArray( m_pTimezoneComboBox->currentText().toStdString().c_str() ) ) );

		auto pWorkshopTreeItem = new QTreeWidgetItem( 0 );
		pWorkshopTreeItem->setText( 0, pDetails.m_rgchTitle );
		pWorkshopTreeItem->setText( 1, pDetails.m_pchFileName );
		pWorkshopTreeItem->setText( 2, itemLastUpdatedTimeStamp.toString() );

		pWorkshopTreeItem->setData( 0, Qt::UserRole, pDetails.m_rtimeUpdated );
		pWorkshopTreeItem->setData( 1, Qt::UserRole, itemIndex );

		pWorkshopTreeItem->setTextAlignment( 0, Qt::AlignCenter );
		pWorkshopTreeItem->setTextAlignment( 1, Qt::AlignCenter );
		pWorkshopTreeItem->setTextAlignment( 2, Qt::AlignCenter );
		m_pWorkshopItemTree->addTopLevelItem( pWorkshopTreeItem );

		m_SteamUGCDetailsList.insert( std::make_pair( itemIndex, fullUGCDetails ) );
	}
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
	int deleteMessageBoxReturnType = deleteWarningMessageBox.exec();

	if ( deleteMessageBoxReturnType == QMessageBox::Ok && deleteWarningMessageCheckBox->isChecked() )
	{
		QTreeWidgetItem *pWorkshopItem = this->m_pWorkshopItemTree->selectedItems()[0];
		int workshopItemIndex = pWorkshopItem->data( 1, Qt::UserRole ).toInt();
		FullUGCDetails workshopItemDetails = m_SteamUGCDetailsList.at( workshopItemIndex );
		SteamAPICall_t deleteWorkshopItemCall = SteamUGC()->DeleteItem( workshopItemDetails.standardDetails.m_nPublishedFileId );
		m_CallResultDeleteItem.Set( deleteWorkshopItemCall, this, &CMainView::onDeleteItem );
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

CMainView::AdditionalUGCDetails CMainView::getAdditionalUGCPreviews( UGCQueryHandle_t queryHandle, int previewCount, int itemIndex, PublishedFileId_t fileID )
{
	auto additionalDetails = AdditionalUGCDetails {};

	additionalDetails.previewItemCount = previewCount;

	for ( uint32 i = 0; i < previewCount; i++ )
	{
		char previewContentURL[MAX_URL_SIZE];
		char previewContentFileName[MAX_URL_SIZE];
		EItemPreviewType previewContentType;
		SteamUGC()->GetQueryUGCAdditionalPreview( queryHandle, itemIndex, i,
												  previewContentURL, MAX_URL_SIZE, previewContentFileName,
												  MAX_URL_SIZE, &previewContentType );

		if ( previewContentType == k_EItemPreviewType_Image )
		{
			QByteArray imageData {};

			auto previewFileName = this->downloadImageFromURL( previewContentURL, imageData );

			if ( previewFileName.isEmpty() )
				return AdditionalUGCDetails {};

			auto tempPreviewDirectory = QDir::tempPath() + "/" + QString::number( fileID );

			if ( !QDir( tempPreviewDirectory ).exists() && !QDir().mkpath( tempPreviewDirectory ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + tempPreviewDirectory );
				return AdditionalUGCDetails {};
			}

			tempPreviewDirectory += "/additional/";

			if ( !QDir( tempPreviewDirectory ).exists() && !QDir().mkpath( tempPreviewDirectory ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + tempPreviewDirectory );
				return AdditionalUGCDetails {};
			}

			quint32 uniqueFolderName = QRandomGenerator::global()->generate();
			tempPreviewDirectory += QString::number( uniqueFolderName ) + "/";

			if ( !QDir( tempPreviewDirectory ).exists() && !QDir().mkpath( tempPreviewDirectory ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to create directory. (Permission Denied)\n" + tempPreviewDirectory );
				return AdditionalUGCDetails {};
			}

			auto fullPreviewFilePath = tempPreviewDirectory + "/" + previewFileName;

			if ( !CMainView::isFileWritable( fullPreviewFilePath ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to download additional images. (Permission Denied)\n" + fullPreviewFilePath );
				return AdditionalUGCDetails {};
			}

			auto additionalImageFile = QFile( fullPreviewFilePath );
			additionalImageFile.open( QFile::WriteOnly );

			if ( !additionalImageFile.write( imageData ) )
			{
				QMessageBox::critical( this, "Fatal Error", "Unable to download additional images. (Permission Denied)\n" + fullPreviewFilePath );
				return AdditionalUGCDetails {};
			}
			additionalImageFile.close();

			auto fileInfoList = QStringList {};
			fileInfoList.append( previewContentFileName );
			fileInfoList.append( fullPreviewFilePath );

			additionalDetails.imagePaths.append( fileInfoList );
		}
		
		if ( previewContentType == k_EItemPreviewType_YouTubeVideo )
			additionalDetails.videoURLs.append( previewContentURL );

	}

	return additionalDetails;
}

QString CMainView::downloadImageFromURL( const QString &imageUrl, QByteArray &imageData )
{
	QNetworkAccessManager networkAccessManager;
	QNetworkReply *pNetworkReply = networkAccessManager.get( QNetworkRequest( QUrl( imageUrl ) ) );

	QEventLoop awaitDownloadEventLoop;
	connect( &networkAccessManager, SIGNAL( finished( QNetworkReply * ) ), &awaitDownloadEventLoop, SLOT( quit() ) );

	QTimer downloadTimeoutTimer;
	static constexpr const uint32 TIMEOUT_TIME = 10 * 1000; // 10 seconds.
	downloadTimeoutTimer.start( TIMEOUT_TIME );
	connect( &downloadTimeoutTimer, SIGNAL( timeout() ), &awaitDownloadEventLoop, SLOT( quit() ) );
	awaitDownloadEventLoop.exec();

	QRegularExpression downloadFilenameRegex( R"(filename[^;=\n]*=(?<fileName>(['"]).*?\2|[^;\n]*))" );
	auto downloadFilenameRegexMatchResult = downloadFilenameRegex.match( pNetworkReply->rawHeader( "Content-Disposition" ) );
	auto downloadFileName = downloadFilenameRegexMatchResult.captured( "fileName" ).replace( "UTF-8''", "" ).replace( R"(")", "" );

	QImageReader downloadImageVerificationImageReader( pNetworkReply );
	if ( pNetworkReply->error() != QNetworkReply::NetworkError::NoError || !downloadImageVerificationImageReader.canRead() )
	{
		QMessageBox::critical( this, "Failed Download", ( QString( "Failed retrieve image : " ) + imageUrl + "\nNetwork error code: " + QString::number( pNetworkReply->error() ) + "\nImage reader error code: " + QString::number( downloadImageVerificationImageReader.error() ) ) );
		return "";
	}

	imageData = pNetworkReply->readAll();

	pNetworkReply->deleteLater();

	return downloadFileName;
}

bool CMainView::isFileWritable( const QString &fullPath )
{
	auto writableFileInfo = QFileInfo( fullPath );
	auto writableFilePathInfo = QFileInfo( writableFileInfo.path() );

	return ( writableFilePathInfo.isWritable() || writableFileInfo.isWritable() );
}