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
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_Advanced
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBox_3;
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    QTreeWidget *treeWidget;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QToolButton *toolButton_4;
    QToolButton *toolButton_5;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QTextEdit *textEdit;
    QLabel *label_5;
    QLineEdit *lineEdit_2;
    QToolButton *toolButton_6;
    QTreeWidget *treeWidget_2;
    QComboBox *comboBox;
    QLabel *label_2;

    void setupUi(QDialog *Advanced)
    {
        if (Advanced->objectName().isEmpty())
            Advanced->setObjectName(QString::fromUtf8("Advanced"));
        Advanced->resize(700, 442);
        Advanced->setWindowFilePath(QString::fromUtf8(""));
        buttonBox = new QDialogButtonBox(Advanced);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(510, 390, 181, 41));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        checkBox_3 = new QCheckBox(Advanced);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));
        checkBox_3->setGeometry(QRect(20, 380, 241, 22));
        lineEdit = new QLineEdit(Advanced);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(20, 180, 141, 32));
        toolButton = new QToolButton(Advanced);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setGeometry(QRect(170, 180, 81, 31));
        treeWidget = new QTreeWidget(Advanced);
        new QTreeWidgetItem(treeWidget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(20, 220, 231, 151));
        toolButton_2 = new QToolButton(Advanced);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
        toolButton_2->setGeometry(QRect(230, 50, 111, 31));
        toolButton_3 = new QToolButton(Advanced);
        toolButton_3->setObjectName(QString::fromUtf8("toolButton_3"));
        toolButton_3->setGeometry(QRect(230, 80, 111, 31));
        toolButton_4 = new QToolButton(Advanced);
        toolButton_4->setObjectName(QString::fromUtf8("toolButton_4"));
        toolButton_4->setGeometry(QRect(230, 110, 111, 31));
        toolButton_5 = new QToolButton(Advanced);
        toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));
        toolButton_5->setGeometry(QRect(230, 140, 111, 31));
        label = new QLabel(Advanced);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 50, 201, 121));
        label->setScaledContents(true);
        label_3 = new QLabel(Advanced);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 10, 81, 31));
        label_4 = new QLabel(Advanced);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(360, 10, 81, 31));
        textEdit = new QTextEdit(Advanced);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(290, 220, 391, 151));
        label_5 = new QLabel(Advanced);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(290, 180, 131, 31));
        lineEdit_2 = new QLineEdit(Advanced);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(440, 10, 141, 32));
        toolButton_6 = new QToolButton(Advanced);
        toolButton_6->setObjectName(QString::fromUtf8("toolButton_6"));
        toolButton_6->setGeometry(QRect(590, 10, 81, 31));
        treeWidget_2 = new QTreeWidget(Advanced);
        treeWidget_2->setObjectName(QString::fromUtf8("treeWidget_2"));
        treeWidget_2->setGeometry(QRect(440, 50, 231, 151));
        comboBox = new QComboBox(Advanced);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(370, 380, 90, 30));
        label_2 = new QLabel(Advanced);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(290, 380, 71, 31));

        retranslateUi(Advanced);
        QObject::connect(buttonBox, SIGNAL(accepted()), Advanced, SLOT(accept()));
        QMetaObject::connectSlotsByName(Advanced);
    } // setupUi

    void retranslateUi(QDialog *Advanced)
    {
        Advanced->setWindowTitle(QCoreApplication::translate("Advanced", "Advanced Options", nullptr));
#if QT_CONFIG(accessibility)
        Advanced->setAccessibleName(QString());
#endif // QT_CONFIG(accessibility)
        checkBox_3->setText(QCoreApplication::translate("Advanced", "Allow Upload Without PTI Instance", nullptr));
        toolButton->setText(QCoreApplication::translate("Advanced", "Add Tag", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("Advanced", "Tags", nullptr));

        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("Advanced", "Singleplayer", nullptr));
        ___qtreewidgetitem1->setDisabled(true);

        toolButton_2->setText(QCoreApplication::translate("Advanced", "Up", nullptr));
        toolButton_3->setText(QCoreApplication::translate("Advanced", "Down", nullptr));
        toolButton_4->setText(QCoreApplication::translate("Advanced", "Add", nullptr));
        toolButton_5->setText(QCoreApplication::translate("Advanced", "Remove", nullptr));
        label->setText(QString());
        label_3->setText(QCoreApplication::translate("Advanced", "<html><head/><body><p><span style=\" font-size:16pt;\">Images</span></p></body></html>", nullptr));
        label_4->setText(QCoreApplication::translate("Advanced", "<html><head/><body><p><span style=\" font-size:16pt;\">Videos</span></p></body></html>", nullptr));
        label_5->setText(QCoreApplication::translate("Advanced", "<html><head/><body><p><span style=\" font-size:16pt;\">Patch Notes:</span></p></body></html>", nullptr));
        toolButton_6->setText(QCoreApplication::translate("Advanced", "Add Video", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget_2->headerItem();
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("Advanced", "Video Link", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("Advanced", "public", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("Advanced", "private", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("Advanced", "friends", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("Advanced", "unlisted", nullptr));

        label_2->setText(QCoreApplication::translate("Advanced", "<html><head/><body><p><span style=\" font-size:12pt;\">Visibility:</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace ui {
    class CP2PublisherAdvancedOptions: public Ui_Advanced {};
} // namespace Ui

QT_END_NAMESPACE

#endif // P2PUBLISHERADVANCEDOPTIONS_H
