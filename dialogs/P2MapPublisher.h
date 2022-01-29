
#pragma once

#include "../libs/Steam-Workshop-API/sdk/public/steam/steam_api.h"
#include "dialogs/P2BSPReader.h"
#include "dialogs/P2LoopManager.h"
#include "dialogs/P2MapMainWindow.h"
#include "dialogs/P2PTIDialog.h"
#include "dialogs/P2PublisherAdvancedOptions.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QBuffer>
#include <QImageReader>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QCloseEvent>
#include <sstream>


namespace ui
{
	class CP2MapPublisher : public QDialog
	{
		Q_OBJECT

	public:
		CP2MapPublisher( QWidget *pParent );
		CP2MapPublisher( QWidget *pParent, bool edit );
		bool m_edit;
		bool m_bspHasPTIInstance;
		uint32 m_EditItemIndex;
		QPixmap *m_pPreviewImage;
		QLabel *pImageLabel;
		QLineEdit *pFileEntry;
		QLineEdit *pTitleLine;
		QTextEdit *pDescLine;
		QString defaultFileLocBSP = "./";
		QString defaultFileLocIMG = "./";
		QCheckBox *pSteamToSAgreement;
		void LoadExistingDetails( SteamUGCDetails_t details, uint32 index );
		void setDefaultBspLoc( QString string );
		void setDefaultImgpLoc( QString string );
		void LoadCreatingItem();
		void UpdateItem( PublishedFileId_t itemID );
		void LoadEditItem();
		uint32 iCount = 0;
		std::vector<std::string> splitString(const std::string& input, char delimiter);

		// Every callback starts with the type sent and a bool
		void OnSendQueryUGCRequest( SteamUGCQueryCompleted_t *pQuery, bool bFailure );
		CCallResult<CP2MapPublisher, SteamUGCQueryCompleted_t> m_CallResultSendQueryUGCRequest;

		void OnCreateItem( CreateItemResult_t *pItem, bool bFailure );
		CCallResult<CP2MapPublisher, CreateItemResult_t> m_CallResultCreateItem;

		void OnSubmitItemUpdate( SubmitItemUpdateResult_t *pItem, bool bFailure );
		CCallResult<CP2MapPublisher, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;

		void OnOldApiSubmitItemUpdate( RemoteStorageUpdatePublishedFileResult_t *pItem, bool pFailure );
		CCallResult<CP2MapPublisher, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;

		void OnOldApiSubmitItemDownload( RemoteStorageDownloadUGCResult_t *pItem, bool pFailure );
		CCallResult<CP2MapPublisher, RemoteStorageDownloadUGCResult_t> m_CallOldApiResultSubmitItemDownload;
		CCallResult<CP2MapPublisher, SteamUGCQueryCompleted_t> m_SteamCallResultUGCRequest;
	signals:
		void mapPublisherClosed();
	private:
		SteamUGCDetails_t m_editItemDetails;
		CP2PublisherAdvancedOptions *AO;
		QDialog *m_advancedOptionsWindow;
	private slots:
		void OpenImageFileExplorer();
		void OpenBSPFileExplorer();
		void onOKPressed();
		void onClosePressed();
		void onAgreementButtonPressed();
		void onAdvancedClicked();
		void closeEvent( QCloseEvent* event );
	};
} // namespace ui