import java.awt.*;
import java.applet.*;
import java.net.*;
import java.io.*;
import java.util.*;
import java.math.BigInteger;
import java.lang.System;


/**
 * Diese Klasse liest PARAM-Marken aus ihrer HTML-Hostseite und setzt
 * die Farb- und Bezeichnungseigenschaften für das Applet. Die Programmausführung
 * beginnt mit der Methode init(). 
 */
public class SocketUnitApplet extends BaseApplet
{

	public CDataSocket m_DataSocket = new CDataSocket(this);

//-------------------------------------------------------------------------------------------
	protected void OnInit()
	{
		m_DataSocket.ConnectTo("ramon",80);
	}
//-------------------------------------------------------------------------------------------	
	protected void Run()
	{
		// TODO: Add your own implementation.
		//SendToApplet("Cam1",null, 0,"Hallo");
	}
//-------------------------------------------------------------------------------------------	
	protected void OnAppletMessage(String sSender, CCMDRecord CMDRecord)
	{
	/*	if(sCommand == "SEND_TO_SERVER")
		{
			if(m_DataSocket.IsConnected())
			{	
				String sEnd = new String("\r\n\r\n");
				String sBuf = new String(Buffer);
				sSender = sSender + sBuf;
				sSender = sSender + sEnd;
				m_DataSocket.Send(sSender.getBytes());
			}
		}
		*/
	}
//-------------------------------------------------------------------------------------------		
	public void HandleCmd(CCMDRecord CmdRecord)
	{
		SendToApplet("Cam1",CmdRecord);
	}
//-------------------------------------------------------------------------------------------	
	protected void OnDestroy()
	{
		// TODO: Add your own implementation.
	}
//-------------------------------------------------------------------------------------------	
	
}

/////////////////////////////////////////////////////////////////////////////////////////////

class CDataSocket extends CTCPSocket
{
	private int m_iDataNextToRead;
	private int m_iBuffersize;
	private byte[] m_Buffer;
	private SocketUnitApplet m_Applet;
	private boolean m_bCMDComplete, m_bNextDataIsCMD;
	private CCMDRecord m_CmdRecord;
	
	//----------------------------------------------------------------------
	public CDataSocket(SocketUnitApplet applet)
	{
		m_iDataNextToRead = 28;
		m_Applet          = applet;
		m_iBuffersize     = 0;
		m_bCMDComplete    = false;
		m_bNextDataIsCMD  = true;
		m_CmdRecord = new CCMDRecord();
		m_Buffer		 = new byte[1024 * 100];
	}
	public void finalize()
	{
	
	
	}
 	//----------------------------------------------------------------------
	protected void ThreadFunc()
	{
		// TODO: Add your own implementation.
	}
	//----------------------------------------------------------------------
	public void OnReceive(byte[] pData, int iDataLen)
	{
		try
		{
			System.arraycopy(pData,0,m_Buffer,m_iBuffersize,iDataLen);
			m_iBuffersize += iDataLen;
		}
		catch(ArrayStoreException ase)
		{
			int a=0;
		}
		if(m_iBuffersize >= m_iDataNextToRead)
		{
			HandleBuffer();
		}
	}
	//----------------------------------------------------------------------
	public void HandleBuffer()
	{
		
		byte[] pData = RemoveFromBuffer(m_iDataNextToRead);
		
		if(m_bNextDataIsCMD)
		{
			int[] iParam = new int[7];
			DataInputStream din  = new DataInputStream( new ByteArrayInputStream(pData,0,28));
			for(int i=0; i<7; i++) 
			{
				try
				{
					iParam[i] = din.readInt();
				}
				catch(IOException ioe)
				{
				}
			}
			m_CmdRecord.m_wId1     = iParam[0] >>16;
			m_CmdRecord.m_wId1     = iParam[0] & 65535;	
			m_CmdRecord.m_iDataLen = iParam[6];
			m_CmdRecord.m_dwCmd    = iParam[1];
			m_CmdRecord.m_dwParam1 = iParam[2];
			m_CmdRecord.m_dwParam2 = iParam[3];
			m_CmdRecord.m_dwParam3 = iParam[4];
			m_CmdRecord.m_dwParam4 = iParam[5];	
		
			if(m_CmdRecord.m_iDataLen == 0)
			{
				m_bCMDComplete    = true;
				m_iDataNextToRead = 28;
				m_bNextDataIsCMD  = true;
			}
			else
			{
				m_iDataNextToRead = m_CmdRecord.m_iDataLen;
				m_bCMDComplete   = false;
				m_bNextDataIsCMD = false;
			}
		}
		else
		{
			m_CmdRecord.m_Data =pData; 
			m_bCMDComplete = true;
		}
		if(m_bCMDComplete)
		{
			m_bCMDComplete = false;
			m_iDataNextToRead = 28;
			m_bNextDataIsCMD = true;
			m_Applet.HandleCmd(m_CmdRecord);
		}
	}
	//----------------------------------------------------------------------
	private byte[] RemoveFromBuffer(int iLen)
	{
		if(iLen>m_iBuffersize)
		{
			return null;
		}
		byte[] buf = new byte[iLen];
		
		m_iBuffersize-=iLen;
		System.arraycopy(m_Buffer,0,buf,0,iLen);
		System.arraycopy(m_Buffer,iLen,m_Buffer,0,m_iBuffersize);
		return buf;
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////	

