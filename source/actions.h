#ifndef ACTIONS_H
#define ACTIONS_H
#include "git_type.h"

#include <QString>
#include <QVariant>
#include <QPointer>

class QToolBar;
class QToolButton;
class QAction;
class QMenu;


enum class Flag { remove, set, replace };

class ActionList : public QObject
{
    Q_OBJECT
public:
    struct Data
    {
        enum e { MsgBoxText, PostCmdAction, IconPath, Flags, CmdAddOn, Cmd, StatusFlagEnable, StatusFlagDisable, MenuStrings, ListSize };
        static const char* name(e eData);
        static bool is_flag(e eData);

    };
    struct Flags
    {
        enum e { BuiltIn           = 0x0001, /// built in command
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
                 MenuOption        = 0x2000, /// option selected by Toolbutton menu
                 Asynchroneous     = 0x4000, /// CallInThread with asynchroneous answer
                 NoHide            = 0x8000, /// dont hide command window
                 };
        static const char* name(e eFlag);
    };

    typedef std::map<git::Cmd::eCmd,  QPointer<QAction>>     tActionMap;
    typedef std::map<git::Cmd::eCmd,  QPointer<QToolButton>> tToolButtonMap;
    typedef std::map<git::Cmd::eCmd,  QPointer<QMenu>>       tMenuMap;

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
    QStringList getMenuStringList(git::Cmd::eCmd cmd) const;
    void     setMenuStringList(git::Cmd::eCmd cmd, const QStringList& list);

    void     initActionIcons();
    git::Cmd::eCmd createNewID(git::Cmd::eCmd fNewCmd) const;
    git::Cmd::eCmd findID(const QAction *action) const;

    void enableItemsByType(const git::Cmd::tVector& aItems, const git::Type& aType) const;
    void fillToolbar(QToolBar& aMenu, const git::Cmd::tVector& aItems);
    void fillContextMenue(QMenu& aMenu, const git::Cmd::tVector& aItems, QWidget *widget=0) const;
    const tActionMap& getList() const { return mActionList; }
    QString check_location(const QString &file_location);
    void setTheme(const QString& theme);
    const QString& getTheme();
    void setIconLocation(const QString& icons);
    const QString& getIconLocation();

    static const QString sNoCustomCommandMessageBox;

private Q_SLOTS:
    void select_action(QAction*action);

private:
    void initAction(git::Cmd::eCmd cmd, QAction* action);
    void setDataVariant(git::Cmd::eCmd aCmd, Data::e aData, const QVariant& aVariant);
    QVariant getDataVariant(git::Cmd::eCmd aCmd, Data::e aData) const;

    constexpr static char resource[] = ":/resource/24X24/";

    tActionMap     mActionList;
    tToolButtonMap mToolButtonList;
    tMenuMap       mMenuList;
    QObject*       mParent;
    QString        m_theme;
    QString        m_icon_location;
};

#endif // ACTIONS_H
