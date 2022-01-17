#include "dialogs/P2MapPublisher.h"

using namespace ui;

CP2MapPublisher::CP2MapPublisher( QWidget *pParent ) :
	CP2MapPublisher( pParent, false )
{
}

CP2MapPublisher::CP2MapPublisher( QWidget *pParent, bool edit ) :
	QDialog( pParent )
{
	this->setWindowTitle( tr( "Publish File" ) );
	m_edit = edit;
	m_bspHasPTIInstance = false;
	auto pDialogLayout = new QGridLayout( this );
	auto pFindLabel = new QLabel( tr( "Preview Image:" ), this );

	QPixmap tempMap = QPixmap( ":/zoo_textures/SampleImage.png" );
	tempMap = tempMap.scaled( 478.5 / 2, 269.5 / 2, Qt::IgnoreAspectRatio );
	m_pPreviewImage = &tempMap;
	pImageLabel = new QLabel( this );
	pImageLabel->setPixmap( *m_pPreviewImage );
	m_advancedOptionsWindow = new QDialog( this );
	AO = new CP2PublisherAdvancedOptions();
	AO->setupUi( m_advancedOptionsWindow );

	auto pBrowseButton = new QPushButton( this );
	pBrowseButton->setText( tr( "Browse..." ) );
	pBrowseButton->setFixedSize( 478.5 / 2, 20 );

	auto pAdvancedOptionsButton = new QPushButton( this );
	pAdvancedOptionsButton->setText( tr( "Advanced Options." ) );
	pAdvancedOptionsButton->setFixedSize( 478.5 / 2, 40 );

	pSteamToSAgreement = new QCheckBox( tr( "I accept the terms of the Steam Workshop Contribution Agreement." ), this );

	auto pAgreementButton = new QPushButton( this );
	pAgreementButton->setText( tr( "View Agreement" ) );
	pAgreementButton->setFixedSize( 478.5 / 2, 20 );

	auto pButtonBox = new QDialogButtonBox( Qt::Orientation::Horizontal, this );
	auto pOKButton = pButtonBox->addButton( tr( m_edit ? "Update" : "Upload" ), QDialogButtonBox::ApplyRole );
	auto pCloseButton = pButtonBox->addButton( tr( "Cancel" ), QDialogButtonBox::RejectRole );

	auto pTitleDescLayout = new QVBoxLayout( this );
	pTitleDescLayout->setSpacing( 10 );
	auto pTitle = new QLabel( tr( "Title:" ), this );
	pTitleLine = new QLineEdit( this );
	auto pDesc = new QLabel( tr( "Description:" ), this );
	pDescLine = new QTextEdit( this );
	pDescLine->setMinimumHeight( 100 );
	pDescLine->setMaximumHeight( 100 );
	auto pFile = new QLabel( tr( "File:" ), this );

	auto pFileLayout = new QHBoxLayout( this );
	pFileEntry = new QLineEdit( this );
	pFileEntry->setReadOnly( true );
	auto pBrowseButton2 = new QPushButton( tr( "Browse..." ), this );
	pFileLayout->addWidget( pFileEntry );
	pFileLayout->addWidget( pBrowseButton2 );

	pTitleDescLayout->addWidget( pTitle, 0, Qt::AlignTop );
	pTitleDescLayout->addWidget( pTitleLine, 0, Qt::AlignTop );
	pTitleDescLayout->addWidget( pDesc, 0, Qt::AlignTop );
	pTitleDescLayout->addWidget( pDescLine, 0, Qt::AlignTop );
	pTitleDescLayout->addWidget( pFile, 0, Qt::AlignTop );
	pTitleDescLayout->addLayout( pFileLayout, 0 );

	pDialogLayout->setHorizontalSpacing( 25 );

	pDialogLayout->addWidget( pFindLabel, 0, 0 );
	pDialogLayout->addWidget( pImageLabel, 1, 0 );
	pDialogLayout->addWidget( pBrowseButton, 2, 0 );
	pDialogLayout->addWidget( pAdvancedOptionsButton, 3, 0 );
	pDialogLayout->addWidget( pSteamToSAgreement, 9, 0, 1, 2, Qt::AlignBottom );
	pDialogLayout->addWidget( pAgreementButton, 10, 0, Qt::AlignBottom );
	pDialogLayout->addWidget( pButtonBox, 12, 0, 1, 2, Qt::AlignLeft );
	pDialogLayout->addLayout( pTitleDescLayout, 0, 1, 8, 8, Qt::AlignTop );

	connect( pBrowseButton, &QPushButton::pressed, this, &CP2MapPublisher::OpenImageFileExplorer );
	connect( pBrowseButton2, &QPushButton::pressed, this, &CP2MapPublisher::OpenBSPFileExplorer );
	connect( pOKButton, &QPushButton::pressed, this, &CP2MapPublisher::onOKPressed );
	connect( pCloseButton, &QPushButton::pressed, this, &CP2MapPublisher::onClosePressed );
	connect( pAdvancedOptionsButton, &QPushButton::pressed, this, &CP2MapPublisher::onAdvancedClicked );
	connect( pAgreementButton, &QPushButton::pressed, this, &CP2MapPublisher::onAgreementButtonPressed );

	this->setLayout( pDialogLayout );
	this->setFixedSize( this->sizeHint() );
	this->setWindowFlag( Qt::WindowContextHelpButtonHint, false );
}

