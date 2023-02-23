#include "mapuploader.h"

#include "bspentityelementparser.h"
#include "bspfilestructure.h"
#include "filedialogpreset.h"

#include <QBuffer>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QImageReader>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QTextEdit>

using namespace ui;

CMapUploader::CMapUploader( QWidget *pParent ) :
	QDialog( pParent )
{
	this->setWindowTitle( "Upload new workshop map" );

	auto pDialogLayout = new QGridLayout( this );

	auto pPreviewTextLabel = new QLabel( tr( "Preview Image:" ), this );

	pDialogLayout->addWidget( pPreviewTextLabel, 0, 0 );

	QPixmap tempMap = QPixmap( ":/resource/SampleImage.png" );
	tempMap = tempMap.scaled( 239, 134, Qt::IgnoreAspectRatio );

	m_pPreviewImageLabel = new QLabel( this );
	m_pPreviewImageLabel->setPixmap( tempMap );
	m_pPreviewImageLabel->setFixedSize( 239, 134 );

	pDialogLayout->addWidget( m_pPreviewImageLabel, 1, 0 );

	auto pThumbnailBrowseButton = new QPushButton( tr( "Browse..." ), this );
	pThumbnailBrowseButton->setFixedSize( 239, 20 );

	pDialogLayout->addWidget( pThumbnailBrowseButton, 2, 0 );

	m_pAdvancedOptionsButton = new QPushButton( tr( "Advanced Options." ), this );
	m_pAdvancedOptionsButton->setFixedSize( 239, 40 );
	m_pAdvancedOptionsButton->setDisabled( true );
	m_pAdvancedOptionsButton->setToolTip( "You need to select a BSP before you can open this menu." );

	m_pAdvancedOptions = new CAdvancedOptionsDialog( this );

	pDialogLayout->addWidget( m_pAdvancedOptionsButton, 3, 0 );

	m_pSteamToSAgreement = new QCheckBox( tr( "I accept the terms of the Steam Workshop Contribution Agreement." ), this );

	pDialogLayout->addWidget( m_pSteamToSAgreement, 4, 0, 1, 2 );

	auto pAgreementButton = new QPushButton( tr( "View Agreement" ), this );
	pAgreementButton->setFixedSize( 239, 20 );

	pDialogLayout->addWidget( pAgreementButton, 5, 0 );

	auto pButtonBox = new QDialogButtonBox( Qt::Orientation::Horizontal, this );
	m_pOKButton = pButtonBox->addButton( tr( "Upload" ), QDialogButtonBox::ApplyRole );
	auto pCloseButton = pButtonBox->addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );
	pDialogLayout->addWidget( pButtonBox, 6, 0, Qt::AlignLeft );

	auto pTitleDescLayout = new QVBoxLayout();
	pTitleDescLayout->setSpacing( 10 );

	auto pTitle = new QLabel( tr( "Title:" ), this );

	pTitleDescLayout->addWidget( pTitle );

	m_pTitleLine = new QLineEdit( this );

	pTitleDescLayout->addWidget( m_pTitleLine );

	auto pDesc = new QLabel( tr( "Description:" ), this );

	pTitleDescLayout->addWidget( pDesc );

	m_pDescLine = new QTextEdit( this );
	m_pDescLine->setMinimumHeight( 100 );
	m_pDescLine->setMaximumHeight( 100 );

	pTitleDescLayout->addWidget( m_pDescLine );

	auto pFile = new QLabel( tr( "File:" ), this );

	pTitleDescLayout->addWidget( pFile );

	auto pFileLayout = new QHBoxLayout();

	m_pBSPFileEntry = new QLineEdit( this );
	m_pBSPFileEntry->setReadOnly( true );

	pFileLayout->addWidget( m_pBSPFileEntry );

	auto pBSPBrowseButton = new QPushButton( tr( "Browse..." ), this );

	pFileLayout->addWidget( pBSPBrowseButton );

	pTitleDescLayout->addLayout( pFileLayout );

	pDialogLayout->addLayout( pTitleDescLayout, 0, 1, 4, 1, Qt::AlignLeft );

	pDialogLayout->setAlignment( Qt::AlignTop );

	connect( pThumbnailBrowseButton, &QPushButton::clicked, this, &CMapUploader::onBrowseThumbnailClicked );

	connect( pBSPBrowseButton, &QPushButton::clicked, this, &CMapUploader::onBrowseBSPClicked );

	connect( pAgreementButton, &QPushButton::clicked, this, []
			 {
				 QDesktopServices::openUrl( QUrl( "https://store.steampowered.com/subscriber_agreement/" ) );
			 } );

	connect( m_pAdvancedOptionsButton, &QPushButton::clicked, this, [this]
			 {
				 m_pAdvancedOptions->exec();
			 } );

	connect( m_pOKButton, &QPushButton::clicked, this, [this]
			 {
				 //
				 QString err {};

				 if ( !this->canUploadProceed( err ) )
				 {
					 QMessageBox::warning( this, "Something's missing!", err );
					 return;
				 }

				 this->setDisabled( true );

				 if ( !m_IsEditing )
					 this->createNewWorkshopItem();
				 else
				 {
					 if ( m_EditedBSP )
						 if ( !this->updateBSPWithOldWorkshop( m_PublishedFileId ) )
						 {
							 QMessageBox::critical( this, "Fatal Error", "Something went wrong with the uploading of the BSP, make sure the BSP exists and is valid." );
							 this->setEnabled( true );
						 }
					 this->updateWorkshopItem( m_PublishedFileId );
				 }
			 } );
	connect( pCloseButton, &QPushButton::clicked, this, [this]
			 {
				 this->close();
			 } );
}

