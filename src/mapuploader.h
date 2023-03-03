#pragma once

#include "mainview.h"
#include "steam_api.h"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>

namespace ui
{
	class CAdvancedOptionsDialog : public QDialog
	{
	public:
		enum ListItemTypes
		{
			NO_TYPE = 0,
			ADD_IMAGE,
			ADD_VIDEO,
			ADD_TAG,
			IMAGE,
			VIDEO,
			TAG
		};

		QListWidget *m_pMediaListWidget;
		QListWidgetItem *m_pImageVideoSeparator;
		QLabel *m_pImagePreviewLabel;
		QGroupBox *m_pPatchNoteGroupBox;
		QListWidget *m_pTagsListWidget;
		QComboBox *m_pVisibilityComboBox;
		QCheckBox *m_pPTIInstanceCheckBox;
		QTextEdit *m_pPatchNoteTextEdit;
		QDialogButtonBox *m_pCloseBox;

	public:
		CAdvancedOptionsDialog( QWidget *pParent );
		void setEditItem( const CMainView::FullUGCDetails &itemDetails );
		void populateDefaultMediaListWidget();
		void addImageWidgetItem( const QString &name, const QString &path, bool removable = true );
		void addVideoWidgetItem( const QString &name, bool removable = true );
		void addTagWidgetItem( const QString &name, bool removable = true );
		void populateDefaultTagListWidget();
		void simpleInputDialog( QString &resultString );
		QListWidgetItem *createBasicListWidgetItem( const QString &labelText, QWidget *baseWidget, QListWidget *itemList, ListItemTypes type, bool removable = true );
	};

	class CMapUploader : public QDialog
	{
	public:
		static constexpr int MAX_IMAGE_SIZE = 1024 * 1024; // 1mb

		// We need to keep the strings alive long enough for
		// m_CallOldApiResultSubmitItemUpdate to process them.
		struct SteamTagListPreserver
		{
			SteamParamStringArray_t m_SteamTagList {};
			std::vector<std::shared_ptr<char[]>> m_SharedCharArrayPointerTagList {};
		};

		enum class SteamImageProcessError
		{
			NO_ERROR = 0,
			FILE_TOO_LARGE,
			FILE_DIRECTORY_CREATE_ERROR,
			FILE_SAVE_ERROR,
		};

		enum class RemoteStorageUploadError
		{
			NO_ERROR = 0,
			FILE_DOESNT_EXIST,
			FILE_INACCESSIBLE,
			QUOTA_EXCEEDED,
			STREAM_CREATE_ERROR,
			UPLOAD_ERROR,
		};

	private:
		bool m_IsEditing = false;
		bool m_EditedBSP = false;
		bool m_EditedThumbnail = false;
		bool m_MeetsPTIRequirements = false;
		int m_EditPreviewCount = 0;
		PublishedFileId_t m_PublishedFileId;

	public:
		QPushButton *m_pAdvancedOptionsButton;
		QPushButton *m_pOKButton;
		QLineEdit *m_pTitleLine;
		QTextEdit *m_pDescLine;
		QLineEdit *m_pBSPFileEntry;
		QCheckBox *m_pSteamToSAgreement;
		QLabel *m_pPreviewImageLabel;
		QString m_ThumbnailPath;
		CAdvancedOptionsDialog *m_pAdvancedOptions;

	private:
		CCallResult<CMapUploader, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;
		void updateWorkshopItemResult( SubmitItemUpdateResult_t *pItem, bool bFailure );

		void updateBSPWithOldWorkshopResult( RemoteStorageUpdatePublishedFileResult_t *pItem, bool bFailure );
		CCallResult<CMapUploader, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;

	public:
		CMapUploader( QWidget *pParent );

		void setEditItem( const CMainView::FullUGCDetails &itemDetails );
		void createNewWorkshopItem();
		void updateWorkshopItem( PublishedFileId_t publishedFileId );
		static QString processImageForSteamUpload( const QString &filePath, SteamImageProcessError &fileError, bool constraints, int width, int height, int size );

	private:
		CCallResult<CMapUploader, RemoteStoragePublishFileResult_t> m_CallResultPublishItem;
		void publishWorkshopItemResult( RemoteStoragePublishFileResult_t *pItem, bool bFailure );

		bool retrieveBSP( const QString &path, QStringList &tagList, bool &ptiRequirements );
		static bool parseBSPEntitiesToStringList( const QString &rawEntityLump, QStringList &entityList );
		static bool getTagsFromEntityStringList( const QStringList &entityList, QStringList &tagList, bool &ptiRequirements );
		bool canUploadProceed( QString &errorString ) const;
		void onBrowseBSPClicked();
		void onBrowseThumbnailClicked();
		CMapUploader::RemoteStorageUploadError uploadToSteamLocalStorage( const QString &localPath, QString &storageFileName );
	};
} // namespace ui