#ifndef ACTIONS_H
#define ACTIONS_H
#include "git_type.h"

#include <QString>
#include <QVariant>
#include <QPointer>

class QToolBar;
class QAction;
class QMenu;


enum class Flag { remove, set, replace };

class ActionList
{

public:
    struct Data   { enum e { MsgBoxText, PostCmdAction, IconPath, Flags, CmdAddOn, Cmd, StatusFlagEnable, StatusFlagDisable, ListSize }; };
    struct Flags  { enum e { BuiltIn           = 0x0001, /// built in command
                             Modified          = 0x0002, /// modified built in command
                             Custom            = 0x0004, /// custom created command
                             Branch            = 0x0008, /// command for branch view
                             History           = 0x0010, /// command for history view
                             DiffOrMergeTool   = 0x0020, /// indicates diff or mergetool is used by git command
                             CallInThread      = 0x0040, /// indicates that git command is called within thread context
                             NotVariableGitCmd = 0x0080, /// indicates that the command is not a modifiable git command
                             FunctionCmd       = 0x0100, /// command invokes a special destinct function
                             Stash             = 0x0200, /// command for stash view
                             DiffCmd           = 0x0400, /// indicates git diff command
                             DependsOnStaged   = 0x0800, /// option to indicate wether the command depends on state staged or not staged
                             StashCmdOption    = 0x1000, /// option for stash command
                           }; };

    typedef std::map<git::Cmd::eCmd,  QPointer<QAction>> tActionMap;

    explicit ActionList(QObject* aParent);

    ~ActionList();

    QAction* createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand="", QObject*aParent=nullptr);
    void     deleteAction(git::Cmd::eCmd aCmd);
    QAction* getAction(git::Cmd::eCmd aCmd) const;
    bool     hasAction(git::Cmd::eCmd aCmd) const;
    void     setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText);
    QString  getCustomCommandMessageBoxText(git::Cmd::eCmd aCmd) const ;
    void     setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction);
    uint     getCustomCommandPostAction(git::Cmd::eCmd aCmd) const;
    void     setIconPath(git::Cmd::eCmd aCmd, const QString& aPath);
    QString  getIconPath(git::Cmd::eCmd aCmd) const;
    void     setFlags(git::Cmd::eCmd aCmd, uint aFlag, Flag aSet = Flag::set, Data::e aData=Data::Flags);
    uint     getFlags(git::Cmd::eCmd aCmd, Data::e aData=Data::Flags) const;
    void     setCmdAddOn(git::Cmd::eCmd aCmd, const QString& aCmdAddOn);
    QString  getCmdAddOn(git::Cmd::eCmd aCmd) const;

    void     initActionIcons();
    git::Cmd::eCmd createNewID(git::Cmd::eCmd fNewCmd) const;

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
