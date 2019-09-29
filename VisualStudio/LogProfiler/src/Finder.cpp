#include "Finder.h"

using namespace std;
#define not !
FoundInfo Finder::EmptyFoundInfo(0, 0, 0, TEXT(""));
SearchResultMarking Finder::EmptySearchResultMarking;

bool Finder::notify(SCNotification *notification)
{
	static bool isDoubleClicked = false;

	switch (notification->nmhdr.code)
	{
		case SCN_MARGINCLICK:
			if (notification->margin == ScintillaEditView::_SC_MARGE_FOLDER)
			{
				_scintView.marginClick(notification->position, notification->modifiers);
			}
			break;

		case SCN_DOUBLECLICK:
		{
			// remove selection from the finder
			isDoubleClicked = true;
			int pos = notification->position;
			if (pos == INVALID_POSITION)
				pos = static_cast<int32_t>(_scintView.execute(SCI_GETLINEENDPOSITION, notification->line));
			_scintView.execute(SCI_SETSEL, pos, pos);

			gotoFoundLine();
		}
		break;

		case SCN_PAINTED :
			if (isDoubleClicked)
			{
				(*_ppEditView)->getFocus();
				isDoubleClicked = false;
			}
			break;
	}
	return false;
}


void Finder::gotoFoundLine()
{
	auto currentPos = _scintView.execute(SCI_GETCURRENTPOS);
	auto lno = _scintView.execute(SCI_LINEFROMPOSITION, currentPos);
	auto start = _scintView.execute(SCI_POSITIONFROMLINE, lno);
	auto end = _scintView.execute(SCI_GETLINEENDPOSITION, lno);

	if (start + 2 >= end) return; // avoid empty lines

	if (_scintView.execute(SCI_GETFOLDLEVEL, lno) & SC_FOLDLEVELHEADERFLAG)
	{
		_scintView.execute(SCI_TOGGLEFOLD, lno);
		return;
	}

	const FoundInfo fInfo = *(_pMainFoundInfos->begin() + lno);

	// Switch to another document
	::SendMessage(::GetParent(_hParent), WM_DOOPEN, 0, reinterpret_cast<LPARAM>(fInfo._fullPath.c_str()));
	Searching::displaySectionCentered(fInfo._start, fInfo._end, *_ppEditView);

	// Then we colourise the double clicked line
	setFinderStyle();

	_scintView.execute(SCI_STYLESETEOLFILLED, SCE_SEARCHRESULT_HIGHLIGHT_LINE, true);
	_scintView.execute(SCI_STARTSTYLING, start, STYLING_MASK);
	_scintView.execute(SCI_SETSTYLING, end - start + 2, SCE_SEARCHRESULT_HIGHLIGHT_LINE);
	_scintView.execute(SCI_COLOURISE, start, end + 1);
}

void Finder::deleteResult()
{
	auto currentPos = _scintView.execute(SCI_GETCURRENTPOS); // yniq - add handling deletion of multiple lines?

	auto lno = _scintView.execute(SCI_LINEFROMPOSITION, currentPos);
	auto start = _scintView.execute(SCI_POSITIONFROMLINE, lno);
	auto end = _scintView.execute(SCI_GETLINEENDPOSITION, lno);
	if (start + 2 >= end) return; // avoid empty lines

	_scintView.setLexer(SCLEX_SEARCHRESULT, L_SEARCHRESULT, 0); // Restore searchResult lexer in case the lexer was changed to SCLEX_NULL in GotoFoundLine()

	if (_scintView.execute(SCI_GETFOLDLEVEL, lno) & SC_FOLDLEVELHEADERFLAG)  // delete a folder
	{
		auto endline = _scintView.execute(SCI_GETLASTCHILD, lno, -1) + 1;
		assert((size_t) endline <= _pMainFoundInfos->size());

		_pMainFoundInfos->erase(_pMainFoundInfos->begin() + lno, _pMainFoundInfos->begin() + endline); // remove found info
		_pMainMarkings->erase(_pMainMarkings->begin() + lno, _pMainMarkings->begin() + endline);

		auto end2 = _scintView.execute(SCI_POSITIONFROMLINE, endline);
		_scintView.execute(SCI_SETSEL, start, end2);
		setFinderReadOnly(false);
		_scintView.execute(SCI_CLEAR);
		setFinderReadOnly(true);
	}
	else // delete one line
	{
		assert((size_t) lno < _pMainFoundInfos->size());

		_pMainFoundInfos->erase(_pMainFoundInfos->begin() + lno); // remove found info
		_pMainMarkings->erase(_pMainMarkings->begin() + lno);

		setFinderReadOnly(false);
		_scintView.execute(SCI_LINEDELETE);
		setFinderReadOnly(true);
	}
	_markingsStruct._length = static_cast<long>(_pMainMarkings->size());

	assert(_pMainFoundInfos->size() == _pMainMarkings->size());
	assert(size_t(_scintView.execute(SCI_GETLINECOUNT)) == _pMainFoundInfos->size() + 1);
}

