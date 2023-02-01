#pragma once

#include "steam_api.h"

#include <QComboBox>
#include <QDialog>
#include <QTimer>
#include <QTreeWidget>

namespace ui
{

	class CMainView : public QDialog
	{
		std::map<int, SteamUGCDetails_t> m_SteamUGCDetailsList;
		QTimer m_CallbackTimer;

	public:
		CMainView( QWidget *pParent = nullptr );
		static constexpr AppId_t ConsumerID = 620;
		void OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure );

		QTreeWidget *m_pWorkshopItemTree;
		QComboBox *m_pTimezoneComboBox;

		CCallResult<CMainView, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;

		void PopulateWorkshopList();
	};

} // namespace ui
