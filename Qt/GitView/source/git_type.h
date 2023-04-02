#ifndef GIT_TYPE_H
#define GIT_TYPE_H

#include <QMap>
#include <vector>
#include <unordered_map>

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
        DiffOfTwoBranches,
        CallMergeTool,
        StashShow,
        StashList,
        StashClear,
        StashDrop,
        Fetch,
        StashApply,
        StashPush,
        Blame,

        /// hint: add further git commands here
        CustomCommand=50,
        /// hint: this range is reserved for custom git commands
        CustomTestCommand=499,
        AutoCommand=500,
        RenderGraphics = AutoCommand+6,
        /// hint: this range is reserved for automatically created commands
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
        CopyFileName,
        CopyFilePath,
        RemoveGitFolder,
        ZoomIn,
        ZoomOut,
        FitInView,
        DeleteTreeItems,
        AddExternalFileOpenExt,
        DeleteExternalFileOpenExt,
        CloneTextBrowser,
        CreateBookMark,
        ShowInformation,
        OpenFileExternally,
        WhatsThisHelp,
        CompareTo,
        CloseAll,
        SaveAll,
        OpenFile,
        FirstGitCommand = GetStatusAll,
        LastGitCommand  = AutoCommand-1,
        LastAutoCommand = NonGitCommands-1,
        LastNonGitCommand = CompareTo,
        Invalid = -1,
        Restore = Checkout
    };
    enum ePostAction
    {
        DoNothing,
        UpdateItemStatus,
        ParseHistoryText,
        ParseBranchListText,
        ParseStashListText,
        ParseBlameText,
        UpdateRootItemStatus,
        UpdateRepository
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
    static tVector mContextMenuStashTree;
    static tVector mContextMenuGraphicsView;
    static tVector mContextMenuTextView;
    static tVector mContextMenuFindTextTree;
    static std::vector<tVector> mToolbars;
#ifdef DOCKED_VIEWS
    static std::vector<QString> mToolbarNames;
#endif

private:
    static int2stringmap mCommandMap;
};

void operator++(git::Cmd::eCmd& cmd);

struct Type
{
    enum TypeFlags
    {
        None                = 0,
        File                = 0x00000001,
        Folder              = 0x00000002,
        SymLink             = 0x00000004,
        GitStaged           = 0x00000008,
        GitModified         = 0x00000010,
        GitDeleted          = 0x00000020,
        GitAdded            = 0x00000040,
        GitUnTracked        = 0x00000080,
        GitRenamed          = 0x00000100,
        GitUnmerged         = 0x00000200,
        GitLocal            = 0x00000400,
        GitRemote           = 0x00000800,
        GitBoth             = GitLocal|GitRemote,
        GitIgnore           = 0x00001000,
        GitFolder           = 0x00002000,
        Repository          = 0x00004000,
        Checked             = 0x00008000,
        Hidden              = 0x00010000,
        Executeable         = 0x00020000,
        Branch              = 0x00040000,
        WildCard            = 0x00100000,
        Negation            = 0x00200000,
        RegExp              = 0x00400000,
        FolderForNavigation = 0x00800000,
        GitMovedFrom        = 0x01000000,
        GitMovedTo          = 0x02000000,
        IgnoreTypeStatus    = 0x04000000,
        IncludeAll          = 0x10000000,
        Consecutive         = 0x20000000,
        DiffOf2Commits      = 0x40000000,
        AllGitActions = GitModified|GitDeleted|GitAdded|GitRenamed|GitUnTracked|GitUnmerged|GitStaged|GitMovedFrom|GitMovedTo,
        FileType      = File|Folder|SymLink,
        FileFlag      = Hidden|Executeable
    };

    Type(uint aType = None, int aLevel = 0)
      : mType(aType)
      , mLevel(aLevel) {}

    void add(TypeFlags aType);
    void remove(TypeFlags aType);
    void setType(uint aType);
    bool is(TypeFlags aType) const;
    uint type() const;
    uint level() const;
    QString getStates(bool extended=false) const;
    QString type_name() const;

    static const char* name(TypeFlags aType);
    static TypeFlags type(uint type);

    void translate(const QString& fIdentifier);
    void translate(const QFileInfo& fInfo);
private:
    uint mType;
    int mLevel;
public:
    static bool mShort;
};

namespace Folder
{
extern const char GitIgnoreFile[];
extern const char GitRepository[];
extern const char FolderUp[];
extern const char FolderSelf[];
}

typedef std::map<QString, Type> stringt2typemap;
typedef std::unordered_map<QString, Type> stringt2type_umap;
typedef std::pair<QString, Type> string2typepair;
typedef std::vector< string2typepair > stringt2type_vector;

void parseGitStatus(const QString& fSource, const QString& aStatus, git::stringt2typemap& aFiles);

}

#endif // GIT_TYPE_H