void CP2MapPublisher::onAdvancedClicked()
{
	AO->label_5->setEnabled( m_edit );
	AO->textEdit->setEnabled( m_edit );
	m_advancedOptionsWindow->exec();
}

void CP2MapPublisher::LoadCreatingItem()
{
	SteamAPICall_t hApiCreateItemHandle = SteamUGC()->CreateItem( CP2MapMainMenu::ConsumerID, k_EWorkshopFileTypeCommunity );
	m_CallResultCreateItem.Set( hApiCreateItemHandle, this, &CP2MapPublisher::OnCreateItem );
	StartLoopCall();
}

void CP2MapPublisher::UpdateItem( PublishedFileId_t itemID )
{
	if ( pTitleLine->text().isEmpty() )
	{
		QMessageBox::warning( this, "File Required!", "You don't have a Title, please insert a title.", QMessageBox::Ok );
		return;
	}

	if ( !defaultFileLocBSP.startsWith( "mymaps/" ) )
	{
		PublishedFileUpdateHandle_t old_API_Handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest( itemID );
		SteamRemoteStorage()->UpdatePublishedFileFile( old_API_Handle, "///home/trico/Documents/test_bsp_upload/mymaps/wall_button_old_aperture.bsp" );
		SteamAPICall_t call = SteamRemoteStorage()->CommitPublishedFileUpdate( old_API_Handle );
		m_CallOldApiResultSubmitItemUpdate.Set( call, this, &CP2MapPublisher::OnOldApiSubmitItemUpdate );
	}
	UGCUpdateHandle_t hUpdateHandle = SteamUGC()->StartItemUpdate( CP2MapMainMenu::ConsumerID, itemID );

	if ( !m_edit || ( m_edit && QString( m_editItemDetails.m_rgchTitle ).compare( pTitleLine->text() ) ) )
		bool bTitleResult = SteamUGC()->SetItemTitle( hUpdateHandle, pTitleLine->text().toStdString().c_str() );

	if ( !pDescLine->toPlainText().isEmpty() )
		if ( !m_edit || ( m_edit && QString( m_editItemDetails.m_rgchDescription ).compare( pDescLine->toPlainText() ) ) )
			bool bDescResult = SteamUGC()->SetItemDescription( hUpdateHandle, "This was a test from an uploader program a friend wrote" );

	ERemoteStoragePublishedFileVisibility visibility; // AO->checkBox->isChecked() ? k_ERemoteStoragePublishedFileVisibilityPrivate : k_ERemoteStoragePublishedFileVisibilityPublic;
	switch ( AO->comboBox->currentIndex() )
	{
		case 0:
			visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
			return;
		case 1:
			visibility = k_ERemoteStoragePublishedFileVisibilityPrivate;
			return;
		case 2:
			visibility = k_ERemoteStoragePublishedFileVisibilityFriendsOnly;
			return;
		case 3:
			visibility = k_ERemoteStoragePublishedFileVisibilityUnlisted;
			return;
		default:
			visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
			return;
	}
	bool bVisibilityResult = SteamUGC()->SetItemVisibility( hUpdateHandle, visibility );

	SteamAPICall_t hApiSubmitItemHandle = SteamUGC()->SubmitItemUpdate( hUpdateHandle, "Test from application" );
	m_CallResultSubmitItemUpdate.Set( hApiSubmitItemHandle, this, &CP2MapPublisher::OnSubmitItemUpdate );
}

