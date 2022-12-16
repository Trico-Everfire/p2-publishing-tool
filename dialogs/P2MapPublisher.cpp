#include "dialogs/P2MapPublisher.h"

#include "P2DialogConfig.h"
#include "P2ElementParser.h"

#include <KeyValue.h>
#include <cmath>
#include <filesystem>
#include <regex>

using namespace ui;

CP2MapPublisher::CP2MapPublisher( QWidget *pParent ) :
	CP2MapPublisher( pParent, false )
{
}

CP2MapPublisher::CP2MapPublisher( QWidget *pParent, bool edit ) :
	QDialog( pParent )
{
	m_EditItemIndex = -1;
	m_editItemDetails = SteamUGCDetails_t();

	this->setWindowTitle( tr( "Publish File" ) );
	m_edit = edit;
	m_bspHasPTIInstance = false;
	auto pDialogLayout = new QGridLayout( this );
	auto pFindLabel = new QLabel( tr( "Preview Image:" ), this );

	QPixmap tempMap = QPixmap( ":/zoo_textures/SampleImage.png" );
	tempMap = tempMap.scaled( 478 / 2, 269 / 2, Qt::IgnoreAspectRatio );
	pImageLabel = new QLabel( this );
	pImageLabel->setPixmap( tempMap );
	pImageLabel->setMaximumSize( 239, 134 );
	m_advancedOptionsWindow = new QDialog( this );
	AO = new CP2PublisherAdvancedOptions();
	AO->setupUi( m_advancedOptionsWindow );
	AO->treeWidget->setSortingEnabled( false );

	auto pBrowseButton = new QPushButton( this );
	pBrowseButton->setText( tr( "Browse..." ) );
	pBrowseButton->setFixedSize( 478 / 2, 20 );

	pAdvancedOptionsButton = new QPushButton( this );
	pAdvancedOptionsButton->setText( tr( "Advanced Options." ) );
	pAdvancedOptionsButton->setFixedSize( 478 / 2, 40 );
	AO->disableTagWidget( true );

	pSteamToSAgreement = new QCheckBox( tr( "I accept the terms of the Steam Workshop Contribution Agreement." ), this );

	auto pAgreementButton = new QPushButton( this );
	pAgreementButton->setText( tr( "View Agreement" ) );
	pAgreementButton->setFixedSize( 478 / 2, 20 );

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
	SteamHelper::StartLoopCall();
}

