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
#if MAP_TYPE == 0
    typedef boost::container::flat_map<int, QAction*> tActionMap;
#elif MAP_TYPE == 1
    typedef std::map<int, QAction*> tActionMap;
#endif

    enum class Data { MsgBoxText, Action, IconPath, Modified, ListSize };

public:
    explicit ActionList(QObject* aParent);
    ~ActionList();

    QAction * createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand="");


    void     initActionIcons();
    QAction* getAction(git::Cmd::eCmd aCmd);
    void     setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText);
    QString  getCustomCommandMessageBoxText(git::Cmd::eCmd aCmd);
    void     setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction);
    uint     getCustomCommandPostAction(git::Cmd::eCmd aCmd);
    void     setIconPath(git::Cmd::eCmd aCmd, const QString& aPath);
    QString  getIconPath(git::Cmd::eCmd aCmd);
    void     setModified(git::Cmd::eCmd aCmd, bool aMod);
    bool     isModified(git::Cmd::eCmd aCmd);

    void     fillToolbar(QToolBar& aMenu, const std::vector<git::Cmd::eCmd>& aItems);
    void     fillContextMenue(QMenu& aMenu, const std::vector<git::Cmd::eCmd>& aItems);
    const tActionMap getList() { return mActionList; }

    static const QString sNoCustomCommandMessageBox;

private:
    void setDataVariant(git::Cmd::eCmd aCmd, Data aData, const QVariant& aVariant);
    QVariant getDataVariant(git::Cmd::eCmd aCmd, Data aData);

    tActionMap mActionList;
    QObject*   mParent;
};

#endif // ACTIONS_H