void CP2MapPublisher::OnCreateItem( CreateItemResult_t *pItem, bool bFailure )
{
	if ( bFailure )
	{
		std::string errMSG = "Your workshop item could not be created, Error code: " + std::to_string( pItem->m_eResult ) + "\nfor information on this error code: https://partner.steamgames.com/doc/api/steam_api#EResult";
		QMessageBox::warning( this, "Item Creation Failure!", errMSG.c_str(), QMessageBox::Ok );
		return;
	}
	UpdateItem( pItem->m_nPublishedFileId );
}

void CP2MapPublisher::OnSubmitItemUpdate( SubmitItemUpdateResult_t *pItem, bool bFailure )
{
	FinishLoopCall();
}

void CP2MapPublisher::OnOldApiSubmitItemUpdate( RemoteStorageUpdatePublishedFileResult_t *pItem, bool pFailure )
{
	qInfo() << pItem->m_eResult << "\n";
}

void CP2MapPublisher::LoadExistingDetails( SteamUGCDetails_t details )
{
	m_editItemDetails = details;
	pFileEntry->setText( details.m_pchFileName );
	pTitleLine->setText( details.m_rgchTitle );
	pDescLine->setText( details.m_rgchDescription );

	std::string dir = QDir::currentPath().toStdString() + "/resources/" + std::to_string( details.m_nPublishedFileId ) + "_Image0.png";
	SteamAPICall_t res = SteamRemoteStorage()->UGCDownloadToLocation( details.m_hPreviewFile, dir.c_str(), 0 );
	m_CallOldApiResultSubmitItemDownload.Set( res, this, &CP2MapPublisher::OnOldApiSubmitItemDownload );
	StartLoopCall();
}

void CP2MapPublisher::OnOldApiSubmitItemDownload( RemoteStorageDownloadUGCResult_t *pItem, bool pFailure )
{
	std::string filepath = QDir::currentPath().toStdString() + "/resources/" + std::to_string( m_editItemDetails.m_nPublishedFileId ) + "_Image0.png";
	QPixmap tempMap = QPixmap( filepath.c_str() );
	if ( tempMap.isNull() )
		tempMap = QPixmap( ":/zoo_textures/InvalidImage.png" );
	tempMap = tempMap.scaled( 239.25, 134.75, Qt::IgnoreAspectRatio );
	m_pPreviewImage = &tempMap;
	pImageLabel->setPixmap( *m_pPreviewImage );
	FinishLoopCall();
}

void CP2MapPublisher::onAgreementButtonPressed()
{
	QDesktopServices::openUrl( QUrl( "https://store.steampowered.com/subscriber_agreement/" ) );
}

void CP2MapPublisher::OpenImageFileExplorer()
{
	auto opts = QFileDialog::Option::DontUseNativeDialog;
	QString filePath = QFileDialog::getOpenFileName( this, "Open", defaultFileLocIMG, "*.tga *.bmp *.png *.jpeg", nullptr, opts );
	if ( filePath.isEmpty() )
		return;
	defaultFileLocIMG = filePath;
	QPixmap tempMap = QPixmap( filePath );
	if ( tempMap.isNull() )
	{
		defaultFileLocIMG = "InvalidImage.png";
		tempMap = QPixmap( ":/zoo_textures/InvalidImage.png" );
	};
	tempMap = tempMap.scaled( 239.25, 134.75, Qt::IgnoreAspectRatio );
	m_pPreviewImage = &( tempMap );
	pImageLabel->setPixmap( *m_pPreviewImage );
}

