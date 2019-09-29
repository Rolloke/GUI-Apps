#include "./DockingFeature/DockingDlgInterface.h"
#include "PluginInterface.h"
#include <string>
#include <vector>

struct FoundInfo {
    FoundInfo(int start, int end, size_t lineNumber, const TCHAR *fullPath)
        : _start(start), _end(end), _lineNumber(lineNumber), _fullPath(fullPath) {};
    int _start;
    int _end;
    size_t _lineNumber;
    std::string _fullPath;
};

struct SearchResultMarking {
    long _start;
    long _end;
};

struct SearchResultMarkings {
    long _length;
    SearchResultMarking *_markings;
};
//Finder: Dockable window that contains search results
class Finder : public DockingDlgInterface {
friend class FindReplaceDlg;
public:
	Finder() : DockingDlgInterface(IDD_FINDRESULT) {
		_markingsStruct._length = 0;
		_markingsStruct._markings = NULL;
	};

	~Finder() {
		_scintView.destroy();
	}
	void init(HINSTANCE hInst, HWND hPere, ScintillaEditView **ppEditView) {
		DockingDlgInterface::init(hInst, hPere);
		_ppEditView = ppEditView;
	};

	void addSearchLine(const TCHAR *searchName);
	void addFileNameTitle(const TCHAR * fileName);
	void addFileHitCount(int count);
	void addSearchHitCount(int count, bool isMatchLines = false);
	void add(FoundInfo fi, SearchResultMarking mi, const TCHAR* foundline);
	void setFinderStyle();
	void removeAll();
	void openAll();
	void copy();
	void beginNewFilesSearch();
	void finishFilesSearch(int count, bool isMatchLines = false);
	void gotoNextFoundResult(int direction);
	void gotoFoundLine();
	void deleteResult();
	std::vector<std::string> getResultFilePaths() const;
	bool canFind(const TCHAR *fileName, size_t lineNumber) const;
	void setVolatiled(bool val) { _canBeVolatiled = val; };

protected :
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool notify(SCNotification *notification);

private:

	enum { searchHeaderLevel = SC_FOLDLEVELBASE + 1, fileHeaderLevel, resultLevel };

	ScintillaEditView **_ppEditView = nullptr;
	std::vector<FoundInfo> _foundInfos1;
	std::vector<FoundInfo> _foundInfos2;
	std::vector<FoundInfo>* _pMainFoundInfos = &_foundInfos1;
	std::vector<SearchResultMarking> _markings1;
	std::vector<SearchResultMarking> _markings2;
	std::vector<SearchResultMarking>* _pMainMarkings = &_markings1;
	SearchResultMarkings _markingsStruct;

	ScintillaEditView _scintView;
	unsigned int _nbFoundFiles = 0;

	int _lastFileHeaderPos = 0;
	int _lastSearchHeaderPos = 0;

	bool _canBeVolatiled = true;


	void setFinderReadOnly(bool isReadOnly) {
		_scintView.execute(SCI_SETREADONLY, isReadOnly);
	};

	bool isLineActualSearchResult(const std::string & s) const;
	std::string & prepareStringForClipboard(std::string & s) const;

	static FoundInfo EmptyFoundInfo;
	static SearchResultMarking EmptySearchResultMarking;
};
