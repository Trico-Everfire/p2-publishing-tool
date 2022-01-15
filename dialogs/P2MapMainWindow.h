#pragma once

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QTreeWidget>
#include <QMessageBox>
#include <QDateTime>
#include <QTimeZone>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFile>
#include "dialogs/P2MapPublisher.h"
#include "dialogs/P2LoopManager.h"
#include <steam_api.h>

namespace ui
{
	class CP2MapMainMenu : public QDialog
	{
		Q_OBJECT
        public:
            CP2MapMainMenu( QWidget *pParent );
            ~CP2MapMainMenu();
            std::map<int,SteamUGCDetails_t> SUGCD;
            static const AppId_t ConsumerID = 620;
        private:
            void runSetCallbacks();
            QPushButton *pEditButton;
            QString unixTimeToHumanReadable(uint32 seconds);
            QTreeWidget *m_treeWidget;
            QComboBox *m_timezoneComboBox;
            void OnSendQueryUGCRequest(SteamUGCQueryCompleted_t* pQuery, bool bFailure);
	        CCallResult<CP2MapMainMenu, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;
		    void OnDeleteItem(DeleteItemResult_t* pItem, bool bFailure);
		    CCallResult<CP2MapMainMenu, DeleteItemResult_t> m_CallResultDeleteItem;
            void OnSubmitItemUpdate(SubmitItemUpdateResult_t* pItem, bool bFailure);
            CCallResult<CP2MapMainMenu, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;
            void OnOldApiSubmitItemUpdate(RemoteStorageUpdatePublishedFileResult_t* pItem, bool pFailure);
		    CCallResult<CP2MapMainMenu, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;
            std::vector<int> totalLoadedItems;
        private slots:
        void onAddPressed();
        void onDeletePressed();
        void onEditPressed();
        void onRefreshPressed();
        void ChangedTimezone(const QString& index);
        void treeSelectionChanged();

    };
}
