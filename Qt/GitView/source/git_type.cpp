
#include "git_type.h"
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
std::vector<Cmd::tVector> Cmd::mToolbars;
#ifdef DOCKED_VIEWS
std::vector<QString> Cmd::mToolbarNames;
#endif

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
    mCommandMap[StashPop]               = "git -C %1 stash pop";
    mCommandMap[StashShow]              = "git -C %1 stash show";
    mCommandMap[Show]                   = "git show %1 %2";

    mCommandMap[BranchList]             = "git -C %1 branch --list";
    mCommandMap[BranchDelete]           = "git -C %1 branch --delete --force %2";
    mCommandMap[BranchShow]             = "git -C %1 show %2";
    mCommandMap[BranchListRemote]       = "git -C %1 branch --list --remotes";
    mCommandMap[BranchListMerged]       = "git -C %1 branch --list --merged";
    mCommandMap[BranchListNotMerged]    = "git -C %1 branch --list --no-merged";
    mCommandMap[BranchCheckout]         = "git -C %1 checkout %2";
    mCommandMap[BranchHistory]          = "git -C %1 log --pretty=format:\"%H<td>%T<td>%P<td>%B<td>%an<td>%ae<td>%ad<td>%cn<td>%ce<td>%cd<tr>\" %2";

    mContextMenuSourceTree      = { Add, Unstage, Restore, Remove, Delete, MoveOrRename, Separator, ShowDifference, CallDiffTool, CallMergeTool, ShowShortStatus, ShowStatus, Commit, History, Separator, ExpandTreeItems, CollapseTreeItems  };
    mContextMenuEmptySourceTree = { AddGitSourceFolder, UpdateGitStatus, Separator, ExpandTreeItems, CollapseTreeItems};

    mContextMenuHistoryTree     = { ShowDifference, CallDiffTool, InsertHashFileNames, Separator, ShowHideTree, ClearTreeItems };
    mContextMenuBranchTree      = { BranchList, BranchListRemote, BranchListMerged, BranchListNotMerged, Separator, BranchShow, BranchHistory, BranchCheckout, BranchDelete, Separator, ShowHideTree, ClearTreeItems };

    mToolbars.push_back({ Add, Unstage, Restore, MoveOrRename, Remove, Separator, ShowDifference, CallDiffTool, CallMergeTool, History, Separator, ShowStatus, ShowShortStatus, BranchList, About, KillBackgroundThread});
    mToolbars.push_back({ AddGitSourceFolder, UpdateGitStatus, Separator, ShowHideTree, ClearTreeItems, ExpandTreeItems, CollapseTreeItems, SelectTextBrowserLanguage, InvokeHighlighterDialog, Separator, Commit, Push, Pull, Separator, Stash, StashShow, StashPop, Separator, CustomGitActionSettings});
#ifdef DOCKED_VIEWS
    mToolbarNames.push_back(QObject::tr("Git Commands"));
    mToolbarNames.push_back(QObject::tr("Control Commands"));
#endif
}


