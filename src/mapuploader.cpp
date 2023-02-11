#include "mapuploader.h"

#include "bspentityelementparser.h"
#include "bspfilestructure.h"
#include "filedialogpreset.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
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

	auto pBrowseButton = new QPushButton( tr( "Browse..." ), this );
	pBrowseButton->setFixedSize( 239, 20 );

	pDialogLayout->addWidget( pBrowseButton, 2, 0 );

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

	m_pFileEntry = new QLineEdit( this );
	m_pFileEntry->setReadOnly( true );

	pFileLayout->addWidget( m_pFileEntry );

	auto pBrowseButton2 = new QPushButton( tr( "Browse..." ), this );

	pFileLayout->addWidget( pBrowseButton2 );

	pTitleDescLayout->addLayout( pFileLayout );

	pDialogLayout->addLayout( pTitleDescLayout, 0, 1, 4, 1, Qt::AlignLeft );

	pDialogLayout->setAlignment( Qt::AlignTop );

	connect( pBrowseButton2, &QPushButton::clicked, this, [this]
			 {
				 QString filePath = QFileDialog::getOpenFileName( this, "Open", "./", "*.bsp", nullptr, FILE_PICKER_OPTS );
				 QStringList tagList {};
				if( this->retrieveBSP( filePath, tagList ))
				 {
					 foreach( auto tag, tagList )
						 m_pAdvancedOptions->addTagWidgetItem( tag, !(tag == "Singleplayer" || tag == "Cooperative") );
					 m_pAdvancedOptionsButton->setEnabled(true);
				 }
			 } );

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
				 if ( m_isEditing )
					 this->createNewWorkshopItem();
				 else
					 this->updateWorkshopItem( m_PublishedFileId );
			 } );
	connect( pCloseButton, &QPushButton::clicked, this, [this]
			 {
				 this->close();
			 } );
}

void CMapUploader::setEditItem( const CMainView::FullUGCDetails &itemDetails )
{
	this->setWindowTitle( "Edit existing workshop map" );
	QPixmap tempMap = QPixmap( itemDetails.thumbnailDetails );
	tempMap = tempMap.scaled( 239, 134, Qt::IgnoreAspectRatio );
	auto standardDetails = itemDetails.standardDetails;
	m_pAdvancedOptionsButton->setEnabled( true );
	m_pAdvancedOptionsButton->setToolTip( "" );
	m_pPreviewImageLabel->setPixmap( tempMap );
	m_pSteamToSAgreement->setChecked( true );
	m_pOKButton->setText( "Update" );
	m_pTitleLine->setText( standardDetails.m_rgchTitle );
	m_pDescLine->setText( standardDetails.m_rgchDescription );
	m_pFileEntry->setText( standardDetails.m_pchFileName );
	m_PublishedFileId = standardDetails.m_nPublishedFileId;
	m_pAdvancedOptions->setEditItem( itemDetails );
	m_isEditing = true;
}

void CMapUploader::createNewWorkshopItem()
{
	SteamAPICall_t hApiCreateItemHandle = SteamUGC()->CreateItem( CMainView::m_GameID, k_EWorkshopFileTypeCommunity );
	m_CallResultCreateItem.Set( hApiCreateItemHandle, this, &CMapUploader::createWorkshopItemResult );
}

void CMapUploader::createWorkshopItemResult( CreateItemResult_t *pItem, bool bFailure )
{
	if ( bFailure )
	{
		QString errMSG = "Your workshop item could not be created, Error code: " + QString::number( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult";
		QMessageBox::warning( this, "Item Creation Failure!", errMSG, QMessageBox::Ok );
		return;
	}

	updateWorkshopItem( pItem->m_nPublishedFileId );
}

void CMapUploader::updateWorkshopItem( PublishedFileId_t publishedFileId )
{
}

bool CMapUploader::retrieveBSP( const QString& path, QStringList &tagList )
{
	auto bspFileInfo = QFileInfo( path );
	if ( !bspFileInfo.isReadable() )
	{
		QMessageBox::critical( this, "Fatal Error", "Unable to read BSP (Permission Denied)", QMessageBox::Ok );
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


	return CMapUploader::getTagsFromEntityStringList( entityStringList, tagList );

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

bool CMapUploader::getTagsFromEntityStringList( const QStringList &entityList, QStringList &tagList )
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

		auto &elementEntitiesKeyValue = elementKeyValues->Get( "entities" );

		for ( int i = 0; i < elementEntitiesKeyValue.childCount; i++ )
		{
			auto &elementEntityKeyValue = elementEntitiesKeyValue[i];
			if ( QString( elementEntityKeyValue.key.string ).compare( entityClassName ) != 0 )
				continue;

			bool tagSuffices = false;
			if ( elementEntityKeyValue.childCount != 1  )
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

	auto pPatchNoteTextEdit = new QTextEdit( this );

	pPatchNoteLayout->addWidget( pPatchNoteTextEdit );

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

	connect( m_pMediaListWidget, &QListWidget::itemDoubleClicked, this, [this]( QListWidgetItem *item )
			 {
				 if ( item->type() == ADD_IMAGE )
				 {
					 auto imageFilePath = QFileDialog::getOpenFileName( this, "Image File", "./", "(Images) *.png *.jpg *.jpeg", nullptr, FILE_PICKER_OPTS );
					 auto imageFileInfo = QFileInfo( imageFilePath );
					 this->addImageWidgetItem( imageFileInfo.fileName(), imageFilePath );
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

void CAdvancedOptionsDialog::populateDefaultTagListWidget()
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

	pSimpleVideoAddLayout->addWidget( pAddVideoLineEdit, 0, 0 );

	auto pAddVideoButtonBox = new QDialogButtonBox( pSimpleVideoAddDialog );
	pAddVideoButtonBox->addButton( "Add", QDialogButtonBox::ApplyRole );
	pAddVideoButtonBox->addButton( "Cancel", QDialogButtonBox::RejectRole );
	pSimpleVideoAddLayout->addWidget( pAddVideoButtonBox, 1, 0, Qt::AlignLeft );

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
	int insertRow = m_pMediaListWidget->indexFromItem( m_pImageVideoSeparator ).row();
	m_pMediaListWidget->insertItem( insertRow, pAddVideoItem );
	m_pMediaListWidget->setItemWidget( pAddVideoItem, pBaseWidget );
}

void CAdvancedOptionsDialog::addTagWidgetItem( const QString &name, bool removable )
{
	auto pBaseWidget = new QWidget( this );
	auto pAddTagItem = createBasicListWidgetItem( name, pBaseWidget, m_pTagsListWidget, TAG, removable );
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
