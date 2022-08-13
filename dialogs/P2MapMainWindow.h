#pragma once

#include "dialogs/P2LoopManager.h"
#include "dialogs/P2MapPublisher.h"

#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTimeZone>
#include <QTreeWidget>
#include <steam_api.h>

namespace ui
{
	class CP2MapMainMenu : public QDialog
	{
		Q_OBJECT
	public:
		CP2MapMainMenu( QWidget *pParent );
		~CP2MapMainMenu();
		std::map<int, SteamUGCDetails_t> SUGCD;
		static const AppId_t ConsumerID = 620;
		void runSetCallbacks();
		void OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure );
		QTreeWidget *m_treeWidget;

	private:
		QPushButton *pEditButton;
		QPushButton *pDeleteButton;
		QString unixTimeToHumanReadable( uint32 seconds );
		QComboBox *m_timezoneComboBox;
		CCallResult<CP2MapMainMenu, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;
		void OnDeleteItem( DeleteItemResult_t *pItem, bool bFailure );
		CCallResult<CP2MapMainMenu, DeleteItemResult_t> m_CallResultDeleteItem;
		void OnSubmitItemUpdate( SubmitItemUpdateResult_t *pItem, bool bFailure );
		CCallResult<CP2MapMainMenu, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;
		void OnOldApiSubmitItemUpdate( RemoteStorageUpdatePublishedFileResult_t *pItem, bool pFailure );
		CCallResult<CP2MapMainMenu, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;
		std::vector<int> totalLoadedItems;
	private slots:
		void onAddPressed();
		void onDeletePressed();
		void onEditPressed();
		void onRefreshPressed();
		void ChangedTimezone( const QString &index );
		void treeSelectionChanged();
	};
} // namespace ui