void CP2MapPublisher::OpenBSPFileExplorer()
{
	auto opts = QFileDialog::Option::DontUseNativeDialog;
	QString filePath = QFileDialog::getOpenFileName( this, "Open", defaultFileLocBSP, "*.bsp", nullptr, opts );
	defaultFileLocBSP = filePath;
	if ( filePath.isEmpty() )
		return;
	if ( !filePath.endsWith( ".bsp" ) )
	{ // sanity check, this shouldn't be possible.
		QMessageBox::warning( this, "Invalid File Selected!", "You don't have a BSP selected! Please select a valid BSP", QMessageBox::Ok );
		return;
	}
	QFile file( filePath );
	if ( !file.open( QIODevice::ReadOnly ) )
	{
		QMessageBox::warning( this, "File Not Available!", "This BSP is not available, could not be read..." );
		return;
	}
	if ( file.size() > 209715200 )
	{
		QMessageBox::warning( this, "File Too Large!", "This BSP is too large, max 200MB." );
		return;
	}
	QDataStream stream { &file };
	char *lmpData = new char();
	QByteArray bArray( file.size(), 0 );
	qint32 bytes = stream.readRawData( bArray.data(), bArray.size() );
	lmpData = bArray.data();
	BSPHeaderStruct_t *castedLump = reinterpret_cast<BSPHeaderStruct_t *>( lmpData );
	qInfo() << castedLump->m_version;
	if ( castedLump->m_version != 21 )
	{
		QMessageBox::warning( this, "Invalid BSP", "Invalid BSP.\nEither the file is corrupt or the map is not for Portal 2.\n(only works for BSP version 21)" );
		return;
	}
	QString Entities = strstr( lmpData + castedLump->lumps[0].fileOffset, "\0" );
	if ( !Entities.contains( "@relay_pti_level_end" ) && !AO->checkBox_3->isChecked() )
	{
		m_bspHasPTIInstance = false;
		QDialog *dialog = new QDialog( this );
		PTIDialogSetup *PTI = new PTIDialogSetup();
		PTI->setupUi( dialog );
		dialog->exec();
		return;
	}
	m_bspHasPTIInstance = true;
	pFileEntry->setText( filePath );
}

void CP2MapPublisher::onOKPressed()
{
	if ( !defaultFileLocBSP.endsWith( ".bsp" ) )
	{
		QMessageBox::warning( this, "No Map Found!", "You don't have a BSP selected! Please select a valid BSP", QMessageBox::Ok );
		return;
	}
	if ( !pSteamToSAgreement->isChecked() )
	{
		QMessageBox::warning( this, "Steam Workshop Contribution Agreement", "You need to accept the Steam Workshop Contribution Agreement to upload your map!", QMessageBox::Ok );
		return;
	}
	QMessageBox::StandardButton reply = QMessageBox::question( this, "Upload Map?", "Are you sure you want to upload " + defaultFileLocBSP + "?", QMessageBox::Yes | QMessageBox::No );
	if ( reply != QMessageBox::Yes )
	{
		return;
	}
	QFile file( defaultFileLocBSP );
	if ( !file.open( QIODevice::ReadOnly ) )
	{
		QMessageBox::warning( this, "File Not Available!", "This BSP is not available, could not be read..." );
		return;
	}
	if ( file.size() > 209715200 )
	{
		QMessageBox::warning( this, "File Too Large!", "This BSP is too large, max 200MB." );
		return;
	}
	QDataStream stream { &file };
	char *lmpData = new char();
	QByteArray bArray( file.size(), 0 );
	qint32 bytes = stream.readRawData( bArray.data(), bArray.size() );
	lmpData = bArray.data();
	BSPHeaderStruct_t *castedLump = reinterpret_cast<BSPHeaderStruct_t *>( lmpData );
	if ( castedLump->m_version != 21 )
	{
		QMessageBox::warning( this, "Invalid BSP", "Invalid BSP.\nEither the file is corrupt or the map is not for Portal 2.\n(only works for BSP version 21)" );
		return;
	}
	QString Entities = strstr( lmpData + castedLump->lumps[0].fileOffset, "\0" );
	if ( !Entities.contains( "@relay_pti_level_end" ) && !AO->checkBox_3->isChecked() )
	{
		QDialog *dialog = new QDialog( this );
		PTIDialogSetup *PTI = new PTIDialogSetup();
		PTI->setupUi( dialog );
		dialog->exec();
		return;
	}
	if ( m_edit )
		LoadEditItem();
	else
		LoadCreatingItem();
}

void CP2MapPublisher::LoadEditItem()
{
	UpdateItem( m_editItemDetails.m_nPublishedFileId );
}

void CP2MapPublisher::onClosePressed()
{
	this->close();
}