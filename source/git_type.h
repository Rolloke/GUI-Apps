#ifndef GIT_TYPE_H
#define GIT_TYPE_H

#include <boost/container/flat_map.hpp>
#include <map>

#include <QString>

#ifdef NDEBUG
#define MAP_TYPE 0
#else
#define MAP_TYPE 1
#endif

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
        Commit,
        MoveOrRename,
        Restore,
        History,
        Push,
        ShowHistoryDifference,
        CallHistoryDiffTool,
        /// hint: add custom commands here
        CustomCommand=20
    };
    enum eCustom
    {
        DoNothing,
        UpdateItemStatus,
        ParseHistoryText
    };

    static const QString& getCommand(eCmd);
private:
    static int2stringmap mCommandMap;
};

struct Type
{
    enum TypeFlags
    {
        None                = 0,
        File                = 0x000001,
        Folder              = 0x000002,
        SymLink             = 0x000004,
        //Reserved          = 0x000008,
        GitModified         = 0x000010,
        GitDeleted          = 0x000020,
        GitAdded            = 0x000040,
        GitUnknown          = 0x000080,
        GitRenamed          = 0x000100,
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
        AllGitActions = GitModified|GitDeleted|GitAdded|GitRenamed|GitUnknown,
        FileType    =File|Folder|SymLink,
        FileFlag    =Hidden|Executeable
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
