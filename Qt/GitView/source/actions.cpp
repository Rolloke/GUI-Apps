#include "actions.h"
#include "helper.h"
#include "logger.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>

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

QAction * ActionList::createAction(Cmd::eCmd aCmd, const QString& aName, const QString& aGitCommand, QObject*aParent)
{
    if (aParent == nullptr)
    {
        aParent = mParent;
    }
    QAction *fNewAction = new QAction(aName, aParent);
    mActionList[aCmd] = fNewAction;
    fNewAction->setStatusTip(aGitCommand);
    fNewAction->setToolTip(aName);
    QVariantList fList;
    fList.append(QVariant(sNoCustomCommandMessageBox));
    for (int i = Data::Action; i<Data::ListSize; ++i)
    {
        fList.append(QVariant());
    }
    fList[ActionList::Data::Cmd] = QVariant(aCmd);
    fNewAction->setData(fList);
    return fNewAction;
}


void ActionList::deleteAction(git::Cmd::eCmd aCmd)
{
    if (mActionList.count(aCmd))
    {
        delete mActionList[aCmd];
        mActionList.erase(aCmd);
    }
}

git::Cmd::eCmd ActionList::getNextCustomID() const
{
    for (int fNewCmd = Cmd::CustomCommand; fNewCmd < Cmd::NonGitCommands; ++fNewCmd)
    {
        if (mActionList.count(fNewCmd)) continue;
        return static_cast<Cmd::eCmd>(fNewCmd);
    }
    return Cmd::Invalid;
}


void ActionList::initActionIcons()
{
    std::map<Cmd::eCmd, std::string> fActionIcons;
    fActionIcons[Cmd::Add]                     = ":/resource/24X24/list-add.png";
    fActionIcons[Cmd::Unstage]                 = ":/resource/24X24/list-remove.png";
    fActionIcons[Cmd::ShowDifference]          = ":/resource/24X24/object-flip-horizontal.png";
    fActionIcons[Cmd::CallDiffTool]            = ":/resource/24X24/distribute-graph-directed.svg";
    fActionIcons[Cmd::CallMergeTool]           = ":/resource/24X24/application-x-addon.png";
    fActionIcons[Cmd::MergeAbort]              = ":/resource/24X24/application-x-addon.png";
    fActionIcons[Cmd::MergeContinue]           = ":/resource/24X24/application-x-addon.png";
    fActionIcons[Cmd::MergeStrategyOurs]       = ":/resource/24X24/application-x-addon.png";
    fActionIcons[Cmd::MergeStrategyTheirs]     = ":/resource/24X24/application-x-addon.png";
    fActionIcons[Cmd::History]                 = ":/resource/24X24/document-open-recent.png";
    fActionIcons[Cmd::ShowStatus]              = ":/resource/24X24/help-faq.png";
    fActionIcons[Cmd::ShowShortStatus]         = ":/resource/24X24/dialog-question.png";
    fActionIcons[Cmd::Remove]                  = ":/resource/24X24/user-trash.png";
    fActionIcons[Cmd::Delete]                  = ":/resource/24X24/edit-delete.png";
    fActionIcons[Cmd::Commit]                  = ":/resource/24X24/user-invisible.png";
    fActionIcons[Cmd::MoveOrRename]            = ":/resource/24X24/format-text-direction-ltr.png";
    fActionIcons[Cmd::Restore]                 = ":/resource/24X24/edit-redo-rtl.png";
    fActionIcons[Cmd::Push]                    = ":/resource/24X24/view-sort-descending.png";
    fActionIcons[Cmd::Pull]                    = ":/resource/24X24/view-sort-ascending.png";
    fActionIcons[Cmd::BranchList]              = ":/resource/24X24/open-menu.png";
    fActionIcons[Cmd::BranchListRemote]        = ":/resource/24X24/open-menu.png";
    fActionIcons[Cmd::BranchListMerged]        = ":/resource/24X24/open-menu.png";
    fActionIcons[Cmd::BranchListNotMerged]     = ":/resource/24X24/open-menu.png";
    fActionIcons[Cmd::BranchDelete]            = ":/resource/24X24/edit-delete.png";
    fActionIcons[Cmd::BranchShow]              = ":/resource/24X24/preferences-system-privacy.png";
    fActionIcons[Cmd::BranchCheckout]          = ":/resource/24X24/emblem-default.png";
    fActionIcons[Cmd::BranchHistory]           = ":/resource/24X24/document-open-recent.png";
    fActionIcons[Cmd::Show]                    = ":/resource/24X24/edit-find.png";

    fActionIcons[Cmd::ExpandTreeItems]         = ":/resource/24X24/svn-update.svg";
    fActionIcons[Cmd::CollapseTreeItems]       = ":/resource/24X24/svn-commit.svg";
    fActionIcons[Cmd::AddGitSourceFolder]      = ":/resource/24X24/folder-open.png";
    fActionIcons[Cmd::UpdateGitStatus]         = ":/resource/24X24/view-refresh.png";
    fActionIcons[Cmd::ShowHideTree]            = ":/resource/24X24/code-class.svg";
    fActionIcons[Cmd::ClearTreeItems]          = ":/resource/24X24/edit-clear.png";
    fActionIcons[Cmd::CustomGitActionSettings] = ":/resource/24X24/preferences-system.png";
    fActionIcons[Cmd::InsertHashFileNames]     = ":/resource/24X24/object-rotate-right.png";
    fActionIcons[Cmd::About]                   = ":/resource/24X24/dialog-information.png";
    fActionIcons[Cmd::SelectTextBrowserLanguage]= ":/resource/24X24/text-x-adasrc.svg";

    for (const auto& fIconPath: fActionIcons )
    {
        setIconPath(static_cast<Cmd::eCmd>(fIconPath.first), fIconPath.second.c_str());
    }
}

