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
        None        = 0,
        File        = 0x00001,
        Folder      = 0x00002,
        SymLink     = 0x00004,
        WildCard    = 0x00008,
        GitModified = 0x00010,
        GitDeleted  = 0x00020,
        GitAdded    = 0x00040,
        GitUnknown  = 0x00080,
        GitRenamed  = 0x00100,
        GitIgnore   = 0x01000,
        GitFolder   = 0x02000|Folder,
        Repository  = 0x04000,
        Checked     = 0x08000,
        Hidden      = 0x10000,
        Executeable = 0x20000,
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
