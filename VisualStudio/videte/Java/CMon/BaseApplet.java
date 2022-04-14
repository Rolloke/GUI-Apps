import java.awt.*;
import java.applet.*;
import java.net.*;
import java.io.*;
import java.util.*;
 

/**
 * Diese Klasse liest PARAM-Marken aus ihrer HTML-Hostseite und setzt
 * die Farb- und Bezeichnungseigenschaften für das Applet. Die Programmausführung
 * beginnt mit der Methode init(). 
 */
public class BaseApplet extends Applet implements Runnable	
{
	protected     String m_sAppletUnit[];
	protected     int    iNrOfUnits      = 0;
    private	final String backgroundParam = "background";
	private	final String foregroundParam = "foreground";
	private final String UnitsParam      = "Units";
	private final String NameParam       = "Name";
	public        Thread AppletThread;
	private		  String m_sAppletName;
	
//-------------------------------------------------------------------------------------------
	protected void OnInit()
	{
		// TODO: Add your own implementation.
	}
//-------------------------------------------------------------------------------------------	
	protected void Run()
	{
		// TODO: Add your own implementation.
	}
//-------------------------------------------------------------------------------------------	
	protected void OnAppletMessage(String sSender, CCMDRecord CmdRecord)
	{
		// TODO: Add your own implementation.
	}
	
//-------------------------------------------------------------------------------------------	
	protected void OnDestroy()
	{
		// TODO: Add your own implementation.
	}

	public void SendToApplet(String sAppletName, CCMDRecord CmdRecord)
	{
		BaseApplet RecvApplet = (BaseApplet)getAppletContext().getApplet(sAppletName);
		RecvApplet.OnAppletMessage(m_sAppletName,CmdRecord);
	}
//-------------------------------------------------------------------------------------------	
	public void init()
	{
		initForm();
		usePageParams();
		OnInit();
		// ZU ERLEDIGEN: Fügen Sie beliebigen Konstruktorcode hinter den Aufruf von initForm hinzu.
	}
//-------------------------------------------------------------------------------------------		
	public void start()
	{
		if(AppletThread==null)					//Applet-Thread wurde noch nicht gestartet
		{	
			AppletThread=new Thread(this);
			AppletThread.start();
		}		
	}
//-------------------------------------------------------------------------------------------	
	public void run()
	{
		while(true)
		{
			Run();
			AppletThread.yield();
		}
	}
//-------------------------------------------------------------------------------------------	
	public void stop()
	{
		OnDestroy();
		AppletThread.stop();
	}
//-------------------------------------------------------------------------------------------		
	/**
	 * Liest Parameter aus dem HTML--Host des Applets und setzt die
	 * Applet-Eigenschaften.
	 */
	private void usePageParams()
	{
		final String defaultBackground = "C0C0C0";
		final String defaultForeground = "000000";
		String backgroundValue;
		String foregroundValue;
		String Units;

		backgroundValue = getParameter(backgroundParam);
		foregroundValue = getParameter(foregroundParam);
		Units           = getParameter(UnitsParam);
		m_sAppletName   = getParameter(NameParam);
		
		if ((backgroundValue == null) ||(foregroundValue == null))
		{
			backgroundValue = defaultBackground;
			foregroundValue = defaultForeground;
		}
		if(Units != null)
		{
			StringTokenizer stoken = new StringTokenizer(Units);
			String st;
			iNrOfUnits = stoken.countTokens();
			m_sAppletUnit = new String[iNrOfUnits];
			iNrOfUnits = 0;
			while (stoken.hasMoreTokens()) 
			{
				m_sAppletUnit[iNrOfUnits] = new String(stoken.nextToken());
				iNrOfUnits++;
			}
		}
		if(m_sAppletName == null)
		{
			m_sAppletName = "Unknown";	
		}
		this.setBackground(stringToColor(backgroundValue));
		this.setForeground(stringToColor(foregroundValue));
	}
//-------------------------------------------------------------------------------------------
	/**
	 * Wandelt eine als "rrggbb" formatierte Zeichenfolge in ein awt.Color-Objekt um
	 */
	private Color stringToColor(String paramValue)
	{
		int red;
		int green;
		int blue;

		red = (Integer.decode("0x" + paramValue.substring(0,2))).intValue();
		green = (Integer.decode("0x" + paramValue.substring(2,4))).intValue();
		blue = (Integer.decode("0x" + paramValue.substring(4,6))).intValue();

		return new Color(red,green,blue);
	}
//-------------------------------------------------------------------------------------------
	/**
	 * Eine externe Schnittstelle, die von Entwurfsprogrammen verwendet wird, um Eigenschaften eines Applets anzuzeigen.
	 */
	public String[][] getParameterInfo()
	{
		String[][] info =
		{
			{ backgroundParam, "String", "Background color, format \"rrggbb\"" },
			{ foregroundParam, "String", "Foreground color, format \"rrggbb\"" },
		};
		return info;
	}
	
//-------------------------------------------------------------------------------------------
	/**
	 * Initialisiert Werte für das Applet und seine Komponenten
	 */
	void initForm()
	{
		this.setBackground(Color.white);
		this.setForeground(Color.black);
		this.setLayout(new BorderLayout());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

class CCMDRecord
{
	int m_wId1     = 0;
	int m_wId2     = 0;
	int m_dwCmd    = 0;
	int m_dwParam1 = 0;
	int m_dwParam2 = 0;
	int m_dwParam3 = 0;
	int m_dwParam4 = 0;
	int m_iDataLen = 0;
	byte[] m_Data  = null;
}
