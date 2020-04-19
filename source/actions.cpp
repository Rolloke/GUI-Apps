#include "actions.h"

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

QAction * ActionList::createAction(git::Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand)
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
    std::map<git::Cmd::eCmd, std::string> fActionIcons;
    fActionIcons[git::Cmd::Add]                     = ":/resource/24X24/list-add.png";
    fActionIcons[git::Cmd::ShowDifference]          = ":/resource/24X24/object-flip-horizontal.png";
    fActionIcons[git::Cmd::CallDiffTool]            = ":/resource/24X24/distribute-graph-directed.svg";
    fActionIcons[git::Cmd::History]                 = ":/resource/24X24/document-open-recent.png";
    fActionIcons[git::Cmd::ShowStatus]              = ":/resource/24X24/help-faq.png";
    fActionIcons[git::Cmd::ShowShortStatus]         = ":/resource/24X24/dialog-question.png";
//    fActionIcons[git::Cmd::GetStatusAll]            = "";
    fActionIcons[git::Cmd::Remove]                  = "://resource/24X24/list-remove.png";
//    fActionIcons[git::Cmd::Commit]                  = "";
//    fActionIcons[git::Cmd::MoveOrRename]            = "";
    fActionIcons[git::Cmd::Restore]                 = "://resource/24X24/edit-redo-rtl.png";
//    fActionIcons[git::Cmd::Push]                    = "";
    fActionIcons[git::Cmd::ShowHistoryDifference]   = ":/resource/24X24/object-flip-horizontal.png";
    fActionIcons[git::Cmd::CallHistoryDiffTool]     = ":/resource/24X24/distribute-graph-directed.svg";

    fActionIcons[git::Cmd::ExpandTreeItems]         = "://resource/24X24/svn-update.svg";
    fActionIcons[git::Cmd::CollapseTreeItems]       = "://resource/24X24/svn-commit.svg";
    for (const auto& fIconPath: fActionIcons )
    {
        getAction(fIconPath.first)->setIcon(QIcon(fIconPath.second.c_str()));
        setIconPath(static_cast<git::Cmd::eCmd>(fIconPath.first), fIconPath.second.c_str());
    }
}

QAction* ActionList::getAction(git::Cmd::eCmd aCmd)
{
    auto fItem = mActionList.find(aCmd);
    if (fItem != mActionList.end())
    {
        return fItem->second;
    }
    return 0;
}

void  ActionList::setCustomCommandMessageBoxText(git::Cmd::eCmd aCmd, const QString& aText)
{
    QAction* fAction = getAction(aCmd);
    QVariantList fVariantList = fAction->data().toList();
    fVariantList[INT(Data::MsgBoxText)] = QVariant(aText);
    fAction->setData(fVariantList);
}

QString ActionList::getCustomCommandMessageBoxText(git::Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::MsgBoxText);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}


void  ActionList::setCustomCommandPostAction(git::Cmd::eCmd aCmd, uint aAction)
{
    setDataVariant(aCmd, Data::Action, QVariant(aAction));
}

uint ActionList::getCustomCommandPostAction(git::Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::Action);
    if (fVariant.isValid())
    {
        return fVariant.toUInt();
    }
    return 0;
}

void ActionList::setIconPath(git::Cmd::eCmd aCmd, const QString& aPath)
{
    setDataVariant(aCmd, Data::IconPath, QVariant(aPath));
}

QString ActionList::getIconPath(git::Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::IconPath);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}

void ActionList::setModified(git::Cmd::eCmd aCmd, bool aMod)
{
    setDataVariant(aCmd, Data::Modified, QVariant(aMod));
}

bool ActionList::isModified(git::Cmd::eCmd aCmd)
{
    QVariant fVariant = getDataVariant(aCmd, Data::Modified);
    if (fVariant.isValid())
    {
        return fVariant.toBool();
    }
    return false;
}

void ActionList::setDataVariant(git::Cmd::eCmd aCmd, ActionList::Data aData, const QVariant& aVariant)
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

QVariant ActionList::getDataVariant(git::Cmd::eCmd aCmd, Data aData)
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
