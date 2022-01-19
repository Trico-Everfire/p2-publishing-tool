#include "dialogs/P2MapMainWindow.h"

using namespace std;
using namespace ui;

CP2MapMainMenu::CP2MapMainMenu( QWidget *pParent ) :
	QDialog( pParent )
{
	// Make sure steam_appid.txt exists
	QString filename = "steam_appid.txt";
	QFile file( filename );
	if ( file.open( QIODevice::ReadWrite ) )
	{
		QTextStream stream( &file );
		// todo: maybe don't hardcode this at some point?
		stream << "644" << Qt::endl;
		file.close();
	}

	if ( !SteamAPI_Init() )
	{
		QMessageBox::StandardButton box = QMessageBox::warning( nullptr, tr( "Steam API Error" ), tr( "The Steam API could not Initialize! \nMake sure you have the steam client running." ) );
		this->close();
	}
	this->setWindowModality( Qt::ApplicationModal );
	auto pDialogLayout = new QGridLayout( this );

	this->setWindowTitle( tr( "Browse Published Files" ) );

	auto pButtonBox = new QDialogButtonBox( Qt::Orientation::Horizontal, this );
	auto pAddButton = pButtonBox->addButton( tr( "Add" ), QDialogButtonBox::ActionRole );
	pDeleteButton = pButtonBox->addButton( tr( "Delete" ), QDialogButtonBox::ActionRole );
	pDeleteButton->setDisabled( true );
	pEditButton = pButtonBox->addButton( tr( "Edit" ), QDialogButtonBox::ActionRole );
	pEditButton->setDisabled( true );
	auto pRefreshButton = pButtonBox->addButton( tr( "Refresh" ), QDialogButtonBox::ActionRole );

	QLabel *timezoneLabel = new QLabel( this );
	timezoneLabel->setText( tr( "Timezone:" ) );
	m_timezoneComboBox = new QComboBox( this );
	m_timezoneComboBox->setFixedWidth( this->width() / 4 );
	for ( auto val : QTimeZone::availableTimeZoneIds() )
		m_timezoneComboBox->addItem( val );

	m_treeWidget = new QTreeWidget( this );
	m_treeWidget->setFixedSize( 720, 300 );
	m_treeWidget->setColumnCount( 3 );
	auto qStringList = QStringList();
	qStringList.append( "Title" );
	qStringList.append( "File" );
	qStringList.append( "Last Updated" );
	m_treeWidget->setHeaderLabels( qStringList );
	m_treeWidget->headerItem()->setTextAlignment( 0, Qt::AlignCenter );
	m_treeWidget->headerItem()->setTextAlignment( 1, Qt::AlignCenter );
	m_treeWidget->headerItem()->setTextAlignment( 2, Qt::AlignCenter );
	m_treeWidget->setColumnWidth( 0, 700 / 3 );
	m_treeWidget->setColumnWidth( 1, 700 / 3 );
	m_treeWidget->setColumnWidth( 2, 700 / 3 );

	connect( m_timezoneComboBox, SIGNAL( currentIndexChanged( const QString & ) ), this, SLOT( ChangedTimezone( const QString & ) ) );
	connect( pAddButton, &QPushButton::pressed, this, &CP2MapMainMenu::onAddPressed );
	connect( pDeleteButton, &QPushButton::pressed, this, &CP2MapMainMenu::onDeletePressed );
	connect( pEditButton, &QPushButton::pressed, this, &CP2MapMainMenu::onEditPressed );
	connect( pRefreshButton, &QPushButton::pressed, this, &CP2MapMainMenu::onRefreshPressed );
	connect( m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &CP2MapMainMenu::treeSelectionChanged );

	pDialogLayout->addWidget( m_treeWidget, 0, 0, 10, 10 );

	pDialogLayout->addWidget( pAddButton, 0, 10, 10, 1, Qt::AlignTop );
	pDialogLayout->addWidget( pDeleteButton, 1, 10, 10, 1, Qt::AlignTop );
	pDialogLayout->addWidget( pEditButton, 2, 10, 10, 1, Qt::AlignTop );
	pDialogLayout->addWidget( pRefreshButton, 3, 10, 10, 1, Qt::AlignTop );

	pDialogLayout->addWidget( timezoneLabel, 11, 0, 1, 11 );
	pDialogLayout->addWidget( m_timezoneComboBox, 11, 1, Qt::AlignLeft );

	this->setLayout( pDialogLayout );
	this->setFixedSize( this->sizeHint() );
	this->setWindowFlag( Qt::WindowContextHelpButtonHint, false );

	runSetCallbacks();
}

