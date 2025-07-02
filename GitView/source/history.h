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
//%s  Subject
//%B  Subject and Body            // 3
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
    struct Column { enum e
    {
        CommitDate,
        CommitGraph,
        Description,
        CommitHash,
        Author,
        Commit,
        Filename = CommitDate
    }; };

    struct Role { enum e
    {
        ContextMenuItem,
        VisibleAuthors
    }; };

    struct Entry { enum e
    {
        CommitHash,
        TreeHash,
        ParentHash,
        SubjectAndBody,
        Author,
        AuthoEmail,
        AuthorDate,
        Committer,
        CommitterEmail,
        CommitterDate,
        NoOfEntries,
        ContextMenuItem=NoOfEntries,
        /// NOTE: addition parameters
        NoOfFiles,      // number of files in commit
        Type,           // git type information
        GitDiffCommand, // the git diff command for this history
        /// NOTE: parameters for drawing a graph
        ListIndex = CommitHash,
        Parent1   = TreeHash,
        Parent2   = ParentHash,
        DrawItems = SubjectAndBody
    }; };

    struct Diff { enum e
    {
        two_commits = 2,
        to_next_commit = -1,
        to_current = -2,
        list_files_to_parent = -3
    }; };
    static int role(Entry::e aEntry);
    static const char* name(Entry::e aEntry);
    static void parse(const QString& aText, QVector<QStringList>& aList);

};


}

#endif // HISTORY_H
