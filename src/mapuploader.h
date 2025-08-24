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

	public:
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

	public:
		void setEditItem( const CMainView::FullUGCDetails &itemDetails );
		QListWidgetItem *createBasicListWidgetItem( const QString &labelText, QWidget *pBaseWidget, QListWidget *pListWidget, ListItemTypes listItemType, bool removable = true );
		void addImageWidgetItem( const QString &imageFileName, const QString &imageFilePath, bool removable = true );
		void addVideoWidgetItem( const QString &videoUID, bool removable = true );
		void addTagWidgetItem( const QString &tagName, bool removable = true );
		void simpleInputDialog( QString &fieldText );
		void populateDefaultMediaListWidget();
		void populateDefaultTagListWidget();
	};

	class CMapUploader : public QDialog
	{
	public:
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
		bool m_isPreviewMarkedDirty = false;
		int m_EditPreviewCount = 0;
		QString remoteFile;
		PublishedFileId_t m_PublishedFileId;

	public:
		static constexpr int MAX_IMAGE_SIZE = 1024 * 1024; // 1mb
		QPushButton *m_pAdvancedOptionsButton;
		QPushButton *m_pOKButton;
		QLineEdit *m_pTitleLine;
		QTextEdit *m_pDescriptionTextEdit;
		QLineEdit *m_pBSPFileLineEdit;
		QCheckBox *m_pSteamToSAgreement;
		QLabel *m_pPreviewImageLabel;
		QString m_ThumbnailPath;
		CAdvancedOptionsDialog *m_pAdvancedOptions;

	public:
		CMapUploader( QWidget *pParent );

	public:
		CCallResult<CMapUploader, RemoteStorageUpdatePublishedFileResult_t> m_CallOldApiResultSubmitItemUpdate;
		void updateBSPWithOldWorkshopResult( RemoteStorageUpdatePublishedFileResult_t *pItem, bool bFailure );

		void updateWorkshopItem( PublishedFileId_t publishedFileId );
		CCallResult<CMapUploader, SubmitItemUpdateResult_t> m_CallResultSubmitItemUpdate;
		void updateWorkshopItemResult( SubmitItemUpdateResult_t *pItem, bool bFailure );

		void createNewWorkshopItem();
		CCallResult<CMapUploader, RemoteStoragePublishFileResult_t> m_CallResultPublishItem;
		void publishWorkshopItemResult( RemoteStoragePublishFileResult_t *pItem, bool bFailure );

	public:
		bool retrieveBSP( const QString &path, QStringList &tagList, bool &ptiRequirements );
		static bool parseBSPEntitiesToStringList( const QString &rawEntityLump, QStringList &entityList );
		static bool getTagsFromEntityStringList( const QStringList &entityList, QStringList &tagList, bool &ptiRequirements );
		static QString processImageForSteamUpload( const QString &filePath, SteamImageProcessError &fileError, bool followSizeConstraints, int width, int height, int size );
		static CMapUploader::RemoteStorageUploadError uploadToSteamLocalStorage( const QString &localPath, QString &storageFileName );
		void setEditItem( const CMainView::FullUGCDetails &itemDetails );
		bool canUploadProceed( QString &errorString ) const;
		void onBrowseBSPClicked();
		void onBrowseThumbnailClicked();
		void markPreviewsDirty();
	};
} // namespace ui