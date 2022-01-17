/********************************************************************************
** Form generated from reading UI file 'PTI Instance Required.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef P2PTIDIALOG_H
#define P2PTIDIALOG_H

#include <QDialogButtonBox>
#include <QGraphicsDropShadowEffect>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PTIIR
{
public:
	QFrame *frame;
	QLabel *label;
	QPushButton *pushButton;
	QDialogButtonBox *buttonBox;

	void setupUi( QDialog *PTIIR )
	{
		if ( PTIIR->objectName().isEmpty() )
			PTIIR->setObjectName( QString::fromUtf8( "PTIIR" ) );
		PTIIR->resize( 720, 440 );
		PTIIR->setStyleSheet( QString::fromUtf8( "background:8a8a8a" ) );
		frame = new QFrame( PTIIR );
		frame->setObjectName( QString::fromUtf8( "frame" ) );
		frame->setGeometry( QRect( 0, 0, 720, 440 ) );
		frame->setStyleSheet( QString::fromUtf8( "#frame{\n"
												 "border-image:url(:/zoo_textures/P2PublisherPTIBG.png);\n"
												 "opacity: 127;\n"
												 "}" ) );
		frame->setFrameShape( QFrame::StyledPanel );
		frame->setFrameShadow( QFrame::Raised );
		label = new QLabel( frame );
		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect( label );
		effect->setOffset( 0 );
		effect->setBlurRadius( 5 );
		effect->setColor( QColor( 0, 0, 0, 255 ) );
		label->setGraphicsEffect( effect );
		label->setObjectName( QString::fromUtf8( "label" ) );
		label->setGeometry( QRect( 10, 0, 720, 440 ) );
		buttonBox = new QDialogButtonBox( PTIIR );
		buttonBox->setGeometry( QRect( 620, 395, 90, 35 ) );
		buttonBox->setOrientation( Qt::Horizontal );
		buttonBox->setStandardButtons( QDialogButtonBox::Ok );
		retranslateUi( PTIIR );

		QObject::connect( buttonBox, SIGNAL( accepted() ), PTIIR, SLOT( accept() ) );
		QMetaObject::connectSlotsByName( PTIIR );
	} // setupUi

	void retranslateUi( QDialog *PTIIR )
	{
		PTIIR->setWindowTitle( QCoreApplication::translate( "PTIIR", "PTI Instance Required!", nullptr ) );
		label->setText( QCoreApplication::translate( "PTIIR", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
															  "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
															  "p, li { white-space: pre-wrap; }\n"
															  "</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#0586ff;\">Portal 2 Workshop Instance!</span></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Maps built using the <span style=\" font-weight:600; color:#ffffff;\">Portal 2 Authoring Tools</span> must have certain map instances added</p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">to them in order to work correctly with Steam Workshop. To make your maps Workshop </p"
															  ">\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">compliant, simply follow these steps:</p>\n"
															  "<p style=\"-qt-paragraph-type:empty; margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1. Add a func_instance to your map and set the filename to: <span style=\" font-weight:600;\">sdk_content/maps</span></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">/instances/p2editor/global_pti_ents.vmf.</span></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2. To trigger the end of the map and open the voting dialog menu, trigger the <span style=\" font-weight:600;\">"
															  "logic_relay</span></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">named <span style=\" font-weight:600;\">@relay_pti_level_end</span>.</p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3. Finally, to suppress the automatic voice over commentary by Cave Johnson, set the </p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">$disable_pti_audio</span> keyvalue to <span style=\" font-weight:600;\">1</span>. This step is optional and only necessary if you have </p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">your own dialog or story to tell.</p>\n"
															  "<p style=\"-qt-paragraph-type:empty; margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent"
															  ":0; text-indent:0px;\"><br /></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If you wish to upload your map without this instance.<br />check the <span style=\" font-weight:600;\">Allow Upload Without PTI Instance </span>checkbox in <span style=\" font-weight:600;\">Advanced Options</span>.</p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; color:#C36162;\">BE AWARE that you should only do this if you know what you're </span></p>\n"
															  "<p style=\" margin-top:6px; margin-bottom:6px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; color:#C36162;\">doing!</span></p></body></html>",
													 nullptr ) );
		buttonBox->buttons()[0]->setText( QCoreApplication::translate( "PTIIR", "Close", nullptr ) );
	} // retranslateUi
};

namespace ui
{
	class PTIDialogSetup : public Ui_PTIIR
	{
	};
} // namespace ui

QT_END_NAMESPACE

#endif // P2PTIDIALOG_H
