#ifndef HISTORY_H
#define HISTORY_H

#include <QString>
#include <QVector>


namespace git
{

//%H  Commit hash                 // 0
//%h  Abbreviated commit hash
//%T  Tree hash                   // 1
//%t  Abbreviated tree hash
//%P  Parent hashes               // 2
//%p  Abbreviated parent hashes
//%s  Subject                     // 3
//%an Author name                 // 4
//%ae Author email                // 5
//%ad Author date                 // 6
//    (format respects the --date=option)
//%ar Author date, relative
//%cn Committer name              // 7
//%ce Committer email             // 8
//%cd Committer date              // 9
//%cr Committer date, relative

struct History
{
    enum class Entry
    {
        CommitHash,
        TreeHash,
        ParentHash,
        Subject,
        Author,
        AuthoEmail,
        AuthorDate,
        Committer,
        CommitterEmail,
        CommitterDate,
        NoOfEntries
    };
    static const char* name(Entry aEntry);
    static void parse(const QString& aText, QVector<QStringList>& aList);

};


}

#endif // HISTORY_H
