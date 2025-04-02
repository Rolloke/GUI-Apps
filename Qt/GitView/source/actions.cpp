#include "actions.h"
#include "helper.h"
#include "logger.h"

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QFileInfo>
#include <QDirIterator>

using namespace git;

const QString ActionList::sNoCustomCommandMessageBox = "None";

ActionList::ActionList(QObject* parent) : QObject(parent), mParent(parent)
{

}

ActionList::~ActionList()
{
}

QAction * ActionList::createAction(Cmd::eCmd cmd, const QString& name, const QString& git_command, QObject* parent)
{
    if (!parent)
    {
        parent = mParent;
    }
    QAction *new_action = new QAction(name, parent);
    mActionList[cmd] = new_action;
    new_action->setStatusTip(git_command);
    new_action->setToolTip(name);
    initAction(cmd, new_action);
    QWidget* parent_widget = dynamic_cast<QWidget*>(parent);
    if (parent_widget)
    {
        parent_widget->addAction(new_action);
    }
    return new_action;
}

void ActionList::initAction(git::Cmd::eCmd cmd, QAction* action)
{
    QVariantList fList;
    fList.append(QVariant(sNoCustomCommandMessageBox));
    for (int i = Data::PostCmdAction; i<Data::ListSize; ++i)
    {
        fList.append(QVariant());
    }
    fList[ActionList::Data::Cmd] = QVariant(cmd);
    action->setData(fList);
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
    for ( ; ; ++new_cmd)
    {
        if (mActionList.count(new_cmd)) continue;
        return static_cast<Cmd::eCmd>(new_cmd);
    }
    return Cmd::Invalid;
}

git::Cmd::eCmd ActionList::findID(const QAction* action) const
{
    auto found = std::find_if(mActionList.begin(), mActionList.end(), [action](auto& item) { return item.second.data() == action;});
    if (found != mActionList.end())
    {
        return found->first;
    }
    return git::Cmd::Invalid;
}

void ActionList::setTheme(const QString& theme)
{
    m_theme = theme;
}

const QString& ActionList::getTheme()
{
    return m_theme;
}

void ActionList::setIconLocation(const QString &icons)
{
    m_icon_location = icons;
}

const QString &ActionList::getIconLocation()
{
    return m_icon_location;
}

