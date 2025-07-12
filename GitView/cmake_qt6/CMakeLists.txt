cmake_minimum_required(VERSION 3.16)

project(GitView, VERSION 1.0.0.1 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# if necessary set Qt6 path
set(Qt6_DIR D:/.conan_short/69d6df/1/lib/cmake/Qt6)

find_package(Qt6 REQUIRED )

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Xml)
find_package(Qt6 REQUIRED COMPONENTS Svg)
find_package(Qt6 REQUIRED COMPONENTS SvgWidgets)

# if available uncomment this, reguard also target_link_libraries section
find_package(Qt6 REQUIRED COMPONENTS BuildInternals)
find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
qt_internal_project_setup()
add_compile_definitions(CORE5COMPAT=1)

# if avaliable uncomment this
#find_package(Qt6 REQUIRED COMPONENTS webenginewidgets)
#find_package(Qt6 REQUIRED COMPONENTS webchannel)
#add_compile_definitions(WEB_ENGINE=1)

# if needed use language support
#qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES de en)

# you may use boost, but it is not necessary
# add_compile_definitions(-DUSE_BOOST=1)

set(HEADER 
    ../source/aboutdlg.h
    ../source/actions.h
    ../source/binary_values_view.h
    ../source/code_browser.h
    ../source/commit_graphics_item.h
    ../source/commitmessage.h
    ../source/customgitactions.h
    ../source/DisplayType.h
    ../source/editable.h
    ../source/gitignore.h
    ../source/git_type.h
    ../source/graphics_view.h
    ../source/helper.h
    ../source/highlighterdialog.h
    ../source/highlighter.h
    ../source/history.h
    ../source/logger.h
    ../source/mainwindow.h
    ../source/mergedialog.h
    ../source/palettecolorselector.h
    ../source/qbinarytableview.h
    ../source/qbranchtreewidget.h
    ../source/qdockwidgetx.h
    ../source/qhistorytreewidget.h
    ../source/qradiobuttongroup.h
    ../source/qsourcetreewidget.h
    ../source/qstashtreewidget.h
    ../source/workerthreadconnector.h
    ../source/xml_functions.h
   )

set(SOURCE 
    ../source/aboutdlg.cpp
    ../source/actions.cpp
    ../source/binary_values_view.cpp
    ../source/code_browser.cpp
    ../source/commit_graphics_item.cpp
    ../source/commitmessage.cpp
    ../source/customgitactions.cpp
    ../source/DisplayType.cpp
    ../source/editable.cpp
    ../source/gitignore.cpp
    ../source/git_type.cpp
    ../source/graphics_view.cpp
    ../source/helper.cpp
    ../source/highlighter.cpp
    ../source/highlighterdialog.cpp
    ../source/history.cpp
    ../source/logger.cpp
    ../source/main.cpp
    ../source/mainwindow_branchtree.cpp
    ../source/mainwindow.cpp
    ../source/mainwindow_historytree.cpp
    ../source/mainwindow_sourcetree.cpp
    ../source/mainwindow_stashtree.cpp
    ../source/mainwindow_textview.cpp
    ../source/mergedialog.cpp
    ../source/palettecolorselector.cpp
    ../source/qbinarytableview.cpp
    ../source/qbranchtreewidget.cpp
    ../source/qdockwidgetx.cpp
    ../source/qhistorytreewidget.cpp
    ../source/qradiobuttongroup.cpp
    ../source/qsourcetreewidget.cpp
    ../source/qstashtreewidget.cpp
    ../source/workerthreadconnector.cpp
    ../source/xml_functions.cpp
   )

set(UIFILES
    ../source/resource.qrc
    ../source/aboutdlg.ui
    ../source/binary_values_view.ui
    ../source/commitmessage.ui
    ../source/customgitactions.ui
    ../source/highlighterdialog.ui
    ../source/mainwindow.ui
    ../source/mergedialog.ui
    ../source/palettecolorselector.ui
   )