CP2MapMainMenu::~CP2MapMainMenu()
{
	SteamAPI_Shutdown();
	qInfo() << "P2_Map_Publisher closing";
	this->close();
}

void CP2MapMainMenu::treeSelectionChanged()
{
	if ( m_treeWidget->selectedItems().length() > 0 )
	{
		m_treeWidget->selectedItems()[0];
		pEditButton->setEnabled( true );
		pDeleteButton->setEnabled( true );
	}
	else
	{
		pEditButton->setEnabled( false );
		pDeleteButton->setEnabled( false );
	}
}

void CP2MapMainMenu::onAddPressed()
{
	CP2MapPublisher *publisher = new CP2MapPublisher( this );
	publisher->exec();
}

void CP2MapMainMenu::onDeletePressed()
{
	QMessageBox box( QMessageBox::Icon::Critical, "DELETING ITEM", "This action will delete this item permanently from the workshop!", QMessageBox::Ok | QMessageBox::Abort, this );
	QCheckBox *checkbox = new QCheckBox( "I understand that this will delete this item from the workshop.", nullptr );
	box.button( QMessageBox::Ok )->setDisabled( true );
	connect( checkbox, &QCheckBox::stateChanged, this, [&]( int state )
			 {
				 if ( state == 0 )
				 {
					 box.button( QMessageBox::Ok )->setDisabled( true );
				 }
				 else
				 {
					 box.button( QMessageBox::Ok )->setDisabled( false );
				 }
			 } );
	box.setCheckBox( checkbox );
	box.exec();

	if ( QMessageBox::Ok && checkbox->isChecked() )
	{
		QTreeWidget *widget = this->m_treeWidget;
		QTreeWidgetItem *item = widget->selectedItems()[0];
		int itemIndex = item->data( 1, Qt::UserRole ).toInt();
		SteamUGCDetails_t Details = SUGCD.at( itemIndex );
		SteamUGC()->DeleteItem( Details.m_nPublishedFileId );
		onRefreshPressed();
	}
}

void CP2MapMainMenu::OnOldApiSubmitItemUpdate( RemoteStorageUpdatePublishedFileResult_t *pItem, bool pFailure )
{
	qInfo() << pItem->m_eResult << "\n";
	FinishLoopCall();
}

