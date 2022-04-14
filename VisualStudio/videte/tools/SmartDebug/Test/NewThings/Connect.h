// Connect.h : Deklaration von CConnect.

#pragma once
#include "resource.h"       // Hauptsymbole

using namespace AddInDesignerObjects;
using namespace EnvDTE;
using namespace EnvDTE80;
using namespace Microsoft_VisualStudio_CommandBars;

/// <summary>Das Objekt für die Implementierung eines Add-Ins.</summary>
/// <seealso class='IDTExtensibility2' />
class ATL_NO_VTABLE CConnect : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnect, &CLSID_Connect>,
	public IDispatchImpl<IDTCommandTarget, &IID_IDTCommandTarget, &LIBID_EnvDTE, 7, 0>,
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects, 1, 0>
{
public:
	/// <summary>Implementiert den Konstruktor für das Add-In-Objekt. Platzieren Sie Ihren Initialisierungscode innerhalb dieser Methode.</summary>
	CConnect()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)
DECLARE_NOT_AGGREGATABLE(CConnect)

BEGIN_COM_MAP(CConnect)
	COM_INTERFACE_ENTRY(IDTExtensibility2)
	COM_INTERFACE_ENTRY(IDTCommandTarget)
	COM_INTERFACE_ENTRY2(IDispatch, IDTExtensibility2)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
//IDTExtensibility2-Implementierung:

	/// <summary>Implementiert die OnConnection-Methode der IDTExtensibility2-Schnittstelle. Empfängt eine Benachrichtigung, wenn das Add-In geladen wird.</summary>
	/// <param term='application'>Stammobjekt der Hostanwendung.</param>
	/// <param term='connectMode'>Beschreibt, wie das Add-In geladen wird.</param>
	/// <param term='addInInst'>Objekt, das dieses Add-In darstellt.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnConnection)(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch *AddInInst, SAFEARRAY **custom);

	/// <summary>Implementiert die OnDisconnection-Methode der IDTExtensibility2-Schnittstelle. Empfängt eine Benachrichtigung, wenn das Add-In entladen wird.</summary>
	/// <param term='disconnectMode'>Beschreibt, wie das Add-In entladen wird.</param>
	/// <param term='custom'>Array von spezifischen Parametern für die Hostanwendung.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY **custom );

	/// <summary>Implementiert die OnAddInsUpdate-Methode der IDTExtensibility2-Schnittstelle. Empfängt eine Benachrichtigung, wenn die Auflistung von Add-Ins geändert wurde.</summary>
	/// <param term='custom'>Array von spezifischen Parametern für die Hostanwendung.</param>
	/// <seealso class='IDTExtensibility2' />	
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY **custom );

	/// <summary>Implementiert die OnStartupComplete-Methode der IDTExtensibility2-Schnittstelle. Empfängt eine Benachrichtigung, wenn der Ladevorgang der Hostanwendung abgeschlossen ist.</summary>
	/// <param term='custom'>Array von spezifischen Parametern für die Hostanwendung.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnStartupComplete)(SAFEARRAY **custom );

	/// <summary>Implementiert die OnBeginShutdown-Methode der IDTExtensibility2-Schnittstelle. Empfängt eine Benachrichtigung, wenn die Hostanwendung entladen wird.</summary>
	/// <param term='custom'>Array von spezifischen Parametern für die Hostanwendung.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnBeginShutdown)(SAFEARRAY **custom );
	
//IDTCommandTarget-Implementierung:

	/// <summary>Implementiert die QueryStatus-Methode der IDTCommandTarget-Schnittstelle. Diese wird aufgerufen, wenn die Verfügbarkeit des Befehls aktualisiert wird.</summary>
	/// <param term='commandName'>Der Name des Befehls, dessen Zustand ermittelt werden soll.</param>
	/// <param term='neededText'>Für den Befehl erforderlicher Text.</param>
	/// <param term='status'>Der Zustand des Befehls in der Benutzeroberfläche.</param>
	/// <param term='commandText'>Vom neededText-Parameter angeforderter Text.</param>
	/// <seealso class='Exec' />
	STDMETHOD(QueryStatus)(BSTR CmdName, vsCommandStatusTextWanted NeededText, vsCommandStatus *StatusOption, VARIANT *CommandText);

	/// <summary>Implementiert die Exec-Methode der IDTCommandTarget-Schnittstelle. Diese wird aufgerufen, wenn der Befehl aufgerufen wird.</summary>
	/// <param term='commandName'>Der Name des auszuführenden Befehls.</param>
	/// <param term='executeOption'>Beschreibt, wie der Befehl ausgeführt werden muss.</param>
	/// <param term='varIn'>Parameter, die vom Aufrufer an den Befehlshandler übergeben werden.</param>
	/// <param term='varOut'>Parameter, die vom Befehlshandler an den Aufrufer übergeben werden.</param>
	/// <param term='handled'>Informiert den Aufrufer, ob der Befehl bearbeitet wurde oder nicht.</param>
	/// <seealso class='Exec' />
	STDMETHOD(Exec)(BSTR CmdName, vsCommandExecOption ExecuteOption, VARIANT *VariantIn, VARIANT *VariantOut, VARIANT_BOOL *Handled);

private:
	CComPtr<DTE2> m_pDTE;
	CComPtr<AddIn> m_pAddInInstance;
};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)
