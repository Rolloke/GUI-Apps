#include "history.h"

namespace git
{

void History::parse(const QString& aText, QVector<QStringList>& aList)
{
    const QString fLineSeparator  = "<tr>";
    const QString fEntrySeparator = "<td>";

    QStringList fLines = aText.split(fLineSeparator);
    for (const auto& fLine: fLines)
    {
        aList.append(fLine.split(fEntrySeparator));
    }
}

const char* History::name(History::Entry aEntry)
{
    switch (aEntry)
    {
        case Entry::CommitHash:     return "Commit hash:";
        case Entry::TreeHash:       return "Tree hash:";
        case Entry::ParentHash:     return "Parent hash:";
        case Entry::SubjectAndBody: return "Description:";
        case Entry::Author:         return "Author:";
        case Entry::AuthoEmail:     return "Author Email:";
        case Entry::AuthorDate:     return "Author date:";
        case Entry::Committer:      return "Committer:";
        case Entry::CommitterEmail: return "Committer email:";
        case Entry::CommitterDate:  return "Committer date:";
        case Entry::NoOfEntries:    return "Number of entries";
        case Entry::NoOfFiles:      return "Number of files:";
        case Entry::Type:           return "Type";
        case Entry::GitDiffCommand: return "git diff command";
    }
    return "Unknown";
}

int History::role(History::Entry aEntry)
{
    return static_cast<int>(aEntry) + 1;
}

}