void CP2MapMainMenu::onEditPressed()
{
	// QFile file("/home/trico/Documents/test_bsp_upload/mymaps/wall_button_old_aperture.bsp");
	// if(!file.exists()) {
	//     qInfo() << "File does not exist!";
	//     return;
	// };
	// file.open(QFile::ReadOnly);
	// if(file.size() > 209715200){
	//     qInfo() << "File too large!";
	//     return;
	// }
	// UGCFileWriteStreamHandle_t filewritestreamhandle = SteamRemoteStorage()->FileWriteStreamOpen( "mymaps/wall_button_old_aperture.bsp" );
	// if(file.size() > 104857600){
	//     QByteArray data = file.readAll();
	//     QByteArray first = data.left(104857600); //gets the first 100mb.
	//     QByteArray rest = data.right(data.size() - 104857600); //gets the rest.
	//    qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, first.constData(), 104857600 );
	//    qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, rest.constData(), data.size() - 104857600 );
	// } else {
	//     QByteArray data = file.readAll();
	//    qInfo() << SteamRemoteStorage()->FileWriteStreamWriteChunk( filewritestreamhandle, data.constData(), data.size());
	// }
	// qInfo() << SteamRemoteStorage()->FileWriteStreamClose( filewritestreamhandle );

	// PublishedFileUpdateHandle_t old_API_Handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest( 2709544274 );
	// qInfo() << SteamRemoteStorage()->UpdatePublishedFileFile( old_API_Handle, "mymaps/wall_button_old_aperture.bsp" );
	// SteamRemoteStorage()->UpdatePublishedFileVisibility(old_API_Handle,k_ERemoteStoragePublishedFileVisibilityPrivate);
	// SteamParamStringArray_t tags =  SteamParamStringArray_t();
	// tags.m_ppStrings = new const char*[1];
	// tags.m_ppStrings[0] = "Singleplayer";
	// tags.m_nNumStrings = 1;
	// SteamRemoteStorage()->UpdatePublishedFileTags(old_API_Handle,&tags);
	// SteamAPICall_t call = SteamRemoteStorage()->CommitPublishedFileUpdate( old_API_Handle );
	// m_CallOldApiResultSubmitItemUpdate.Set(call, this, &CP2MapMainMenu::OnOldApiSubmitItemUpdate);

	// SteamParamStringArray_t strs = SteamParamStringArray_t();
	// strs.m_nNumStrings = 0;
	// SteamRemoteStorage()->PublishWorkshopFile("/home/trico/Documents/test_bsp_upload/mymaps/wall_button_old_aperture.bsp","/home/trico/Documents/Images/SWP w8.png",620,"this is a test","Test",k_ERemoteStoragePublishedFileVisibilityPrivate,&strs,EWorkshopFileType::k_EWorkshopFileTypeCommunity);
	// m_setLoaded = false;
	// BaseCallbackLoop();
	// CP2MapPublisher *publisher = new CP2MapPublisher(nullptr,true);
	// publisher->exec();

	// PublishedFileId_t field = 2709544274;
	// qInfo() << "Published FileID:" << field << "\n";

	QTreeWidget *widget = this->m_treeWidget;
	QTreeWidgetItem *item = widget->selectedItems()[0];
	int itemIndex = item->data( 1, Qt::UserRole ).toInt();
	qInfo() << itemIndex;
	SteamUGCDetails_t Details = SUGCD.at( itemIndex );
	qInfo() << Details.m_pchFileName;
	CP2MapPublisher *publisher = new CP2MapPublisher( this, true );
	publisher->LoadExistingDetails( Details, itemIndex );
	publisher->exec();

	// UGCUpdateHandle_t hUpdateHandle = SteamUGC()->StartItemUpdate(CP2MapMainMenu::ConsumerID, Details.m_nPublishedFileId);

	// bool bTitleResult = SteamUGC()->SetItemTitle(hUpdateHandle, "Test Map Upload");
	// bool bDescResult = SteamUGC()->SetItemDescription(hUpdateHandle, "This was a test from an uploader program a friend wrote");
	// bool bVisibilityResult = SteamUGC()->SetItemVisibility(hUpdateHandle, k_ERemoteStoragePublishedFileVisibilityPrivate);
	// const char* preview = "///home/trico/Documents/Images/swp logo discord.png";
	// bool bPreviewResult = SteamUGC()->SetItemPreview(hUpdateHandle, preview);

	// SteamAPICall_t hApiSubmitItemHandle = SteamUGC()->SubmitItemUpdate(hUpdateHandle, "Test from application");
	// m_CallResultSubmitItemUpdate.Set(hApiSubmitItemHandle, this, &CP2MapMainMenu::OnSubmitItemUpdate);
	// m_setLoaded = false;
	// BaseCallbackLoop();
}

