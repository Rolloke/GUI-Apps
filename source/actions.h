#ifndef ACTIONS_H
#define ACTIONS_H
#include "git_type.h"

#include <QString>
#include <QVariant>

class QToolBar;
class QAction;
class QMenu;


enum class Flag { remove, set, replace };

class ActionList
{

public:
    struct Data   { enum e { MsgBoxText, Action, IconPath, Flags, StagedCmdAddOn, Cmd, StatusFlagEnable, StatusFlagDisable, ListSize }; };
    struct Flags  { enum e { BuiltIn = 1,           /// built in command
                             Modified = 2,          /// modified built in command
                             Custom=4,              /// custom created command
                             Branch=8,              /// command for branch view
                             History=16,            /// command for history view
                             DiffOrMergeTool=32,    /// indicates diff or mergetool is used by git command
                             CallInThread=64,       /// indicates that git command is called within thread context
                             NotVariableGitCmd=128, /// indicates that the command is not a modifiable git command
                             FunctionCmd=256}; };   /// command invokes a special destinct function

    typedef std::map<int, QAction*> tActionMap;

    explicit ActionList(QObject* aParent);

    ~ActionList();

    QAction* createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand="", QObject*aParent=nullptr);
    void     deleteAction(git::Cmd::eCmd aCmd);
    QAction* getAction(git::Cmd::eCmd aCmd) const;
    void     setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText);
    QString  getCustomCommandMessageBoxText(git::Cmd::eCmd aCmd) const ;
    void     setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction);
    uint     getCustomCommandPostAction(git::Cmd::eCmd aCmd) const;
    void     setIconPath(git::Cmd::eCmd aCmd, const QString& aPath);
    QString  getIconPath(git::Cmd::eCmd aCmd) const;
    void     setFlags(git::Cmd::eCmd aCmd, uint aFlag, Flag aSet = Flag::set, Data::e aData=Data::Flags);
    uint     getFlags(git::Cmd::eCmd aCmd, Data::e aData=Data::Flags) const;
    void     setStagedCmdAddOn(git::Cmd::eCmd aCmd, const QString& aCmdAddOn);
    QString  getStagedCmdAddOn(git::Cmd::eCmd aCmd) const;

    void     initActionIcons();
    git::Cmd::eCmd getNextCustomID() const;

    void enableItemsByType(const git::Cmd::tVector& aItems, const git::Type& aType) const;
    void fillToolbar(QToolBar& aMenu, const git::Cmd::tVector& aItems) const;
    void fillContextMenue(QMenu& aMenu, const git::Cmd::tVector& aItems) const;
    const tActionMap& getList() const { return mActionList; }

    static const QString sNoCustomCommandMessageBox;

private:
    void setDataVariant(git::Cmd::eCmd aCmd, Data::e aData, const QVariant& aVariant);
    QVariant getDataVariant(git::Cmd::eCmd aCmd, Data::e aData) const;

    tActionMap mActionList;
    QObject*   mParent;
};

#endif // ACTIONS_H
