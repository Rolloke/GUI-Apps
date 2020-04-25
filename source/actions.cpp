#include "actions.h"

#include <QMenu>

using namespace git;

const QString ActionList::sNoCustomCommandMessageBox = "None";

ActionList::ActionList(QObject* aParent): mParent(aParent)
{

}

ActionList::~ActionList()
{
    for (auto& fAction : mActionList)
    {
        delete fAction.second;
    }
}

QAction * ActionList::createAction(Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand)
{
    QAction *fNewAction = new QAction(aName, mParent);
    mActionList[aCmd] = fNewAction;
    fNewAction->setStatusTip(aGitCommand);
    fNewAction->setToolTip(aName);
    QVariantList fList;
    fList.append(QVariant(sNoCustomCommandMessageBox));
    for (int i = INT(Data::Action); i<INT(Data::ListSize); ++i)
    {
        fList.append(QVariant());
    }
    fNewAction->setData(fList);
    return fNewAction;
}


void ActionList::initActionIcons()
{
    std::map<Cmd::eCmd, std::string> fActionIcons;
    fActionIcons[Cmd::Add]                     = ":/resource/24X24/list-add.png";
    fActionIcons[Cmd::Unstage]                 = ":/resource/24X24/list-remove.png";
    fActionIcons[Cmd::ShowDifference]          = ":/resource/24X24/object-flip-horizontal.png";
    fActionIcons[Cmd::CallDiffTool]            = ":/resource/24X24/distribute-graph-directed.svg";
    fActionIcons[Cmd::History]                 = ":/resource/24X24/document-open-recent.png";
    fActionIcons[Cmd::ShowStatus]              = ":/resource/24X24/help-faq.png";
    fActionIcons[Cmd::ShowShortStatus]         = ":/resource/24X24/dialog-question.png";
    fActionIcons[Cmd::Remove]                  = "://resource/24X24/user-trash.png";
    fActionIcons[Cmd::Commit]                  = "://resource/24X24/user-invisible.png";
    fActionIcons[Cmd::MoveOrRename]            = "://resource/24X24/format-text-direction-ltr.png";
    fActionIcons[Cmd::Restore]                 = "://resource/24X24/edit-redo-rtl.png";
    fActionIcons[Cmd::Push]                    = "://resource/24X24/view-sort-descending.png";
    fActionIcons[Cmd::ShowHistoryDifference]   = ":/resource/24X24/object-flip-horizontal.png";
    fActionIcons[Cmd::CallHistoryDiffTool]     = ":/resource/24X24/distribute-graph-directed.svg";

    fActionIcons[Cmd::ExpandTreeItems]         = "://resource/24X24/svn-update.svg";
    fActionIcons[Cmd::CollapseTreeItems]       = "://resource/24X24/svn-commit.svg";
    fActionIcons[Cmd::AddGitSourceFolder]      = "://resource/24X24/folder-open.png";
    fActionIcons[Cmd::UpdateGitStatus]         = "://resource/24X24/view-refresh.png";
    fActionIcons[Cmd::ShowHideHistoryTree]     = "://resource/24X24/code-class.svg";
    fActionIcons[Cmd::ClearHistory]            = "://resource/24X24/edit-clear.png";
    for (const auto& fIconPath: fActionIcons )
    {
        getAction(fIconPath.first)->setIcon(QIcon(fIconPath.second.c_str()));
        setIconPath(static_cast<Cmd::eCmd>(fIconPath.first), fIconPath.second.c_str());
    }
}

void ActionList::fillContextMenue(QMenu& aMenu, const std::vector<Cmd::eCmd>& aItems)
{
    for (auto fCmd : aItems)
    {
        if (fCmd == Cmd::Separator)
        {
            aMenu.addSeparator();
        }
        else
        {
            aMenu.addAction(getAction(fCmd));
        }
    }
}


QAction* ActionList::getAction(Cmd::eCmd aCmd)
{
    auto fItem = mActionList.find(aCmd);
    if (fItem != mActionList.end())
    {
        return fItem->second;
    }
    return 0;
}

void  ActionList::setCustomCommandMessageBoxText(Cmd::eCmd aCmd, const QString& aText)
{
    QAction* fAction = getAction(aCmd);
    QVariantList fVariantList = fAction->data().toList();
    fVariantList[INT(Data::MsgBoxText)] = QVariant(aText);
    fAction->setData(fVariantList);
}

QString ActionList::getCustomCommandMessageBoxText(Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::MsgBoxText);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}


void  ActionList::setCustomCommandPostAction(Cmd::eCmd aCmd, uint aAction)
{
    setDataVariant(aCmd, Data::Action, QVariant(aAction));
}

uint ActionList::getCustomCommandPostAction(Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::Action);
    if (fVariant.isValid())
    {
        return fVariant.toUInt();
    }
    return 0;
}

void ActionList::setIconPath(Cmd::eCmd aCmd, const QString& aPath)
{
    setDataVariant(aCmd, Data::IconPath, QVariant(aPath));
}

QString ActionList::getIconPath(Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::IconPath);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}

void ActionList::setModified(Cmd::eCmd aCmd, bool aMod)
{
    setDataVariant(aCmd, Data::Modified, QVariant(aMod));
}

bool ActionList::isModified(Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::Modified);
    if (fVariant.isValid())
    {
        return fVariant.toBool();
    }
    return false;
}

void ActionList::setDataVariant(Cmd::eCmd aCmd, ActionList::Data aData, const QVariant& aVariant)
{
    QAction* fAction = getAction(aCmd);
    QVariantList fVariantList = fAction->data().toList();
    int fData = INT(aData);
    if (fData >= 0 && fData < INT(Data::ListSize))
    {
        fVariantList[fData] = aVariant;
    }
    fAction->setData(fVariantList);
}

QVariant ActionList::getDataVariant(Cmd::eCmd aCmd, Data aData)
{
    QAction* fAction = getAction(aCmd);
    QVariantList fVariantList = fAction->data().toList();
    int fData = INT(aData);
    if (fData >= 0 && fData < INT(Data::ListSize))
    {
        return fVariantList[fData];
    }
    return QVariant();
}
