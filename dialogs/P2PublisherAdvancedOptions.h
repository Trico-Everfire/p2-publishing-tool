/********************************************************************************
** Form generated from reading UI file 'AO.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#pragma once
#ifndef P2PUBLISHERADVANCEDOPTIONS_H
#define P2PUBLISHERADVANCEDOPTIONS_H

#include <QComboBox>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QAction>
#include <QMessageBox>
#include <regex>
#include <QRegExp>
#include <QFileDialog>
#include <QPixmap>
#include <QDebug>
#include <QFileInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

class Ui_Advanced
{
public:
	QDialogButtonBox *buttonBox;
	QCheckBox *checkBox_3;
	QLineEdit *lineEdit;
	QPushButton *toolButton;
	QTreeWidget *treeWidget;
	// QPushButton *toolButton_2;
	// QPushButton *toolButton_3;
	QPushButton *toolButton_4;
	QPushButton *toolButton_5;
	QPushButton *toolButton_6;
	QPushButton *toolButton_7;
    QPushButton *toolButton_8;
	QLabel *label;
	QLabel *label_3;
	QLabel *label_4;
	QTextEdit *textEdit;
	QLabel *label_5;
	QLineEdit *lineEdit_2;
	QTreeWidget *treeWidget_2;
	QComboBox *comboBox;
	QLabel *label_2;
    QTreeWidget *ImageTree;
	QString defaultFileLocIMG = "./";

	void setupUi( QDialog *Advanced )
	{
		if ( Advanced->objectName().isEmpty() )
			Advanced->setObjectName( QString::fromUtf8( "Advanced" ) );
		Advanced->resize( 700, 442 );
		Advanced->setWindowFilePath( QString::fromUtf8( "" ) );
		buttonBox = new QDialogButtonBox( Advanced );
		buttonBox->setObjectName( QString::fromUtf8( "buttonBox" ) );
		buttonBox->setGeometry( QRect( 510, 390, 181, 41 ) );
		buttonBox->setOrientation( Qt::Horizontal );
		buttonBox->setStandardButtons( QDialogButtonBox::Ok );
		checkBox_3 = new QCheckBox( Advanced );
		checkBox_3->setObjectName( QString::fromUtf8( "checkBox_3" ) );
		checkBox_3->setGeometry( QRect( 20, 405, 241, 22 ) );
		lineEdit = new QLineEdit( Advanced );
		lineEdit->setObjectName( QString::fromUtf8( "lineEdit" ) );
		lineEdit->setGeometry( QRect( 20, 180, 141, 32 ) );
		toolButton = new QPushButton( Advanced );
		toolButton->setObjectName( QString::fromUtf8( "toolButton" ) );
		toolButton->setGeometry( QRect( 170, 180, 81, 31 ) );
		treeWidget = new QTreeWidget( Advanced );
		new QTreeWidgetItem( treeWidget );
		treeWidget->setObjectName( QString::fromUtf8( "treeWidget" ) );
		treeWidget->setGeometry( QRect( 20, 220, 231, 131 ) );
		ImageTree = new QTreeWidget( Advanced );
		ImageTree->setGeometry( QRect( 180, 10, 181, 131 ) );
		ImageTree->setObjectName( QString::fromUtf8( "ImageTree" ) );
		toolButton_4 = new QPushButton( Advanced );
		toolButton_4->setObjectName( QString::fromUtf8( "toolButton_4" ) );
		toolButton_4->setGeometry( QRect( 20, 150, 151, 21 ) );
		toolButton_5 = new QPushButton( Advanced );
		toolButton_5->setObjectName( QString::fromUtf8( "toolButton_5" ) );
		toolButton_5->setDisabled(true);
		toolButton_5->setGeometry( QRect( 180, 150, 181, 21 ) );
		toolButton_7 = new QPushButton( Advanced );
		toolButton_7->setObjectName( QString::fromUtf8( "toolButton_5" ) );
		toolButton_7->setGeometry( QRect( 20, 360, 231, 31 ) );
        toolButton_8 = new QPushButton( Advanced );
		toolButton_8->setObjectName( QString::fromUtf8( "toolButton_5" ) );
		toolButton_8->setGeometry( QRect( 440, 180, 231, 31 ) );
		label = new QLabel( Advanced );
		label->setObjectName( QString::fromUtf8( "label" ) );
		label->setGeometry( QRect( 20, 50, 151, 91 ) );
		label->setScaledContents( true );
		label_3 = new QLabel( Advanced );
		label_3->setObjectName( QString::fromUtf8( "label_3" ) );
		label_3->setGeometry( QRect( 20, 10, 81, 31 ) );
		label_4 = new QLabel( Advanced );
		label_4->setObjectName( QString::fromUtf8( "label_4" ) );
		label_4->setGeometry( QRect( 380, 10, 81, 31 ) );
		textEdit = new QTextEdit( Advanced );
		textEdit->setObjectName( QString::fromUtf8( "textEdit" ) );
		textEdit->setGeometry( QRect( 290, 220, 391, 151 ) );
		label_5 = new QLabel( Advanced );
		label_5->setObjectName( QString::fromUtf8( "label_5" ) );
		label_5->setGeometry( QRect( 290, 180, 131, 31 ) );
		lineEdit_2 = new QLineEdit( Advanced );
		lineEdit_2->setObjectName( QString::fromUtf8( "lineEdit_2" ) );
		lineEdit_2->setGeometry( QRect( 440, 10, 141, 32 ) );
		toolButton_6 = new QPushButton( Advanced );
		toolButton_6->setObjectName( QString::fromUtf8( "toolButton_6" ) );
		toolButton_6->setGeometry( QRect( 590, 10, 81, 31 ) );
		treeWidget_2 = new QTreeWidget( Advanced );
		treeWidget_2->setObjectName( QString::fromUtf8( "treeWidget_2" ) );
		treeWidget_2->setGeometry( QRect( 440, 50, 231, 121 ) );
		comboBox = new QComboBox( Advanced );
		comboBox->addItem( QString() );
		comboBox->addItem( QString() );
		comboBox->addItem( QString() );
		comboBox->addItem( QString() ); 
		comboBox->setObjectName( QString::fromUtf8( "comboBox" ) );
		comboBox->setGeometry( QRect( 350, 380, 90, 30 ) );
		label_2 = new QLabel( Advanced );
		label_2->setObjectName( QString::fromUtf8( "label_2" ) );
		label_2->setGeometry( QRect( 290, 380, 71, 31 ) );

		retranslateUi( Advanced );
		QObject::connect( buttonBox, SIGNAL( accepted() ), Advanced, SLOT( accept() ) );
		QObject::connect( toolButton, &QPushButton::pressed, Advanced, [&](){
            if(lineEdit->text().isEmpty()) return;
            if(lineEdit_2->text().length() > 255){
                QMessageBox::warning( nullptr, "TOO MANY CHARACTERS!", "This tag contains too many characters! (max is 255)" );
                return;
            }
            for(QString string : lineEdit_2->text()){
                QString valid_characters = "!\"#$%&'()*+-./:;<=>?@[\\]^_`{|}~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
                if(!valid_characters.contains(string)){
                    QMessageBox::warning( nullptr, "INVALID CHARACTERS", "This tag contains a invalid character.\nhttps://en.cppreference.com/w/c/string/byte/isprint\n for more information." );
                    return;
                }
            }
            auto item = new QTreeWidgetItem( 0 );
            item->setText( 0, lineEdit->text() );
            treeWidget->addTopLevelItem(item);
            lineEdit->setText(QString());
        });
        QObject::connect( treeWidget, &QTreeWidget::itemSelectionChanged, Advanced, [&](){
            if ( treeWidget->selectedItems().length() == 1 ){
                toolButton_7->setEnabled(true);
            } else {
                toolButton_7->setEnabled(false);
            }
        });
        QObject::connect( toolButton_7, &QPushButton::pressed, Advanced, [&](){
            delete treeWidget->selectedItems()[0];
        });

        QObject::connect( toolButton_6, &QPushButton::pressed, Advanced, [&](){
            if(lineEdit_2->text().isEmpty()) return;
            if(lineEdit_2->text().contains("http://") || lineEdit_2->text().contains("https://") || lineEdit_2->text().contains("www.you")){
                QMessageBox::warning( nullptr, "Invalid Handle", "You need to enter in a youtube handle, not a link.\nhandles look like this: (dQw4w9WgXcQ) and can be found at the end of a youtube link: (https://www.youtube.com/watch?v=)\nOnly the handle is allowed!" );
                return;
            }
            auto item = new QTreeWidgetItem( 0 );
            item->setText( 0, lineEdit_2->text() );
            treeWidget_2->addTopLevelItem(item);
            lineEdit_2->setText(QString());
        });
        QObject::connect( treeWidget_2, &QTreeWidget::itemSelectionChanged, Advanced, [&](){
            if ( treeWidget_2->selectedItems().length() == 1 ){
                toolButton_8->setEnabled(true);
            } else {
                toolButton_8->setEnabled(false);
            }
        });
        QObject::connect( toolButton_8, &QPushButton::pressed, Advanced, [&](){
            //int index = treeWidget->indexOfTopLevelItem(treeWidget->selectedItems()[0]);
            delete treeWidget_2->selectedItems()[0];
            //treeWidget->removeItemWidget(treeWidget->selectedItems()[0],0);
        });

        QObject::connect( toolButton_4, &QPushButton::pressed, Advanced, [&](){
			auto opts = QFileDialog::Option::DontUseNativeDialog;
			QString filePath = QFileDialog::getOpenFileName( nullptr, "Open", defaultFileLocIMG, "*.tga *.bmp *.png *.jpeg", nullptr, opts );
			if(filePath.isEmpty()) return;
			defaultFileLocIMG = filePath;
			QPixmap tempMap = QPixmap( filePath );
			if(tempMap.isNull()){
				QMessageBox::warning( nullptr, "Invalid Image", "Could not load the image." );
				return;
			}
			tempMap = tempMap.scaled( 201, 121, Qt::IgnoreAspectRatio );
			label->setPixmap(tempMap);
			auto item = new QTreeWidgetItem( 0 );
			QFileInfo fileInfo(filePath);
            item->setText( 0, fileInfo.fileName() );
			item->setData(0,Qt::UserRole,filePath);
			ImageTree->addTopLevelItem(item);
			ImageTree->clearSelection();
			item->setSelected(true);
		});
		QObject::connect( toolButton_5, &QPushButton::pressed, Advanced, [&](){
			delete ImageTree->selectedItems()[0];
		});
		QObject::connect( ImageTree, &QTreeWidget::itemSelectionChanged, Advanced, [&](){
			
			if ( ImageTree->selectedItems().length() == 1 ){
				QString filePath = ImageTree->selectedItems()[0]->data(0,Qt::UserRole).toString();
				QPixmap tempMap;
				if(filePath.startsWith("https://steamuserimages-a.akamaihd.net/ugc/")){
					qInfo() << filePath;
					QNetworkAccessManager manager{};

					QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(filePath)));

					QEventLoop loop;
					QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
					loop.exec();

					QByteArray bts = reply->readAll();
					// qInfo() << bts;
					delete reply;

					tempMap = QPixmap();
					tempMap.loadFromData(bts);
				} else {
					tempMap = QPixmap( filePath );
					toolButton_5->setEnabled(true);
				}
				// qInfo() << ImageTree->selectedItems()[0]->data();
				label->setPixmap(tempMap);
            } else {
				label->setPixmap(QPixmap( "" ));
                toolButton_5->setEnabled(false);
            }
        });

        QMetaObject::connectSlotsByName( Advanced );
	} // setupUi

	void retranslateUi( QDialog *Advanced )
	{
		Advanced->setWindowTitle( QCoreApplication::translate( "Advanced", "Advanced Options", nullptr ) );
#if QT_CONFIG( accessibility )
		Advanced->setAccessibleName( QString() );
#endif // QT_CONFIG(accessibility)
		checkBox_3->setText( QCoreApplication::translate( "Advanced", "Allow Upload Without PTI Instance", nullptr ) );
		toolButton->setText( QCoreApplication::translate( "Advanced", "Add Tag", nullptr ) );
		QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
		___qtreewidgetitem->setText( 0, QCoreApplication::translate( "Advanced", "Tags", nullptr ) );

		treeWidget->setSortingEnabled( false );
		QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem( 0 );
		___qtreewidgetitem1->setText( 0, QCoreApplication::translate( "Advanced", "Singleplayer", nullptr ) );
		___qtreewidgetitem1->setDisabled( true );

		//toolButton_2->setText( QCoreApplication::translate( "Advanced", "Up", nullptr ) );
		//toolButton_3->setText( QCoreApplication::translate( "Advanced", "Down", nullptr ) );
		toolButton_4->setText( QCoreApplication::translate( "Advanced", "Add", nullptr ) );
		toolButton_5->setText( QCoreApplication::translate( "Advanced", "Remove", nullptr ) );
		toolButton_7->setText( QCoreApplication::translate( "Advanced", "Remove Tag", nullptr ) );
		toolButton_7->setDisabled( true );
        toolButton_8->setText( QCoreApplication::translate( "Advanced", "Remove Video", nullptr ) );
		toolButton_8->setDisabled( true );
		label->setText( QString() );
		label_3->setText( QCoreApplication::translate( "Advanced", "Images", nullptr ) );
		label_4->setText( QCoreApplication::translate( "Advanced", "Videos", nullptr ) );
		label_5->setText( QCoreApplication::translate( "Advanced", "Patch Notes", nullptr ) );
		toolButton_6->setText( QCoreApplication::translate( "Advanced", "Add Video", nullptr ) );
		QTreeWidgetItem *___qtreewidgetitem2 = treeWidget_2->headerItem();
		___qtreewidgetitem2->setText( 0, QCoreApplication::translate( "Advanced", "Video Handles", nullptr ) );
		comboBox->setItemText( 0, QCoreApplication::translate( "Advanced", "public", nullptr ) );
		comboBox->setItemText( 1, QCoreApplication::translate( "Advanced", "private", nullptr ) );
		comboBox->setItemText( 2, QCoreApplication::translate( "Advanced", "friends", nullptr ) );
		comboBox->setItemText( 3, QCoreApplication::translate( "Advanced", "unlisted", nullptr ) );
				QTreeWidgetItem *___qtreewidgetitemImage = ImageTree->headerItem();
		___qtreewidgetitemImage->setText( 0, QCoreApplication::translate( "Advanced", "Images", nullptr ) );
		label_2->setText( QCoreApplication::translate( "Advanced", "Visibility:", nullptr ) );
		Advanced->setFixedSize( 700, 442 );
	} // retranslateUi
};

namespace ui
{
	class CP2PublisherAdvancedOptions : public Ui_Advanced
	{
	};
} // namespace ui

QT_END_NAMESPACE

#endif // P2PUBLISHERADVANCEDOPTIONS_H
