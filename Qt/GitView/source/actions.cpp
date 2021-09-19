#include "actions.h"
#include "helper.h"
#include "logger.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>

using namespace git;

const QString ActionList::sNoCustomCommandMessageBox = "None";

ActionList::ActionList(QObject* parent): mParent(parent)
{

}

ActionList::~ActionList()
{
}

QAction * ActionList::createAction(Cmd::eCmd cmd, const QString& name, const QString& git_command, QObject* parent)
{
    if (parent == nullptr)
    {
        parent = mParent;
    }
    QAction *new_action = new QAction(name, parent);
    mActionList[cmd] = new_action;
    new_action->setStatusTip(git_command);
    new_action->setToolTip(name);
    QVariantList fList;
    fList.append(QVariant(sNoCustomCommandMessageBox));
    for (int i = Data::Action; i<Data::ListSize; ++i)
    {
        fList.append(QVariant());
    }
    fList[ActionList::Data::Cmd] = QVariant(cmd);
    new_action->setData(fList);
    return new_action;
}


void ActionList::deleteAction(git::Cmd::eCmd cmd)
{
    if (mActionList.count(cmd))
    {
        mActionList.erase(cmd);
    }
}

git::Cmd::eCmd ActionList::createNewID(git::Cmd::eCmd new_cmd) const
{
    for (; new_cmd < Cmd::NonGitCommands; ++new_cmd)
    {
        if (mActionList.count(new_cmd)) continue;
        return static_cast<Cmd::eCmd>(new_cmd);
    }
    return Cmd::Invalid;
}