void ActionList::enableItemsByType(const git::Cmd::tVector& aItems, const git::Type& aType) const
{
    for (const auto& fCmd : aItems)
    {
        if (fCmd != Cmd::Separator)
        {
            bool fEnabled = true;
            auto fStatusEnabled    = getFlags(fCmd, Data::StatusFlagEnable);
            auto fStatusDisabled   = getFlags(fCmd, Data::StatusFlagDisable);
            auto fStatusNotEnabled = fStatusEnabled & fStatusDisabled;
            if (   Logger::isSeverityActive(Logger::trace)
                && (fStatusEnabled  || fStatusDisabled || fStatusNotEnabled))
            {
                Logger::printDebug(Logger::trace, "%s: e:%d|%d, d:%d|%d, ne:%d|%d", getAction(fCmd)->text().toStdString().c_str(),
                                   INT(fStatusEnabled), aType.is(static_cast<Type::TypeFlags>(fStatusEnabled)),
                                   INT(fStatusDisabled), aType.is(static_cast<Type::TypeFlags>(fStatusDisabled)),
                                   INT(fStatusNotEnabled), aType.is(static_cast<Type::TypeFlags>(fStatusNotEnabled)));
            }
            if (aType.mType & Type::File)
            {
                if (fStatusNotEnabled)
                {
                    fEnabled = (aType.mType & fStatusNotEnabled) != 0;
                }
                else
                {
                    if (fStatusEnabled  && (aType.mType & fStatusEnabled) == 0)
                    {
                        fEnabled = false;
                    }
                    else if (fStatusDisabled && (aType.mType & fStatusDisabled) != 0)
                    {
                        fEnabled = false;
                    }
                }
            }
            auto action = getAction(fCmd);
            if (action)
            {
                action->setEnabled(fEnabled);
            }
        }
    }

}

void ActionList::fillToolbar(QToolBar& aToolbar, const Cmd::tVector& aItems) const
{
    for (const auto& fCmd : aItems)
    {
        if (fCmd == Cmd::Separator)
        {
            aToolbar.addSeparator();
        }
        else
        {
            auto * action = getAction(fCmd);
            if (action)
            {
                aToolbar.addAction(action);
            }
        }
    }
}

void ActionList::fillContextMenue(QMenu& aMenu, const Cmd::tVector& aItems) const
{
    for (const auto& fCmd : aItems)
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


QAction* ActionList::getAction(Cmd::eCmd aCmd) const
{
    auto fItem = mActionList.find(aCmd);
    if (fItem != mActionList.end())
    {
        return fItem->second;
    }
    return nullptr;
}

void  ActionList::setCustomCommandMessageBoxText(Cmd::eCmd aCmd, const QString& aText)
{
    QAction* fAction = getAction(aCmd);
    QVariantList fVariantList = fAction->data().toList();
    fVariantList[Data::MsgBoxText] = QVariant(aText);
    fAction->setData(fVariantList);
}

QString ActionList::getCustomCommandMessageBoxText(Cmd::eCmd aCmd) const
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

uint ActionList::getCustomCommandPostAction(Cmd::eCmd aCmd) const
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
    QAction* fAction = getAction(aCmd);
    if (fAction)
    {
        fAction->setIcon(QIcon(aPath));
    }
    setDataVariant(aCmd, Data::IconPath, QVariant(aPath));
}

QString ActionList::getIconPath(Cmd::eCmd aCmd) const
{
    QVariant fVariant = getDataVariant(aCmd, Data::IconPath);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}

void ActionList::setStagedCmdAddOn(git::Cmd::eCmd aCmd, const QString& aCmdAddOn)
{
    setDataVariant(aCmd, Data::StagedCmdAddOn, QVariant(aCmdAddOn));
}

QString ActionList::getStagedCmdAddOn(git::Cmd::eCmd aCmd) const
{
    QVariant fVariant = getDataVariant(aCmd, Data::IconPath);
    if (fVariant.isValid())
    {
        return fVariant.toString();
    }
    return "";
}

void ActionList::setFlags(Cmd::eCmd aCmd, uint aFlag, Flag aSet, Data::e aData)
{
    uint fFlags = getFlags(aCmd, aData);
    switch (aSet)
    {
        case Flag::remove:  fFlags &= ~aFlag; break;
        case Flag::set:     fFlags |=  aFlag; break;
        case Flag::replace: fFlags  =  aFlag; break;
    }

    setDataVariant(aCmd, aData, QVariant(fFlags));
}

uint ActionList::getFlags(Cmd::eCmd aCmd, Data::e aData) const
{
    QVariant fVariant = getDataVariant(aCmd, aData);
    if (fVariant.isValid() && fVariant.canConvert<uint>())
    {
        return fVariant.toUInt();
    }
    return 0;
}


void ActionList::setDataVariant(Cmd::eCmd aCmd, ActionList::Data::e aData, const QVariant& aVariant)
{
    QAction* fAction = getAction(aCmd);
    if (fAction)
    {
        QVariantList fVariantList = fAction->data().toList();
        if (aData >= 0 && aData < Data::ListSize)
        {
            fVariantList[aData] = aVariant;
        }
        fAction->setData(fVariantList);
    }
}

QVariant ActionList::getDataVariant(Cmd::eCmd aCmd, Data::e aData) const
{
    QAction* fAction = getAction(aCmd);
    if (fAction)
    {
        QVariantList fVariantList = fAction->data().toList();
        if (aData >= 0 && aData < Data::ListSize)
        {
            return fVariantList[aData];
        }
    }
    return QVariant();
}
