
#pragma once

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDesktopServices>
#include "dialogs/P2MapMainWindow.h"
#include "dialogs/P2PublisherAdvancedOptions.h"
#include "dialogs/P2LoopManager.h"
#include "dialogs/P2BSPReader.h"
#include "dialogs/P2PTIDialog.h"
#include "../libs/Steam-Workshop-API/sdk/public/steam/steam_api.h"

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
        QPixmap *m_pPreviewImage;
		QLabel *pImageLabel;
		QLineEdit *pFileEntry;
		QLineEdit *pTitleLine;
		QTextEdit *pDescLine; 
		QString defaultFileLocBSP = "./";
		QString defaultFileLocIMG = "./";
		QCheckBox *pSteamToSAgreement;
		void LoadExistingDetails(SteamUGCDetails_t details);
		void setDefaultBspLoc(QString string);
		void setDefaultImgpLoc(QString string);
		void LoadCreatingItem();
		void UpdateItem(PublishedFileId_t itemID);
		void LoadEditItem();

		// Every callback starts with the type sent and a bool
		void OnSendQueryUGCRequest(SteamUGCQueryCompleted_t* pQuery, bool bFailure);
		CCallResult<CP2MapPublisher, SteamUGCQueryCompleted_t> m_CallResultSendQueryUGCRequest;

		void OnCreateItem(CreateItemResult_t* pItem, bool bFailure);
		CCallResult<CP2MapPublisher, CreateItemResult_t> m_CallResultCreateItem;

		void OnSubmitItemUpdate(SubmitItemUpdateResult_t* pItem, bool bFailure);
		CCallResult<CP2MapPublisher, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;

		void OnOldApiSubmitItemUpdate(RemoteStorageUpdatePublishedFileResult_t* pItem, bool pFailure);
		CCallResult<CP2MapPublisher, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;

		void OnOldApiSubmitItemDownload(RemoteStorageDownloadUGCResult_t  * pItem, bool pFailure);
		CCallResult<CP2MapPublisher, RemoteStorageDownloadUGCResult_t  > m_CallOldApiResultSubmitItemDownload;


	private:
	SteamUGCDetails_t m_editItemDetails;
	CP2PublisherAdvancedOptions* AO;
	QDialog* m_advancedOptionsWindow;
	private slots:
		void OpenImageFileExplorer();
		void OpenBSPFileExplorer();
		void onOKPressed();
		void onClosePressed();
		void onAgreementButtonPressed();
		void onAdvancedClicked();
    };
}