void ActionList::initActionIcons()
{
    std::map<Cmd::eCmd, std::string> action_icons;
    action_icons[Cmd::Add]                     = ":/resource/24X24/list-add.png";
    action_icons[Cmd::Unstage]                 = ":/resource/24X24/list-remove.png";
    action_icons[Cmd::ShowDifference]          = ":/resource/24X24/object-flip-horizontal.png";
    action_icons[Cmd::CallDiffTool]            = ":/resource/24X24/distribute-graph-directed.svg";
    action_icons[Cmd::CallMergeTool]           = ":/resource/24X24/application-x-addon.png";
    action_icons[Cmd::InvokeGitMergeDialog]    = ":/resource/24X24/application-x-addon.png";
    action_icons[Cmd::History]                 = ":/resource/24X24/document-open-recent.png";
    action_icons[Cmd::ShowStatus]              = ":/resource/24X24/help-faq.png";
    action_icons[Cmd::ShowShortStatus]         = ":/resource/24X24/dialog-question.png";
    action_icons[Cmd::Remove]                  = ":/resource/24X24/user-trash.png";
    action_icons[Cmd::Delete]                  = ":/resource/24X24/edit-delete.png";
    action_icons[Cmd::Commit]                  = ":/resource/24X24/user-invisible.png";
    action_icons[Cmd::MoveOrRename]            = ":/resource/24X24/format-text-direction-ltr.png";
    action_icons[Cmd::Restore]                 = ":/resource/24X24/edit-redo-rtl.png";
    action_icons[Cmd::Push]                    = ":/resource/24X24/view-sort-descending.png";
    action_icons[Cmd::Pull]                    = ":/resource/24X24/view-sort-ascending.png";
    action_icons[Cmd::Fetch]                   = ":/resource/24X24/go-jump.png";
    action_icons[Cmd::Stash]                   = ":/resource/24X24/go-up.png";
    action_icons[Cmd::StashPop]                = ":/resource/24X24/go-down.png";
    action_icons[Cmd::StashShow]               = ":/resource/24X24/edit-find.png";
    action_icons[Cmd::StashClear]              = ":/resource/24X24/edit-delete.png";
    action_icons[Cmd::StashDrop]               = ":/resource/24X24/user-trash.png";
    action_icons[Cmd::StashList]               = ":/resource/24X24/text-x-log.png";

    action_icons[Cmd::BranchList]              = ":/resource/24X24/open-menu.png";
    action_icons[Cmd::BranchListRemote]        = ":/resource/24X24/open-menu.png";
    action_icons[Cmd::BranchListMerged]        = ":/resource/24X24/open-menu.png";
    action_icons[Cmd::BranchListNotMerged]     = ":/resource/24X24/open-menu.png";
    action_icons[Cmd::BranchDelete]            = ":/resource/24X24/edit-delete.png";
    action_icons[Cmd::BranchShow]              = ":/resource/24X24/preferences-system-privacy.png";
    action_icons[Cmd::BranchCheckout]          = ":/resource/24X24/emblem-default.png";
    action_icons[Cmd::BranchHistory]           = ":/resource/24X24/document-open-recent.png";
    action_icons[Cmd::Show]                    = ":/resource/24X24/edit-find.png";

    action_icons[Cmd::ExpandTreeItems]         = ":/resource/24X24/svn-update.svg";
    action_icons[Cmd::CollapseTreeItems]       = ":/resource/24X24/svn-commit.svg";
    action_icons[Cmd::AddGitSourceFolder]      = ":/resource/24X24/folder-open.png";
    action_icons[Cmd::UpdateGitStatus]         = ":/resource/24X24/view-refresh.png";
    action_icons[Cmd::ShowHideTree]            = ":/resource/24X24/code-class.svg";
    action_icons[Cmd::ClearTreeItems]          = ":/resource/24X24/edit-clear.png";
    action_icons[Cmd::CustomGitActionSettings] = ":/resource/24X24/preferences-system.png";
    action_icons[Cmd::InsertHashFileNames]     = ":/resource/24X24/object-rotate-right.png";
    action_icons[Cmd::About]                   = ":/resource/24X24/dialog-information.png";
    action_icons[Cmd::SelectTextBrowserLanguage]= ":/resource/24X24/text-x-adasrc.svg";
    action_icons[Cmd::InvokeHighlighterDialog] = ":/resource/24X24/emblem-system.png";
    action_icons[Cmd::KillBackgroundThread]    = ":/resource/24X24/media-record.png";
    action_icons[Cmd::CopyFileName]            = ":/resource/24X24/edit-copy.png";
    action_icons[Cmd::CopyFilePath]            = ":/resource/24X24/edit-copy.png";
    action_icons[Cmd::RemoveGitFolder]         = ":/resource/24X24/folder.png";


    for (const auto& icon_path: action_icons )
    {
        setIconPath(static_cast<Cmd::eCmd>(icon_path.first), icon_path.second.c_str());
    }
}

void ActionList::enableItemsByType(const git::Cmd::tVector& items, const git::Type& type) const
{
    for (const auto& cmd : items)
    {
        if (cmd != Cmd::Separator)
        {
            bool enabled = true;
            auto status_enabled     = getFlags(cmd, Data::StatusFlagEnable);
            auto status_disabled    = getFlags(cmd, Data::StatusFlagDisable);
            auto status_not_enabled = status_enabled & status_disabled;
            if (   Logger::isSeverityActive(Logger::trace)
                && (status_enabled  || status_disabled || status_not_enabled))
            {
                Logger::printDebug(Logger::trace, "%s: e:%d|%d, d:%d|%d, ne:%d|%d", getAction(cmd)->text().toStdString().c_str(),
                                   INT(status_enabled), type.is(static_cast<Type::TypeFlags>(status_enabled)),
                                   INT(status_disabled), type.is(static_cast<Type::TypeFlags>(status_disabled)),
                                   INT(status_not_enabled), type.is(static_cast<Type::TypeFlags>(status_not_enabled)));
            }
            if (type.type() & Type::File)
            {
                if (status_not_enabled)
                {
                    enabled = (type.type() & status_not_enabled) != 0;
                }
                else
                {
                    if (status_enabled  && (type.type() & status_enabled) == 0)
                    {
                        enabled = false;
                    }
                    else if (status_disabled && (type.type() & status_disabled) != 0)
                    {
                        enabled = false;
                    }
                }
            }
            auto action = getAction(cmd);
            if (action)
            {
                action->setEnabled(enabled);
            }
        }
    }

}

