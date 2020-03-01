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
    enum eType
    {
        None        = 0,
        File        = 0x0001,
        Folder      = 0x0002,
        Hidden      = 0x0004,
        WildCard    = 0x0008,
        GitModified = 0x0010,
        GitDeleted  = 0x0020,
        GitAdded    = 0x0040,
        GitUnknown  = 0x0080,
        GitRenamed  = 0x0100,
        GitIgnore   = 0x1000,
        GitFolder   = 0x2000|Folder,
        Repository  = 0x4000,
        Checked     = 0x8000,
        AllGitActions = GitModified|GitDeleted|GitAdded|GitUnknown|GitRenamed,
        FileType    =File|Folder
    };

    Type(uint aType = None, int aLevel = 0)
      : mType(aType)
      , mLevel(aLevel) {}

    void add(eType aType);
    void remove(eType aType);
    bool is(eType aType) const;

    static const char* name(eType aType);
    static eType       translate(const QString& fIdentifier);

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