void ActionList::initActionIcons()
{
#ifdef __linux__
/// NOTE: availeable themes
/// - Adwaita/24x24
/// - hicolor/24x24
/// - gnome/24x24
/// - Tango/24x24
/// - menta/24x24
/// - mate/24x24
/// Command to determine theme returns e.g. 'Mint-Y'
/// gsettings get org.gnome.desktop.interface gtk-theme
    if (!m_theme.isEmpty())
    {
        m_icon_location = "/usr/share/icons/" + m_theme;
    }
    QFileInfo info(m_icon_location);
    if (info.exists())
    {
        TRACEX(Logger::info, "using theme icons " << m_icon_location);
    }
    else
    {
        m_icon_location.clear();
    }
#endif

    std::map<Cmd::eCmd, QString> action_icons;
    action_icons[Cmd::Add]                     = "list-add.png";
    action_icons[Cmd::Unstage]                 = "list-remove.png";
    action_icons[Cmd::ShowDifference]          = "object-flip-horizontal.png";
    action_icons[Cmd::CallDiffTool]            = "distribute-graph-directed.svg";
    action_icons[Cmd::CallMergeTool]           = "application-x-addon.png";
    action_icons[Cmd::InvokeGitMergeDialog]    = "application-x-addon.png";
    action_icons[Cmd::History]                 = "document-open-recent.png";
    action_icons[Cmd::ShowStatus]              = "help-faq.png";
    action_icons[Cmd::ShowShortStatus]         = "dialog-question.png";
    action_icons[Cmd::Remove]                  = "user-trash.png";
    action_icons[Cmd::Delete]                  = "edit-delete.png";
    action_icons[Cmd::Commit]                  = "user-invisible.png";
    action_icons[Cmd::MoveOrRename]            = "format-text-direction-ltr.png";
    action_icons[Cmd::Restore]                 = "edit-redo-rtl.png";
    action_icons[Cmd::Push]                    = "view-sort-descending.png";
    action_icons[Cmd::Pull]                    = "view-sort-ascending.png";
    action_icons[Cmd::Fetch]                   = "go-jump.png";
    action_icons[Cmd::Stash]                   = "go-up.png";
    action_icons[Cmd::StashPop]                = "go-down.png";
    action_icons[Cmd::StashPush]               = "go-previous.png";
    action_icons[Cmd::StashApply]              = "go-next.png";
    action_icons[Cmd::StashShow]               = "edit-find.png";
    action_icons[Cmd::StashClear]              = "edit-delete.png";
    action_icons[Cmd::StashDrop]               = "user-trash.png";
    action_icons[Cmd::StashList]               = "text-x-log.png";
    action_icons[Cmd::Blame]                   = "emblem-urgent.png";
    action_icons[Cmd::Clone]                   = "icons8-clone-30.png";

    action_icons[Cmd::BranchList]              = "open-menu.png";
    action_icons[Cmd::BranchListRemote]        = "open-menu.png";
    action_icons[Cmd::BranchListMerged]        = "open-menu.png";
    action_icons[Cmd::BranchListNotMerged]     = "open-menu.png";
    action_icons[Cmd::BranchDelete]            = "edit-delete.png";
    action_icons[Cmd::BranchShow]              = "preferences-system-privacy.png";
    action_icons[Cmd::BranchCheckout]          = "emblem-default.png";
    action_icons[Cmd::DiffOfTwoBranches]       = "document-open-recent.png";
    action_icons[Cmd::Show]                    = "edit-find.png";

    action_icons[Cmd::ExpandTreeItems]         = "svn-update.svg";
    action_icons[Cmd::CollapseTreeItems]       = "svn-commit.svg";
    action_icons[Cmd::AddGitSourceFolder]      = "folder-open.png";
    action_icons[Cmd::UpdateGitStatus]         = "view-refresh.png";
    action_icons[Cmd::ClearTreeItems]          = "edit-clear.png";
    action_icons[Cmd::CustomGitActionSettings] = "preferences-system.png";
    action_icons[Cmd::InsertHashFileNames]     = "object-rotate-right.png";
    action_icons[Cmd::About]                   = "dialog-information.png";
    action_icons[Cmd::SelectTextBrowserLanguage]= "text-x-adasrc.svg";
    action_icons[Cmd::InvokeHighlighterDialog] = "emblem-system.png";
    action_icons[Cmd::KillBackgroundThread]    = "media-record.png";
    action_icons[Cmd::CopyFileName]            = "edit-copy.png";
    action_icons[Cmd::CopyFilePath]            = "edit-copy.png";
    action_icons[Cmd::RemoveGitFolder]         = "folder.png";
    action_icons[Cmd::ZoomIn]                  = "list-add.png";
    action_icons[Cmd::ZoomOut]                 = "list-remove.png";
    action_icons[Cmd::FitInView]               = "view-fullscreen.png";
    action_icons[Cmd::DeleteTreeItems]         = "edit-cut.png";
    action_icons[Cmd::AddExternalFileOpenExt]  = "gnome-mime-text.png";
    action_icons[Cmd::OpenFileExternally]      = "x-office-document.png";
    action_icons[Cmd::DeleteExternalFileOpenExt] = "window-close.png";
    action_icons[Cmd::CloneTextBrowser]        = "emblem-shared.png";
    action_icons[Cmd::CreateBookMark]          = "non-starred.png";
    action_icons[Cmd::ShowInformation]         = "dialog-information.png";
    action_icons[Cmd::WhatsThisHelp]           = "dialog-question.png";
    action_icons[Cmd::CompareTo]               = "object-flip-horizontal.png";
    action_icons[Cmd::CloseAll]                = "window-close.png";
    action_icons[Cmd::SaveAll]                 = "document-save.png";
    action_icons[Cmd::CustomTestCommand]       = "window-close.png";
    action_icons[Cmd::OpenFile]                = "gnome-mime-text.png";
    action_icons[Cmd::SaveAs]                  = "document-save.png";
    action_icons[Cmd::ReplaceAll]              = "edit-find-replace.png";
    action_icons[Cmd::EditToUpper]             = "window-close.png";
    action_icons[Cmd::EditToLower]             = "window-close.png";
    action_icons[Cmd::EditToggleComment]       = "window-close.png";
    action_icons[Cmd::EditToSnakeCase]         = "window-close.png";
    action_icons[Cmd::EditToCamelCase]         = "window-close.png";
    action_icons[Cmd::EditTabIndent]           = "window-close.png";
    action_icons[Cmd::EditTabOutdent]          = "window-close.png";

    for (auto& icon_path: action_icons )
    {
        setIconPath(static_cast<Cmd::eCmd>(icon_path.first), check_location(icon_path.second));
    }
}

