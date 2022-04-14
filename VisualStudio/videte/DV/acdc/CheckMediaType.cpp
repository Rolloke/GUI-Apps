/******************************************************************************
|*	This file keeps all function for getting information about an inserted medium
|*
|*	implemented main functions:
|*		-	CheckMediaCapacities -	Retrieves information about mediums capacity
|*		-	CheckOldMediumType	 -	Retrieve medium information using NeroAPI from version
|*									5.5.8.2. to less than 5.5.9.10.
|*
|*		-	CheckNewMediaType  	 -	Retrieve medium information using NeroAPI from version
|*									5.5.9.10. or higher
|*
|*	PROGRAM: CheckMediaType.cpp
|*
******************************************************************************/


#include "stdafx.h"
#include "BurnContext.h"

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CheckMediaCapacities 
 Description   : Get capicities (free and used) of a medium

 Parameters:  
  sDriveLetter: (i): drive letter of the recorder (CString)

 Result type:  if TRUE, getting capacity was successful

 Author: TKR
 created: July, 03 2003
 <TITLE CheckMediaCapacities >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, CheckMediaCapacities>
*********************************************************************************************/
BOOL CBurnContext::CheckMediaCapacities(CString sDriveLetter)
{
	BOOL bReturn = TRUE;

	//get information about mediums capacities with NeroAPI.dll
	//---------------------------------------------------------
	m_dwNumOfTracks = m_NeroCDInfo->ncdiNumTracks;
	ULONGLONG ullFreeBlocks, ullUnusedBlocks;
	ullFreeBlocks		= 0;
	ullUnusedBlocks		= 0;
	ullFreeBlocks		= m_NeroCDInfo->ncdiFreeCapacityInBlocks;
	ullUnusedBlocks		= m_NeroCDInfo->ncdiUnusedBlocks;

	//muss mit max DVD Kapazität initialisiert werden, da bei leeren DVD+RW`s weder
	//CIPCDrive noch die NeroAPI irgendwelche Daten bezüglich der Kapazitäten liefert
	//Sollten doch Daten geliefert werden (wie bei beschriebenen DVD`s (+R/+RW) und allen CD`s
	//wird der Wert der freien Kapazität gesetzt

	//ab DVD double layer nun mit doppelter Größe initialisieren
	m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityDVDDoubleLayer;


	//get information about mediums capacities with CIPCDrive
	//---------------------------------------------------------
	ULONGLONG ullCIPCUsedMB = 0;
	ULONGLONG ullCIPCFreeMB = 0;

	CIPCDrive removableDrive(sDriveLetter+_T(":\\"), DVR_WRITE_BACKUP_DRIVE);

	ullCIPCUsedMB = removableDrive.GetUsedMB();
	ullCIPCFreeMB = removableDrive.GetFreeMB();

	if(ullFreeBlocks > 0)
	{
		m_ullMediumFreeCapacityInBytes = ullFreeBlocks * m_ullBytesPerBlock;
	}

	//get unused blocks in MB between current LeadOut and last possible LeadOut
	if(ullUnusedBlocks > 0)
	{
		if(ullUnusedBlocks > m_ullMaxDVDBlocks)
		{
			bReturn = FALSE;
		}
		else
		{
			//get unused blocks in MB between current LeadOut and last possible LeadOut
			m_ullMediumFreeCapacityInBytes = ullUnusedBlocks * m_ullBytesPerBlock;
		}
	}

	//get mediums used capacity (all tracks) in MB
	m_ullMediumUsedCapacityInBytes = 0;

	ULONGLONG ullTrackLengthInBlocks;
	for (DWORD j = 0; j < m_dwNumOfTracks; j ++)
	{
		//get used MB of each track on medium in Bytes
		ullTrackLengthInBlocks = m_NeroCDInfo->ncdiTrackInfos[j].ntiTrackLengthInBlks;
		m_ullMediumUsedCapacityInBytes += ullTrackLengthInBlocks * m_ullBytesPerBlock;

		//bei DVD+RW`s wird immer die beschriebene Kapazität UND die freie Kapazität mit 4,7 GB
		//erkannt. Liegt daran, dass die DVD+RW formatiert wird und er formatierte Platz einmal die
		//gesamte Kapazität wiederspiegelt aber gleichzeitig der belegte Platz auch als frei verfügbarer
		//erkannt wird. An anderer Stelle im Quellcode werden
		// m_ullMediumUsedCapacityInBytes und m_ullMediumFreeCapacityInBytes addiert, was zu einem Fehler führte.
	}

	//prüfe, ob die mit Nero bestimmte Größe des belegten Speichers des Mediums übereinstimmt
	//mit der selben Größe, allerdings mit CIPCDrive bestimmt.
	//Dies ist nötig, da Nero bei beschriebenen DVD`s nicht die korrekte Größe des
	//belegten Speichers der DVD liefert. Es ist dann die belegte Größe gleich der Gesamtgröße
	//und somit die freie Kapazität == 0
	if(    m_NeroCDInfo->ncdiMediumType != NMT_CD_ROM
		&& m_ullMediumUsedCapacityInBytes != (ullCIPCUsedMB * 1024 * 1024)
		&& (m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
			|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer))
	{
		//Bei MultiSession ist schon eine Session auf der DVD
		//(bei DVD`s ist es immer nur eine einzige Session, in die neue Tracks geschrieben werden
		//können. Mehrere Sessions, wie bei CD`s, gibt es nicht. Es können aber mehrere Sessions
		//gebrannt werden. Diese landen dann aber alle in der einzigen, ersten Session)
		//Im Falle einer MultiSession DVD liefert CICPDrive nur die Größe des zuletzt 
		//gebrannten Tracks innerhalb der Session. Deshalb dann hier die korrekte Größe des belegten
		//Speichers auf der DVD bestimmen. Ansonsten (Tracks == 0) liefert CIPCDrive korrekte Werte.

		if(m_dwNumOfTracks == 0)
		{
			//CIPCDrive hat korrekte Größe des belegten Speichers erkannt
			m_ullMediumFreeCapacityInBytes = m_ullMediumUsedCapacityInBytes; //setze free Kapaz. = Used Kapaz
																			 //in diesem Fall also auf max. Kapaz.
			m_ullMediumFreeCapacityInBytes -= ullCIPCUsedMB * 1024 * 1024;		//ziehe wirklich belegten Speicher ab
			m_ullMediumUsedCapacityInBytes = ullCIPCUsedMB * 1024 * 1024;		//setze wirklich belegten Speicher
		
		}
		else
		{
			//bestimme belegten Speicherplatz der DVD 
			m_ullMediumUsedCapacityInBytes = 0;
			GetUsedCapacityRecursiv(sDriveLetter+_T(":\\"), m_ullMediumUsedCapacityInBytes);
			m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityDVD - m_ullMediumUsedCapacityInBytes;
		}

	}
	return bReturn;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CheckOldMediumType 
 Description   : This function gets information about the medium type of an inserted medium
				 when using NeroAPI version 5.5.8.2. to less than 5.5.9.10.

 Parameters:  --


 Result type:  medium information, medium type  (CString)

 Author: TKR
 created: July, 01 2003
 <TITLE CheckOldMediumType >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, CheckOldMediumType>
*********************************************************************************************/
CString CBurnContext::CheckOldMediumType()
{
	CString sMedienType;

	ULONGLONG ullReserve = RESERVE_SPACE_FOR_READER_SOFTWARE * 1024 * 1024;
	if(m_ullMediumUsedCapacityInBytes > ullReserve)
	{
		//bei zu löschenden Medien einen Sicherheitspuffer von 10 MB einräumen
		m_ullMediumUsedCapacityInBytes -= ullReserve;
	}
	else
	{
		if(m_ullMediumFreeCapacityInBytes > ullReserve)
		{
			m_ullMediumFreeCapacityInBytes -= ullReserve;
		}
	}

	if(m_MediumInfo == MI_UNKNOWN_MEDIUM)
	{
		//unknown media type, maybe cd-drive is out of order
		//reset all capacities
		m_ullMediumUsedCapacityInBytes = 0;
		m_ullMediumFreeCapacityInBytes = 0;
	}
	else
	{
		switch (m_NeroCDInfo->ncdiMediumType)
		{
			case NMT_CD_ROM:
				m_MediumInfo = MI_CDREADONLY;
				m_ullMediumFreeCapacityInBytes = 0;
				m_ullMediumUsedCapacityInBytes = 0;
				sMedienType = _T("Medientype: read only");
				break;

			case NMT_CD_RECORDABLE:
				//no tracks means empty medium,
				//one or more tracks means, non empty medium, 
				//multiseesion will not yet be supported

				//prüfe, welches Medium im Laufwerk (CD-R, DVD+R)
				//----------------------------------------------------
				if(!m_NeroCDInfo->ncdiNumTracks)
				{
					//leeres Medium gefunden
					//prüfe, welches Medium (CD-R, DVD+R)
					if(m_ullMediumUsedCapacityInBytes + m_ullMediumFreeCapacityInBytes < (m_ullOneMegaByte * 1024))
					{
						//leeres CD-R medium
						m_MediumInfo = MI_CDOK;
						sMedienType = _T("Medientype: CD-R");
					}
					else
					{
						//leeres DVD+R medium
						m_MediumInfo = MI_DVDOK;
						sMedienType = _T("Medientype: DVD R");
					}
				}
				else
				{
					//nicht leeres Medium
					//prüfe, welches Medium (CD-R, DVD+R)

					if(m_ullMediumUsedCapacityInBytes + m_ullMediumFreeCapacityInBytes < (m_ullOneMegaByte * 1024))
					{
						m_MediumInfo = MI_CDREADONLY;
						sMedienType = _T("Medientype: CD-ROM");
					}
					else
					{
						m_MediumInfo = MI_DVDREADONLY;
						sMedienType = _T("Medientype: DVD ROM");
					}
				}
				break;

			case NMT_CD_REWRITEABLE:
				//no tracks means empty medium,
				//one or more tracks means, non empty medium, 
				//multiseesion will not yet be supported

				//prüfe, welches RW-Medium im Laufwerk (CD-RW, DVD+RW)
				//----------------------------------------------------
				if(!m_dwNumOfTracks)
				{
					//leeres Medium gefunden
					//prüfe, welches Medium (CD-RW, DVD+RW)
					if(m_ullMediumUsedCapacityInBytes + m_ullMediumFreeCapacityInBytes < (m_ullOneMegaByte * 1024))
					{
						//leeres CD-RW medium
						m_MediumInfo = MI_CDRWOK;
						sMedienType = _T("Medientype: CD-RW");
					}
					else
					{
						//leeres DVD+RW medium
						m_MediumInfo = MI_DVDRWOK;
						sMedienType = _T("Medientype: DVD RW");
					}
				}
				else
				{
					//nicht leeres Medium
					//prüfe, welches Medium (CD-RW, DVD+RW)
					if(m_ullMediumUsedCapacityInBytes + m_ullMediumFreeCapacityInBytes < (m_ullOneMegaByte * 1024))
					{
						m_MediumInfo = MI_CDRWDELETE;
						sMedienType = _T("Medientype: CD-RW");
					}
					else
					{
						m_MediumInfo = MI_DVDRWDELETE;
						sMedienType = _T("Medientype: DVD RW");
					}
				}
				break;

			default:
				sMedienType = UNKNOWN_MEDIA_TYPE;
				break;
		}
	}

	return sMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CheckNewMediaType 
 Description   : This function gets information about the medium type of an inserted medium
				 when using NeroAPI version  5.5.9.10. or higher

 Parameters:  
  MediaType:			(i): type of inserted medium (NERO_MEDIA_TYPE)

 Result type:  medium information, medium type  (CString)

 Author: TKR
 created: July, 01 2003
 <TITLE CheckNewMediaType >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, CheckNewMediaType>
*********************************************************************************************/
CString CBurnContext::CheckNewMediaType(NERO_MEDIA_TYPE MediaType)
{
	CString sMedienType;

	switch(MediaType)
	{

	//identified ROM medien (CD-ROM, DVD-ROM, DVD+ROM)
	//------------------------------------------------
	case MEDIA_CDROM:
		sMedienType = OnMedia_CDROM();
		break;

	case MEDIA_DVD_ROM:
		sMedienType = OnMedia_DVD_ROM();
		break;

	//identified R medien (CD-R, DVD-R, DVD+R)
	//------------------------------------------------
	case MEDIA_CDR:
		sMedienType = OnMedia_CDR();
		break;

	case MEDIA_DVD_M_R:
		sMedienType = OnMedia_DVD_M_R();
		break;

	case MEDIA_DVD_P_R:
		sMedienType = OnMedia_DVD_P_R();
		break;

	case MEDIA_DVD_P_R9:
		sMedienType = OnMedia_DVD_P_R9();
		break;

	//identified RW medien (CD-RW, DVD-RW, DVD+RW)
	//------------------------------------------------
	case MEDIA_CDRW:
		sMedienType = OnMedia_CDRW();
		break;
		
	case MEDIA_DVD_M_RW:
	case MEDIA_DVD_P_RW:
		sMedienType = OnMedia_DVD_PW_RW(MediaType);
		break;

	//identified RAM medien (DVD+RAM)
	//------------------------------------------------
	case MEDIA_DVD_RAM:
		m_dwNumOfTracks ? m_MediumInfo = MI_DVDRWDELETE : m_MediumInfo = MI_DVDRWOK;
		break;


	//NOT YET identified medien (CD-RW, DVD-RW, DVD+RW)
	//------------------------------------------------
	case MEDIA_CD:		//CD-R or CD-RW
		sMedienType = OnMedia_CD();
		break;

	case MEDIA_DVD_M:	//DVD-R or DVD-RW
		sMedienType = OnMedia_DVD_M();
		break;

	case MEDIA_DVD_P:	//DVD+R or DVD+RW
		sMedienType = OnMedia_DVD_P();
		break;
		
	case MEDIA_DDCD: //Double Datarate CD-R or CD-RW
	case MEDIA_NONE:
	default:
		m_MediumInfo = MI_UNKNOWN_MEDIUM;
		sMedienType = UNKNOWN_MEDIA_TYPE;
		break;
	}

	return sMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_CDROM 
 Description   : Set capacity of a medium and set medium type to MI_CDREADONLY

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_CDROM >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_CDROM>
*********************************************************************************************/
CString CBurnContext::OnMedia_CDROM()
{
	m_MediumInfo = MI_CDREADONLY;
	m_ullMediumFreeCapacityInBytes = 0;
	m_ullMediumUsedCapacityInBytes = 0;

	return _T("Medientype: read only");
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_ROM 
 Description   : Set capacity of a medium and set medium type to MI_DVDREADONLY

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_ROM >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_ROM>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_ROM()
{
	m_MediumInfo = MI_DVDREADONLY;
	m_ullMediumFreeCapacityInBytes = 0;
	m_ullMediumUsedCapacityInBytes = 0;
	
	return _T("Medientype: DVD-ROM");
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_CDR 
 Description   : Set capacity of a medium and set medium type to MI_CDOK or MI_CDREADONLY

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_CDR >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_CDR>
*********************************************************************************************/
CString CBurnContext::OnMedia_CDR()
{
	CString sRetMedienType;
	if(!m_dwNumOfTracks)
	{
		//no tracks, empty CD-R
		m_MediumInfo = MI_CDOK;
		sRetMedienType = _T("Medientype: CD-R");
	}
	else
	{
		m_MediumInfo = MI_CDREADONLY;
		m_ullMediumFreeCapacityInBytes = 0;
		m_ullMediumUsedCapacityInBytes = 0;
		sRetMedienType = _T("Medientype: read only");
	}

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_M_R 
 Description   : Set capacity of a medium and set medium type to MI_DVDOK or MI_DVDREADONLY

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_M_R >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_M_R>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_M_R()
{
	CString sRetMedienType;
	if(!m_dwNumOfTracks)
	{
		//no tracks, empty DVD-R
		m_MediumInfo = MI_DVDOK;
		sRetMedienType = _T("Medientype: DVD-R");
	}
	else
	{
		m_MediumInfo = MI_DVDREADONLY;
		m_ullMediumFreeCapacityInBytes = 0;
		m_ullMediumUsedCapacityInBytes = 0;
		sRetMedienType = _T("Medientype: DVD-ROM");
	}

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_P_R 
 Description   : Set capacity of a medium and set medium type to MI_DVDOK or MI_DVDREADONLY

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_P_R >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_P_R>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_P_R()
{
	CString sRetMedienType;
	if(!m_dwNumOfTracks)
	{
		//no tracks, empty DVD+R
		m_MediumInfo = MI_DVDOK;
		sRetMedienType = _T("Medientype: DVD+R");
	}
	else
	{	
		m_MediumInfo = MI_DVDREADONLY;
		m_ullMediumFreeCapacityInBytes = 0;
		m_ullMediumUsedCapacityInBytes = 0;
		sRetMedienType = _T("Medientype: DVD-ROM");
	}

	return sRetMedienType;
}

/*********************************************************************************************
Class		   : CBurnContext
Function      : OnMedia_DVD_P_R9 
Description   : Set capacity of a medium and set medium type to MI_DVDOK or MI_DVDREADONLY

Parameters:  --

Result type:  medium type  (CString)

Author: TKR
created: March, 23 2005
<TITLE OnMedia_DVD_P_R9 >
<GROUP CheckMediaType>
<TOPICORDER 0>
<KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_P_R9>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_P_R9()
{
	CString sRetMedienType;
	if(!m_dwNumOfTracks)
	{
		//no tracks, empty DVD+R9
		m_MediumInfo = MI_DVDOK;
		sRetMedienType = _T("Medientype: DVD+R Double Layer");
	}
	else
	{	
		m_MediumInfo = MI_DVDREADONLY;
		m_ullMediumFreeCapacityInBytes = 0;
		m_ullMediumUsedCapacityInBytes = 0;
		sRetMedienType = _T("Medientype: DVD-ROM");
	}

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_CDRW 
 Description   : Set capacity of a medium and set medium type to MI_CDRWOK or MI_CDRWDELETE

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_CDRW >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_CDRW>
*********************************************************************************************/
CString CBurnContext::OnMedia_CDRW()
{
	CString sRetMedienType = _T("Medientype: CD-RW");
	if(!m_bCheckMediaCapacity)
	{
		m_MediumInfo = MI_CDRWDELETE;

		//there was an error getting unused blocks on CD
		//set default free capacity (max) to provide backup
		m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityCD;
		if(m_ullMediumFreeCapacityInBytes > m_ullMediumUsedCapacityInBytes)
		{
			m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
		}
		else
		{
			//fake used capacity to force deleting medium
			m_ullMediumUsedCapacityInBytes = 5 * 1024 * 1024;
			m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
		}

	}
	else
	{
		if(m_dwNumOfTracks)
		{
			//CD-RW not empty
			if(    m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
				|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer)
			{
				//free capacity was not set, has not changed, because nr of unusedblocks was 0
				m_ullMediumFreeCapacityInBytes -=  m_ullMediumUsedCapacityInBytes;
			}
			m_MediumInfo = MI_CDRWDELETE;
		}
		else
		{
			//empty CD-RW 
			m_MediumInfo = MI_CDRWOK;
		}
	}

	return sRetMedienType;
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_PW_RW 
 Description   : Set capacity of a medium and set medium type to MI_DVDRWOK or MI_DVDRWDELETE

 Parameters:  
  MediaType (i): media type of medium (DVD+RW or DVD-RW) (NERO_MEDIA_TYPE)

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_PW_RW >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_PW_RW>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_PW_RW(NERO_MEDIA_TYPE MediaType)
{
	CString sRetMedienType;
		
	if(!m_bCheckMediaCapacity)
	{
		m_MediumInfo = MI_DVDRWDELETE;
		
		//there was an error getting unused blocks on DVD
		//set default free capacity (max) to provide backup
		m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityDVD;
		if(m_ullMediumFreeCapacityInBytes > m_ullMediumUsedCapacityInBytes)
		{
			m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
		}
		else
		{
			//fake used capacity to force deleting medium
			m_ullMediumUsedCapacityInBytes = 5 * 1024 * 1024;
			m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
		}
	}
	else
	{
		if(m_dwNumOfTracks)
		{
			//DVD RW not empty
			if(    m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
				|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer)
			{
				//free capacity was not set, has not changed, because nr of unusedblocks was 0
				m_ullMediumFreeCapacityInBytes -=  m_ullMediumUsedCapacityInBytes;
			}
			m_MediumInfo = MI_DVDRWDELETE;
		}
		else
		{
			//empty DVD RW 
			m_MediumInfo = MI_DVDRWOK;
		}
	}
	MediaType == MEDIA_DVD_M_RW ? (sRetMedienType = _T("Medientype: DVD-RW")) : sRetMedienType = _T("Medientype: DVD+RW");

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_CD 
 Description   : If NeroAPI returns that medium is a CD, we have to use the older MediumType
				 (Nero said, should not be use anymore) to see if it is a CD-R or CD-RW.
				 Set capacity of a medium and set medium type to MI_CDOK, MI_CDREADONLY or 
				 MI_CDRWDELETE

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_CD >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_CD>
*********************************************************************************************/
CString CBurnContext::OnMedia_CD()
{
	CString sRetMedienType;
	NERO_MEDIUM_TYPE OldMediumType = m_NeroCDInfo->ncdiMediumType;
		
	if(OldMediumType == NMT_CD_RECORDABLE)
	{
		//CD-R
		if(!m_dwNumOfTracks)
		{
			//no tracks, empty CD-R
			m_MediumInfo = MI_CDOK;
			sRetMedienType = _T("Medientype: CD-R");
		}
		else
		{
			m_MediumInfo = MI_CDREADONLY;
			m_ullMediumFreeCapacityInBytes = 0;
			m_ullMediumUsedCapacityInBytes = 0;
			sRetMedienType = _T("Medientype: read only");
		}
	}
	else if(OldMediumType == NMT_CD_REWRITEABLE)
	{
		sRetMedienType = _T("Medientype: CD-RW");
		
		if(!m_bCheckMediaCapacity)
		{
			m_MediumInfo = MI_CDRWDELETE;

			//there was an error getting unused blocks on CD
			//set default free capacity (max) to provide backup
			m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityCD;
			if(m_ullMediumFreeCapacityInBytes > m_ullMediumUsedCapacityInBytes)
			{
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}
			else
			{
				//fake used capacity to force deleting medium
				m_ullMediumUsedCapacityInBytes = 5 * 1024 * 1024;
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}

		}
		else
		{
			if(m_dwNumOfTracks)
			{
				//CD-RW not empty
				if(    m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
					|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer)
				{
					//free capacity was not set, has not changed, because nr of unusedblocks was 0
					m_ullMediumFreeCapacityInBytes -=  m_ullMediumUsedCapacityInBytes;
				}
				m_MediumInfo = MI_CDRWDELETE;
			}
			else
			{
				//empty CD-RW 
				m_MediumInfo = MI_CDRWOK;
			}
		}
	}
	else
	{
		m_MediumInfo = MI_UNKNOWN_MEDIUM;
		sRetMedienType = UNKNOWN_MEDIA_TYPE;
	}

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_M 
 Description   : If NeroAPI returns that medium is a DVD-, we have to use the older MediumType
				 (Nero said, should not be use anymore) to see if it is a R or RW medium.
				 Set capacity of a medium and set medium type to MI_DVDOK, MI_DVDREADONLY or 
				 MI_DVDRWDELETE

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_M >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_M>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_M()
{
	CString sRetMedienType;
	NERO_MEDIUM_TYPE OldMediumType = m_NeroCDInfo->ncdiMediumType;

	if(OldMediumType == NMT_CD_RECORDABLE)
	{
		//DVD-R
		if(!m_dwNumOfTracks)
		{
			//no tracks, empty DVD-R
			m_MediumInfo = MI_DVDOK;
			sRetMedienType = _T("Medientype: DVD-R");
		}
		else
		{
			m_MediumInfo = MI_DVDREADONLY;
			m_ullMediumFreeCapacityInBytes = 0;
			m_ullMediumUsedCapacityInBytes = 0;
			sRetMedienType = _T("Medientype: DVD-ROM");
		}
	}
	else if(OldMediumType == NMT_CD_REWRITEABLE)
	{
		sRetMedienType = _T("Medientype: DVD-RW");
		if(!m_bCheckMediaCapacity)
		{
			m_MediumInfo = MI_DVDRWDELETE;
			
			//there was an error getting unused blocks on DVD
			//set default free capacity (max) to provide backup
			m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityDVD;
			if(m_ullMediumFreeCapacityInBytes > m_ullMediumUsedCapacityInBytes)
			{
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}
			else
			{
				//fake used capacity to force deleting medium
				m_ullMediumUsedCapacityInBytes = 5 * 1024 * 1024;
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}
		}
		else
		{
			if(m_dwNumOfTracks)
			{
				//DVD-RW not empty
				if(    m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
					|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer)
				{
					//free capacity was not set, has not changed, because nr of unusedblocks was 0
					m_ullMediumFreeCapacityInBytes -=  m_ullMediumUsedCapacityInBytes;
				}
				m_MediumInfo = MI_DVDRWDELETE;
			}
			else
			{
				//empty DVD-RW 
				m_MediumInfo = MI_DVDRWOK;
			}
		}
	}
	else
	{
		m_MediumInfo = MI_UNKNOWN_MEDIUM;
		sRetMedienType = UNKNOWN_MEDIA_TYPE;
	}

	return sRetMedienType;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OnMedia_DVD_P 
 Description   : If NeroAPI returns that medium is a DVD+, we have to use the older MediumType
				 (Nero said, should not be use anymore) to see if it is a R or RW medium.
				 Set capacity of a medium and set medium type to MI_DVDOK, MI_DVDREADONLY or 
				 MI_DVDRWDELETE

 Parameters:  --

 Result type:  medium type  (CString)

 Author: TKR
 created: July, 03 2003
 <TITLE OnMedia_DVD_P >
 <GROUP CheckMediaType>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, CheckMediaType, OnMedia_DVD_P>
*********************************************************************************************/
CString CBurnContext::OnMedia_DVD_P()
{
	CString sRetMedienType;
	NERO_MEDIUM_TYPE OldMediumType = m_NeroCDInfo->ncdiMediumType;

	if(OldMediumType == NMT_CD_RECORDABLE)
	{
		//DVD+R
		if(!m_dwNumOfTracks)
		{
			//no tracks, empty DVD+R
			m_MediumInfo = MI_DVDOK;
			sRetMedienType = _T("Medientype: DVD+R");
		}
		else
		{
			m_MediumInfo = MI_DVDREADONLY;
			m_ullMediumFreeCapacityInBytes = 0;
			m_ullMediumUsedCapacityInBytes = 0;
			sRetMedienType = _T("Medientype: DVD+ROM");
		}
	}
	else if(OldMediumType == NMT_CD_REWRITEABLE)
	{
		sRetMedienType = _T("Medientype: DVD+RW");

		if(!m_bCheckMediaCapacity)
		{
			m_MediumInfo = MI_DVDRWDELETE;
			
			//there was an error getting unused blocks on DVD
			//set default free capacity (max) to provide backup
			m_ullMediumFreeCapacityInBytes = m_ullMaxCapacityDVD;
			if(m_ullMediumFreeCapacityInBytes > m_ullMediumUsedCapacityInBytes)
			{
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}
			else
			{
				//fake used capacity to force deleting medium
				m_ullMediumUsedCapacityInBytes = 5 * 1024 * 1024;
				m_ullMediumFreeCapacityInBytes -= m_ullMediumUsedCapacityInBytes;
			}
		}
		else
		{
			if(m_dwNumOfTracks)
			{
				//DVD+RW not empty
				if(    m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVD
					|| m_ullMediumFreeCapacityInBytes == m_ullMaxCapacityDVDDoubleLayer)
				{
					//free capacity was not set, has not changed, because nr of unusedblocks was 0
					m_ullMediumFreeCapacityInBytes -=  m_ullMediumUsedCapacityInBytes;
				}
				m_MediumInfo = MI_DVDRWDELETE;
			}
			else
			{
				//empty DVD-RW 
				m_MediumInfo = MI_DVDRWOK;
			}
		}
	}
	else
	{
		m_MediumInfo = MI_UNKNOWN_MEDIUM;
		sRetMedienType = UNKNOWN_MEDIA_TYPE;
	}

	return sRetMedienType;
}