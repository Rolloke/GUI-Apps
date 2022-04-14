// NeroBurn.h: interface for the CNeroBurn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEROBURN_H__DD1B55BC_7F6D_4F28_AB62_F8A705642CAB__INCLUDED_)
#define AFX_NEROBURN_H__DD1B55BC_7F6D_4F28_AB62_F8A705642CAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExitCode.h"
#include "BurnContext.h"
#include "AcdcDlg.h"
#include "Producer.h"

#define MAXPARAMS 1000;
class CBurnContext;
									
#define NERO_MINIMUM_VERSION 5582;

class CNeroBurn  

{
//functions
public:
	CNeroBurn(CProducer* pProducer);
	virtual ~CNeroBurn();

	//lead all NeroAPI calls and handles all errors.
	EXITCODE	MainApp(CStringArray& saParams);

	//directs all processes for burning CD`s or DVD`s
	void		BurnThread(MainFunction mf);

	EXITCODE GetApiVersion();	//check the installed NeroAPI.
	
	//decode the argument list and fills the PARAMETERS object
	BOOL		GetParams(PARAMETERS & params, CStringArray& saParams);

	EXITCODE OnCDListDrives();	//call MainApp with all neccessary parameters
								//to check for all available CD-RW drives.
	EXITCODE OnCDInfo();		//call the MainApp with all neccessary parameters,
								//check the inserted medium and it`s contents.
	EXITCODE OnCDWriteIso();	// call the MainApp with all neccessary parameters 
								//to burn a CD in ISO format.
	EXITCODE OnCDWriteVCD();	//call the MainApp with all neccessary parameters to burn a VideoCD.
	EXITCODE OnCDWriteSVCD();	//call the MainApp with all neccessary parameters to burn a SuperVideoCD.
	EXITCODE OnEject();			//call the MainApp with all neccessary parameters to eject 
							//the medium from the CD / DVD drive.
	void OnError();			//handle all errors.

	//reading and parsing the parameter list from a disk file.
	BOOL	ReadParameterFile (PARAMETERS & params, LPCSTR psFilename);

	void	SetAbortFlag();

	void	SetNewMessage(CString sNewMsg);
	CString GetNewMessage();
	int		NeroExitCodeToDCErrorCode(EXITCODE code);
	void	SetCDRWAbortFlag(BOOL bFlag);
	BOOL	GetCDRWEraseSuccess();
	void	SetNeroAPIInitialized(BOOL bIsInitialized);
	BOOL	IsNeroAPIInitialized();
	void	SetFileNotFound(CString& sFileNotFound);
	CString	GetFileNotFound();
	void	SetDriveLetter(CString sDriveLetter);
	CString GetDriveLetter();
	CProducer* GetProducer();
	

	CBurnContext* GetBurnContext();
	BOOL    CheckImapi();
	void	SetIsDVD(BOOL bIsDVD);
	BOOL	IsDVD();
	
private:
//functions

	void DeleteBurnContext();
	BOOL EnablePrivilege( HANDLE htok, LPCTSTR pszPriv,
						  TOKEN_PRIVILEGES& tpOld );
	BOOL RestorePrivilege( HANDLE htok, const TOKEN_PRIVILEGES& tpOld );
	BOOL AdjustDacl(HANDLE h, DWORD dwDesiredAccess);

//members
private:

	CProducer*		m_pProducer;

	CBurnContext*	m_pBurnContext;
	PARAMETERS		m_params;		//the PARAMETER object which holds all parameters for burning
	CString			m_sVersionInstalled;	//installed NeroAPI version
	CString			m_sVersionRequired;	//required installed NeroAPI version 
	CStringArray	m_saNewMessage;	//new message [0] to be displayed in CAcdcDlg
	BOOL			m_bNeroAPIInitialized; //if TRUE, NeroAPI is initialized
	CString			m_sFileNotFound;	//if a file was not found, this is the filename
	CString			m_sDriveLetter;		//drive letter from current CD-RW drive
	BOOL			m_bIsDVD;
};

#endif // !defined(AFX_NEROBURN_H__DD1B55BC_7F6D_4F28_AB62_F8A705642CAB__INCLUDED_)
