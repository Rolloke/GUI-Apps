// ConnectionRecord.h: interface for the CConnectionRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONRECORD_H__50B5B755_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
#define AFX_CONNECTIONRECORD_H__50B5B755_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cipc.h"
#include "User.h"

#define LOCAL_LOOP_BACK _T("LocalLoopBack")
#define ROUTING_CHAR	   '#'

#define CRF_IPADDRESS	_T("IPADDRESS")
#define CRF_BITRATE		_T("BITRATE")
#define CRF_MSN			_T("MSN")
#define CRF_B_CHANNELS	_T("BCHANNELS")
#define CRF_SERIAL		_T("SERIAL")
#define CRF_VERSION		_T("VERSION")
#define CRF_FULLSCREEN	_T("FS")
#define CRF_SAVE		_T("SAVE")
#define CRF_EXCLUSIVE	_T("EXCLUSIVE")
#define CRF_PIN			_T("PIN")
#define CRF_DEST_HOST_NAME	_T("D_HOSTNAME")
#define CRF_DISCONNECT	_T("DISCONNECT")

#ifdef _UNICODE
#define CRF_CODEPAGE	_T("CODEPAGE")
#endif

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
/*  CConnectionRecord | The class encapsulated necessary
parameters for establishing connections between client and server,
such as user name, permission password, dial number and so
on. There are some default parameters and a dynamic parameter array
as well.
 <c CIPCServerControl>, <c CIPCFetch>
*/
class AFX_EXT_CLASS CConnectionRecord 
{
	//  construction/destruction
public:
	//!ic! default constructor
	CConnectionRecord();
	//!ic! parameter constructor
	CConnectionRecord(const CString &remoteHost,
				      const CUser &user,
					  const CString &sPermission,
					  const CString &sPassword,
					  DWORD dwTimeout=30000);
	//!ic! copy constructor
	CConnectionRecord(const CConnectionRecord &Source);
	//!ic! assignment operator
	const CConnectionRecord &operator =(const CConnectionRecord &Source);
	//!ic! constructor from command
	CConnectionRecord(const CIPCCmdRecord &pCmd);

	//  attributes
public:
	// the dial number or IP address
	const CString &GetDestinationHost() const;
	// the logged in user name
	const CString &GetUser() const;
	// the users permission name
	const CString &GetPermission() const;
	// the permissions password
	const CString &GetPassword() const;
	// the name of the calling station
	const CString &GetSourceHost() const;
	// the Input shared memory name for alarm connections
	const CString &GetInputShm() const;
	// the Output shared memory name for alarm connections
	const CString &GetOutputShm() const;
	// flags for several connection options
	DWORD GetOptions() const;
	// timeout in ms to try to establish the connection
	DWORD GetTimeout() const;
	// the default camera id for alarm connections
	CSecID GetCamID() const;

	//  operations
public:
	//!ic! create a command record from CConnectionRecord
	CIPCCmdRecord *CreateCmdRecord(const CIPC *pCipc, DWORD dwCmd ) const;
	// sets the destination host number or IP address
	void SetDestinationHost(const CString &sDestination);
	// sets the logged in user name
	void SetUser(const CString &sUser);
	// sets the permission name
	void SetPermission(const CString &sPermission);
	// sets the permissions password
	void SetPassword(const CString &sPassword);
	// sets the name of the source host
	void SetSourceHost(const CString &sSource);
	// sets the shared memory name of the input for alarm connections
	void SetInputShm(const CString &sSource);
	// sets the shared memory name of the output for alarm connections
	void SetOutputShm(const CString &sSource);
	// sets the flags for the connection
	void SetOptions(DWORD dwOptions);
	// sets the timeout in ms for the connection establishing
	void SetTimeout(DWORD dwTimeout);
	// sets the default camera id for alarm connections
	void SetCamID(CSecID id);
	
	//  dynamic parameter functions
public:
	// gets the value for the field with specified name
	BOOL GetFieldValue(const CString &sFieldName, CString &sResult) const;
	// sets the value for the field with specified name
	void SetFieldValue(const CString &sFieldName,const CString &sFieldValue);
	// gets all field into a string array
	void GetAllFields(CStringArray &fields) const;

private:
	CString m_sDestinationHost;
	CString m_sUser;
	CString m_sPermission;
	CString m_sPassword;
	CString m_sSourceHost;
	//
	CString m_sInputShm;
	CString m_sOutputShm;
	//
	DWORD m_dwOptions;
	DWORD m_dwTimeout;
	CSecID m_camID;
	// 
	CStringArray m_fields;
};
/////////////////////////////////////////////////////////////////////////////
typedef CConnectionRecord* CConnectionRecordPtr;
WK_PTR_LIST_CS(CConnectionRecordArray,CConnectionRecordPtr,CConnectionRecords)
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CONNECTIONRECORD_H__50B5B755_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
