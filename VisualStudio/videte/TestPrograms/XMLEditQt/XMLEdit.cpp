#include "XMLEdit.h"
#include "NewXMLItemDlg.h"

#ifdef _MSC_VER
#pragma warning( push)
#pragma warning( disable : 4127 4244 4251 4311 4312 4512 4800)
#include <QtGui/QtGui>
#pragma warning( pop )
#else
#include <QtGui>
#endif

#include "XMLEdit.moc"

#define IsBetween(val, lowerval, upperval) ((val>=lowerval) && (val<= upperval))

extern QApplication*g_pApp;

enum TreeUserRole
{
   XML_FIRST = Qt::UserRole,
   XML_ELEMENT_TYPE,
   XML_ELEMENT,
   XML_ATTRIBUTE_INDEX,
   XML_ELEMENT_MIN,
   XML_ELEMENT_MAX,
   XML_ELEMENT_VALUES
};

#define TYPE_XML_ELEMENT    (QVariant::UserType+1)

enum TreeColumn
{
   Invald = -1,
   Tree = 0,
   Value,
};


ComboBoxDelegate::ComboBoxDelegate(CXMLEditDlg *pDlg, QObject *parent):
   QItemDelegate(parent),
   mp_Dlg(pDlg)
{

}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &option,
     const QModelIndex & index ) const
{
   if (mp_Dlg->mp_CurrentItem)
   {
      QVariant var = mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_TYPE);
      QString type;
      if (var.isValid()) type = var.toString();
      if (type == "fixed") return 0;
      if (   index.column() == 1 
          && mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_VALUES).isValid())
      {
         QComboBox*editor = new QComboBox(parent);
         editor->installEventFilter(const_cast<ComboBoxDelegate*>(this));
         return editor;
      }
      else
      {
         QString value = index.model()->data(index, Qt::DisplayRole).toString();
         QLineEdit*editor = new QLineEdit(value, parent);
         return editor;
      }
   }
   return 0;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
   QString value = index.model()->data(index, Qt::DisplayRole).toString();
   if (index.column() == 1 && mp_Dlg->mp_CurrentItem)
   {
      QVariant var = mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_VALUES);
      if (var.isValid())
      {
         QComboBox *comboBox = static_cast<QComboBox*>(editor);
         QString values = var.toString();
         QStringList aValues;
         SplitString(values, QChar(','), aValues);
         int j, m = aValues.count();
         for (j=0; j<m; j++)
         {
            comboBox->addItem(aValues[j]);
         }
         comboBox->setCurrentIndex(comboBox->findText(value));
      }
   }
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
   QString value;
   if (   index.column() == 1
       && mp_Dlg->mp_CurrentItem
       && mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_VALUES).isValid())
   {
      QComboBox *comboBox = static_cast<QComboBox*>(editor);
      value = comboBox->currentText();
   }
   else
   {
      QLineEdit*editBox = static_cast<QLineEdit*>(editor);
      value = editBox->text();
      if (  mp_Dlg->mp_CurrentItem
         && mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_TYPE).isValid()
         && mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_MIN).isValid()
         && mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_MAX).isValid())
      {
         QString min  = mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_MIN ).toString();
         QString max  = mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_MAX ).toString();
         QString type = mp_Dlg->mp_CurrentItem->data(0, XML_ELEMENT_TYPE).toString();
         bool inValid = false;
         if (type == "float")
         {
            double dmin   = min.toDouble();
            double dmax   = max.toDouble();
            double dvalue = value.toDouble();
            if (dmin > dvalue || dvalue > dmax)
            {
               inValid = true;
            }
         }
         else if (type == "int")
         {
            int imin   = min.toInt();
            int imax   = max.toInt();
            int ivalue = value.toInt();
            if (imin > ivalue || ivalue > imax)
            {
               inValid = true;
            }
         }
         if (inValid)
         {
            type = tr("use value between ") + min + tr(" and ") + max;
            QMessageBox box(QMessageBox::Warning, tr("Value out or range"), type); 
            box.exec();
            return;
         }
      }
   }
   model->setData(index, value);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
   editor->setGeometry(option.rect);
}