void ActionList::enableItemsByType(const git::Cmd::tVector& items, const git::Type& type) const
{
    for (const auto& cmd : std::as_const(items))
    {
        if (cmd != Cmd::Separator)
        {
            bool enabled = true;
            auto status_enabled     = getFlags(cmd, Data::StatusFlagEnable);
            auto status_disabled    = getFlags(cmd, Data::StatusFlagDisable);
            auto status_not_enabled = status_enabled & status_disabled;
            if (status_enabled & Type::IgnoreTypeStatus)
            {
                continue;
            }
            if (   Logger::isSeverityActive(Logger::trace)
                && (status_enabled  || status_disabled || status_not_enabled))
            {
                Logger::printDebug(Logger::trace, "%s: e:%d|%d, d:%d|%d, ne:%d|%d", getAction(cmd)->text().toStdString().c_str(),
                                   INT(status_enabled), type.is(Type::type(status_enabled)),
                                   INT(status_disabled), type.is(Type::type(status_disabled)),
                                   INT(status_not_enabled), type.is(Type::type(status_not_enabled)));
            }
            if (type.is(Type::File))
            {
                status_not_enabled &= ~Type::Folder;
                if (status_not_enabled)
                {
                    enabled = (type.type() & status_not_enabled) != 0;
                }
                else
                {
                    status_enabled  &= ~Type::Folder;
                    status_disabled &= ~Type::Folder;
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
            else if (type.is(Type::Folder))
            {
                if (status_not_enabled & (Type::Folder|Type::Repository))
                {
                    enabled = true;
                }
                else
                {
                    status_enabled  &= ~Type::AllGitActions;
                    status_disabled &= ~Type::AllGitActions;
                    status_disabled &= ~Type::Folder;
                    if (status_enabled && (type.type() & status_enabled) == 0)
                    {
                        enabled = false;
                    }
                    else if (status_disabled && (type.type() & status_enabled) != 0)
                    {
                        enabled = false;
                    }
                }
            }
            auto* action = getAction(cmd);
            if (action)
            {
                action->setEnabled(enabled);
            }
        }
    }

}

void ActionList::fillToolbar(QToolBar& tool_bar, const Cmd::tVector& items)
{
    for (const auto& cmd : std::as_const(items))
    {
        if (cmd == Cmd::Separator)
        {
            tool_bar.addSeparator();
        }
        else
        {
            auto* action = getAction(cmd);
            if (action)
            {
                auto menu_list = getMenuStringList(cmd);
                if (menu_list.count())
                {
                    QToolButton* tool_button = new QToolButton();
                    mToolButtonList[cmd] = tool_button;
                    tool_button->setPopupMode(QToolButton::MenuButtonPopup);
                    connect(tool_button, SIGNAL(triggered(QAction*)), this, SLOT(select_action(QAction*)));
                    tool_button->setDefaultAction(action);
                    tool_button->setToolTip(action->toolTip());
                    connect(&tool_bar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)), tool_button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
                    tool_button->setToolButtonStyle(tool_bar.toolButtonStyle());
                    tool_bar.addWidget(tool_button);

                    QMenu* sub_menu = new QMenu;
                    mMenuList[cmd] = sub_menu;
                    tool_button->setMenu(sub_menu);
                    QString option;
                    for (int index=0; index<menu_list.size(); ++index)
                    {
                        QString &item = menu_list[index];
                        if (item.indexOf("--") == 0)
                        {
                            option = item; // the git option
                            QAction *sub_action = sub_menu->addAction(item.right(item.size()-2));
                            sub_action->setEnabled(false);
                            continue;
                        }
                        bool selected = item.indexOf("*") == 0;
                        if (selected) item.remove(0, 1);
                        QAction *sub_action = sub_menu->addAction(item);
                        sub_action->setCheckable(true);
                        sub_action->setChecked(selected);
                        sub_action->setStatusTip(option + "=" + item);
                    }
                }
                else
                {
                    tool_bar.addAction(action);
                }
            }
        }
    }
}

void ActionList::select_action(QAction* action)
{
    const auto parent = action->parent();
    const auto found = std::find_if(mMenuList.begin(), mMenuList.end(), [parent] (const auto& item) { return parent == item.second; });
    if (found != mMenuList.end())
    {
        const auto cmd = found->first;
        const bool action_is_checked = action->isChecked();
        const QMenu* menu = found->second;
        auto list = menu->actions();
        for (auto& item : list)
        {
            item->setChecked(item == action && action_is_checked);
        }

        QStringList strings = getMenuStringList(cmd);
        QString action_text = action->text();
        for (QString& string : strings)
        {
            if (string.indexOf("*") == 0) string.remove(0, 1);
            if (action_is_checked && string == action_text)
            {
                string = "*" + string;
            }
        }
        setFlags(cmd, Flags::MenuOption, action_is_checked ? Flag::set : Flag::remove);
        setMenuStringList(cmd, strings);
    }
}

void ActionList::fillContextMenue(QMenu& menu, const Cmd::tVector& items, QWidget* widget) const
{
    QMenu* sub_menu = nullptr;
    for (const auto& cmd : std::as_const(items))
    {
        if (cmd == Cmd::Separator)
        {
            if (sub_menu)
            {
                sub_menu = nullptr;
            }
            else
            {
                menu.addSeparator();
            }
        }
        else if (cmd >= Cmd::Submenu)
        {
            auto* action = getAction(cmd);
            if (action) sub_menu = menu.addMenu(action->text());
        }
        else
        {
            auto* action = getAction(cmd);
            if (action)
            {
                if (sub_menu) sub_menu->addAction(action);
                else menu.addAction(action);
                if (widget) widget->addAction(action);
            }
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
    TRACEX(static_cast<Logger::eSeverity>(Logger::warning|Logger::to_browser), "no action for command id " << cmd);
    return nullptr;
}

bool ActionList::hasAction(git::Cmd::eCmd aCmd) const
{
    return mActionList.count(aCmd) != 0;
}

void  ActionList::setCustomCommandMessageBoxText(Cmd::eCmd cmd, const QString& text)
{
    QAction* action = getAction(cmd);
    if (action)
    {
        QVariantList variant_list = action->data().toList();
        variant_list[Data::MsgBoxText] = QVariant(text);
        action->setData(variant_list);
    }
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
    setDataVariant(cmd, Data::PostCmdAction, QVariant(action));
}

uint ActionList::getCustomCommandPostAction(Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::PostCmdAction);
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

void ActionList::setCmdAddOn(git::Cmd::eCmd cmd, const QString& cmd_add_on)
{
    setDataVariant(cmd, Data::CmdAddOn, QVariant(cmd_add_on));
}

QString ActionList::getCmdAddOn(git::Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::CmdAddOn);
    if (variant.isValid())
    {
        return variant.toString();
    }
    return "";
}

void ActionList::setMenuStringList(git::Cmd::eCmd cmd, const QStringList& list)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto unknown = QVariant();
    setDataVariant(cmd, Data::MenuStrings, list.size() ? list : unknown);
#else
    setDataVariant(cmd, Data::MenuStrings, list.size() ? list : QVariant(QVariant::Invalid));
#endif
}

QStringList ActionList::getMenuStringList(git::Cmd::eCmd cmd) const
{
    QVariant variant = getDataVariant(cmd, Data::MenuStrings);
    if (variant.isValid())
    {
        return variant.toStringList();
    }
    return {};
}


void ActionList::setFlags(Cmd::eCmd cmd, uint flag, Flag set, Data::e data)
{
    if (Data::is_flag(data))
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
    else
    {
        TRACEX(Logger::to_browser, "ActionList::setFlags "<< Data::name(data) << " data is not a flag");
    }
}

uint ActionList::getFlags(Cmd::eCmd cmd, Data::e data) const
{
    if (Data::is_flag(data))
    {
        QVariant fVariant = getDataVariant(cmd, data);
        if (fVariant.isValid() && fVariant.canConvert<uint>())
        {
            return fVariant.toUInt();
        }
    }
    else
    {
        TRACEX(Logger::to_browser, "ActionList::getFlags " << Data::name(data) << " data is not a flag");
    }
    return 0;
}


void ActionList::setDataVariant(Cmd::eCmd cmd, ActionList::Data::e data, const QVariant& variant)
{
    QAction* action = getAction(cmd);
    if (action)
    {
        QVariantList variant_list = action->data().toList();
        if (is_in_range(0, Data::ListSize-1, data))
        {
            variant_list[data] = variant;
        }
        action->setData(variant_list);
    }
}

QVariant ActionList::getDataVariant(Cmd::eCmd cmd, Data::e data) const
{
    QAction* action = getAction(cmd);
    if (action)
    {
        QVariantList variant_list = action->data().toList();
        if (is_in_range(0, Data::ListSize-1, data))
        {
            return variant_list[data];
        }
    }
    return QVariant();
}

QString find_icon(const QString& path, const QString& file)
{
    QDirIterator iterator(QDir(path), QDirIterator::NoIteratorFlags);
    do
    {
        iterator.next();
        const QFileInfo& file_info = iterator.fileInfo();
        auto name = file_info.fileName();
        if (file_info.isDir() && !is_any_equal_to(name, Folder::FolderSelf, Folder::FolderUp))
        {
            QString found = find_icon(path + "/" + name, file);
            if (!found.isEmpty())
            {
                return found;
            }
        }
        else if (   file_info.isFile())
        {
            auto base_name = file_info.baseName();
            auto suffix = file_info.suffix();
            if (    file.contains(base_name, Qt::CaseInsensitive)
                 && (suffix.contains("svg",  Qt::CaseInsensitive) || suffix.contains("png",  Qt::CaseInsensitive)))
            {
                return file_info.filePath();
            }
        }
    } while (iterator.hasNext());
    return {};
}

QString ActionList::check_location(const QString &file_location)
{
    if (m_icon_location.isEmpty())
    {
        return resource + file_location;
    }
    else
    {
        auto found_icon = find_icon(m_icon_location, file_location);
        if (found_icon.isEmpty())
        {
            TRACEX(Logger::info, file_location << " not found in theme icons");
            return resource + file_location;
        }
        return found_icon;
    }
}

#define RETURN_NAME(NAME) case NAME: return #NAME

const char* ActionList::Data::name(e eData)
{
    switch (eData)
    {
    RETURN_NAME(MsgBoxText);
    RETURN_NAME(PostCmdAction);
    RETURN_NAME(IconPath);
    RETURN_NAME(Flags);
    RETURN_NAME(CmdAddOn);
    RETURN_NAME(Cmd);
    RETURN_NAME(StatusFlagEnable);
    RETURN_NAME(StatusFlagDisable);
    RETURN_NAME(MenuStrings);
    RETURN_NAME(ListSize);
    }
    return txt::invalid;
}

bool ActionList::Data::is_flag(e eData)
{
    switch (eData)
    {
    case Flags: case StatusFlagEnable: case StatusFlagDisable: return true;
    default: return false;
    }
}

const char* ActionList::Flags::name(e eFlag)
{
    switch (eFlag)
    {
    RETURN_NAME(BuiltIn);
    RETURN_NAME(Modified);
    RETURN_NAME(Custom);
    RETURN_NAME(Branch);
    RETURN_NAME(History);
    RETURN_NAME(DiffOrMergeTool);
    RETURN_NAME(CallInThread);
    RETURN_NAME(NotVariableGitCmd);
    RETURN_NAME(FunctionCmd);
    RETURN_NAME(Stash);
    RETURN_NAME(DiffCmd);
    RETURN_NAME(DependsOnStaged);
    RETURN_NAME(StashCmdOption);
    RETURN_NAME(MenuOption);
    RETURN_NAME(Asynchroneous);
    RETURN_NAME(NoHide);
    }
    return txt::invalid;
}