qt_add_resources(GitView RESOURCE
    PREFIX ":/"
    FILES 
    ../source/resource/default.md
    ../source/resource/index.html
    ../source/resource/logo@2x.ico
    ../source/resource/logo@2x.png
    ../source/resource/resource.rc
    ../source/resource/3rdparty/markdown.css
    ../source/resource/3rdparty/MARKDOWN-LICENSE.txt
    ../source/resource/3rdparty/marked.js
    ../source/resource/3rdparty/MARKED-LICENSE.txt
    ../source/resource/3rdparty/qt_attribution.json
    ../source/resource/24X24/accessories-dictionary.png
    ../source/resource/24X24/applications-colors.png
    ../source/resource/24X24/applications-development.png
    ../source/resource/24X24/applications-engineering.png
    ../source/resource/24X24/applications-graphics.png
    ../source/resource/24X24/applications-office.png
    ../source/resource/24X24/applications-other.png
    ../source/resource/24X24/applications-science.png
    ../source/resource/24X24/applications-system.png
    ../source/resource/24X24/applications-utilities.png
    ../source/resource/24X24/application-x-addon.png
    ../source/resource/24X24/appointment-soon.png
    ../source/resource/24X24/code-class.svg
    ../source/resource/24X24/dialog-error.png
    ../source/resource/24X24/dialog-information.png
    ../source/resource/24X24/dialog-question.png
    ../source/resource/24X24/dialog-warning.png
    ../source/resource/24X24/distribute-graph-directed.svg
    ../source/resource/24X24/document-new.png
    ../source/resource/24X24/document-open-recent.png
    ../source/resource/24X24/document-revert.png
    ../source/resource/24X24/document-save.png
    ../source/resource/24X24/edit-clear.png
    ../source/resource/24X24/edit-copy.png
    ../source/resource/24X24/edit-cut.png
    ../source/resource/24X24/edit-delete.png
    ../source/resource/24X24/edit-find.png
    ../source/resource/24X24/edit-find-replace.png
    ../source/resource/24X24/edit-redo.png
    ../source/resource/24X24/edit-redo-rtl.png
    ../source/resource/24X24/edit-undo.png
    ../source/resource/24X24/edit-undo-rtl.png
    ../source/resource/24X24/emblem-default.png
    ../source/resource/24X24/emblem-favorite.png
    ../source/resource/24X24/emblem-generic.png
    ../source/resource/24X24/emblem-important.png
    ../source/resource/24X24/emblem-new.png
    ../source/resource/24X24/emblem-package.png
    ../source/resource/24X24/emblem-photos.png
    ../source/resource/24X24/emblem-shared.png
    ../source/resource/24X24/emblem-system.png
    ../source/resource/24X24/emblem-urgent.png
    ../source/resource/24X24/folder-open.png
    ../source/resource/24X24/folder.png
    ../source/resource/24X24/format-text-bold.png
    ../source/resource/24X24/format-text-direction-ltr.png
    ../source/resource/24X24/format-text-direction-rtl.png
    ../source/resource/24X24/gnome-app-install-star.png
    ../source/resource/24X24/gnome-mime-text.png
    ../source/resource/24X24/go-bottom.png
    ../source/resource/24X24/go-down.png
    ../source/resource/24X24/go-home.png
    ../source/resource/24X24/go-jump.png
    ../source/resource/24X24/go-last.png
    ../source/resource/24X24/go-last-rtl.png
    ../source/resource/24X24/go-next.png
    ../source/resource/24X24/go-previous.png
    ../source/resource/24X24/go-top.png
    ../source/resource/24X24/go-up.png
    ../source/resource/24X24/help-about.png
    ../source/resource/24X24/help-contents.png
    ../source/resource/24X24/help-faq.png
    ../source/resource/24X24/icons8-clone-30.png
    ../source/resource/24X24/image-svg+xml.png
    ../source/resource/24X24/input-tablet.png
    ../source/resource/24X24/junction.svg
    ../source/resource/24X24/list-add.png
    ../source/resource/24X24/list-remove.png
    ../source/resource/24X24/mail-attachment.png
    ../source/resource/24X24/mail-signed-verified.png
    ../source/resource/24X24/mate.png
    ../source/resource/24X24/media-playlist-shuffle.png
    ../source/resource/24X24/media-record.png
    ../source/resource/24X24/model.png
    ../source/resource/24X24/network-wireless.png
    ../source/resource/24X24/non-starred.png
    ../source/resource/24X24/object-flip-horizontal.png
    ../source/resource/24X24/object-flip-vertical.png
    ../source/resource/24X24/object-rotate-left.png
    ../source/resource/24X24/object-rotate-right.png
    ../source/resource/24X24/open-menu.png
    ../source/resource/24X24/preferences-desktop-accessibility.png
    ../source/resource/24X24/preferences-desktop-locale.png
    ../source/resource/24X24/preferences-system.png
    ../source/resource/24X24/preferences-system-privacy.png
    ../source/resource/24X24/stylesheet-branch-closed.png
    ../source/resource/24X24/stylesheet-branch-end.png
    ../source/resource/24X24/stylesheet-branch-more.png
    ../source/resource/24X24/stylesheet-branch-open.png
    ../source/resource/24X24/stylesheet-vline.png
    ../source/resource/24X24/svn-commit.svg
    ../source/resource/24X24/svn-update.svg
    ../source/resource/24X24/system-help.png
    ../source/resource/24X24/text-x-adasrc.svg
    ../source/resource/24X24/text-x-log.png
    ../source/resource/24X24/text-x-patch.png
    ../source/resource/24X24/trophy-bronze.png
    ../source/resource/24X24/user-available.png
    ../source/resource/24X24/user-away.png
    ../source/resource/24X24/user-busy.png
    ../source/resource/24X24/user-idle.png
    ../source/resource/24X24/user-invisible.png
    ../source/resource/24X24/user-trash.png
    ../source/resource/24X24/view-fullscreen.png
    ../source/resource/24X24/view-refresh.png
    ../source/resource/24X24/view-sort-ascending.png
    ../source/resource/24X24/view-sort-descending.png
    ../source/resource/24X24/window-close.png
    ../source/resource/24X24/x-office-document.png
)

qt_add_executable(GitView
    ${HEADER}
    ${SOURCE}
    ${UIFILES} 
)

target_include_directories(GitView PUBLIC
  ../source
)

target_link_libraries(GitView PRIVATE Qt6::Core)
target_link_libraries(GitView PRIVATE Qt6::Widgets)
target_link_libraries(GitView PRIVATE Qt6::Gui)
target_link_libraries(GitView PRIVATE Qt6::Xml)
target_link_libraries(GitView PRIVATE Qt6::Svg)
target_link_libraries(GitView PRIVATE Qt6::SvgWidgets)
# if available uncomment this
target_link_libraries(GitView PRIVATE Qt6::Core5Compat)
