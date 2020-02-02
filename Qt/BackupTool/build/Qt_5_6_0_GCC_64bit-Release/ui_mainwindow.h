/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSelect_source_folder;
    QAction *actionSelect_Destination;
    QWidget *centralWidget;
    QVBoxLayout *topLayout;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeSource;
    QTreeWidget *treeBackup;
    QHBoxLayout *layoutButtons;
    QVBoxLayout *verticalLayoutCopy;
    QPushButton *btnBackup;
    QPushButton *btnRestore;
    QPushButton *btnCancel;
    QVBoxLayout *verticalLayoutPreparation;
    QPushButton *btnAddSourceFolder;
    QPushButton *btnSelectDestination;
    QPushButton *btnUpdateSearch;
    QVBoxLayout *verticalLayoutSettings2;
    QHBoxLayout *horizontalLayoutSettings1;
    QCheckBox *ckDirectories;
    QCheckBox *ckFiles;
    QLabel *labelBlacklist;
    QLineEdit *editBlacklist;
    QVBoxLayout *verticalLayoutSettings1;
    QCheckBox *ckHiddenFiles;
    QCheckBox *ckSystemFiles;
    QCheckBox *ckSymbolicLinks;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(905, 504);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        MainWindow->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionSelect_source_folder = new QAction(MainWindow);
        actionSelect_source_folder->setObjectName(QStringLiteral("actionSelect_source_folder"));
        actionSelect_Destination = new QAction(MainWindow);
        actionSelect_Destination->setObjectName(QStringLiteral("actionSelect_Destination"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setMinimumSize(QSize(905, 455));
        topLayout = new QVBoxLayout(centralWidget);
        topLayout->setSpacing(6);
        topLayout->setContentsMargins(11, 11, 11, 11);
        topLayout->setObjectName(QStringLiteral("topLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        treeSource = new QTreeWidget(centralWidget);
        treeSource->setObjectName(QStringLiteral("treeSource"));
        treeSource->setSortingEnabled(true);

        horizontalLayout->addWidget(treeSource);

        treeBackup = new QTreeWidget(centralWidget);
        treeBackup->setObjectName(QStringLiteral("treeBackup"));
        treeBackup->setSortingEnabled(true);

        horizontalLayout->addWidget(treeBackup);


        topLayout->addLayout(horizontalLayout);

        layoutButtons = new QHBoxLayout();
        layoutButtons->setSpacing(6);
        layoutButtons->setObjectName(QStringLiteral("layoutButtons"));
        verticalLayoutCopy = new QVBoxLayout();
        verticalLayoutCopy->setSpacing(6);
        verticalLayoutCopy->setObjectName(QStringLiteral("verticalLayoutCopy"));
        btnBackup = new QPushButton(centralWidget);
        btnBackup->setObjectName(QStringLiteral("btnBackup"));
        btnBackup->setMaximumSize(QSize(16777215, 16777215));

        verticalLayoutCopy->addWidget(btnBackup);

        btnRestore = new QPushButton(centralWidget);
        btnRestore->setObjectName(QStringLiteral("btnRestore"));
        btnRestore->setMaximumSize(QSize(16777215, 16777215));

        verticalLayoutCopy->addWidget(btnRestore);

        btnCancel = new QPushButton(centralWidget);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));
        btnCancel->setMinimumSize(QSize(160, 0));
        btnCancel->setMaximumSize(QSize(16777215, 16777215));

        verticalLayoutCopy->addWidget(btnCancel);


        layoutButtons->addLayout(verticalLayoutCopy);

        verticalLayoutPreparation = new QVBoxLayout();
        verticalLayoutPreparation->setSpacing(6);
        verticalLayoutPreparation->setObjectName(QStringLiteral("verticalLayoutPreparation"));
        verticalLayoutPreparation->setSizeConstraint(QLayout::SetDefaultConstraint);
        btnAddSourceFolder = new QPushButton(centralWidget);
        btnAddSourceFolder->setObjectName(QStringLiteral("btnAddSourceFolder"));

        verticalLayoutPreparation->addWidget(btnAddSourceFolder);

        btnSelectDestination = new QPushButton(centralWidget);
        btnSelectDestination->setObjectName(QStringLiteral("btnSelectDestination"));

        verticalLayoutPreparation->addWidget(btnSelectDestination);

        btnUpdateSearch = new QPushButton(centralWidget);
        btnUpdateSearch->setObjectName(QStringLiteral("btnUpdateSearch"));
        btnUpdateSearch->setMinimumSize(QSize(160, 0));
        btnUpdateSearch->setMaximumSize(QSize(200, 16777215));

        verticalLayoutPreparation->addWidget(btnUpdateSearch);


        layoutButtons->addLayout(verticalLayoutPreparation);

        verticalLayoutSettings2 = new QVBoxLayout();
        verticalLayoutSettings2->setSpacing(6);
        verticalLayoutSettings2->setObjectName(QStringLiteral("verticalLayoutSettings2"));
        horizontalLayoutSettings1 = new QHBoxLayout();
        horizontalLayoutSettings1->setSpacing(6);
        horizontalLayoutSettings1->setObjectName(QStringLiteral("horizontalLayoutSettings1"));
        ckDirectories = new QCheckBox(centralWidget);
        ckDirectories->setObjectName(QStringLiteral("ckDirectories"));
        ckDirectories->setChecked(true);

        horizontalLayoutSettings1->addWidget(ckDirectories);

        ckFiles = new QCheckBox(centralWidget);
        ckFiles->setObjectName(QStringLiteral("ckFiles"));
        ckFiles->setChecked(true);

        horizontalLayoutSettings1->addWidget(ckFiles);


        verticalLayoutSettings2->addLayout(horizontalLayoutSettings1);

        labelBlacklist = new QLabel(centralWidget);
        labelBlacklist->setObjectName(QStringLiteral("labelBlacklist"));
        labelBlacklist->setMinimumSize(QSize(0, 0));
        labelBlacklist->setMaximumSize(QSize(16777215, 16777215));

        verticalLayoutSettings2->addWidget(labelBlacklist);

        editBlacklist = new QLineEdit(centralWidget);
        editBlacklist->setObjectName(QStringLiteral("editBlacklist"));
        editBlacklist->setMinimumSize(QSize(0, 0));
        editBlacklist->setMaximumSize(QSize(16777215, 16777215));

        verticalLayoutSettings2->addWidget(editBlacklist);


        layoutButtons->addLayout(verticalLayoutSettings2);

        verticalLayoutSettings1 = new QVBoxLayout();
        verticalLayoutSettings1->setSpacing(6);
        verticalLayoutSettings1->setObjectName(QStringLiteral("verticalLayoutSettings1"));
        ckHiddenFiles = new QCheckBox(centralWidget);
        ckHiddenFiles->setObjectName(QStringLiteral("ckHiddenFiles"));
        ckHiddenFiles->setMinimumSize(QSize(160, 0));
        ckHiddenFiles->setChecked(true);

        verticalLayoutSettings1->addWidget(ckHiddenFiles);

        ckSystemFiles = new QCheckBox(centralWidget);
        ckSystemFiles->setObjectName(QStringLiteral("ckSystemFiles"));
        ckSystemFiles->setCheckable(true);
        ckSystemFiles->setChecked(true);

        verticalLayoutSettings1->addWidget(ckSystemFiles);

        ckSymbolicLinks = new QCheckBox(centralWidget);
        ckSymbolicLinks->setObjectName(QStringLiteral("ckSymbolicLinks"));

        verticalLayoutSettings1->addWidget(ckSymbolicLinks);


        layoutButtons->addLayout(verticalLayoutSettings1);


        topLayout->addLayout(layoutButtons);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Backup Tool", 0));
        actionSelect_source_folder->setText(QApplication::translate("MainWindow", "Select source folder...", 0));
        actionSelect_Destination->setText(QApplication::translate("MainWindow", "Select Destination...", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeSource->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "Size", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Date", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Name", 0));
        QTreeWidgetItem *___qtreewidgetitem1 = treeBackup->headerItem();
        ___qtreewidgetitem1->setText(2, QApplication::translate("MainWindow", "Size", 0));
        ___qtreewidgetitem1->setText(1, QApplication::translate("MainWindow", "Date", 0));
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "Name", 0));
        btnBackup->setText(QApplication::translate("MainWindow", "Backup", 0));
        btnRestore->setText(QApplication::translate("MainWindow", "Restore", 0));
        btnCancel->setText(QApplication::translate("MainWindow", "Cancel", 0));
        btnAddSourceFolder->setText(QApplication::translate("MainWindow", "Add Source Folder", 0));
        btnSelectDestination->setText(QApplication::translate("MainWindow", "Select Destination", 0));
        btnUpdateSearch->setText(QApplication::translate("MainWindow", "Update Status", 0));
        ckDirectories->setText(QApplication::translate("MainWindow", "Directories", 0));
        ckFiles->setText(QApplication::translate("MainWindow", "Files", 0));
        labelBlacklist->setText(QApplication::translate("MainWindow", "Blacklist Wildcard (e.g.: *.abc, Separated by \";\" )", 0));
        ckHiddenFiles->setText(QApplication::translate("MainWindow", "Hidden Files", 0));
        ckSystemFiles->setText(QApplication::translate("MainWindow", "System Files", 0));
        ckSymbolicLinks->setText(QApplication::translate("MainWindow", "Symbolic Links", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
