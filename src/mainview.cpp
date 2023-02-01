#include <QGridLayout>
#include <QTimeZone>
#include <QLabel>
#include <QMessageBox>
#include "mainview.h"

using namespace ui;

CMainView::CMainView(QWidget *pParent) : QDialog(pParent) {

    this->setWindowTitle("Portal 2 Map Uploader");

    auto pMainViewLayout = new QGridLayout(this);

    m_pWorkshopItemTree = new QTreeWidget(this);
    auto qStringList = QStringList();
    qStringList.append("Title");
    qStringList.append("File");
    qStringList.append("Last Updated");
    m_pWorkshopItemTree->setHeaderLabels(qStringList);
    m_pWorkshopItemTree->headerItem()->setTextAlignment(0, Qt::AlignCenter);
    m_pWorkshopItemTree->headerItem()->setTextAlignment(1, Qt::AlignCenter);
    m_pWorkshopItemTree->headerItem()->setTextAlignment(2, Qt::AlignCenter);
    m_pWorkshopItemTree->setColumnWidth(0, 700 / 3);
    m_pWorkshopItemTree->setColumnWidth(1, 700 / 3);
    m_pWorkshopItemTree->setColumnWidth(2, 700 / 3);

    pMainViewLayout->addWidget(m_pWorkshopItemTree, 0, 0, 1, 6);

    auto pTimezoneLabel = new QLabel("Timezone:", this);
    auto timezoneFont = pTimezoneLabel->font();
    timezoneFont.setPointSizeF(timezoneFont.pointSizeF() * 1.4);
    pTimezoneLabel->setFont(timezoneFont);

    pMainViewLayout->addWidget(pTimezoneLabel, 1, 0, 1, 0, Qt::AlignLeft);

    m_pTimezoneComboBox = new QComboBox(this);
    foreach(auto timezoneID, QTimeZone::availableTimeZoneIds())
            m_pTimezoneComboBox->addItem(timezoneID);

    QDateTime time = QDateTime::currentDateTime();
    m_pTimezoneComboBox->setCurrentIndex( QTimeZone::availableTimeZoneIds().indexOf( time.timeZone().id() ) );

    pMainViewLayout->addWidget(m_pTimezoneComboBox, 1, 1, Qt::AlignLeft);

    pMainViewLayout->setSpacing(0);

    m_CallbackTimer.setSingleShot(false);
    connect(&m_CallbackTimer, &QTimer::timeout, this, [](){
        SteamAPI_RunCallbacks();
    });

    m_CallbackTimer.start(100);

    connect(m_pTimezoneComboBox, &QComboBox::currentTextChanged, this, [this](const QString &index){
        QTimeZone zone = QTimeZone( QByteArray::fromStdString( index.toStdString() ) );
        QTreeWidgetItemIterator workshopListIterator( m_pWorkshopItemTree );
        int i = 0;
        while ( *workshopListIterator )
        {
            QDateTime time = QDateTime::fromSecsSinceEpoch(( *workshopListIterator )->data(0, Qt::UserRole).toInt(), zone );
            ( *workshopListIterator )->setText(2, time.toString() );
            i++;
            ++workshopListIterator;
        }
    });

    this->PopulateWorkshopList();

    this->resize(740, 300);

}

void CMainView::OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure )
{
    if ( bFailure )
    {
        QMessageBox::critical( nullptr, "Fatal Error", "Failed retrieve Query." );
        return;
    }

    m_SteamUGCDetailsList.clear();

    for ( int index = 0; index < pQuery->m_unNumResultsReturned; index++ )
    {
        SteamUGCDetails_t pDetails {};
        SteamUGC()->GetQueryUGCResult( pQuery->m_handle, index, &pDetails );

        QDateTime time = QDateTime::fromSecsSinceEpoch( pDetails.m_rtimeUpdated );
        time.setTimeZone(QTimeZone(QByteArray(m_pTimezoneComboBox->currentText().toStdString().c_str())));
        m_pTimezoneComboBox->setCurrentIndex( QTimeZone::availableTimeZoneIds().indexOf( time.timeZone().id() ) );
        auto pWorkshopitem = new QTreeWidgetItem(0 );
        pWorkshopitem->setText(0, pDetails.m_rgchTitle );
        pWorkshopitem->setText(1, pDetails.m_pchFileName );
        pWorkshopitem->setText(2, time.toString() );
        pWorkshopitem->setData(0, Qt::UserRole, pDetails.m_rtimeUpdated );
        pWorkshopitem->setData(1, Qt::UserRole, index );
        pWorkshopitem->setTextAlignment(0, Qt::AlignCenter );
        pWorkshopitem->setTextAlignment(1, Qt::AlignCenter );
        pWorkshopitem->setTextAlignment(2, Qt::AlignCenter );
        m_pWorkshopItemTree->addTopLevelItem(pWorkshopitem );

        m_SteamUGCDetailsList.insert( std::make_pair( index, pDetails ) );
    }

}

void CMainView::PopulateWorkshopList() {

    UGCQueryHandle_t hQueryResult = SteamUGC()->CreateQueryUserUGCRequest( SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published, k_EUGCMatchingUGCType_Items_ReadyToUse, k_EUserUGCListSortOrder_CreationOrderDesc, SteamUtils()->GetAppID(), ConsumerID, 1 );
    SteamAPICall_t hApiQueryHandle = SteamUGC()->SendQueryUGCRequest( hQueryResult );
    m_SteamCallResultUGCRequest.Set( hApiQueryHandle, this, &CMainView::OnSendQueryUGCRequest );
    SteamUGC()->ReleaseQueryUGCRequest( hQueryResult );

}
