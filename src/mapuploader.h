#pragma once

#include "mainview.h"
#include "steam_api.h"

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QGroupBox>
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

	public:
		CAdvancedOptionsDialog( QWidget *pParent );
		void setEditItem( const CMainView::FullUGCDetails& itemDetails );
		void populateDefaultMediaListWidget();
		void addImageWidgetItem( const QString& name, const QString& path, bool removable = true );
		void addVideoWidgetItem( const QString& name, bool removable = true );
		void addTagWidgetItem( const QString &name, bool removable = true );
		void populateDefaultTagListWidget();
		void simpleInputDialog( QString &resultString );
		QListWidgetItem *createBasicListWidgetItem( const QString& labelText, QWidget* baseWidget, QListWidget *itemList, ListItemTypes type, bool removable = true );
	};

	class CMapUploader : public QDialog
	{

	private:
		bool m_isEditing = false;
		bool m_editedBSP = false;
		bool m_editedThumbnail = false;
		PublishedFileId_t m_PublishedFileId;

	public:
		QPushButton *m_pAdvancedOptionsButton;
		QPushButton *m_pOKButton;
		QLineEdit *m_pTitleLine;
		QTextEdit *m_pDescLine;
		QLineEdit *m_pFileEntry;
		QCheckBox *m_pSteamToSAgreement;
		QLabel *m_pPreviewImageLabel;
		QString m_thumbnailPath;
		CAdvancedOptionsDialog *m_pAdvancedOptions;

	public:
		CMapUploader( QWidget *pParent );
		void setEditItem( const CMainView::FullUGCDetails& itemDetails );
		void createNewWorkshopItem();
		void updateWorkshopItem(PublishedFileId_t publishedFileId);

	private:
		CCallResult<CMapUploader, CreateItemResult_t> m_CallResultCreateItem;
		void createWorkshopItemResult( CreateItemResult_t *pItem, bool bFailure );

		bool retrieveBSP( const QString& path, QStringList &tagList );
		static bool parseBSPEntitiesToStringList( const QString &rawEntityLump, QStringList &entityList );
		static bool getTagsFromEntityStringList( const QStringList &entityList, QStringList &tagList );
	};
} // namespace ui