void CP2MapPublisher::UpdateItem( PublishedFileId_t itemID )
{
	constexpr const int fileChunkSize = 104857600; // 100mb
	if ( !defaultFileLocBSP.startsWith( "mymaps/" ) )
	{
		QFile file( defaultFileLocBSP );
		QFileInfo info( defaultFileLocBSP );
		if ( !file.exists() )
		{
			qInfo() << "File does not exist!";
			return;
		}
		file.open( QFile::ReadOnly );

		UGCFileWriteStreamHandle_t filewritestreamhandle = SteamRemoteStorage()->FileWriteStreamOpen( ( QString( "mymaps/" ) + info.fileName() ).toStdString().c_str() );
		QByteArray data = file.readAll();
		int amount = data.size();
		int i = 0;
		while ( 1 )
		{
			qInfo() << "iteration " + QString( std::to_string( i ).c_str() );

			if ( amount < fileChunkSize )
			{
				// if we start with a lower than 100mb file, we throw in the entire buffer.
				qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, data.constData(), file.size() );
				break;
			}
			// we shove 100MB at the time into the stream.
			qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, data.mid( fileChunkSize * i, ( fileChunkSize * ( i + 1 ) ) ).constData(), fileChunkSize );
			amount -= fileChunkSize;
			if ( amount == 0 )
				break; // if the file happens to be a multiple of 100mb exactly, we break when no data is left.
			if ( amount < fileChunkSize )
			{
				// if the last bit of data is below 100mb, we throw in the remainder amount.
				qInfo() << amount;
				qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, data.right( amount ).constData(), amount );
				break;
			}
			i++;
		}
		qInfo() << SteamRemoteStorage()->FileWriteStreamClose( filewritestreamhandle );

		PublishedFileUpdateHandle_t old_API_Handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest( itemID );
		SteamRemoteStorage()->UpdatePublishedFileFile( old_API_Handle, ( QString( "mymaps/" ) + info.fileName() ).toStdString().c_str() );

		SteamAPICall_t call = SteamRemoteStorage()->CommitPublishedFileUpdate( old_API_Handle );
		m_CallOldApiResultSubmitItemUpdate.Set( call, this, &CP2MapPublisher::OnOldApiSubmitItemUpdate );
	}
	UGCUpdateHandle_t hUpdateHandle = SteamUGC()->StartItemUpdate( CP2MapMainMenu::ConsumerID, itemID );

	if ( !m_edit || ( m_edit && QString( m_editItemDetails.m_rgchTitle ).compare( pTitleLine->text() ) ) )
		qInfo() << SteamUGC()->SetItemTitle( hUpdateHandle, pTitleLine->text().toStdString().c_str() );

	if ( !pDescLine->toPlainText().isEmpty() )
		if ( !m_edit || ( m_edit && QString( m_editItemDetails.m_rgchDescription ).compare( pDescLine->toPlainText() ) ) )
			qInfo() << SteamUGC()->SetItemDescription( hUpdateHandle, pDescLine->toPlainText().toStdString().c_str() );

	ERemoteStoragePublishedFileVisibility visibility; // AO->checkBox->isChecked() ? k_ERemoteStoragePublishedFileVisibilityPrivate : k_ERemoteStoragePublishedFileVisibilityPublic;
	switch ( AO->comboBox->currentIndex() )
	{
		case 0:
			visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
			break;
		case 1:
			visibility = k_ERemoteStoragePublishedFileVisibilityPrivate;
			break;
		case 2:
			visibility = k_ERemoteStoragePublishedFileVisibilityFriendsOnly;
			break;
		case 3:
			visibility = k_ERemoteStoragePublishedFileVisibilityUnlisted;
			break;
		default:
			visibility = k_ERemoteStoragePublishedFileVisibilityPublic;
			break;
	}
	SteamUGC()->SetItemVisibility( hUpdateHandle, visibility );

	char *descr = strdup( AO->textEdit->toPlainText().toStdString().c_str() );

	SteamParamStringArray_t tags {};

	std::vector<char *> charray;
	QTreeWidgetItemIterator iterator2( AO->treeWidget );
	while ( *iterator2 )
	{
		qInfo() << ( *iterator2 )->text( 0 );
		charray.push_back( strdup( ( *iterator2 )->text( 0 ).toStdString().c_str() ) );
		++iterator2;
	}
	tags.m_nNumStrings = (int32)charray.size();
	tags.m_ppStrings = (const char **)charray.data();

	qInfo() << SteamUGC()->SetItemTags( hUpdateHandle, &tags );

	for ( auto &str : charray )
		free( str );

	for ( int ind = 0; ind < iCount; ind++ )
	{
		SteamUGC()->RemoveItemPreview( hUpdateHandle, ind );
	}

	if ( defaultFileLocIMG != "./" && QFile::exists( QDir::tempPath() + "/AdditionImageCurrentThumbnail.jpg" ) )
	{
		qInfo() << SteamUGC()->SetItemPreview( hUpdateHandle, QString( QDir::tempPath() + "/AdditionImageCurrentThumbnail.jpg" ).toStdString().c_str() );
	}

	QTreeWidgetItemIterator iterator4( AO->ImageTree );
	while ( *iterator4 )
	{
		qInfo() << SteamUGC()->AddItemPreviewFile( hUpdateHandle, ( ( *iterator4 )->data( 0, Qt::UserRole ).toString() ).toStdString().c_str(), k_EItemPreviewType_Image );
		++iterator4;
	}

	QTreeWidgetItemIterator iterator3( AO->treeWidget_2 );
	while ( *iterator3 )
	{
		qInfo() << ( *iterator3 )->text( 0 );
		SteamUGC()->AddItemPreviewVideo( hUpdateHandle, ( *iterator3 )->text( 0 ).toStdString().c_str() );
		++iterator3;
	}

	qInfo() << "Reached Upadte Handle";
	qInfo() << QString( descr );
	SteamAPICall_t hApiSubmitItemHandle = SteamUGC()->SubmitItemUpdate( hUpdateHandle, (const char *)descr );
	m_CallResultSubmitItemUpdate.Set( hApiSubmitItemHandle, this, &CP2MapPublisher::OnSubmitItemUpdate );
	SteamHelper::StartLoopCall();
	qInfo() << "End of Function Reached!";
	free( descr );
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
	qInfo() << bFailure;
	qInfo() << pItem->m_eResult;
	this->close();
}

