/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Activation.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/Activation.h $
// CHECKIN:		$Date: 9.11.05 20:35 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 9.11.05 20:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CActivation_H
#define __CActivation_H

#include "wk.h"
#include "SecID.h"

class CInput;
class CProcess;
class CProcessMacro;

#define IO_DEFAULTPRIOR		3

enum TypeOfActivation 
{
	TOA_NONE,
	TOA_INPUT_OK,
	TOA_INPUT_ALARM,
	TOA_USER
};
///////////////////////////////////////////////////////////////////////////
class CActivation
{
	// construction / destruction
public:
	CActivation(int iIONr);
	CActivation(CProcessMacro *pMacro,
				CProcess *pProcess);
	virtual ~CActivation();

	// attributes
public:
	inline TypeOfActivation		GetTypeOfActivation() const;
	inline CInput*				GetInput() const;
	inline CProcess*			GetActiveProcess() const;
	inline const CProcessMacro& GetMacro() const;
	inline CProcessMacro &		GetTmpMacro() const;	// client modify
	inline CSecID				GetOutputID() const;
	inline CSecID				GetArchiveID() const;
	inline CSecID				GetNotificationID() const;
	inline DWORD				GetPriority() const;
	inline int					GetIONr() const;
	inline CameraControlCmd		GetCameraControlCmd() const;
	//

	// operations
public:
	void SetTypeOfActivation(TypeOfActivation toa);
	void SetOutputID(CSecID id);
	void SetInput(CInput *pInput);
	void SetActiveProcess(CProcess *pProcess);
	void SetPriority(DWORD dwPriority);
	void SetArchiveID(CSecID id);
	void SetNotificationID(CSecID id);
	void SetMacro(const CProcessMacro* pMacro);
	void SetCameraControlCommand(CameraControlCmd ccc);

	//
	CSecID	m_idTimer;			// Timer
	CSecID	m_idProcessMacro;	// Prozess der gestartet werden soll
	//
	const CProcessMacro *m_pProcessMacro;

	// implementation
private:
	void Init(int iIONr);

	// data member
private:
	int		m_iIONr;
	CInput*	m_pInput;
	CSecID	m_idOutput;			// Ausgangsnummer

	DWORD	m_dwPriority;		// Priority 0..NUM_PRIORITIES-1
	TypeOfActivation m_activation;
	CProcess *m_pActiveProcess;
	//
	CProcessMacro *m_pTmpMacro;	// OOPS check usage
	CSecID m_archiveID;
	CSecID m_idNotification;
	CameraControlCmd  m_PTZCommand;
};

#include "activation.inl"

typedef CActivation* CActivationPtr;
WK_PTR_ARRAY(CActivationArray,CActivationPtr)

#endif