vector<std::string> Finder::getResultFilePaths() const
{
	vector<std::string> paths;
	size_t len = _pMainFoundInfos->size();
	for (size_t i = 0; i < len; ++i)
	{
		// make sure that path is not already in
		std::string & path2add = (*_pMainFoundInfos)[i]._fullPath;
		bool found = path2add.empty();
		for (size_t j = 0; j < paths.size() && not found; ++j)
		{
			if (paths[j] == path2add)
				found = true;

		}
		if (not found)
			paths.push_back(path2add);
	}
	return paths;
}

bool Finder::canFind(const TCHAR *fileName, size_t lineNumber) const
{
	size_t len = _pMainFoundInfos->size();
	for (size_t i = 0; i < len; ++i)
	{
		if ((*_pMainFoundInfos)[i]._fullPath == fileName)
		{
			if (lineNumber == (*_pMainFoundInfos)[i]._lineNumber)
				return true;
		}
	}
	return false; 
}

void Finder::gotoNextFoundResult(int direction)
{
	int increment = direction < 0 ? -1 : 1;
	auto currentPos = _scintView.execute(SCI_GETCURRENTPOS);
	auto lno = _scintView.execute(SCI_LINEFROMPOSITION, currentPos);
	auto total_lines = _scintView.execute(SCI_GETLINECOUNT);
	if (total_lines <= 1) return;
	
	if (lno == total_lines - 1) lno--; // last line doesn't belong to any search, use last search

	auto init_lno = lno;
	auto max_lno = _scintView.execute(SCI_GETLASTCHILD, lno, searchHeaderLevel);

	assert(max_lno <= total_lines - 2);

	// get the line number of the current search (searchHeaderLevel)
	int level = _scintView.execute(SCI_GETFOLDLEVEL, lno) & SC_FOLDLEVELNUMBERMASK;
	auto min_lno = lno;
	while (level-- >= fileHeaderLevel)
	{
		min_lno = _scintView.execute(SCI_GETFOLDPARENT, min_lno);
		assert(min_lno >= 0);
	}

	if (min_lno < 0) min_lno = lno; // when lno is a search header line

	assert(min_lno <= max_lno);

	lno += increment;
	
	if      (lno > max_lno) lno = min_lno;
	else if (lno < min_lno) lno = max_lno;

	while (_scintView.execute(SCI_GETFOLDLEVEL, lno) & SC_FOLDLEVELHEADERFLAG)
	{
		lno += increment;
		if      (lno > max_lno) lno = min_lno;
		else if (lno < min_lno) lno = max_lno;
		if (lno == init_lno) break;
	}

	if ((_scintView.execute(SCI_GETFOLDLEVEL, lno) & SC_FOLDLEVELHEADERFLAG) == 0)
	{
		auto start = _scintView.execute(SCI_POSITIONFROMLINE, lno);
		_scintView.execute(SCI_SETSEL, start, start);
		_scintView.execute(SCI_ENSUREVISIBLE, lno);
		_scintView.execute(SCI_SCROLLCARET);

		gotoFoundLine();
	}
}