void CP2MapPublisher::OnOldApiSubmitItemUpdate( RemoteStorageUpdatePublishedFileResult_t *pItem, bool pFailure )
{
	qInfo() << pItem->m_eResult << "\n";
}

void CP2MapPublisher::LoadExistingDetails( SteamUGCDetails_t details, uint32 index )
{
	m_editItemDetails = details;
	pFileEntry->setText( details.m_pchFileName );
	pTitleLine->setText( details.m_rgchTitle );
	pDescLine->setText( details.m_rgchDescription );
	defaultFileLocBSP = details.m_pchFileName;

	switch ( details.m_eVisibility )
	{
		case k_ERemoteStoragePublishedFileVisibilityPublic:
			AO->comboBox->setCurrentIndex( 0 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityPrivate:
			AO->comboBox->setCurrentIndex( 1 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityFriendsOnly:
			AO->comboBox->setCurrentIndex( 2 );
			break;
		case k_ERemoteStoragePublishedFileVisibilityUnlisted:
			AO->comboBox->setCurrentIndex( 3 );
			break;
		default:
			AO->comboBox->setCurrentIndex( 0 );
			break;
	}

	m_EditItemIndex = index;

	std::vector<std::string> vector = splitString( details.m_rgchTags, ',' );
	for ( const std::string &str : vector )
	{
		auto item = new QTreeWidgetItem( 0 );
		item->setText( 0, QString( str.c_str() ) );
		if ( str == "Singleplayer" || str == "Cooperative" || str == "Custom Visuals" )
			item->setDisabled( true );
		AO->treeWidget->addTopLevelItem( item );
	}

	AO->disableTagWidget( false );

	std::string dir = QDir::tempPath().toStdString() + "/" + std::to_string( details.m_nPublishedFileId ) + "_Image0.png";
	SteamAPICall_t res = SteamRemoteStorage()->UGCDownloadToLocation( details.m_hPreviewFile, dir.c_str(), 0 );
	m_CallOldApiResultSubmitItemDownload.Set( res, this, &CP2MapPublisher::OnOldApiSubmitItemDownload );
	SteamHelper::StartLoopCall();
	UGCQueryHandle_t hQueryResult = SteamUGC()->CreateQueryUserUGCRequest( SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published, k_EUGCMatchingUGCType_Items_ReadyToUse, k_EUserUGCListSortOrder_CreationOrderDesc, SteamUtils()->GetAppID(), CP2MapMainMenu::ConsumerID, 1 );
	SteamUGC()->SetReturnAdditionalPreviews( hQueryResult, true );
	SteamAPICall_t hApiQueryHandle = SteamUGC()->SendQueryUGCRequest( hQueryResult );
	m_CallResultSendQueryUGCRequest.Set( hApiQueryHandle, this, &CP2MapPublisher::OnSendQueryUGCRequest );
	SteamHelper::StartLoopCall();
	SteamUGC()->ReleaseQueryUGCRequest( hQueryResult );
}

void CP2MapPublisher::OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure )
{
	qInfo() << "testing";

	SteamUGCDetails_t pDetails {};
	SteamUGC()->GetQueryUGCResult( pQuery->m_handle, m_EditItemIndex, &pDetails );
	// qInfo() << pDetails.m_flScore;

	iCount = SteamUGC()->GetQueryUGCNumAdditionalPreviews( pQuery->m_handle, m_EditItemIndex );
	int imageIndex = 0;
	for ( uint32 i = 0; i < iCount; i++ )
	{
		const uint iUrlSize = 2000;
		char pchUrl[iUrlSize];
		const uint iFileSize = 2000;
		char pchFileName[iFileSize];
		EItemPreviewType pType;

		qInfo() << SteamUGC()->GetQueryUGCAdditionalPreview( pQuery->m_handle, m_EditItemIndex, i,
															 pchUrl, iUrlSize, pchFileName,
															 iFileSize, &pType );
		auto *pItem = new QTreeWidgetItem( 0 );

		if ( pType == k_EItemPreviewType_Image )
		{
			qInfo() << pchFileName;
			qInfo() << pchUrl;
			QNetworkAccessManager manager;
			QNetworkReply *reply = manager.get( QNetworkRequest( QUrl( pchUrl ) ) );
			QEventLoop loop;
			QObject::connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
			QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), &loop, SLOT( quit() ) );
			loop.exec();

			QImage image;
			image.loadFromData( reply->readAll() );
			QByteArray ba;
			QBuffer buffer( &ba );
			buffer.open( QIODevice::ReadWrite );
			image = image.scaled( 1914, 1078, Qt::KeepAspectRatio );
			image.save( &buffer, "JPG");
			QString filepath = QString( QDir::tempPath() + "/AdditionImage" + QString( std::to_string( i ).c_str() ) + ".jpg" );
			QFile file( filepath );
			file.open( QIODevice::ReadWrite );
			file.write( ba );
			file.close();

			pItem->setText( 0, QString( pchFileName ) );
			pItem->setData( 0, Qt::UserRole, filepath );
			pItem->setData( 1, Qt::UserRole, imageIndex );
			AO->ImageTree->addTopLevelItem( pItem );

			delete reply;
			imageIndex++;
		}
		if ( pType == k_EItemPreviewType_YouTubeVideo )
		{
			pItem->setText( 0, QString( pchUrl ) );
			AO->treeWidget_2->addTopLevelItem( pItem );
			break;
		}
	}

	SteamHelper::FinishLoopCall();
}

