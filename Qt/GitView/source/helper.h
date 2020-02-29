#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QTreeWidget>
#include <string>



#define STORE_PTR(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)       SETTING.setValue(getSettingsName(#ITEM), ITEM)

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT) ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)  ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)           ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()

QString getSettingsName(const QString& aItemName);

QString formatFileSize(quint64 aSize);

void deleteSelectedTreeWidgetItem(QTreeWidget& aTree);
void deleteAllTreeWidgetItem(QTreeWidget& aTree);

int execute(const QString& command, QString& aResultText);



#endif // HELPER_H