void ActionList::fillToolbar(QToolBar& tool_bar, const Cmd::tVector& items) const
{
    for (const auto& cmd : items)
    {
        if (cmd == Cmd::Separator)
        {
            tool_bar.addSeparator();
        }
        else
        {
            auto * action = getAction(cmd);
            if (action)
            {
                tool_bar.addAction(action);
            }
        }
    }
}

void ActionList::fillContextMenue(QMenu& menu, const Cmd::tVector& items) const
{
    for (const auto& cmd : items)
    {
        if (cmd == Cmd::Separator)
        {
            menu.addSeparator();
        }
        else
        {
            menu.addAction(getAction(cmd));
        }
    }
}


QAction* ActionList::getAction(Cmd::eCmd cmd) const
{
    auto item = mActionList.find(cmd);
    if (item != mActionList.end())
    {
        return item->second;
    }
    return nullptr;
}

void  ActionList::setCustomCommandMessageBoxText(Cmd::eCmd cmd, const QString& text)
{
    QAction* action = getAction(cmd);
    QVariantList variant_list = action->data().toList();
    variant_list[Data::MsgBoxText] = QVariant(text);
    action->setData(variant_list);
}

QString ActionList::getCustomCommandMessageBoxText(Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::MsgBoxText);
    if (variant.isValid())
    {
        return variant.toString();
    }
    return "";
}


void  ActionList::setCustomCommandPostAction(Cmd::eCmd cmd, uint action)
{
    setDataVariant(cmd, Data::Action, QVariant(action));
}

uint ActionList::getCustomCommandPostAction(Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::Action);
    if (variant.isValid())
    {
        return variant.toUInt();
    }
    return 0;
}

void ActionList::setIconPath(Cmd::eCmd cmd, const QString& path)
{
    QAction* action = getAction(cmd);
    if (action)
    {
        action->setIcon(QIcon(path));
    }
    setDataVariant(cmd, Data::IconPath, QVariant(path));
}

QString ActionList::getIconPath(Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::IconPath);
    if (variant.isValid())
    {
        return variant.toString();
    }
    return "";
}

void ActionList::setStagedCmdAddOn(git::Cmd::eCmd cmd, const QString& cmd_add_on)
{
    setDataVariant(cmd, Data::StagedCmdAddOn, QVariant(cmd_add_on));
}

QString ActionList::getStagedCmdAddOn(git::Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::IconPath);
    if (variant.isValid())
    {
        return variant.toString();
    }
    return "";
}

void ActionList::setFlags(Cmd::eCmd cmd, uint flag, Flag set, Data::e data)
{
    uint fFlags = getFlags(cmd, data);
    switch (set)
    {
        case Flag::remove:  fFlags &= ~flag; break;
        case Flag::set:     fFlags |=  flag; break;
        case Flag::replace: fFlags  =  flag; break;
    }

    setDataVariant(cmd, data, QVariant(fFlags));
}

uint ActionList::getFlags(Cmd::eCmd cmd, Data::e data) const
{
    QVariant fVariant = getDataVariant(cmd, data);
    if (fVariant.isValid() && fVariant.canConvert<uint>())
    {
        return fVariant.toUInt();
    }
    return 0;
}


void ActionList::setDataVariant(Cmd::eCmd cmd, ActionList::Data::e data, const QVariant& variant)
{
    QAction* fAction = getAction(cmd);
    if (fAction)
    {
        QVariantList fVariantList = fAction->data().toList();
        if (data >= 0 && data < Data::ListSize)
        {
            fVariantList[data] = variant;
        }
        fAction->setData(fVariantList);
    }
}

QVariant ActionList::getDataVariant(Cmd::eCmd cmd, Data::e data) const
{
    QAction* fAction = getAction(cmd);
    if (fAction)
    {
        QVariantList fVariantList = fAction->data().toList();
        if (data >= 0 && data < Data::ListSize)
        {
            return fVariantList[data];
        }
    }
    return QVariant();
}