void CP2MapPublisher::OnOldApiSubmitItemDownload( RemoteStorageDownloadUGCResult_t *pItem, bool pFailure )
{
	std::string filepath = QDir::tempPath().toStdString() + "/" + std::to_string( m_editItemDetails.m_nPublishedFileId ) + "_Image0.png";
	QPixmap tempMap = QPixmap( filepath.c_str() );
	if ( tempMap.isNull() )
		tempMap = QPixmap( ":/zoo_textures/InvalidImage.png" );
	tempMap = tempMap.scaled( 239, 134, Qt::IgnoreAspectRatio );
	pImageLabel->setPixmap( tempMap );
	SteamHelper::FinishLoopCall();
}

void CP2MapPublisher::onAgreementButtonPressed()
{
	QDesktopServices::openUrl( QUrl( "https://store.steampowered.com/subscriber_agreement/" ) );
}

void CP2MapPublisher::OpenImageFileExplorer()
{
	QString filePath = QFileDialog::getOpenFileName( this, "Open", defaultFileLocIMG, "*.png *.jpg", nullptr, FILE_PICKER_OPTS );
	QString fPathOG = filePath;
	if ( filePath.isEmpty() )
		return;
	QPixmap tempMap = QPixmap( filePath );
	if ( tempMap.isNull() )
	{
		filePath = ":/zoo_textures/InvalidImage.png";
		tempMap = QPixmap( ":/zoo_textures/InvalidImage.png" );
	}
	tempMap = tempMap.scaled( 239, 134., Qt::IgnoreAspectRatio );
	pImageLabel->setPixmap( tempMap );

	if ( tempMap.isNull() )
		return;
	// QImage image(filePath);
	QPixmap thumbnail( filePath );
	thumbnail = thumbnail.scaled( 1914, 1078, Qt::IgnoreAspectRatio );

	QString filepath = QString( QDir::tempPath() + "/AdditionImageCurrentThumbnail.jpg" );
	QByteArray brAy = QByteArray();
	qInfo() << QDir::tempPath() + "/AdditionImageCurrentThumbnail.jpg";
	if(thumbnail.save( QDir::tempPath() + "/AdditionImageCurrentThumbnail.jpg", "JPG" ));


	defaultFileLocIMG = fPathOG;
}

