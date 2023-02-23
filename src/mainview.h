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
	public:
		struct AdditionalUGCDetails
		{
			int amount = 0;
			QVector<QStringList> imagePaths {};
			QVector<QString> videoURLs {};
		};

		struct FullUGCDetails
		{
			SteamUGCDetails_t standardDetails;
			QString thumbnailDetails {};
			AdditionalUGCDetails additionalDetails;
		};

		std::map<int, FullUGCDetails> m_SteamUGCDetailsList;

		static constexpr int MAX_URL_SIZE = 2048;

	private:
		QTimer m_CallbackTimer;
	public:

		CMainView( QWidget *pParent = nullptr );
		static constexpr AppId_t m_GameID = 620;
		void onSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure );

		QTreeWidget *m_pWorkshopItemTree;
		QComboBox *m_pTimezoneComboBox;

		CCallResult<CMainView, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;
		CCallResult<CMainView, DeleteItemResult_t> m_CallResultDeleteItem;

		void populateWorkshopList();
		void onDeletePressed();
		void onDeleteItem( DeleteItemResult_t *pItem, bool bFailure );
		CMainView::AdditionalUGCDetails getAdditionalUGCPreviews( UGCQueryHandle_t queryHandle, int count, int itemIndex, PublishedFileId_t fileID );
		bool downloadImageFromURL( const QString &url, QString &fileName, QByteArray &imageData );
		static bool isFileWritable( const QString& fullPath );
	};

} // namespace ui