CXMLEditDlg::CXMLEditDlg():
   mp_Tree(0),
   mp_doc(0),
   mp_CurrentItem(0),
   m_sTitle(tr("XML Editor")), 
   m_bIsParsing(false), 
   m_bChanged(false)
{
   mp_Attribute  = new QIcon("attrib.PNG");
   mp_Folder     = new QIcon("folder.PNG");
   mp_OpenFolder = new QIcon("opfolder.PNG");

   mp_doc        = new QDomDocument();
   setWindowTitle(m_sTitle);
   QVBoxLayout* pVBox = new QVBoxLayout(this);
   QHBoxLayout* pHBox = new QHBoxLayout(this);
   mp_Tree = new QTreeWidget(this);
   pVBox->addWidget(mp_Tree);
   pVBox->addLayout(pHBox);
   QStringList aHeader;
   aHeader.append("Name");
   aHeader.append("Value");
   mp_Tree->setHeaderLabels(aHeader);
   mp_Tree->setIconSize(QSize(16,16));
   // TODO set minimum width
   //mp_Tree->setColumnWidth(Tree,  150);
   //mp_Tree->setColumnWidth(Value, 150);
   mp_Tree->header()->setResizeMode(Tree, QHeaderView::ResizeToContents);
   mp_delegate = new ComboBoxDelegate(this, mp_Tree);
   mp_Tree->setItemDelegate(mp_delegate);

   connect(mp_Tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(OnItemClicked(QTreeWidgetItem*,int))); 
   connect(mp_Tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(OnItemChanged(QTreeWidgetItem*,int))); 

   QPushButton *pButton;
   pButton = new QPushButton("Open File...");
   connect(pButton, SIGNAL(clicked()), SLOT(OnOpenFile())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Save");
   connect(pButton, SIGNAL(clicked()), SLOT(OnSaveFile())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Create Element");
   connect(pButton, SIGNAL(clicked()), SLOT(OnCreateXMLElement())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Create Attribute");
   connect(pButton, SIGNAL(clicked()), SLOT(OnCreateXMLAttibute())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Delete");
   connect(pButton, SIGNAL(clicked()), SLOT(OnDelete())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Copy");
   connect(pButton, SIGNAL(clicked()), SLOT(OnCopy())); 
   pHBox->addWidget(pButton);
   pButton = new QPushButton("Paste");
   connect(pButton, SIGNAL(clicked()), SLOT(OnPaste())); 
   pHBox->addWidget(pButton);
}

CXMLEditDlg::~CXMLEditDlg(void)
{
   delete mp_doc;
   delete mp_Attribute;
   delete mp_Folder;
   delete mp_OpenFolder;
}

void CXMLEditDlg::OnItemClicked ( QTreeWidgetItem * item, int column )
{
	// just a test
   //QString values = item->data(0, XML_ELEMENT_VALUES).toString();
   mp_CurrentItem = item;
}

void CXMLEditDlg::OnItemChanged(QTreeWidgetItem * item, int column )
{
   if (!m_bIsParsing)
   {
      OnItemChanged(item, item->text(column), column);
   }
}

void CXMLEditDlg::OnItemChanged(QTreeWidgetItem * item, const QString & text, int column )
{
   QVariant variant =  item->data(0, XML_ELEMENT);
   if (variant.isValid() && variant.userType() == TYPE_XML_ELEMENT)
   {
      QDomNode *docElem = (QDomNode*)variant.data();
      QVariant index =  item->data(0, XML_ATTRIBUTE_INDEX);

      if (index.isValid() && index.type() == QVariant::Int)
      {
         int i = index.toInt();
         if (column == 0)
         {
            QString name  = docElem->attributes().item(i).toAttr().name();
            QString value = docElem->attributes().item(i).toAttr().value();
            docElem->toElement().setAttribute(text, value);
            docElem->toElement().removeAttribute(name);
         }
         else if (column == 1)
         {
            docElem->attributes().item(i).toAttr().setValue(text);
         }
      }
      else
      {
          if (column == 0)
          {
              ((QDomElement*)docElem)->setTagName(text);
          }
          else if (column == 1)
          {
              docElem->firstChild().toText().setData(text);
          }
      }
   }
}

void CXMLEditDlg::OnOpenFile()
{
   m_sFileName = QFileDialog::getOpenFileName(this, tr("Open XML File"), m_sFolder, tr("XML File (*.xml)"));
   QFile file(m_sFileName);
   QString sError;
   int nLine, nColumn;
   int nSlash = m_sFileName.lastIndexOf(QChar('\\'));
   if (nSlash == -1) nSlash = m_sFileName.lastIndexOf(QChar('/'));
   if (nSlash != -1) m_sFolder = m_sFileName.left(nSlash);
   else m_sFolder.clear();

   if (file.open(QIODevice::ReadOnly))
   {
      mp_Tree->clear();
      delete mp_doc;
      mp_doc = new QDomDocument();
      if (mp_doc->setContent(&file, &sError, &nLine, &nColumn))
      {
         QDomElement docElem = mp_doc->documentElement();
         m_bIsParsing = true;
         IterateXMLItems(&docElem, NULL, NULL);
         QStringList list;
         IterateItems(mp_Tree->topLevelItem(0), list);
         m_bIsParsing = false;
         setWindowTitle(m_sTitle+"["+m_sFileName+"]");
         mp_Tree->header()->resizeSections(QHeaderView::ResizeToContents);
      }
   }
}

void CXMLEditDlg::OnSaveFile()
{
   if (m_sFileName.isEmpty())
   {
      m_sFileName = QFileDialog::getSaveFileName(this, tr("Save XML File"), m_sFolder, tr("XML File (*.xml)"));
   }

   QFile file(m_sFileName);
   if (file.open(QIODevice::WriteOnly))
   {
      QString sXML;
      sXML = mp_doc->toString(3);
#ifdef _WIN32
      sXML.replace("-->", "-->\r\n");
#elif (__APPLE__ & __MACH__)
      sXML.replace("-->", "-->\r");
#else
      sXML.replace("-->", "-->\n");
#endif
      file.write(sXML.toAscii().constData(), sXML.count());
   }
   else
   {
       QMessageBox::information(this, "File is write protected", m_sFileName, "Ok");
   }
}

void CXMLEditDlg::OnCreateXMLElement()
{
   CNewXMLItemDlg dlg(tr("Element"));
   if (dlg.exec())
   {
      QDomElement element = mp_doc->createElement(dlg.m_sTag);
      QDomText    text    = mp_doc->createTextNode(dlg.m_sValue);
      element.appendChild(text);
      if (mp_CurrentItem)
      {
         QVariant var = mp_CurrentItem->data(0, XML_ELEMENT);
         if (var.userType() ==  TYPE_XML_ELEMENT)
         {
            QDomNode *docElem = (QDomNode*)var.data();
            docElem->appendChild(element);
         }
      }
      else
      {
         mp_doc->appendChild(element);
      }

      m_bIsParsing = true;
      QTreeWidgetItem*pNew = IterateXMLItems(&element, mp_CurrentItem, NULL);
      if (pNew)
      {
         pNew->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
      }
      m_bIsParsing = false;
      mp_Tree->header()->resizeSections(QHeaderView::ResizeToContents);
   }
}

void CXMLEditDlg::OnCreateXMLAttibute()
{
   if (mp_CurrentItem)
   {
      QVariant var = mp_CurrentItem->data(0, XML_ELEMENT);
      if (var.userType() ==  TYPE_XML_ELEMENT)
      {
         CNewXMLItemDlg dlg(tr("Attribute"));
         if (dlg.exec())
         {
            QDomNode *docElem = (QDomNode*)var.data();
            QDomNode parentNode;
            int iold = docElem->attributes().count();
            docElem->toElement().setAttribute(dlg.m_sTag, dlg.m_sValue);
            int inew = docElem->attributes().count();
            if (inew > iold)
            {
               m_bIsParsing = true;
               QDomNode attr = docElem->attributes().item(0);
               // TODO! determine index, 0 is wrong
               QTreeWidgetItem*pAttr = new QTreeWidgetItem(mp_CurrentItem);
               pAttr->setText(0, attr.nodeName());
               pAttr->setText(1, attr.nodeValue());
               pAttr->setIcon(0, *mp_Attribute);
               pAttr->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
               pAttr->setData(0, XML_ELEMENT, QVariant((int)TYPE_XML_ELEMENT, docElem));
               pAttr->setData(0, XML_ATTRIBUTE_INDEX, QVariant((int)0));
               m_bIsParsing = false;
            }
         }
      }
   }
}

void CXMLEditDlg::OnDelete()
{
   if (mp_CurrentItem)
   {
      QTreeWidgetItem*parent = mp_CurrentItem->parent();
      if (parent)
      {
         parent->takeChild(parent->indexOfChild(mp_CurrentItem));
      }
      else
      {
         mp_Tree->clear();
      }
      QVariant var = mp_CurrentItem->data(0, XML_ELEMENT);
      if (var.isValid() && var.userType() ==  TYPE_XML_ELEMENT)
      {
         QDomNode *docElem = (QDomNode*)var.data();
         QVariant index =  mp_CurrentItem->data(0, XML_ATTRIBUTE_INDEX);
         if (index.isValid() && index.type() == QVariant::Int)
         {
            int i = index.toInt();
            docElem->toElement().removeAttribute(docElem->attributes().item(i).toAttr().name());
         }
         else
         {
            QDomNode parentElem = docElem->parentNode();
            parentElem.removeChild(*docElem);
         }
      }
      delete mp_CurrentItem;
      mp_CurrentItem = 0;
   }
}

void CXMLEditDlg::OnCopy()
{
   if (mp_CurrentItem)
   {
      QVariant var = mp_CurrentItem->data(0, XML_ELEMENT);
      if (var.userType() ==  TYPE_XML_ELEMENT)
      {
         m_bIsParsing = true;
         QDomNode *docElem = (QDomNode*)var.data();
         m_CopiedNode.clear();
         m_CopiedNode = docElem->cloneNode();
      }
   }
}

void CXMLEditDlg::OnPaste()
{
   if (!m_CopiedNode.isNull())
   {
      if (mp_CurrentItem)
      {
         QVariant var = mp_CurrentItem->data(0, XML_ELEMENT);
         if (var.userType() ==  TYPE_XML_ELEMENT)
         {
            QDomNode *docElem = (QDomNode*)var.data();
            docElem->appendChild(m_CopiedNode);
         }
      }
      else
      {
         mp_doc->appendChild(m_CopiedNode);
      }

      m_bIsParsing = true;
      QTreeWidgetItem*pNew = IterateXMLItems(&m_CopiedNode, mp_CurrentItem, NULL);
      if (pNew)
      {
         pNew->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
      }
      m_bIsParsing = false;
   }
}

QTreeWidgetItem* CXMLEditDlg::IterateXMLItems(QDomNode*docElem, QTreeWidgetItem*pTreeParent, QTreeWidgetItem*pTreePreceding)
{
   QTreeWidgetItem*pItem = NULL;

   if (docElem->isElement())
   {
      if (pTreeParent)
      {
         if (pTreePreceding) pItem = new QTreeWidgetItem(pTreeParent, pTreePreceding);
         else                pItem = new QTreeWidgetItem(pTreeParent);
      }
      else
      {
         pItem = new QTreeWidgetItem(mp_Tree);
      }
      pItem->setText(0, docElem->nodeName());
      if (   docElem->childNodes().count() == 1
          && docElem->firstChild().nodeName() == "#text")
      {
         pItem->setText(1, docElem->toElement().text());
         pItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
         pItem->setIcon(0, *mp_OpenFolder);
      }
      else
      {
         pItem->setIcon(0, *mp_Folder);
      }
      pItem->setData(0, XML_ELEMENT, QVariant((int)TYPE_XML_ELEMENT, docElem));
   }
   else if (docElem->isComment())
   {
      QString str = docElem->toComment().nodeValue();
      QStringList aAdd;
      int i, n;
      str = str.simplified();
      SplitString(str, QChar('@'), aAdd, 1);
      n = aAdd.count();
      for (i=0; i<n; i++)
      {
         QStringList aAttr;
         SplitString(aAdd[i], QChar(';'), aAttr);
         IterateItems(pTreePreceding, aAttr);
      }
   }
   if (pItem)
   {
      QDomNode n;
      QTreeWidgetItem *pAttr;
      int i, m = docElem->attributes().count();
      for (i=0; i<m; i++)
      {
         n = docElem->attributes().item(i);
         pAttr = new QTreeWidgetItem(pItem);
         pAttr->setText(0, n.nodeName());
         pAttr->setText(1, n.nodeValue());
         pAttr->setIcon(0, *mp_Attribute);
         pAttr->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
         pAttr->setData(0, XML_ELEMENT, QVariant((int)TYPE_XML_ELEMENT, docElem));
         pAttr->setData(0, XML_ATTRIBUTE_INDEX, QVariant((int)i));
      }

      n = docElem->firstChild();
      QTreeWidgetItem*pPreceding = NULL;
      while(!n.isNull()) 
      {
         pPreceding = IterateXMLItems(&n, pItem, pPreceding);
         n = n.nextSibling();
      }
   }
   return pItem;
}

void CXMLEditDlg::IterateItems(QTreeWidgetItem*pTreeParent, QStringList &aAttr)
{
   bool hasAttributes = false;
   bool bTypeBool     = false;
   QString sValue     = pTreeParent->text(1);
   QVariant var0      = pTreeParent->data(0, Qt::UserRole);
   if (!var0.isNull() && var0.type() == QVariant::Bool && var0.toBool())
   {
      hasAttributes = true;
   }
   else // no attributes
   {
      if      (sValue.compare("false", Qt::CaseInsensitive) == 0) bTypeBool = true;
      else if (sValue.compare("true" , Qt::CaseInsensitive) == 0) bTypeBool = true;
   }
   int i, n = aAttr.count();
   if (  !hasAttributes && n > 1
       && aAttr[0].compare(pTreeParent->text(0), Qt::CaseInsensitive)==0)
   {
      for (i=1; i<n; i++)
      {
         if (aAttr[i].startsWith("help:",  Qt::CaseInsensitive))
         {
            pTreeParent->setToolTip(0, aAttr[i].mid(5));
         }
         else if (aAttr[i].startsWith("type:",  Qt::CaseInsensitive))
         {
            QString sType = aAttr[i].mid(5);
            if (sType.compare("bool", Qt::CaseInsensitive) == 0)
            {
               bTypeBool = true;
            }
            pTreeParent->setData(0, XML_ELEMENT_TYPE, QVariant(sType));
         }
         else if (aAttr[i].startsWith("values:",  Qt::CaseInsensitive))
         {
            pTreeParent->setData(0, XML_ELEMENT_VALUES, QVariant(aAttr[i].mid(7)));
         }
         else if (aAttr[i].startsWith("min:",  Qt::CaseInsensitive))
         {
            pTreeParent->setData(0, XML_ELEMENT_MIN, QVariant(aAttr[i].mid(4)));
         }
         else if (aAttr[i].startsWith("max:",  Qt::CaseInsensitive))
         {
            pTreeParent->setData(0, XML_ELEMENT_MAX, QVariant(aAttr[i].mid(4)));
         }
      }
      pTreeParent->setData(0, Qt::UserRole, QVariant(true));
   }
   if (bTypeBool)
   {
      pTreeParent->setData(0, XML_ELEMENT_VALUES, QVariant("false,true"));
   }
   n = pTreeParent->childCount();
   for (i=0; i<n; i++)
   {
      IterateItems(pTreeParent->child(i), aAttr);
   }
}

int  SplitString(const QString &str, QChar c, QStringList&arr, int start)
{
  QString s;
  int i;
  for (i=start; ; i++)
  {
     s = str.section(c, i, i);
     if (s.isEmpty()) break;
     arr.append(s.simplified());
  }
  return arr.count();
}