void CP2MapPublisher::OpenBSPFileExplorer()
{
	QString filePath = QFileDialog::getOpenFileName( this, "Open", defaultFileLocBSP, "*.bsp", nullptr, FILE_PICKER_OPTS );
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

	BSPHeaderStruct_t castedLump {};
	const auto bArray = file.readAll();
	memcpy( &castedLump, bArray.constData(), sizeof( BSPHeaderStruct_t ) );
	qInfo() << castedLump.m_version;
	if ( castedLump.m_version != 21 )
	{
		QMessageBox::warning( this, "Invalid BSP", "Invalid BSP.\nEither the file is corrupt or the map is not for Portal 2.\n(only works for BSP version 21)" );
		return;
	}
	QString Entities = bArray.constData() + castedLump.lumps[0].fileOffset;
	if ( !Entities.contains( "@relay_pti_level_end" ) && !AO->checkBox_3->isChecked() )
	{
		m_bspHasPTIInstance = false;
		QDialog *dialog = new QDialog( this );
		PTIDialogSetup *PTI = new PTIDialogSetup();
		PTI->setupUi( dialog );
		dialog->exec();
		return;
	}

	ListInitResponse res = P2ElementParser::initialiseElementList();
	if ( res == ListInitResponse::FILEINVALID )
		QMessageBox::warning( this, "Invalid KV File", "File elements.kv is Invalid. Using default configuration." );

	if ( P2ElementParser::isInitialised() )
	{
		KeyValueRoot *keyvals = P2ElementParser::getElementList();
		qInfo() << keyvals->Get( "entities" ).childCount;
		//		keyvals.Solidify();

		std::vector<QString> entArray {};
		QString entityStack = "";
		bool in_quotes = false;
		int nests = 0;
		for ( char character : Entities.toStdString() )
		{
			entityStack += character;
			if ( character == '"' )
			{
				in_quotes = !in_quotes;
				continue;
			}
			if ( !in_quotes && character == '{' )
				nests++;
			if ( !in_quotes && character == '}' )
				nests--;
			if ( nests < 0 )
			{
				QMessageBox::critical( this, "Invalid BSP", "Invalid BSP.\nThe parser failed to read the entity lump data properly, please recompile the BSP and try again." );
				return;
			};

			if ( nests == 0 && character == '}' )
			{
				QString parsable = ( ( "\"entity\" " + entityStack ) );
				entArray.push_back( parsable );
				entityStack = "";
			}
		}

		QStringList tags;

		bool isCoop = false;
		bool isSingeplayer = false;

		for ( QString entityQStr : entArray )
		{
			KeyValueRoot *entity = new KeyValueRoot();
			entity->Parse( entityQStr.toStdString().c_str() );
			bool tagSuffices = false;

			if ( !isCoop )
				isCoop = QString( entity->Get( "entity" ).Get( "classname" ).value.string ).compare( "info_coop_spawn" ) == 0;

			if ( !isSingeplayer )
				isSingeplayer = QString( entity->Get( "entity" ).Get( "classname" ).value.string ).compare( "info_player_start" ) == 0;

			if ( isSingeplayer && !tags.contains( "Singleplayer" ) )
			{
				tags << "Singleplayer";
			}
			if ( isCoop && !tags.contains( "Cooperative" ) )
			{
				tags << "Cooperative";
			}
			//			qInfo() << keyvals->Get( "entities" )["prop_tractor_beam"].key.string;
			//			for(int i = 0; i < keyvals->Get( "entities" ).childCount; i++){
			//				auto entityClassName = entity->Get( "entity" ).Get( "classname" ).value.string;
			//				if(QString(keyvals->Get( "entities" )[i].key.string).compare(entityClassName)){
			//					qInfo() << keyvals->Get( "entities" )[i].key.string;
			//				}
			//			}

			for ( int i = 0; i < keyvals->Get( "entities" ).childCount; i++ )
			{
				//				QRegExp r(keyvals->Get( "entities" )[i].key.string);
				//				qInfo() << keyvals->Get( "entities" )[i].key.string;
				//				qInfo() << r.exactMatch(QString( entity->Get( "entity" ).Get( "classname" ).value.string ));
				//				qInfo() << entity->Get( "entity" ).Get( "classname" ).value.string;
				char *str = entity->Get( "entity" ).Get( "classname" ).value.string;
				if ( QString( keyvals->Get( "entities" )[i].key.string ).compare( str ) == 0 )
				{
					// qInfo() << (QString(entity->Get( "entity" ).Get( "classname" ).value.string ));

					if ( keyvals->Get( "entities" )[i].childCount != 1 )
						for ( int j = 0; j < keyvals->Get( "entities" )[i].childCount; j++ )
						{
							// qInfo() << keyvals->Get( "entities" )[i][j].value.string;
							if ( QString( keyvals->Get( "entities" )[i][j].key.string ).compare( "tag" ) == 0 )
								continue;
							if ( QString( entity->Get( "entity" )[keyvals->Get( "entities" )[i][j].key.string].value.string ).isEmpty() )
								continue;
							QString a = QString( keyvals->Get( "entities" )[i][j].value.string );
							QString b = QString( entity->Get( "entity" )[keyvals->Get( "entities" )[i][j].key.string].value.string );
							qInfo() << a;
							qInfo() << b;
							tagSuffices = a.compare( b, Qt::CaseInsensitive ) == 0;
							if ( !tagSuffices )
								break;
						}
					else
						tagSuffices = true;
					// qInfo() << keyvals->Get("entities")[i]["tag"].value.string;
					if ( tagSuffices && !tags.contains( keyvals->Get( "entities" )[i]["tag"].value.string ) )
						tags << keyvals->Get( "entities" )[i]["tag"].value.string;
				}
			}
		}

		if ( isCoop && isSingeplayer )
		{
			QMessageBox::critical( this, "Map Error: Conflicting Player Spawn", "Invalid BSP.\nThe parser failed to read the entity lump data properly, please recompile the BSP and try again." );
			return;
		}

		if ( !isCoop && !isSingeplayer )
		{
			QMessageBox::critical( this, "Map Error: No Player Spawn", "Info" );
			return;
		}

		AO->treeWidget->clear();

		qInfo() << tags;
		std::reverse( tags.begin(), tags.end() );
		for ( int i = 0; i < tags.count(); i++ )
		{
			QTreeWidgetItem *___qtreewidgetitem1 = new QTreeWidgetItem( AO->treeWidget );
			___qtreewidgetitem1->setText( 0, QCoreApplication::translate( "Advanced", tags[i].toStdString().c_str(), nullptr ) );
			if ( tags[i] == "Singleplayer" || tags[i] == "Cooperative" )
				___qtreewidgetitem1->setDisabled( true );
		}
	}

	//	{
	//		QTreeWidgetItem *___qtreewidgetitem1 = AO->treeWidget->topLevelItem( 0 );
	//		___qtreewidgetitem1->setText( 0, QCoreApplication::translate( "Advanced", "Singleplayer", nullptr ) );
	//		___qtreewidgetitem1->setDisabled( true );
	//	}

	m_bspHasPTIInstance = true;
	AO->disableTagWidget( false );
	pFileEntry->setText( filePath );
}

