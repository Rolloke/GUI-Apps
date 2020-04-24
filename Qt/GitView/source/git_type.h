#ifndef GIT_TYPE_H
#define GIT_TYPE_H

#include <boost/container/flat_map.hpp>
#include <map>
#include <vector>

#include <QString>

#ifdef NDEBUG
#define MAP_TYPE 0
#else
#define MAP_TYPE 1
#endif

#define INT(n) static_cast<qint32>(n)

namespace git
{
#if MAP_TYPE == 0
typedef boost::container::flat_map<int, QString> int2stringmap;
#elif MAP_TYPE == 1
typedef std::map<int, QString> int2stringmap;
#endif
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
        Restore,
        History,
        Push,
        ShowHistoryDifference,
        CallHistoryDiffTool,
        /// hint: add custom commands here
        CustomCommand=50,

        NonGitCommands = 1000,
        /// hint: add non git commands here
        Separator = NonGitCommands,
        ExpandTreeItems,
        CollapseTreeItems,
        AddGitSourceFolder,
        UpdateGitStatus,
        ShowHideHistoryTree,
        ClearHistory,
        FirstGitCommand = GetStatusAll,
        LastGitCommand  = NonGitCommands-1

    };
    enum eCustom
    {
        DoNothing,
        UpdateItemStatus,
        ParseHistoryText
    };

    static const QString& getCommand(eCmd);

    static std::vector<eCmd> mContextMenuSourceTree;
    static std::vector<eCmd> mContextMenuEmptySourceTree;
    static std::vector<eCmd> mContextMenuHistoryTree;
    static std::vector<std::vector<eCmd>> mToolbars;

private:
    static int2stringmap mCommandMap;
};

// TODO: change

struct Type
{
    enum TypeFlags
    {
        None                = 0,
        File                = 0x000001,
        Folder              = 0x000002,
        SymLink             = 0x000004,
        GitStaged           = 0x000008,
        GitModified         = 0x000010,
        GitDeleted          = 0x000020,
        GitAdded            = 0x000040,
        GitUnTracked        = 0x000080,
        GitRenamed          = 0x000100,
        GitUnmerged         = 0x000200,
        GitLocal            = 0x000400,
        GitRemote           = 0x000800,
        GitBoth             = GitLocal|GitRemote,
        GitIgnore           = 0x001000,
        GitFolder           = 0x002000|Folder,
        Repository          = 0x004000,
        Checked             = 0x008000,
        Hidden              = 0x010000,
        Executeable         = 0x020000,
        WildCard            = 0x100000,
        Negation            = 0x200000,
        RegExp              = 0x400000,
        ContainingNegation  = 0x800000,
        AllGitActions = GitModified|GitDeleted|GitAdded|GitRenamed|GitUnTracked|GitUnmerged|GitStaged,
        FileType      = File|Folder|SymLink,
        FileFlag      = Hidden|Executeable
    };

    Type(uint aType = None, int aLevel = 0)
      : mType(aType)
      , mLevel(aLevel) {}

    void add(TypeFlags aType);
    void remove(TypeFlags aType);
    bool is(TypeFlags aType) const;

    static const char* name(TypeFlags aType);
    static TypeFlags   translate(const QString& fIdentifier);

    uint mType;
    int mLevel;
};

namespace Folder
{
extern const char GitIgnoreFile[];
extern const char GitRepository[];
extern const char FolderUp[];
extern const char FolderSelf[];
}



#if MAP_TYPE == 0
typedef boost::container::flat_map<std::string, Type> stringt2typemap;
#elif MAP_TYPE == 1
typedef std::map<std::string, Type> stringt2typemap;
#endif

}

#endif // GIT_TYPE_H