QString Cmd::toString(const ePostAction anAction)
{
    switch (anAction)
    {
    case DoNothing:             return QObject::tr("Do Nothing");
    case UpdateItemStatus:      return QObject::tr("Update Item Status");
    case ParseHistoryText:      return QObject::tr("Parse History List Text");
    case ParseBranchListText:   return QObject::tr("Parse Branch List Text");
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
    QStringList fStrings = aString.split('|');
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

Cmd mInstance;

namespace Folder
{
    const char GitIgnoreFile[] = ".gitignore";
    const char GitRepository[] = ".git";
    const char FolderUp[]      = "..";
    const char FolderSelf[]    = ".";
}

bool Type::mShort = true;

void Type::add(TypeFlags aType)
{
    mType = static_cast<TypeFlags>(mType|aType);
}

void Type::remove(TypeFlags aType)
{
    mType = static_cast<TypeFlags>(mType&~aType);
}

bool Type::is(TypeFlags aType) const
{
    return (mType & aType) != None;
}

QString Type::getStates() const
{
    const QString fSep = "|";
    QString fState = fSep;
    if (     is(GitAdded    ))  fState += name(GitAdded)     + fSep;
    if (     is(GitDeleted  ))  fState += name(GitDeleted)   + fSep;
    if (     is(GitModified ))  fState += name(GitModified)  + fSep;
    if (     is(GitUnTracked))  fState += name(GitUnTracked) + fSep;
    if (     is(GitStaged   ))  fState += name(GitStaged)    + fSep;
    if (     is(GitUnmerged ))  fState += name(GitUnmerged)  + fSep;
    if (     is(GitLocal    ))  fState += name(GitLocal)     + fSep;
    if (     is(GitRemote   ))  fState += name(GitRemote)    + fSep;
    if (     is(GitBoth     ))  fState += name(GitBoth)      + fSep;
    if (     is(GitStashed  ))  fState += name(GitStashed)   + fSep;

    if      (is(GitMovedFrom))  fState += name(GitMovedFrom) + fSep;
    else if (is(GitMovedTo  ))  fState += name(GitMovedTo)   + fSep;
    else if (is(GitRenamed  ))  fState += name(GitRenamed)   + fSep;
    return fState;
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
        case GitIgnore:    return mShort ? "GIg" : "git ignore";
        case GitMovedFrom: return mShort ? "MvF" : "MovedFrom";
        case GitMovedTo:   return mShort ? "MvT" : "MovedTo";
        case GitStashed:   return mShort ? "Sts" : "Stashed";
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
        RETURN_NAME(ContainingNegation);
        RETURN_NAME(None);
        RETURN_NAME(Checked);
        RETURN_NAME(Executeable);
    }
    return "";
}

QString Type::type_name() const
{
    return Type::name(static_cast<Type::TypeFlags>(Type::FileType&mType));
}

void Type::translate(const QString& fIdentifier)
{

    if (fIdentifier.contains('D'))  add(GitDeleted);
    if      (fIdentifier[1]=='M')   add(GitModified);
    else if (fIdentifier[0]=='M')   add(static_cast<TypeFlags>(GitModified|GitStaged));
    if      (fIdentifier[1]=='A')   add(GitAdded);
    else if (fIdentifier[0]=='A')   add(static_cast<TypeFlags>(GitAdded|GitStaged));
    if      (fIdentifier[1]=='R')   add(GitRenamed);
    else if (fIdentifier[0]=='R')   add(static_cast<TypeFlags>(GitRenamed|GitStaged));
    if      (fIdentifier=="DD")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both deleted
    else if (fIdentifier=="AU")     add(static_cast<TypeFlags>(GitUnmerged|GitLocal));  // unmerged, added by us
    else if (fIdentifier=="UD")     add(static_cast<TypeFlags>(GitUnmerged|GitRemote)); // unmerged, deleted by them
    else if (fIdentifier=="UA")     add(static_cast<TypeFlags>(GitUnmerged|GitRemote)); // unmerged, added by them
    else if (fIdentifier=="DU")     add(static_cast<TypeFlags>(GitUnmerged|GitLocal));  // unmerged, deleted by us
    else if (fIdentifier=="AA")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both added
    else if (fIdentifier=="UU")     add(static_cast<TypeFlags>(GitUnmerged|GitBoth));   // unmerged, both modified
    if (fIdentifier.contains("?"))  add(GitUnTracked);
    if (fIdentifier.contains("##")) add(Repository);

}

void Type::translate(const QFileInfo& fInfo)
{
    if (fInfo.isFile())       add(File);
    if (fInfo.isDir())        add(Folder);
    if (fInfo.isHidden())     add(Hidden);
    if (fInfo.isExecutable()) add(Executeable);
    if (fInfo.isSymLink())    add(SymLink);
}


}
