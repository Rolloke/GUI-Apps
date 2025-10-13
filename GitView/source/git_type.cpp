
#include "git_type.h"
#include "logger.h"
#include <QStringList>
#include <QObject>
#include <QFileInfo>

namespace git
{

int2stringmap Cmd::mCommandMap;
Cmd::tVector  Cmd::mContextMenuSourceTree;
Cmd::tVector  Cmd::mContextMenuEmptySourceTree;
Cmd::tVector  Cmd::mContextMenuHistoryTree;
Cmd::tVector  Cmd::mContextMenuBranchTree;
Cmd::tVector  Cmd::mContextMenuStashTree;
Cmd::tVector  Cmd::mContextMenuGraphicsView;
Cmd::tVector  Cmd::mContextMenuTextView;
Cmd::tVector  Cmd::mContextMenuFindTextTree;
std::vector<Cmd::tVector> Cmd::mToolbars;
std::vector<QString> Cmd::mToolbarNames;

void operator++(Cmd::eCmd& cmd)
{
    cmd = static_cast<Cmd::eCmd>(cmd + 1);
}

Cmd::Cmd()
{
    mCommandMap[GetStatusAll]           = "git -C %1 status -sb";
    mCommandMap[ShowStatus]             = "git status %1 %2";
    mCommandMap[ShowShortStatus]        = "git status -sb %1 %2";
    mCommandMap[CallDiffTool]           = "git difftool %1 --no-prompt %2";
    mCommandMap[CallMergeTool]          = "git mergetool %1 --no-prompt %2";
    mCommandMap[ShowDifference]         = "git diff %1 %2";
    mCommandMap[Remove]                 = "git rm -r --cached %1 %2";
    mCommandMap[Add]                    = "git add %1 %2";
    mCommandMap[Unstage]                = "git reset HEAD %1 %2";
    mCommandMap[Commit]                 = "git -C %1 commit -m \"%2\"";
    mCommandMap[MoveOrRename]           = "git -C %1 mv %2 %3";
    mCommandMap[Restore]                = "git checkout %1 %2";
    mCommandMap[History]                = "git log --pretty=format:\"%H<td>%T<td>%P<td>%B<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\" %1 -- %2";
    mCommandMap[Push]                   = "git -C %1 push";
    mCommandMap[Pull]                   = "git -C %1 pull";
    mCommandMap[Stash]                  = "git -C %1 stash";
    mCommandMap[StashPush]              = "git stash push %1 %2";
    mCommandMap[StashPop]               = "git -C %1 stash pop %2";
    mCommandMap[StashApply]             = "git -C %1 stash apply %2";
    mCommandMap[StashShow]              = "git -C %1 stash show";
    mCommandMap[StashList]              = "git -C %1 stash list";
    mCommandMap[StashClear]             = "git -C %1 stash clear";
    mCommandMap[StashDrop]              = "git -C %1 stash drop %2";
    mCommandMap[Show]                   = "git show %1 %2";
    mCommandMap[Fetch]                  = "git -C %1 fetch";
    mCommandMap[UndoCommitSoft]         = "git reset --soft %1 %2";
    mCommandMap[UndoCommitHard]         = "git reset --hard %1 %2";

    mCommandMap[BranchList]             = "git -C %1 branch --list";
    mCommandMap[BranchDelete]           = "git -C %1 branch --delete --force %2";
    mCommandMap[BranchCreate]           = "git -C %1 branch --create --force %2";
    mCommandMap[BranchShow]             = "git -C %1 show %2";
    mCommandMap[BranchListRemote]       = "git -C %1 branch --list --remotes";
    mCommandMap[BranchListMerged]       = "git -C %1 branch --list --merged";
    mCommandMap[BranchListNotMerged]    = "git -C %1 branch --list --no-merged";
    mCommandMap[BranchCheckout]         = "git -C %1 checkout %2";
    mCommandMap[DiffOfTwoBranches]      = " ";
    mCommandMap[Blame]                  = "git blame -c %1 %2";
    mCommandMap[Clone]                  = "git clone -v %1";
    mCommandMap[MergeTwoBranches]       = " ";

    mContextMenuSourceTree      = { CopyFileName, CopyFilePath, Delete, Separator, Add, Unstage, Restore, Remove, MoveOrRename, Separator, CompareTo, AddExternalFileOpenExt, DeleteExternalFileOpenExt, OpenFileExternally, Separator, ShowDifference, CallDiffTool, CallMergeTool, Commit, StashPush, History, Separator, StashShow, ShowShortStatus, ShowStatus, ShowInformation };
    mContextMenuEmptySourceTree = { AddGitSourceFolder, RemoveGitFolder, Clone, UpdateGitStatus, Separator, ExpandTreeItems, CollapseTreeItems };

    mContextMenuHistoryTree     = { ShowDifference, CallDiffTool, InsertHashFileNames, Separator, Restore, UndoCommitSoft, UndoCommitHard, Separator, ExpandTreeItems, CollapseTreeItems, ClearTreeItems, DeleteTreeItems };
    mContextMenuBranchTree      = { BranchList, BranchListRemote, BranchListMerged, BranchListNotMerged, Separator, BranchShow, DiffOfTwoBranches, MergeTwoBranches, BranchCreate, BranchCheckout, BranchDelete, Separator, ExpandTreeItems, CollapseTreeItems, ClearTreeItems, DeleteTreeItems };
    mContextMenuStashTree       = { ShowDifference, CallDiffTool, Separator, StashPop, StashApply, StashDrop, StashClear, Separator, ExpandTreeItems, CollapseTreeItems, ClearTreeItems, DeleteTreeItems };
    mContextMenuGraphicsView    = { ZoomIn, ZoomOut, Separator, FitInView };
    mContextMenuTextView        = { CloneTextBrowser, CreateBookMark, Separator, CloseAll, SaveAll, ReplaceAll, SaveAs };
    mContextMenuFindTextTree    = { ExpandTreeItems, CollapseTreeItems, ClearTreeItems, DeleteTreeItems };

    mToolbars.push_back({ Add, Unstage, Restore, MoveOrRename, Remove, Separator, ShowDifference, CallDiffTool, CallMergeTool, History, StashShow, Blame, Separator, ShowStatus, ShowShortStatus, About, KillBackgroundThread});
    mToolbars.push_back({ AddGitSourceFolder, RemoveGitFolder, UpdateGitStatus, Separator, OpenFile, ClearTreeItems, ExpandTreeItems, CollapseTreeItems, SelectTextBrowserLanguage, InvokeHighlighterDialog, InvokeOutputParserDialog, RenderGraphics, Separator, Commit, Push, Pull, Fetch, Separator, Stash, StashList, Separator, BranchList, WhatsThisHelp, CustomGitActionSettings});
    mToolbarNames.push_back(QObject::tr("Toolbar Git Commands"));
    mToolbarNames.push_back(QObject::tr("Toolbar Control Commands"));
}


QString Cmd::toString(const ePostAction anAction)
{
    switch (anAction)
    {
    case DoNothing:             return QObject::tr("Do Nothing");
    case UpdateItemStatus:      return QObject::tr("Update Item Status");
    case ParseHistoryText:      return QObject::tr("Parse History List Text");
    case ParseBranchListText:   return QObject::tr("Parse Branch List Text");
    case ParseStashListText:    return QObject::tr("Parse Stash List Text");
    case ParseBlameText:        return QObject::tr("Parse Blame Text");
    case UpdateRootItemStatus:  return QObject::tr("Update Root Item Status");
    case UpdateRepository:      return QObject::tr("Update Repository Status");
    case UpdateStash:           return QObject::tr("Update Stash Status");
    case UpdateRepositorySubFolder: return QObject::tr("Update Repository Folder Status");
    case InsertRepository:      return QObject::tr("Insert Repository");
    }
    return "";
}

QString  Cmd::toString(const tVector& aItems)
{
    QString fString = "|";
    for (const auto& fItem: aItems)
    {
        fString += QString::number(fItem);
        fString += "|";
    }
    return fString;
}


Cmd::tVector Cmd::fromString(const QString& aString)
{
    const QStringList fStrings = aString.split('|');
    tVector fItems;
    for (const auto& fItem: fStrings)
    {
        if (fItem.size() == 0) continue;
        fItems.push_back(static_cast<Cmd::eCmd>(fItem.toInt()));
    }
    return fItems;
}

QString  Cmd::toStringMT(const string2bool_map &aItems)
{
    QString fString = "|";
    for ( auto fItem = aItems.begin(); fItem != aItems.end(); ++fItem)
    {
        fString += fItem.key();
        fString += "|";
        fString += fItem.value() ? "true" : "false";
        fString += "|";
    }
    if (fString.size() == 1)
    {
        fString.clear();
    }
    return fString;
}


string2bool_map Cmd::fromStringMT(const QString& aString)
{
    QStringList fStrings = aString.split('|');
    string2bool_map fItems;
    for (int i=1; i<fStrings.size()-1; i+=2)
    {
        fItems[fStrings[i]] = fStrings[i+1] == "true";
    }
    return fItems;
}

const QString& Cmd::getCommand(eCmd aCmd)
{
    return mCommandMap[aCmd];
}

bool Cmd::isCommandUsed(eCmd cmd)
{
    {
        const auto& tb = mContextMenuSourceTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuEmptySourceTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuHistoryTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuBranchTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuStashTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuGraphicsView;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuTextView;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    {
        const auto& tb = mContextMenuFindTextTree;
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    for (const auto& tb : mToolbars)
    {
        if (std::find(tb.begin(), tb.end(), cmd) != tb.end()) return true;
    }
    return false;
}

Cmd mInstance;

namespace Folder
{
    const char GitIgnoreFile[] = ".gitignore";
    const char GitRepository[] = ".git";
    const char FolderUp[]      = "..";
    const char FolderSelf[]    = ".";
}

bool Type::mShort = false;

void Type::add(TypeFlags aType)
{
    mType = type(mType|aType);
}

void Type::remove(TypeFlags aType)
{
    mType = type(mType&~aType);
}

bool Type::is(TypeFlags aType) const
{
    return (mType & aType) != None;
}

uint Type::type() const
{
    return mType;
}

void Type::setType(uint aType)
{
    mType = aType;
}

uint Type::level() const
{
    return mLevel;
}

QString Type::getStates(bool extended) const
{
    const QString sep = "|";
    QString states = sep;
    if (     is(GitAdded    ))  states += name(GitAdded)     + sep;
    if (     is(GitDeleted  ))  states += name(GitDeleted)   + sep;
    if (     is(GitModified ))  states += name(GitModified)  + sep;
    if (     is(GitUnTracked))  states += name(GitUnTracked) + sep;
    if (     is(GitStaged   ))  states += name(GitStaged)    + sep;
    if (     is(GitUnmerged ))  states += name(GitUnmerged)  + sep;
    if (     is(GitLocal    ))  states += name(GitLocal)     + sep;
    if (     is(GitRemote   ))  states += name(GitRemote)    + sep;
    if (     is(GitBoth     ))  states += name(GitBoth)      + sep;
    if (     is(GitIgnored  ))  states += name(GitIgnored)   + sep;

    if      (is(GitMovedFrom))  states += name(GitMovedFrom) + sep;
    else if (is(GitMovedTo  ))  states += name(GitMovedTo)   + sep;
    else if (is(GitRenamed  ))  states += name(GitRenamed)   + sep;

    if (extended)
    {
        if (is(SymLink))             states += name(SymLink)     + sep;
        if (is(Repository))          states += name(Repository)  + sep;
        if (is(File))                states += name(File)        + sep;
        if (is(Folder))              states += name(Folder)      + sep;
        if (is(Branch))              states += name(Branch)      + sep;
        if (is(Hidden))              states += name(Hidden)      + sep;
        if (is(WildCard))            states += name(WildCard)    + sep;
        if (is(RegExp))              states += name(RegExp)      + sep;
        if (is(Negation))            states += name(Negation)    + sep;
        if (is(Checked))             states += name(Checked)     + sep;
        if (is(Executeable))         states += name(Executeable) + sep;
        if (is(Consecutive))         states += name(Consecutive) + sep;
        if (is(FolderForNavigation)) states += name(FolderForNavigation) + sep;
        if (is(IgnoreTypeStatus))    states += name(IgnoreTypeStatus) + sep;
    }
    if (states.size() == 1)
    {
        states.clear();
    }
    return states;
}

#define RETURN_NAME(NAME) case NAME: return #NAME
const char* Type::name(TypeFlags aType)
{
    switch (aType)
    {
        case GitStaged:    return mShort ? "Stg" : "Staged";
        case GitDeleted:   return mShort ? "Del" : "Deleted";
        case GitModified:  return mShort ? "Mod" : "Modified";
        case GitAdded:     return mShort ? "Add" : "Added";
        case GitUnTracked: return mShort ? "UTk" : "Untracked";
        case GitRenamed:   return mShort ? "Ren" : "Renamed";
        case GitUnmerged:  return mShort ? "UMg" : "Unmerged";
        case GitLocal:     return mShort ? "Loc" : "Local";
        case GitRemote:    return mShort ? "Rem" : "Remote";
        case GitBoth:      return mShort ? "Bot" : "Both";
        case GitFolder:    return mShort ? "GFd" : "git folder";
        case GitIgnored:   return mShort ? "Ign" : "Ignored";
        case GitMovedFrom: return mShort ? "MvF" : "MovedFrom";
        case GitMovedTo:   return mShort ? "MvT" : "MovedTo";
        case SymLink:      return mShort ? "SyL" : "symbolic link";
        case FileType:     return mShort ? "FTy" : "file types (group)";
        case FileFlag:     return mShort ? "FFg" : "file flags (group)";
        RETURN_NAME(AllGitActions);
        RETURN_NAME(Repository);
        RETURN_NAME(File);
        RETURN_NAME(Folder);
        RETURN_NAME(Branch);
        RETURN_NAME(Hidden);
        RETURN_NAME(WildCard);
        RETURN_NAME(RegExp);
        RETURN_NAME(Negation);
        RETURN_NAME(FolderForNavigation);
        RETURN_NAME(IgnoreTypeStatus);
        RETURN_NAME(None);
        RETURN_NAME(Checked);
        RETURN_NAME(Executeable);
        RETURN_NAME(IncludeAll);
        RETURN_NAME(Consecutive);
        RETURN_NAME(DiffOf2Commits);
        RETURN_NAME(Unused1);
        RETURN_NAME(Unused2);
        RETURN_NAME(Unused3);
    }
    return "";
}

Type::TypeFlags Type::type(uint type)
{
    return static_cast<TypeFlags>(type);
}

QString Type::type_name() const
{
    return Type::name(type(Type::FileType&mType));
}

void Type::translate(const QString& git_identifier)
{
    if (git_identifier.contains('D'))  add(GitDeleted);
    if      (git_identifier[1]=='M')   add(GitModified);
    else if (git_identifier[0]=='M')   add(type(GitModified|GitStaged));
    if      (git_identifier[1]=='A')   add(GitAdded);
    else if (git_identifier[0]=='A')   add(type(GitAdded|GitStaged));
    if      (git_identifier[1]=='R')   add(GitRenamed);
    else if (git_identifier[0]=='R')   add(type(GitRenamed|GitStaged));
    if      (git_identifier=="DD")     add(type(GitUnmerged|GitBoth));   // unmerged, both deleted
    else if (git_identifier=="AU")     add(type(GitUnmerged|GitLocal));  // unmerged, added by us
    else if (git_identifier=="UD")     add(type(GitUnmerged|GitRemote)); // unmerged, deleted by them
    else if (git_identifier=="UA")     add(type(GitUnmerged|GitRemote)); // unmerged, added by them
    else if (git_identifier=="DU")     add(type(GitUnmerged|GitLocal));  // unmerged, deleted by us
    else if (git_identifier=="AA")     add(type(GitUnmerged|GitBoth));   // unmerged, both added
    else if (git_identifier=="UU")     add(type(GitUnmerged|GitBoth));   // unmerged, both modified
    if (git_identifier.contains("?"))  add(GitUnTracked);
    if (git_identifier.contains("##")) add(Repository);

}

void Type::translate(const QFileInfo& fInfo)
{
    if (fInfo.isFile())       add(File);
    if (fInfo.isDir())        add(Folder);
    if (fInfo.isHidden())     add(Hidden);
    if (fInfo.isExecutable()) add(Executeable);
    if (fInfo.isSymLink())    add(SymLink);
}

void parseGitStatus(const QString& aSource, const QString& aStatus, stringt2typemap& aFiles)
{
    const auto fLines = aStatus.split("\n");

    for (const QString& fLine : std::as_const(fLines))
    {
        const int     fStateSize = 2;
        const QString fState = fLine.left(fStateSize);
        auto          fRelativePath = fLine.mid(fStateSize).trimmed();

        if (fRelativePath.size())
        {
            Type fType;
            fType.translate(fState);
            while (fRelativePath.indexOf('"') != -1)
            {
                fRelativePath = fRelativePath.remove('"');
            }
            if (fType.is(Type::Repository))
            {
                aFiles[fRelativePath] = fType;
            }
            else
            {
                QString fFullPath = aSource + fRelativePath;
                if (fType.is(Type::GitRenamed) && fRelativePath.contains("->"))
                {
                    auto fPaths = fRelativePath.split(" -> ");
                    if (fPaths.size() > 1)
                    {
                        fFullPath = aSource + fPaths[1];
                        QFileInfo fFileInfo(fFullPath);
                        fType.translate(fFileInfo);
                        Type fDestinationType = fType;
                        fDestinationType.add(Type::GitMovedTo);
                        aFiles[fFullPath] = fDestinationType;
                        fType.add(Type::GitMovedFrom);
                        fFullPath = aSource + fPaths[0];
                    }
                }
                QFileInfo fFileInfo(fFullPath);
                fType.translate(fFileInfo);
                auto file_path = fFileInfo.filePath();
                if (fType.is(Type::Folder))
                {
                    if (file_path.endsWith('/'))
                    {
                        file_path.resize(file_path.size()-1);
                    }
                }
                aFiles[file_path] = fType;
            }

            TRACEX(Logger::trace, fState << ": " << fRelativePath << ": " << fType.type());
        }
    }
}

}