void CP2MapPublisher::onOKPressed()
{
	if ( pTitleLine->text().isEmpty() )
	{
		QMessageBox::warning( this, "File Required!", "You don't have a Title, please insert a title.", QMessageBox::Ok );
		return;
	}

	if ( !m_edit && ( defaultFileLocIMG == "./" || !QFile::exists( defaultFileLocIMG ) ) )
	{
		QMessageBox::warning( this, "Preview Image Required!", "You don't have a Preview Image, please insert a Preview Image", QMessageBox::Ok );
		return;
	}

	if ( !pSteamToSAgreement->isChecked() )
	{
		QMessageBox::warning( this, "Steam Workshop Contribution Agreement", "You need to accept the Steam Workshop Contribution Agreement to upload your map!", QMessageBox::Ok );
		return;
	}

	if ( !m_edit || ( m_edit && !defaultFileLocBSP.startsWith( "mymaps/" ) ) )
	{
		if ( !defaultFileLocBSP.endsWith( ".bsp" ) )
		{
			QMessageBox::warning( this, "No Map Found!", "You don't have a BSP selected! Please select a valid BSP", QMessageBox::Ok );
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
		// if ( file.size() > 209715200 )
		// {
		// 	QMessageBox::warning( this, "File Too Large!", "This BSP is too large, max 200MB." );
		// 	return;
		// }
		QDataStream stream { &file };
		QByteArray bArray( (int)file.size(), 0 );
		stream.readRawData( bArray.data(), bArray.size() );
		BSPHeaderStruct_t castedLump {};
		memcpy( &castedLump, bArray.data(), sizeof( BSPHeaderStruct_t ) );
		qInfo() << castedLump.m_version;
		if ( castedLump.m_version != 21 )
		{
			QMessageBox::warning( this, "Invalid BSP", "Invalid BSP.\nEither the file is corrupt or the map is not for Portal 2.\n(only works for BSP version 21)" );
			return;
		}
		QString Entities = bArray.data() + castedLump.lumps[0].fileOffset;
		if ( !Entities.contains( "@relay_pti_level_end" ) && !AO->checkBox_3->isChecked() )
		{
			m_bspHasPTIInstance = false;
			QDialog *dialog = new QDialog( this );
			PTIDialogSetup *PTI = new PTIDialogSetup();
			PTI->setupUi( dialog );
			dialog->exec();
			return;
		}
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

void CP2MapPublisher::closeEvent( QCloseEvent *event )
{
	emit mapPublisherClosed();
	event->accept();
}

std::vector<std::string> CP2MapPublisher::splitString( const std::string &input, char delimiter )
{
	std::stringstream ss { input };
	std::vector<std::string> out;
	std::string token;
	while ( std::getline( ss, token, delimiter ) )
		out.push_back( token );
	return out;
}