void CMapUploader::onBrowseBSPClicked()
{
	QString filePath = QFileDialog::getOpenFileName( this, "Open", "./", "*.bsp", nullptr, FILE_PICKER_OPTS );

	if ( filePath.isEmpty() )
		return;

	QStringList tagList {};

	if ( !this->retrieveBSP( filePath, tagList, m_MeetsPTIRequirements ) )
		return;

	if ( !m_MeetsPTIRequirements )
	{
		QString noticeMessage = "Unable to locate PTI instances. "
								"Please be advised that although you are able to upload without one "
								"by checking the corresponding checkbox in the advanced options. "
								"It's strongly advised to include the instance, as the workshop systems "
								"may not function without it.\n"
								"What is a PTI Instance? Click here: <a href='http://www.thinkwithportals.com/puzzlemaker/workshop_compatibility.php'>PTI Instances</a>";

		QMessageBox msgBox( QMessageBox::Information, "Notice", noticeMessage, QMessageBox::NoButton, this );
		msgBox.setTextFormat( Qt::RichText );
		msgBox.exec();
	}

	m_pAdvancedOptions->m_pTagsListWidget->clear();

	m_pAdvancedOptions->populateDefaultTagListWidget();

	foreach( auto tag, tagList )
		m_pAdvancedOptions->addTagWidgetItem( tag, !( tag == "Singleplayer" || tag == "Cooperative" ) );

	m_pAdvancedOptionsButton->setEnabled( true );
	m_pBSPFileEntry->setText( filePath );
	m_EditedBSP = true;
}

void CMapUploader::onBrowseThumbnailClicked()
{
	QString filePath = QFileDialog::getOpenFileName( this, "Open", "./", "*.png *.jpg", nullptr, FILE_PICKER_OPTS );

	SteamImageProcessError processError;
	auto thumbnailScaledFilepath = processImageForSteamUpload( filePath, processError, true, 1914, 1078, MAX_IMAGE_SIZE );

	switch ( processError )
	{
		case SteamImageProcessError::FILE_TOO_LARGE:
			QMessageBox::critical( this, "Fatal Error", "The file is too large to post to the Steam Workshop.\n We do attempt to reduce quality to fit the image, but were unable to get it down far enough." );
			break;
		case SteamImageProcessError::FILE_DIRECTORY_CREATE_ERROR:
			QMessageBox::critical( this, "Fatal Error", "Unable to create temp thumbnail folder. (Permission Denied)\n" + thumbnailScaledFilepath );
			break;
		case SteamImageProcessError::FILE_SAVE_ERROR:
			QMessageBox::critical( this, "Fatal Error", "Unable to store scaled thumbnail. (Permission Denied)\n" + thumbnailScaledFilepath );
			break;
		default:
			break;
	}

	m_pPreviewImageLabel->setPixmap( thumbnailScaledFilepath );
	m_EditedThumbnail = true;
	m_ThumbnailPath = thumbnailScaledFilepath;
}

QString CMapUploader::processImageForSteamUpload( const QString &filePath, SteamImageProcessError &fileError, bool constraints, int width, int height, int size )
{
	fileError = SteamImageProcessError::NO_ERROR;

	auto steamUploadImage = QImage( filePath );

	auto steamUploadImageFileInfo = QFileInfo(filePath);

	if(constraints)
	{
		steamUploadImage = steamUploadImage.scaled( width, height, Qt::IgnoreAspectRatio );

		if ( steamUploadImageFileInfo.size() <= MAX_IMAGE_SIZE )
			return filePath;
	}

	QByteArray imageBufferArray {};
	QBuffer imageBuffer(&imageBufferArray );
	imageBuffer.open(QIODevice::WriteOnly);

	if( steamUploadImageFileInfo.size() > MAX_IMAGE_SIZE)
	{
		steamUploadImage.save(&imageBuffer, "JPG");
		for(int i = 95; imageBufferArray.size() > MAX_IMAGE_SIZE && i > 0; i-=5)
		{
			imageBufferArray.clear();
			steamUploadImage.save(&imageBuffer, "JPG", i);
		}

		if( imageBufferArray.size() > MAX_IMAGE_SIZE)
		{
			fileError = SteamImageProcessError::FILE_TOO_LARGE;
			imageBuffer.close();
			return "";
		}

		steamUploadImage = QImage::fromData( imageBufferArray, "JPG");
	}

	imageBuffer.close();

	auto thumbnailScaledDirectory = QDir::tempPath() + "/uploader_temp_images/";

	if ( !QDir( thumbnailScaledDirectory ).exists() && !QDir().mkpath( thumbnailScaledDirectory ) )
	{
		fileError = SteamImageProcessError::FILE_DIRECTORY_CREATE_ERROR;
		return thumbnailScaledDirectory;
	}

	quint32 uniqueFolderName = QRandomGenerator::global()->generate();
	thumbnailScaledDirectory += QString::number( uniqueFolderName ) + "/";

	if ( !QDir( thumbnailScaledDirectory ).exists() && !QDir().mkpath( thumbnailScaledDirectory ) )
	{
		fileError = SteamImageProcessError::FILE_DIRECTORY_CREATE_ERROR;
		return thumbnailScaledDirectory;
	}

	auto thumbnailScaledFilepath = thumbnailScaledDirectory + steamUploadImageFileInfo.baseName() + ".jpg";

	if ( !CMainView::isFileWritable( thumbnailScaledFilepath ) )
	{
		fileError = SteamImageProcessError::FILE_SAVE_ERROR;
		return thumbnailScaledFilepath;
	}

	if(!steamUploadImage.save( thumbnailScaledFilepath ))
	{
		fileError = SteamImageProcessError::FILE_SAVE_ERROR;
		return thumbnailScaledFilepath;
	}

	return thumbnailScaledFilepath;
}

