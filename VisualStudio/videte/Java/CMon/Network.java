import java.net.*;
import java.io.*;

///////////////////////////////////////////////////////////////////////////////////////////////////

public class Network{}

///////////////////////////////////////////////////////////////////////////////////////////////////

class CUDPSocket extends Thread
{
	public DatagramSocket m_Socket = null;
	private int m_iPort = 0;
	private DatagramPacket recvPacket, sendPacket;
	
//----------------------------------------------------------------------	
	public int GetLocalPort()
	{
		int returnValue;
		returnValue = 0;
		returnValue = m_Socket.getLocalPort();
		return returnValue;
	}
//----------------------------------------------------------------------
	public boolean SendTo(byte[] buf, int ibufLen,InetAddress RemoteIP, int iRemotePort)
	{
		boolean returnValue;
		returnValue = false;

		sendPacket = new DatagramPacket(buf,ibufLen, RemoteIP,iRemotePort);
		try
		{
			m_Socket.send(sendPacket);
			returnValue = true;
		}
		catch (SocketException soe)
		{
			returnValue = false;	
		}
		catch (IOException ioe)
		{
			returnValue = false;
		}
	return returnValue;
	}

	public void SetPort(int iPort)
	{
		m_iPort = iPort;
	}	
//----------------------------------------------------------------------	
	private boolean SetTimeout(int Timeout)
	{
		boolean returnValue;
		returnValue = false;
		try
		{
			m_Socket.setSoTimeout(Timeout);
		}
		catch (SocketException soe)
		{
			returnValue = false;
		}		
		return returnValue;
	}
//----------------------------------------------------------------------	
	public boolean StartSocket()
	{
		boolean bRet = false;
		
		if(m_iPort!=0)
		{
			try
			{
				m_Socket = new DatagramSocket(m_iPort);
				bRet = true;
			}
			catch(SocketException soe)
			{
				bRet = false;	
			}
		}
		else
		{
			try
			{
				m_Socket = new DatagramSocket();
				bRet = true;
			}
			catch(SocketException soe)
			{
				bRet = false;	
			}
		}
		SetTimeout(30);
		start();
		return bRet;
	}
//----------------------------------------------------------------------
	protected void ThreadFunc()
	{
		// TODO: Add your own implementation.
	}
//----------------------------------------------------------------------
	public void run()
	{
		ThreadFunc();
	}
//----------------------------------------------------------------------	
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTCPSocket extends Thread
{
	public Socket m_Socket = null;
	private OutputStream m_output = null;
	private boolean m_bConnected = false;
	private InputStream m_input = null;

//----------------------------------------------------------------------	
	public boolean Close()
	{
		// TODO: Add your own implementation.
		boolean returnValue;
		returnValue = false;
		stop();
		return returnValue;
	}
//----------------------------------------------------------------------	
	public boolean ConnectTo(String sRemoteAddr, int iRemotePort)
	{
		boolean returnValue;
		returnValue = false;
		try
		{
			m_Socket = new Socket(sRemoteAddr,iRemotePort);
			returnValue = true;
			m_bConnected = true;
			SetTimeout(20);
			m_output = m_Socket.getOutputStream();
			m_input = m_Socket.getInputStream();
			start();
		}
		catch (UnknownHostException uhe)
		{
			returnValue = false;
		}
		catch (IOException ioe)
		{
			returnValue = false;
		}
		return returnValue;
	}
//----------------------------------------------------------------------
	public boolean IsConnected()
	{
		return m_bConnected;
	}
//----------------------------------------------------------------------
	public void OnReceive(byte[] pData, int iDataLen)
	{
		// TODO: Add your own implementation.
	}
//----------------------------------------------------------------------
	public int PeekBuffer()
	{
		int returnValue=0;
		try
		{
			returnValue = m_input.available();
		}
		catch(IOException ioe)
		{
			
		}
		return returnValue;
	}
//----------------------------------------------------------------------
	private int Receive(CStreamData StreamData)
	{
		int returnValue;
		int iRead;
		int iDataAvailable;
		returnValue = 0;
		
		iDataAvailable = PeekBuffer();
		if(iDataAvailable==0)
		{
			returnValue = 0;	
		}
		else
		{
			StreamData.buffer = new byte[iDataAvailable];
			try
			{
				returnValue = m_input.read(StreamData.buffer,0,iDataAvailable);	
			}
			catch(IOException ioe)
			{ 
				returnValue = 0;
			}
			StreamData.iSize = returnValue;
		}
		
		return returnValue;
	}
//----------------------------------------------------------------------
	public boolean Send(byte[] buf)
	{
		boolean returnValue;
		returnValue = false;
		if(m_bConnected)
		{
			try
			{
				m_output.write(buf);
				returnValue = true;
			}
			catch(IOException ioe)
			{
				returnValue = false;
			}
		}
		return returnValue;
	}
//----------------------------------------------------------------------
	private boolean SetTimeout(int Timeout)
	{
		boolean returnValue;
		returnValue = false;
		try
		{
			m_Socket.setSoTimeout(Timeout);
		}
		catch (SocketException soe)
		{
			returnValue = false;
		}		
		return returnValue;
	}
//----------------------------------------------------------------------
	protected void ThreadFunc()
	{
		// TODO: Add your own implementation.
	}
//----------------------------------------------------------------------
	public void run()
	{
		int iReceived=0;

		while(iReceived!=-1)
		{
			CStreamData streamData=new CStreamData();
			iReceived=Receive(streamData);
			if(iReceived!=0)
			{
				OnReceive(streamData.buffer,streamData.iSize);	
			}
		ThreadFunc();
		yield();
		}
	}
//----------------------------------------------------------------------	
		
}

class CStreamData
{
	public byte buffer[]=null;
	int iSize = 0;
}
