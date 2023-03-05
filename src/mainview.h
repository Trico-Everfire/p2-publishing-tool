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
			int previewItemCount = 0;
			QVector<QStringList> imagePaths {};
			QVector<QString> videoURLs {};
		};

		struct FullUGCDetails
		{
			SteamUGCDetails_t standardDetails;
			QString thumbnailDetails {};
			AdditionalUGCDetails additionalDetails;
		};

	public:
		static constexpr int MAX_URL_SIZE = 2048;
		static constexpr AppId_t m_GameID = 440000;

		std::map<int, FullUGCDetails> m_SteamUGCDetailsList;
		QTreeWidget *m_pWorkshopItemTree;
		QComboBox *m_pTimezoneComboBox;

	private:
		QTimer m_CallbackTimer;

	public:
		CMainView( QWidget *pParent = nullptr );

	public:
		void populateWorkshopList();
		CCallResult<CMainView, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;
		void onSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure );

		void onDeletePressed();
		CCallResult<CMainView, DeleteItemResult_t> m_CallResultDeleteItem;
		void onDeleteItem( DeleteItemResult_t *pItem, bool bFailure );

	public:
		CMainView::AdditionalUGCDetails getAdditionalUGCPreviews( UGCQueryHandle_t queryHandle, int previewCount, int itemIndex, PublishedFileId_t fileID );
		QString downloadImageFromURL( const QString &imageUrl, QByteArray &imageData );
		static bool isFileWritable( const QString &fullPath );
	};

} // namespace ui