void CMapUploader::setEditItem( const CMainView::FullUGCDetails &itemDetails )
{
	this->setWindowTitle( "Edit existing workshop map" );
	QPixmap tempMap = QPixmap( itemDetails.thumbnailDetails );
	tempMap = tempMap.scaled( 239, 134, Qt::IgnoreAspectRatio );
	auto standardDetails = itemDetails.standardDetails;
	m_EditPreviewCount = itemDetails.additionalDetails.amount;
	m_pAdvancedOptionsButton->setEnabled( true );
	m_pAdvancedOptionsButton->setToolTip( "" );
	m_pPreviewImageLabel->setPixmap( tempMap );
	m_pSteamToSAgreement->setChecked( true );
	m_pOKButton->setText( "Update" );
	m_pTitleLine->setText( standardDetails.m_rgchTitle );
	m_pDescLine->setText( standardDetails.m_rgchDescription );
	m_pBSPFileEntry->setText( standardDetails.m_pchFileName );
	m_PublishedFileId = standardDetails.m_nPublishedFileId;
	m_pAdvancedOptions->setEditItem( itemDetails );
	m_IsEditing = true;
}

void CMapUploader::createNewWorkshopItem()
{
	SteamAPICall_t hApiCreateItemHandle = SteamUGC()->CreateItem( CMainView::m_GameID, k_EWorkshopFileTypeCommunity );
	m_CallResultCreateItem.Set( hApiCreateItemHandle, this, &CMapUploader::createWorkshopItemResult );
}

void CMapUploader::createWorkshopItemResult( CreateItemResult_t *pItem, bool bFailure )
{
	if ( bFailure || pItem->m_eResult != 1 )
	{
		QString errMSG = "Your workshop item could not be created, Error code: " + QString::number( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult";
		QMessageBox::warning( this, "Item Creation Failure!", errMSG, QMessageBox::Ok );
		return;
	}

	if ( m_EditedBSP )
	{
		if ( !this->updateBSPWithOldWorkshop( pItem->m_nPublishedFileId ) )
		{
			QMessageBox::critical( this, "Fatal Error", "Something went wrong with the uploading of the BSP, make sure the BSP exists and is valid." );
			if ( auto mainWindowParent = dynamic_cast<CMainView *>( this->parent() ) )
			{
				SteamAPICall_t deleteItemCall = SteamUGC()->DeleteItem( pItem->m_nPublishedFileId );
				mainWindowParent->m_CallResultDeleteItem.Set( deleteItemCall, mainWindowParent, &CMainView::onDeleteItem );
			}
			this->setEnabled( true );
		}
	}
	else
		this->updateWorkshopItem( pItem->m_nPublishedFileId );
}

bool CMapUploader::canUploadProceed( QString &errorString ) const
{
	bool canProceed = true;

	if ( !m_IsEditing && !m_EditedBSP )
	{
		errorString += "x | BSP Selected\n";
		canProceed = false;
	}
	else
		errorString += "✓ | BSP Selected\n";

	if ( m_pTitleLine->text().isEmpty() )
	{
		errorString += "x | Has title\n";
		canProceed = false;
	}
	else
		errorString += "✓ | Has title\n";

	if ( !m_IsEditing && m_ThumbnailPath.isEmpty() )
	{
		errorString += "x | Has Thumbnail\n";
		canProceed = false;
	}
	else
		errorString += "✓ | Has Thumbnail\n";

	if ( !m_IsEditing && !m_MeetsPTIRequirements && !m_pAdvancedOptions->m_pPTIInstanceCheckBox->isChecked() )
	{
		errorString += "x | Meets PTI requirements\n";
		canProceed = false;
	}
	else
		errorString += "✓ | Meets PTI requirements\n";

	if(!m_pSteamToSAgreement->isChecked())
	{
		errorString += "x | Agreed to Workshop ToS\n";
		canProceed = false;
	}
	else
		errorString += "✓ | Agreed to Workshop ToS\n";

	return canProceed;
}

bool CMapUploader::updateBSPWithOldWorkshop( PublishedFileId_t publishedFileId )
{
	constexpr const int MAX_BSP_UPLOAD_CHUNK = 1024 * 1024 * 100; // 100mb
	QFile bspFile( m_pBSPFileEntry->text() );

	auto fileName = ( QString( "mymaps/" ) + bspFile.fileName() );

	if ( !bspFile.exists() )
		return false;

	bspFile.open( QFile::ReadOnly );

	UGCFileWriteStreamHandle_t filewritestreamhandle = SteamRemoteStorage()->FileWriteStreamOpen( fileName.toStdString().c_str() );
	QByteArray bspData = bspFile.readAll();

	for ( int j = bspData.size(), i = 0; j > 0; j -= MAX_BSP_UPLOAD_CHUNK, i++ )
	{
		if ( j < MAX_BSP_UPLOAD_CHUNK )
		{
			if ( !SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, bspData.mid( MAX_BSP_UPLOAD_CHUNK * i, ( MAX_BSP_UPLOAD_CHUNK * ( i + 1 ) ) ).constData(), j ) )
				return false;
			continue;
		}
		if ( !SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, bspData.mid( MAX_BSP_UPLOAD_CHUNK * i, ( MAX_BSP_UPLOAD_CHUNK * ( i + 1 ) ) ).constData(), MAX_BSP_UPLOAD_CHUNK ) )
			return false;
	}

	if ( !SteamRemoteStorage()->FileWriteStreamClose( filewritestreamhandle ) )
		return false;

	PublishedFileUpdateHandle_t old_API_Handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest( publishedFileId );
	if ( !SteamRemoteStorage()->UpdatePublishedFileFile( old_API_Handle, fileName.toStdString().c_str() ) )
		return false;

	SteamAPICall_t publishFileUpdateCall = SteamRemoteStorage()->CommitPublishedFileUpdate( old_API_Handle );
	m_CallOldApiResultSubmitItemUpdate.Set( publishFileUpdateCall, this, &CMapUploader::updateBSPWithOldWorkshopResult );

	bspFile.close();
	return true;
}