void CP2MapMainMenu::OnSubmitItemUpdate( SubmitItemUpdateResult_t *pItem, bool bFailure )
{
	// qInfo() << "Uploading" << "\n";
	// qInfo() << pItem->m_eResult << "\n";
	// void CP2MapPublisher::OnSubmitItemUpdate(SubmitItemUpdateResult_t* pItem, bool bFailure){
	// qInfo() << "Uploading" << "\n";
	// qInfo() << pItem->m_eResult << "\n";
	// PublishedFileUpdateHandle_t old_API_Handle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest( 2709544274 );
	// SteamRemoteStorage()->UpdatePublishedFileFile( old_API_Handle, "///home/trico/Documents/test_bsp_upload/mymaps/wall_button_old_aperture.bsp" );
	// SteamAPICall_t call = SteamRemoteStorage()->CommitPublishedFileUpdate( old_API_Handle );
	// m_CallOldApiResultSubmitItemUpdate.Set(call, this, &CP2MapPublisher::OnOldApiSubmitItemUpdate);
	FinishLoopCall();
	// Parent->isload
};

void CP2MapMainMenu::onRefreshPressed()
{
	m_treeWidget->clear();
	runSetCallbacks();
}

void CP2MapMainMenu::runSetCallbacks()
{
	UGCQueryHandle_t hQueryResult = SteamUGC()->CreateQueryUserUGCRequest( SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published, k_EUGCMatchingUGCType_Items_ReadyToUse, k_EUserUGCListSortOrder_CreationOrderDesc, SteamUtils()->GetAppID(), ConsumerID, 1 );
	SteamAPICall_t hApiQueryHandle = SteamUGC()->SendQueryUGCRequest( hQueryResult );
	m_SteamCallResultUGCRequest.Set( hApiQueryHandle, this, &CP2MapMainMenu::OnSendQueryUGCRequest );
	SteamUGC()->ReleaseQueryUGCRequest( hQueryResult );
	StartLoopCall();
}

void CP2MapMainMenu::ChangedTimezone( const QString &index )
{
	QTimeZone zone = QTimeZone( QByteArray::fromStdString( index.toStdString() ) );
	QTreeWidgetItemIterator it( m_treeWidget );
	int i = 0;
	while ( *it )
	{
		QDateTime time = QDateTime::fromSecsSinceEpoch( totalLoadedItems[i], zone );
		( *it )->setText( 2, time.toString() );
		i++;
		++it;
	}
}

void CP2MapMainMenu::OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure )
{
	if ( bFailure )
	{
		qInfo() << "Query Failed"
				<< "\n";
		FinishLoopCall(); // sanity check if close doesn't work.
		this->close();
		return;
	}

	totalLoadedItems.clear();
	// Loop through all our items
	for ( int index = 0; index < pQuery->m_unNumResultsReturned; index++ )
	{
		// Get the workshop item from index
		SteamUGCDetails_t pDetails{};
		SteamUGC()->GetQueryUGCResult( pQuery->m_handle, index, &pDetails );

		// QTimeZone timezone = QTimeZone(QTimeZone::availableTimeZoneIds()[0]); //we default to the first ID as that'll be the first ID inside the selection box.
		QDateTime time = QDateTime::fromSecsSinceEpoch( pDetails.m_rtimeUpdated );

		m_timezoneComboBox->setCurrentIndex( QTimeZone::availableTimeZoneIds().indexOf( time.timeZone().id() ) );
		// Retrieve information. https://partner.steamgames.com/doc/api/ISteamUGC#SteamUGCDetails_t
		auto item = new QTreeWidgetItem( 0 );
		item->setText( 0, pDetails.m_rgchTitle );
		item->setText( 1, pDetails.m_pchFileName );
		item->setText( 2, time.toString() );
		item->setData( 1, Qt::UserRole, index );
		SUGCD.insert( std::pair<int, SteamUGCDetails_t>( index, pDetails ) );
		item->setTextAlignment( 0, Qt::AlignCenter );
		item->setTextAlignment( 1, Qt::AlignCenter );
		item->setTextAlignment( 2, Qt::AlignCenter );
		item->setForeground( 0, QColor( 255, 255, 255 ) );
		item->setForeground( 1, QColor( 255, 255, 255 ) );
		item->setForeground( 2, QColor( 255, 255, 255 ) );
		m_treeWidget->addTopLevelItem( item );

		totalLoadedItems.push_back( pDetails.m_rtimeUpdated );
		qInfo() << time.toString() << "\n";
	}
	FinishLoopCall();

	return;
}
