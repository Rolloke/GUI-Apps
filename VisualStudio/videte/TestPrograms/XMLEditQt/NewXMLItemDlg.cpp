#include "NewXMLItemDlg.h"

#ifdef _MSC_VER
#pragma warning( push)
#pragma warning( disable : 4127 4244 4251 4311 4312 4512 4800)
#include <qt/QGridLayout.h>
#include <qt/QLineEdit.h>
#include <qt/QLabel.h>
#include <qt/QPushButton.h>
#pragma warning( pop)
#else
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#endif

#include "NewXMLItemDlg.moc"

CNewXMLItemDlg::CNewXMLItemDlg(const QString&tag)
{
   QString stitle = tr("New XML ");
   setWindowTitle(stitle+ tag);
   QGridLayout* pGrid = new QGridLayout(this);

   pGrid->addWidget(new QLabel(tr("Tag"), this), 0, 0);
   pGrid->addWidget(new QLabel(tr("Value"), this), 0, 1);
   m_Tag = new QLineEdit(tag, this);
   pGrid->addWidget(m_Tag, 1, 0);
   m_Value = new QLineEdit(tr("value"), this);
   pGrid->addWidget(m_Value, 1, 1);

   QPushButton *pButton;
   pButton = new QPushButton("Cancel");
   pGrid->addWidget(pButton, 2, 0);
   connect(pButton, SIGNAL(clicked()), SLOT(reject())); 

   pButton = new QPushButton("Ok");
   connect(pButton, SIGNAL(clicked()), SLOT(OnOk())); 
   pGrid->addWidget(pButton, 2, 1);
}

CNewXMLItemDlg::~CNewXMLItemDlg(void)
{
}

void CNewXMLItemDlg::OnOk()
{
   m_sTag   = m_Tag->text();
   m_sValue = m_Value->text();
   accept();
}
