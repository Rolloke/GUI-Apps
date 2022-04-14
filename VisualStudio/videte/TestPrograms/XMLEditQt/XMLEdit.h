#pragma once

#ifdef _MSC_VER
#pragma warning( push)
#pragma warning( disable : 4127 4244 4251 4311 4312 4512 4800)
#include <qt/QDialog.h>
#include <qt/QItemDelegate.h>
#include <qt/QComboBox.h>
#include <QtXml/QDomDocument>
#pragma warning( pop)
#else
#include <QDialog>
#include <QItemDelegate>
#include <QComboBox>
#include <QtXml/QDomDocument>
#endif

class QDomDocument;
class QTreeWidget;
class QTreeWidgetItem;
class CXMLEditDlg;


class ComboBoxDelegate : public QItemDelegate
{
     Q_OBJECT

 public:
     ComboBoxDelegate(CXMLEditDlg*pDlg, QObject *parent = 0);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
     CXMLEditDlg *mp_Dlg;
};


/*
   \brief dialog class to edit xml code
   \author Rolf Kary-Ehlers
*/
class CXMLEditDlg : public QDialog
{
   friend class ComboBoxDelegate;
    Q_OBJECT
public:
    CXMLEditDlg();
    virtual ~CXMLEditDlg();

    QTreeWidgetItem* IterateXMLItems(QDomNode*pParent, QTreeWidgetItem*pTreeParent, QTreeWidgetItem*pTreePreceding);
    void IterateItems(QTreeWidgetItem*pTreeParent, QStringList &aAttr);
protected slots:
   void OnOpenFile(); 
   void OnSaveFile(); 
   void OnCreateXMLElement(); 
   void OnCreateXMLAttibute(); 
   void OnDelete(); 
   void OnCopy(); 
   void OnPaste(); 
   void OnItemClicked(QTreeWidgetItem * item, int column );
   void OnItemChanged(QTreeWidgetItem * item, int column );
   void OnItemChanged(QTreeWidgetItem * item, const QString & text, int column );

private:
   QTreeWidget  *mp_Tree;
   QDomDocument *mp_doc;
   QTreeWidgetItem*mp_CurrentItem;
   ComboBoxDelegate*mp_delegate;
   QIcon        *mp_Folder;
   QIcon        *mp_OpenFolder;
   QIcon        *mp_Attribute;
   QDomNode      m_CopiedNode;
   QString       m_sTitle;
   QString       m_sFileName;
   QString       m_sFolder;
   bool          m_bIsParsing;
   bool          m_bChanged;
};

int  SplitString(const QString &str, QChar c, QStringList&arr, int start=0);
