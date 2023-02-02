#include "mainview.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimeZone>

using namespace ui;

CMainView::CMainView( QWidget *pParent ) :
	QDialog( pParent )
{
	this->setWindowTitle( "Portal 2 Map Uploader" );

	auto pMainViewLayout = new QGridLayout( this );

	pMainViewLayout->setAlignment( Qt::AlignTop );

	m_pWorkshopItemTree = new QTreeWidget( this );
	auto qStringList = QStringList();
	qStringList.append( "Title" );
	qStringList.append( "File" );
	qStringList.append( "Last Updated" );
	m_pWorkshopItemTree->setHeaderLabels( qStringList );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 0, Qt::AlignCenter );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 1, Qt::AlignCenter );
	m_pWorkshopItemTree->headerItem()->setTextAlignment( 2, Qt::AlignCenter );
	m_pWorkshopItemTree->setColumnWidth( 0, 600 / 3 );
	m_pWorkshopItemTree->setColumnWidth( 1, 600 / 3 );
	m_pWorkshopItemTree->setColumnWidth( 2, 600 / 3 );
	m_pWorkshopItemTree->setMinimumSize( 600, 100 );

	pMainViewLayout->addWidget( m_pWorkshopItemTree, 0, 0, 4, 6 );

	auto pTimezoneLabel = new QLabel( "Timezone:", this );
	auto timezoneFont = pTimezoneLabel->font();
	timezoneFont.setPointSizeF( timezoneFont.pointSizeF() * 1.4 );
	pTimezoneLabel->setFont( timezoneFont );

	pMainViewLayout->addWidget( pTimezoneLabel, 5, 0, 1, 0, Qt::AlignLeft );

	m_pTimezoneComboBox = new QComboBox( this );
	foreach( auto timezoneID, QTimeZone::availableTimeZoneIds() )
		m_pTimezoneComboBox->addItem( timezoneID );

	QDateTime time = QDateTime::currentDateTime();
	m_pTimezoneComboBox->setCurrentIndex( QTimeZone::availableTimeZoneIds().indexOf( time.timeZone().id() ) );

	pMainViewLayout->addWidget( m_pTimezoneComboBox, 5, 1, Qt::AlignLeft );

	auto pAddButton = new QPushButton( "Add", this );
	pMainViewLayout->addWidget( pAddButton, 0, 6, Qt::AlignRight );

	auto pDeleteButton = new QPushButton( "Delete", this );
	pDeleteButton->setEnabled( false );
	pMainViewLayout->addWidget( pDeleteButton, 1, 6, Qt::AlignRight );

	auto pEditButton = new QPushButton( "Edit", this );
	pEditButton->setEnabled( false );
	pMainViewLayout->addWidget( pEditButton, 2, 6, Qt::AlignRight );

	auto pRefreshButton = new QPushButton( "Refresh", this );
	pMainViewLayout->addWidget( pRefreshButton, 3, 6, Qt::AlignRight );

	pDeleteButton->setEnabled( false );

	m_CallbackTimer.setSingleShot( false );
	connect( &m_CallbackTimer, &QTimer::timeout, this, []()
			 {
				 SteamAPI_RunCallbacks();
			 } );

	m_CallbackTimer.start( 100 );

	connect( m_pTimezoneComboBox, &QComboBox::currentTextChanged, this, [this]( const QString &index )
			 {
				 QTimeZone zone = QTimeZone( QByteArray::fromStdString( index.toStdString() ) );
				 QTreeWidgetItemIterator workshopListIterator( m_pWorkshopItemTree );
				 int i = 0;
				 while ( *workshopListIterator )
				 {
					 QDateTime time = QDateTime::fromSecsSinceEpoch( ( *workshopListIterator )->data( 0, Qt::UserRole ).toInt(),
																	 zone );
					 ( *workshopListIterator )->setText( 2, time.toString() );
					 i++;
					 ++workshopListIterator;
				 }
			 } );

	connect( pAddButton, &QPushButton::pressed, this, [] {

	} );

	connect( pDeleteButton, &QPushButton::pressed, this, [this]
			 {
				 this->onDeletePressed();
			 } );

	connect( pEditButton, &QPushButton::pressed, this, [] {

	} );

	connect( pRefreshButton, &QPushButton::pressed, this, [this]
			 {
				 this->PopulateWorkshopList();
			 } );

	connect( m_pWorkshopItemTree, &QTreeWidget::itemSelectionChanged, this, [pEditButton, pDeleteButton, this]
			 {
				 pEditButton->setEnabled( m_pWorkshopItemTree->selectedItems().length() > 0 );
				 pDeleteButton->setEnabled( m_pWorkshopItemTree->selectedItems().length() > 0 );
			 } );

	this->PopulateWorkshopList();
}

void CMainView::OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure )
{
	if ( bFailure )
	{
		QMessageBox::critical( nullptr, "Fatal Error", "Failed retrieve Query." );
		return;
	}

	m_SteamUGCDetailsList.clear();
	m_pWorkshopItemTree->clear();

	for ( int index = 0; index < pQuery->m_unNumResultsReturned; index++ )
	{
		SteamUGCDetails_t pDetails {};
		SteamUGC()->GetQueryUGCResult( pQuery->m_handle, index, &pDetails );

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

		m_SteamUGCDetailsList.insert( std::make_pair( index, pDetails ) );
	}
}

void CMainView::PopulateWorkshopList()
{
	UGCQueryHandle_t hQueryResult = SteamUGC()->CreateQueryUserUGCRequest( SteamUser()->GetSteamID().GetAccountID(),
																		   k_EUserUGCList_Published,
																		   k_EUGCMatchingUGCType_Items_ReadyToUse,
																		   k_EUserUGCListSortOrder_CreationOrderDesc,
																		   SteamUtils()->GetAppID(), ConsumerID, 1 );
	SteamAPICall_t hApiQueryHandle = SteamUGC()->SendQueryUGCRequest( hQueryResult );
	m_SteamCallResultUGCRequest.Set( hApiQueryHandle, this, &CMainView::OnSendQueryUGCRequest );
	SteamUGC()->ReleaseQueryUGCRequest( hQueryResult );
}

void CMainView::onDeletePressed()
{
	QMessageBox box( QMessageBox::Icon::Critical, "DELETING ITEM", "This action will delete this item permanently from the workshop!", QMessageBox::Ok | QMessageBox::Abort, this );
	QCheckBox *checkbox = new QCheckBox( "I understand that this will delete this item from the workshop.", &box );
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
	int ret = box.exec();

	if ( ret == QMessageBox::Ok && checkbox->isChecked() )
	{
		QTreeWidgetItem *item = this->m_pWorkshopItemTree->selectedItems()[0];
		int itemIndex = item->data( 1, Qt::UserRole ).toInt();
		SteamUGCDetails_t Details = m_SteamUGCDetailsList.at( itemIndex );
		SteamAPICall_t call = SteamUGC()->DeleteItem( Details.m_nPublishedFileId );
		m_CallResultDeleteItem.Set( call, this, &CMainView::OnDeleteItem );
	}
}

void CMainView::OnDeleteItem( DeleteItemResult_t *pItem, bool bFailure )
{
	if ( bFailure )
	{
		QMessageBox::critical( nullptr, "Fatal Error", "Failed to delete item." );
		return;
	}

	this->PopulateWorkshopList();
}