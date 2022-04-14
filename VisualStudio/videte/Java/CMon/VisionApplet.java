public class VisionApplet extends BaseApplet
{
	CCMDRecord m_CmdRecord;
	boolean m_bNewCmdRecord = false;
	
//-------------------------------------------------------------------------------------------
	protected void OnInit()
	{
		m_CmdRecord = new CCMDRecord();
	}
//-------------------------------------------------------------------------------------------	
	protected void Run()
	{
		CCMDRecord CmdRecord = SetRecord(null);
		if(CmdRecord != null)
		{
			
		}
	}
//-------------------------------------------------------------------------------------------	
	protected void OnDestroy()
	{
		// TODO: Add your own implementation.
	}
//-------------------------------------------------------------------------------------------	
	protected void OnAppletMessage(String sSender,CCMDRecord CmdRecord)
	{
		SetRecord(CmdRecord);
	}
//-------------------------------------------------------------------------------------------	
	public synchronized CCMDRecord SetRecord(CCMDRecord CmdRecord)
	{
		if(CmdRecord != null)
		{
			m_CmdRecord = CmdRecord;
			m_bNewCmdRecord = true;
			return null;
		}
		else
		{
			if(m_bNewCmdRecord)
			{
				m_bNewCmdRecord = false;
				return 	m_CmdRecord;
			}
			else
			{
				return null;
			}
		}
	}
}
