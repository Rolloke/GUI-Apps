#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QTreeWidget>
#include <string>



#define STORE_PTR(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)   SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)        SETTING.setValue(getSettingsName(#ITEM), ITEM)
#define STORE_STRF(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), FUNC(ITEM))

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT)  ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)   ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)            ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()
#define LOAD_STRF(SETTING, ITEM, FUNC_OUT, FUNC_IN, CONVERT) ITEM = FUNC_OUT(fSettings.value(getSettingsName(#ITEM), QVariant(FUNC_IN(ITEM))).CONVERT());

QString getSettingsName(const QString& aItemName);

QString formatFileSize(quint64 aSize);

void deleteSelectedTreeWidgetItem(QTreeWidget& aTree);
void deleteAllTreeWidgetItem(QTreeWidget& aTree);

int execute(const QString& command, QString& aResultText);

int callMessageBox(const QString& fMessageBoxText, const QString& fFileTypeName="", const QString& fFileName="", bool aIsFile = true);


class QTreeWidgetHook: public QTreeWidget
{
public:
    QTreeWidgetHook();

    QModelIndex indexFromItem(QTreeWidgetItem *item, int column = 0) const;
    QTreeWidgetItem *itemFromIndex(const QModelIndex &index) const;

private:
};

#endif // HELPER_H
