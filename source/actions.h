#ifndef ACTIONS_H
#define ACTIONS_H
#include "git_type.h"

#include <QString>
#include <QVariant>

class QToolBar;
class QAction;
class QMenu;


class ActionList
{
    enum class Data { MsgBoxText, Action, IconPath, Flags, ListSize };

public:

#if MAP_TYPE == 0
    typedef boost::container::flat_map<int, QAction*> tActionMap;
#elif MAP_TYPE == 1
    typedef std::map<int, QAction*> tActionMap;
#endif

    enum  { BuiltIn = 1,  Modified = 2, Custom=4 };

    explicit ActionList(QObject* aParent);

    ~ActionList();

    QAction* createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand="");
    void     deleteAction(git::Cmd::eCmd aCmd);
    QAction* getAction(git::Cmd::eCmd aCmd);
    void     setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText);
    QString  getCustomCommandMessageBoxText(git::Cmd::eCmd aCmd);
    void     setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction);
    uint     getCustomCommandPostAction(git::Cmd::eCmd aCmd);
    void     setIconPath(git::Cmd::eCmd aCmd, const QString& aPath);
    QString  getIconPath(git::Cmd::eCmd aCmd);
    void     setFlags(git::Cmd::eCmd aCmd, uint aFlag, bool aSet = true);
    uint     getFlags(git::Cmd::eCmd aCmd);

    void     initActionIcons();
    git::Cmd::eCmd getNextCustomID();

    void     fillToolbar(QToolBar& aMenu, const git::Cmd::tVector& aItems);
    void     fillContextMenue(QMenu& aMenu, const git::Cmd::tVector& aItems);
    const tActionMap getList() { return mActionList; }

    static const QString sNoCustomCommandMessageBox;

private:
    void setDataVariant(git::Cmd::eCmd aCmd, Data aData, const QVariant& aVariant);
    QVariant getDataVariant(git::Cmd::eCmd aCmd, Data aData);

    tActionMap mActionList;
    QObject*   mParent;
};

#endif // ACTIONS_H
