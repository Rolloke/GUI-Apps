#ifndef GIT_TYPE_H
#define GIT_TYPE_H

#include <QMap>
#include <vector>

#include <QString>

#define DOCKED_VIEWS 1

class QFileInfo;
typedef QMap<QString, bool> string2bool_map;

namespace git
{
typedef std::map<int, QString> int2stringmap;

struct Cmd
{
    Cmd();
    enum eCmd
    {
        GetStatusAll,
        ShowStatus,
        ShowShortStatus,
        ShowDifference,
        CallDiffTool,
        Remove,
        Add,
        Unstage,
        Commit,
        MoveOrRename,
        Checkout,
        History,
        Push,
        Pull,
        Show,
        Stash,
        StashPop,
        BranchList,
        BranchDelete,
        BranchShow,
        BranchListRemote,
        BranchListMerged,
        BranchListNotMerged,
        BranchCheckout,
        BranchHistory,
        CallMergeTool,
        StashShow,

        /// hint: add further git commands here
        CustomCommand=50,
        /// hint: this range is reserved for custom git commands
        NonGitCommands = 1000,
        /// hint: add non git commands here
        Separator = NonGitCommands,
        ExpandTreeItems,
        CollapseTreeItems,
        AddGitSourceFolder,
        UpdateGitStatus,
        ShowHideTree,
        ClearTreeItems,
        CustomGitActionSettings,
        InsertHashFileNames,
        About,
        Delete,
        SelectTextBrowserLanguage,
        InvokeGitMergeDialog,
        InvokeHighlighterDialog,
        KillBackgroundThread,
        FirstGitCommand = GetStatusAll,
        LastGitCommand  = NonGitCommands-1,
        Invalid = -1,
        Restore = Checkout
    };
    enum ePostAction
    {
        DoNothing,
        UpdateItemStatus,
        ParseHistoryText,
        ParseBranchListText,
    };

    typedef std::vector<eCmd> tVector;

    static const QString& getCommand(eCmd);

    static QString toString(const ePostAction anAction);
    static QString toString(const tVector& aItems);
    static tVector fromString(const QString& aString);
    static QString toStringMT(const string2bool_map& aItems);
    static string2bool_map fromStringMT(const QString& aString);

    static tVector mContextMenuSourceTree;
    static tVector mContextMenuEmptySourceTree;
    static tVector mContextMenuHistoryTree;
    static tVector mContextMenuBranchTree;
    static std::vector<tVector> mToolbars;
#ifdef DOCKED_VIEWS
    static std::vector<QString> mToolbarNames;
#endif

private:
    static int2stringmap mCommandMap;
};

struct Type
{
    enum TypeFlags
    {
        None                = 0,
        File                = 0x0000001,
        Folder              = 0x0000002,
        SymLink             = 0x0000004,
        GitStaged           = 0x0000008,
        GitModified         = 0x0000010,
        GitDeleted          = 0x0000020,
        GitAdded            = 0x0000040,
        GitUnTracked        = 0x0000080,
        GitRenamed          = 0x0000100,
        GitUnmerged         = 0x0000200,
        GitLocal            = 0x0000400,
        GitRemote           = 0x0000800,
        GitBoth             = GitLocal|GitRemote,
        GitIgnore           = 0x0001000,
        GitFolder           = 0x0002000|Folder,
        Repository          = 0x0004000,
        Checked             = 0x0008000,
        Hidden              = 0x0010000,
        Executeable         = 0x0020000,
        Branch              = 0x0040000,
        WildCard            = 0x0100000,
        Negation            = 0x0200000,
        RegExp              = 0x0400000,
        ContainingNegation  = 0x0800000,
        GitMovedFrom        = 0x1000000,
        GitMovedTo          = 0x2000000,
        GitStashed          = 0x4000000,
        AllGitActions = GitModified|GitDeleted|GitAdded|GitRenamed|GitUnTracked|GitUnmerged|GitStaged|GitMovedFrom|GitMovedTo|GitStashed,
        FileType      = File|Folder|SymLink,
        FileFlag      = Hidden|Executeable
    };

    Type(uint aType = None, int aLevel = 0)
      : mType(aType)
      , mLevel(aLevel) {}

    void add(TypeFlags aType);
    void remove(TypeFlags aType);
    bool is(TypeFlags aType) const;
    QString getStates() const;
    QString type_name() const;

    static const char* name(TypeFlags aType);

    void translate(const QString& fIdentifier);
    void translate(const QFileInfo& fInfo);

    uint mType;
    int mLevel;
    static bool mShort;
};

namespace Folder
{
extern const char GitIgnoreFile[];
extern const char GitRepository[];
extern const char FolderUp[];
extern const char FolderSelf[];
}



typedef std::map<std::string, Type> stringt2typemap;

}

#endif // GIT_TYPE_H
