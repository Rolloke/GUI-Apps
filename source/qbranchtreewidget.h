#ifndef BRANCH_H
#define BRANCH_H

#include <QTreeWidget>

class ActionList;

class QBranchTreeWidget : public QTreeWidget
{
    Q_OBJECT

    enum class Column
    {
        Text
    };
public:
    QBranchTreeWidget(QWidget *parent = 0);
    ~QBranchTreeWidget();

    QString getBranchItem();

    void parseBranchListText(const QString& aBranchText);

    void on_customContextMenuRequested(const ActionList& aMain, const QPoint &pos);

Q_SIGNALS:

private Q_SLOTS:


private:
    QString mBranchItem;
};

#endif // BRANCH_H
