#pragma once

#ifdef _MSC_VER
#pragma warning( push)
#pragma warning( disable : 4127 4244 4251 4311 4312 4512 4800)
#include <qt/QDialog.h>
#pragma warning( pop)
#else
#include <QDialog>
#endif

class QLineEdit;

/*
   \brief dialog class enter Element and value
   \author Rolf Kary-Ehlers
*/
class CNewXMLItemDlg : public QDialog
{
   Q_OBJECT
public:
   CNewXMLItemDlg(const QString&tag);
   virtual ~CNewXMLItemDlg(void);
   QString m_sTag;
   QString m_sValue;

protected slots:
   void OnOk(); 

private:
   QLineEdit *m_Tag;
   QLineEdit *m_Value;
};