void CMapUploader::updateBSPWithOldWorkshopResult( RemoteStorageUpdatePublishedFileResult_t *pItem, bool bFailure )
{
	if ( bFailure || pItem->m_eResult != 1 )
	{

		QMessageBox::critical( this, "Fatal Error", "Something went wrong with the uploading of the BSP, make sure the BSP exists and is valid, Error code: " + QString::number( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult" );
		if ( auto mainWindowParent = dynamic_cast<CMainView *>( this->parent() ) )
		{
			SteamAPICall_t deleteItemCall = SteamUGC()->DeleteItem( pItem->m_nPublishedFileId );
			mainWindowParent->m_CallResultDeleteItem.Set( deleteItemCall, mainWindowParent, &CMainView::onDeleteItem );
		}
		this->setEnabled( true );
		return;
	}

	this->updateWorkshopItem( pItem->m_nPublishedFileId );
}

void CMapUploader::updateWorkshopItem( PublishedFileId_t publishedFileId )
{
	auto updateItemHandle = SteamUGC()->StartItemUpdate( CMainView::m_GameID, publishedFileId );

	SteamUGC()->SetItemTitle( updateItemHandle, m_pTitleLine->text().toLocal8Bit().constData() );
	if ( !m_pDescLine->toPlainText().isEmpty() )
		SteamUGC()->SetItemDescription( updateItemHandle, m_pDescLine->toPlainText().toLocal8Bit().constData() );

	SteamUGC()->SetItemVisibility( updateItemHandle, static_cast<ERemoteStoragePublishedFileVisibility>( m_pAdvancedOptions->m_pVisibilityComboBox->currentData( Qt::UserRole ).toInt() ) );

	auto charTagVector = std::vector<const char *> {};

	auto m_pSteamTagList = steamTagListPreserver {};

	for ( int i = 0; i < m_pAdvancedOptions->m_pTagsListWidget->count(); i++ )
	{
		QListWidgetItem *item = m_pAdvancedOptions->m_pTagsListWidget->item( i );
		auto text = QString( item->data( Qt::UserRole ).toString() );

		std::shared_ptr<char[]> charUPtr( new char[text.length()] );
		strcpy( charUPtr.get(), text.toLocal8Bit().constData() );
		m_pSteamTagList.m_SharedCharArrayPointerTagList.push_back( charUPtr );
		charTagVector.push_back( charUPtr.get() );
	}

	m_pSteamTagList.m_SteamTagList.m_nNumStrings = (int32)charTagVector.size();
	m_pSteamTagList.m_SteamTagList.m_ppStrings = (const char **)charTagVector.data();

	SteamUGC()->SetItemTags( updateItemHandle, &m_pSteamTagList.m_SteamTagList );

	if ( m_EditedThumbnail )
		SteamUGC()->SetItemPreview( updateItemHandle, m_ThumbnailPath.toLocal8Bit().constData() );

	if ( m_EditPreviewCount > 0 )
		for ( int i = 0; i < m_EditPreviewCount; i++ )
			SteamUGC()->RemoveItemPreview( updateItemHandle, i );

	for ( int i = 0; i < m_pAdvancedOptions->m_pMediaListWidget->count(); ++i )
	{
		QListWidgetItem *item = m_pAdvancedOptions->m_pMediaListWidget->item( i );

		switch ( item->type() )
		{
			case CAdvancedOptionsDialog::IMAGE:
				SteamUGC()->AddItemPreviewFile( updateItemHandle, item->data( Qt::UserRole ).toString().toLocal8Bit().constData(), k_EItemPreviewType_Image );
				break;

			case CAdvancedOptionsDialog::VIDEO:
				SteamUGC()->AddItemPreviewVideo( updateItemHandle, item->data( Qt::UserRole ).toString().toLocal8Bit().constData() );
				break;

			default:
				continue;
		}
	}

	auto updateCall = SteamUGC()->SubmitItemUpdate( updateItemHandle, m_pAdvancedOptions->m_pPatchNoteTextEdit->toPlainText().toLocal8Bit().constData() );
	m_CallResultSubmitItemUpdate.Set( updateCall, this, &CMapUploader::updateWorkshopItemResult );
}

void CMapUploader::updateWorkshopItemResult( SubmitItemUpdateResult_t *pItem, bool bFailure )
{
	if ( bFailure || pItem->m_eResult != 1 )
	{
		QString errMSG = "Your workshop item could not be created, Error code: " + QString::number( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult";
		QMessageBox::warning( this, "Item Creation Failure!", errMSG, QMessageBox::Ok );
		this->setEnabled( true );

		if ( !m_IsEditing )
			if ( auto mainWindowParent = dynamic_cast<CMainView *>( this->parent() ) )
			{
				SteamAPICall_t deleteItemCall = SteamUGC()->DeleteItem( pItem->m_nPublishedFileId );
				mainWindowParent->m_CallResultDeleteItem.Set( deleteItemCall, mainWindowParent, &CMainView::onDeleteItem );
			}
		return;
	}

	this->close();
}

bool CMapUploader::retrieveBSP( const QString &path, QStringList &tagList, bool &ptiRequirements )
{
	ptiRequirements = false;

	auto bspFileInfo = QFileInfo( path );
	if ( !bspFileInfo.isReadable() )
	{
		QMessageBox::critical( this, "Fatal Error", "Unable to read BSP (Permission Denied)\n" + path, QMessageBox::Ok );
		return false;
	}

	auto bspFile = QFile( path );
	bspFile.open( QFile::ReadOnly );

	BSPHeaderStruct_t castedLump {};
	const auto bspByteArray = bspFile.readAll();
	memcpy( &castedLump, bspByteArray.constData(), sizeof( BSPHeaderStruct_t ) );

	if ( castedLump.m_version != 21 )
	{
		QMessageBox::warning( this, "Invalid BSP", "Invalid BSP.\nEither the file is corrupt or the map is not for Portal 2.\n(only works for BSP version 21)" );
		return false;
	}

	QString rawEntityLump = bspByteArray.constData() + castedLump.lumps[0].fileOffset;

	auto entityStringList = QStringList {};

	if ( !CMapUploader::parseBSPEntitiesToStringList( rawEntityLump, entityStringList ) )
	{
		QMessageBox::critical( this, "Invalid BSP", "Invalid BSP.\nThe parser failed to read the entity lump data properly, please recompile the BSP and try again." );
		return false;
	}

	CElementList::ListInitResponse res = CElementList::initialiseElementList();
	if ( res == CElementList::ListInitResponse::FILEINVALID )
		QMessageBox::warning( this, "Invalid KV File", "File elements.kv is Invalid. Using default configuration." );

	return CMapUploader::getTagsFromEntityStringList( entityStringList, tagList, ptiRequirements );
}

bool CMapUploader::parseBSPEntitiesToStringList( const QString &rawEntityLump, QStringList &entityList )
{
	QString entityStack = "";
	bool inQuotes = false;
	int nestCount = 0;
	for ( char character : rawEntityLump.toStdString() )
	{
		entityStack += character;
		if ( character == '"' )
		{
			inQuotes = !inQuotes;
			continue;
		}
		if ( !inQuotes && character == '{' )
			nestCount++;
		if ( !inQuotes && character == '}' )
			nestCount--;
		if ( nestCount < 0 )
			return false;

		if ( nestCount == 0 && character == '}' )
		{
			QString parsable = ( ( R"("entity" )" + entityStack ) );
			entityList.push_back( parsable );
			entityStack = "";
		}
	}
	return true;
}

bool CMapUploader::getTagsFromEntityStringList( const QStringList &entityList, QStringList &tagList, bool &ptiRequirements )
{
	CElementList::initialiseElementList();

	if ( !CElementList::isInitialised() )
	{
		return false;
	}
	KeyValueRoot *elementKeyValues = CElementList::getElementList();

	foreach( auto entityLumpString, entityList )
	{
		auto entityKeyValue = new KeyValueRoot();
		entityKeyValue->Parse( entityLumpString.toStdString().c_str() );
		auto entityClassName = QString( entityKeyValue->Get( "entity" ).Get( "classname" ).value.string );

		if ( entityClassName.compare( "Singleplayer" ) == 0 && tagList.contains( "Cooperative" ) )
			return false;

		if ( entityClassName.compare( "Cooperative" ) == 0 && tagList.contains( "Singleplayer" ) )
			return false;

		if ( entityClassName.compare( "info_player_start" ) == 0 && !tagList.contains( "Singleplayer" ) )
			tagList << "Singleplayer";

		if ( entityClassName.compare( "info_coop_spawn" ) == 0 && !tagList.contains( "Cooperative" ) )
			tagList << "Cooperative";

		if ( QString( entityKeyValue->Get( "entity" ).Get( "targetname" ).value.string ).compare( "@relay_pti_level_end" ) == 0 )
			ptiRequirements = true;

		auto &elementEntitiesKeyValue = elementKeyValues->Get( "entities" );

		for ( int i = 0; i < elementEntitiesKeyValue.childCount; i++ )
		{
			auto &elementEntityKeyValue = elementEntitiesKeyValue[i];
			if ( QString( elementEntityKeyValue.key.string ).compare( entityClassName ) != 0 )
				continue;

			bool tagSuffices = false;
			if ( elementEntityKeyValue.childCount != 1 )
				for ( int j = 0; j < elementEntityKeyValue.childCount; j++ )
				{
					auto &elementEntityValueKeyValue = elementEntityKeyValue[j];
					if ( QString( elementEntityValueKeyValue.key.string ).compare( "tag" ) == 0 )
						continue;
					if ( QString( entityKeyValue->Get( "entity" )[elementEntityValueKeyValue.key.string].value.string ).isEmpty() )
						continue;
					QString a = QString( elementEntityValueKeyValue.value.string );
					QString b = QString( entityKeyValue->Get( "entity" )[elementEntityValueKeyValue.key.string].value.string );

					tagSuffices = a.compare( b, Qt::CaseInsensitive ) == 0;
					if ( !tagSuffices )
						break;
				}
			else
				tagSuffices = true;

			if ( tagSuffices && !tagList.contains( elementEntityKeyValue["tag"].value.string ) )
				tagList << QString( elementEntityKeyValue["tag"].value.string );
		}
	}
	return true;
}

CAdvancedOptionsDialog::CAdvancedOptionsDialog( QWidget *pParent ) :
	QDialog( pParent )
{
	this->setWindowTitle( "Advanced Options" );

	auto pDialogLayout = new QGridLayout( this );

	auto pMediaGroupBox = new QGroupBox( tr( "Media" ), this );

	auto pMediaLayout = new QGridLayout( pMediaGroupBox );

	m_pImagePreviewLabel = new QLabel( this );
	m_pImagePreviewLabel->setMinimumSize( 256, 144 );
	QPixmap tempMap = QPixmap( ":/resource/SampleImage.png" );
	tempMap = tempMap.scaled( 239, 134, Qt::IgnoreAspectRatio );
	m_pImagePreviewLabel->setPixmap( tempMap );
	pMediaLayout->addWidget( m_pImagePreviewLabel, 0, 0 );

	m_pMediaListWidget = new QListWidget( this );
	m_pMediaListWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	this->populateDefaultMediaListWidget();

	pMediaLayout->addWidget( m_pMediaListWidget, 0, 1 );

	pDialogLayout->addWidget( pMediaGroupBox, 0, 0, 1, 4 );

	auto pTagsGroupBox = new QGroupBox( tr( "Tags" ), this );

	auto pTagsLayout = new QGridLayout( pTagsGroupBox );

	m_pTagsListWidget = new QListWidget( this );
	m_pTagsListWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	this->populateDefaultTagListWidget();

	pTagsLayout->addWidget( m_pTagsListWidget, 0, 0 );

	pDialogLayout->addWidget( pTagsGroupBox, 1, 0, 1, 3 );

	m_pPatchNoteGroupBox = new QGroupBox( tr( "Patch Notes" ), this );
	m_pPatchNoteGroupBox->setDisabled( true );

	auto pPatchNoteLayout = new QGridLayout( m_pPatchNoteGroupBox );

	m_pPatchNoteTextEdit = new QTextEdit( this );

	pPatchNoteLayout->addWidget( m_pPatchNoteTextEdit );

	pDialogLayout->addWidget( m_pPatchNoteGroupBox, 1, 3, 1, 1 );

	auto pPTIInstanceVisibilityGroupBox = new QGroupBox( this );

	auto pPTIInstanceVisibilityLayout = new QGridLayout( pPTIInstanceVisibilityGroupBox );

	m_pPTIInstanceCheckBox = new QCheckBox( "Allow upload without PTI Instance", this );
	pPTIInstanceVisibilityLayout->addWidget( m_pPTIInstanceCheckBox, 0, 0, 1, 3, Qt::AlignLeft );

	auto pVisbilityLayout = new QHBoxLayout();

	auto pVisibilityLabel = new QLabel( "Visibility:", this );
	pVisbilityLayout->addWidget( pVisibilityLabel );

	m_pVisibilityComboBox = new QComboBox( this );
	m_pVisibilityComboBox->addItem( "public", k_ERemoteStoragePublishedFileVisibilityPublic );
	m_pVisibilityComboBox->addItem( "private", k_ERemoteStoragePublishedFileVisibilityPrivate );
	m_pVisibilityComboBox->addItem( "friends", k_ERemoteStoragePublishedFileVisibilityFriendsOnly );
	m_pVisibilityComboBox->addItem( "unlisted", k_ERemoteStoragePublishedFileVisibilityUnlisted );

	pVisbilityLayout->addWidget( m_pVisibilityComboBox );

	pPTIInstanceVisibilityLayout->addLayout( pVisbilityLayout, 0, 4, Qt::AlignRight );

	pDialogLayout->addWidget( pPTIInstanceVisibilityGroupBox, 2, 0, 1, 4 );

	m_pCloseBox = new QDialogButtonBox( this );
	auto closeButton = m_pCloseBox->addButton( "Close", QDialogButtonBox::ApplyRole );
	pDialogLayout->addWidget( m_pCloseBox, 3, 0, 1, 4, Qt::AlignLeft );

	connect( closeButton, &QPushButton::clicked, this, &CAdvancedOptionsDialog::close );

	connect( m_pMediaListWidget, &QListWidget::itemDoubleClicked, this, [this]( QListWidgetItem *item )
			 {
				 if ( item->type() == ADD_IMAGE )
				 {
					 auto imageFilePath = QFileDialog::getOpenFileName( this, "Image File", "./", "(Images) *.png *.jpg *.jpeg", nullptr, FILE_PICKER_OPTS );

					 CMapUploader::SteamImageProcessError processError;
					 auto thumbnailScaledFilepath = CMapUploader::processImageForSteamUpload( imageFilePath, processError, false, 0, 0, CMapUploader::MAX_IMAGE_SIZE );

					 switch ( processError )
					 {
						 case CMapUploader::SteamImageProcessError::FILE_TOO_LARGE:
							 QMessageBox::critical( this, "Fatal Error", "The file is too large to post to the Steam Workshop.\n We do attempt to reduce quality to fit the image, but were unable to get it down far enough." );
							 return;
						 case CMapUploader::SteamImageProcessError::FILE_DIRECTORY_CREATE_ERROR:
							 QMessageBox::critical( this, "Fatal Error", "Unable to create temp folder. (Permission Denied)\n" + thumbnailScaledFilepath );
							 return;
						 case CMapUploader::SteamImageProcessError::FILE_SAVE_ERROR:
							 QMessageBox::critical( this, "Fatal Error", "Unable to store scaled image. (Permission Denied)\n" + thumbnailScaledFilepath );
							 return;
						 default:
							 break;
					 }

					 auto imageFileInfo = QFileInfo( thumbnailScaledFilepath );
					 this->addImageWidgetItem( imageFileInfo.fileName(), thumbnailScaledFilepath );
				 }

				 if ( item->type() == ADD_VIDEO )
				 {
					 QString addVideoResult;
					 this->simpleInputDialog( addVideoResult );
					 this->addVideoWidgetItem( addVideoResult );
				 }
			 } );

	connect( m_pMediaListWidget, &QListWidget::itemClicked, this, [this]( QListWidgetItem *item )
			 {
				 if ( item->type() == IMAGE )
					 m_pImagePreviewLabel->setPixmap( QPixmap( ( item->data( Qt::UserRole ).toString() ) ).scaled( 239, 134, Qt::IgnoreAspectRatio ) );
			 } );

	connect( m_pTagsListWidget, &QListWidget::itemDoubleClicked, this, [this]( QListWidgetItem *item )
			 {
				 if ( item->type() == ADD_TAG )
				 {
					 QString addTagResult;

					 this->simpleInputDialog( addTagResult );
					 if ( !addTagResult.isEmpty() )
						 this->addTagWidgetItem( addTagResult );
				 }
			 } );
}

void CAdvancedOptionsDialog::populateDefaultMediaListWidget()
{
	auto pAddVideoItem = new QListWidgetItem( "+ Add Video", nullptr, ADD_VIDEO );
	m_pMediaListWidget->addItem( pAddVideoItem );

	m_pImageVideoSeparator = new QListWidgetItem( nullptr, NO_TYPE );
	m_pImageVideoSeparator->setFlags( Qt::ItemFlag::NoItemFlags );
	m_pMediaListWidget->addItem( m_pImageVideoSeparator );

	auto separatorFrame = new QFrame();
	separatorFrame->setFrameShape( QFrame::HLine );
	m_pMediaListWidget->setItemWidget( m_pImageVideoSeparator, separatorFrame );

	auto pAddImageItem = new QListWidgetItem( "+ Add Image", nullptr, ADD_IMAGE );

	m_pMediaListWidget->addItem( pAddImageItem );
}

void CAdvancedOptionsDialog::populateDefaultTagListWidget() const
{
	auto pAddTagItem = new QListWidgetItem( "+ Add Tag", nullptr, ADD_TAG );
	m_pTagsListWidget->addItem( pAddTagItem );
}

void CAdvancedOptionsDialog::simpleInputDialog( QString &resultString )
{
	auto pSimpleVideoAddDialog = new QDialog( this );
	pSimpleVideoAddDialog->setWindowTitle( "Input" );
	pSimpleVideoAddDialog->setAttribute( Qt::WA_DeleteOnClose );

	auto pSimpleVideoAddLayout = new QGridLayout( pSimpleVideoAddDialog );

	auto pAddVideoLineEdit = new QLineEdit( pSimpleVideoAddDialog );
	pAddVideoLineEdit->setMaxLength( 256 );
	pSimpleVideoAddLayout->addWidget( pAddVideoLineEdit, 0, 0 );

	auto pAddVideoButtonBox = new QDialogButtonBox( pSimpleVideoAddDialog );

	auto pAddButton = pAddVideoButtonBox->addButton( "Add", QDialogButtonBox::ApplyRole );
	pAddButton->setToolTip( "Cannot insert empty tag." );
	pAddButton->setDisabled( true );
	pAddVideoButtonBox->addButton( "Cancel", QDialogButtonBox::RejectRole );

	pSimpleVideoAddLayout->addWidget( pAddVideoButtonBox, 1, 0, Qt::AlignLeft );

	connect( pAddVideoLineEdit, &QLineEdit::textEdited, this, [pAddButton]( const QString &text )
			 {
				 if ( text.toStdString().find_first_not_of( ' ' ) == std::string::npos || text.isEmpty() )
				 {
					 pAddButton->setDisabled( true );
					 pAddButton->setToolTip( "Cannot insert empty tag." );
					 return;
				 }

				 auto remStartSpaces = QString( text ).replace( " ", "" );

				 if ( remStartSpaces.compare( "Singleplayer" ) == 0 || remStartSpaces.compare( "Cooperative" ) == 0 )
				 {
					 pAddButton->setDisabled( true );
					 pAddButton->setToolTip( "Cannot (re)assign Singleplayer/Cooperative tags." );
					 return;
				 }

				 foreach( int ch, text.toStdString() )
					 if ( isprint( ch ) == 0 || char( ch ) == ',' )
					 {
						 pAddButton->setDisabled( true );
						 pAddButton->setToolTip( "Tag must not contain non printable characters or ',' (commas)" );
						 return;
					 }
				 pAddButton->setToolTip( "" );
				 pAddButton->setDisabled( false );
			 } );

	connect( pAddVideoButtonBox, &QDialogButtonBox::clicked, pSimpleVideoAddDialog, [pSimpleVideoAddDialog, pAddVideoLineEdit, &resultString]( QAbstractButton *btn )
			 {
				 if ( btn->text() == "Add" && !pAddVideoLineEdit->text().isEmpty() )
					 resultString = pAddVideoLineEdit->text();
				 pSimpleVideoAddDialog->close();
			 } );

	pSimpleVideoAddDialog->exec();
}

QListWidgetItem *CAdvancedOptionsDialog::createBasicListWidgetItem( const QString &labelText, QWidget *baseWidget, QListWidget *itemList, ListItemTypes type, bool removable )
{
	auto pBasicListItem = new QListWidgetItem( nullptr, type );

	auto pListItemLayout = new QHBoxLayout( baseWidget );

	auto pListItemText = new QLabel( labelText, this );
	pListItemLayout->addWidget( pListItemText, Qt::AlignLeft );

	if ( removable )
	{
		auto pRemoveItemButton = new QPushButton( "X", baseWidget );
		pRemoveItemButton->setFixedSize( 16, 16 );

		connect( pRemoveItemButton, &QPushButton::clicked, baseWidget, [pBasicListItem, itemList]
				 {
					 int insertRow = itemList->indexFromItem( pBasicListItem ).row();
					 auto listItem = itemList->takeItem( insertRow );
					 itemList->removeItemWidget( listItem );
					 delete listItem;
				 } );

		pListItemLayout->addWidget( pRemoveItemButton, Qt::AlignRight );
	}

	pBasicListItem->setSizeHint( QSize( 0, 32 ) );
	baseWidget->setLayout( pListItemLayout );
	return pBasicListItem;
}

void CAdvancedOptionsDialog::addImageWidgetItem( const QString &name, const QString &path, bool removable )
{
	auto pBaseWidget = new QWidget( this );

	auto pAddImageItem = createBasicListWidgetItem( name, pBaseWidget, m_pMediaListWidget, IMAGE, removable );

	pAddImageItem->setData( Qt::UserRole, path );

	int insertRow = m_pMediaListWidget->indexFromItem( m_pImageVideoSeparator ).row();
	m_pMediaListWidget->insertItem( insertRow + 2, pAddImageItem );

	m_pMediaListWidget->setItemWidget( pAddImageItem, pBaseWidget );
}

void CAdvancedOptionsDialog::addVideoWidgetItem( const QString &name, bool removable )
{
	auto pBaseWidget = new QWidget( this );
	auto pAddVideoItem = createBasicListWidgetItem( name, pBaseWidget, m_pMediaListWidget, VIDEO, removable );

	pAddVideoItem->setData( Qt::UserRole, name );

	int insertRow = m_pMediaListWidget->indexFromItem( m_pImageVideoSeparator ).row();
	m_pMediaListWidget->insertItem( insertRow, pAddVideoItem );
	m_pMediaListWidget->setItemWidget( pAddVideoItem, pBaseWidget );
}

void CAdvancedOptionsDialog::addTagWidgetItem( const QString &name, bool removable )
{
	auto pBaseWidget = new QWidget( this );
	auto pAddTagItem = createBasicListWidgetItem( name, pBaseWidget, m_pTagsListWidget, TAG, removable );

	pAddTagItem->setData( Qt::UserRole, name );

	m_pTagsListWidget->addItem( pAddTagItem );
	m_pTagsListWidget->setItemWidget( pAddTagItem, pBaseWidget );
}

void CAdvancedOptionsDialog::setEditItem( const CMainView::FullUGCDetails &itemDetails )
{
	m_pPatchNoteGroupBox->setEnabled( true );

	auto tagList = QString( itemDetails.standardDetails.m_rgchTags ).split( "," );
	foreach( auto tag, tagList )
		addTagWidgetItem( tag, !( tag == "Singleplayer" || tag == "Multiplayer" ) );

	foreach( auto imageInfo, itemDetails.additionalDetails.imagePaths )
		addImageWidgetItem( imageInfo[0], imageInfo[1] );

	foreach( auto videoURL, itemDetails.additionalDetails.videoURLs )
		addVideoWidgetItem( videoURL );

	switch ( itemDetails.standardDetails.m_eVisibility )
	{
		case k_ERemoteStoragePublishedFileVisibilityPublic:
			m_pVisibilityComboBox->setCurrentIndex( 0 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityFriendsOnly:
			m_pVisibilityComboBox->setCurrentIndex( 2 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityPrivate:
			m_pVisibilityComboBox->setCurrentIndex( 1 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityUnlisted:
			m_pVisibilityComboBox->setCurrentIndex( 3 );
			break;
	}
}
