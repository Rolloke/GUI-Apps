/* GlobalReplace: AccesssControl --> AccessControl */
/* GlobalReplace: Industrial --> IndustrialEquipment */
/* GlobalReplace: EntryControl --> AccessControl */
/* GlobalReplace: CashSystem --> CashSystems */
/* GlobalReplace: SDI_ZKS --> SDIAccessControl */
/* GlobalReplace: LocalStorageProcesses --> LocalProcesses */
/* GlobalReplace: IsStorage --> IsLocal */

// all boolean fields
// excluding IsLocal, IsTransmitter, IsReceiver which are calculated from the productCode
// ~~~~~~~~~~~~~~~ Do NOT change the order ~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~ Append new fields at the end,
// ~~~~~~~~~~~~~~~ so they are compatible to burned dongle data

	TRANSFER(RunLauncher);
	TRANSFER(RunSupervisor);
	TRANSFER(RunServer);
	TRANSFER(RunExplorer);

	TRANSFER(RunDatabaseClient);
	TRANSFER(RunDatabaseServer);
	TRANSFER(RunCDRWriter);

	// codecs
	TRANSFER(RunCoCoUnit);
	TRANSFER(RunMiCoUnit);
	TRANSFER(RunCoCoUnitPCI);
	TRANSFER(RunMiCoUnitPCI);

	// link
	TRANSFER(RunISDNUnit);
	TRANSFER(RunSocketUnit);
	
	// boxes
	TRANSFER(RunVCSUnit);
	TRANSFER(RunPTUnit);

	// misc unit
	TRANSFER(RunAKUUnit);
	TRANSFER(RunCommUnit);
	TRANSFER(RunGAUnit);
	TRANSFER(RunSDIUnit);
	TRANSFER(RunSimUnit);

	// misc
	TRANSFER(RunSecAnalyzer);
	TRANSFER(RunUpdateHandler);
	TRANSFER(RunServicePack);

	TRANSFER(RunCoCoTest);
	TRANSFER(RunMiCoTest);

	TRANSFER(RunMiCoUnit2);
	TRANSFER(RunMiCoUnit3);
	TRANSFER(RunMiCoUnit4);
	TRANSFER(RunJaCobUnit1);
	TRANSFER(RunJaCobUnit2);
	TRANSFER(RunJaCobUnit3);
	TRANSFER(RunJaCobUnit4);
	TRANSFER(RunGemosUnit);
	TRANSFER(RunICPCONUnit);
	TRANSFER(RunUSBCameraUnit);

	TRANSFER(RunSDIConfig);
	TRANSFER(RunHTTP);
	TRANSFER(RunICPCONClient);
	TRANSFER(RunClient4);
	TRANSFER(RunClient5);
	TRANSFER(RunClient6);
	TRANSFER(RunClient7);
	TRANSFER(RunClient8);
	TRANSFER(RunClient9);
	TRANSFER(RunClient10);

	TRANSFER(RunSaVicUnit1);
	TRANSFER(RunSaVicUnit2);
	TRANSFER(RunSaVicUnit3);
	TRANSFER(RunSaVicUnit4);
	TRANSFER(RunAudioUnit);
	TRANSFER(RunQUnit);
	TRANSFER(RunIPCameraUnit);
	TRANSFER(RunProgram8);
	TRANSFER(RunProgram9);
	TRANSFER(RunProgram10);

	TRANSFER(RunISDNUnit2);
	TRANSFER(RunPTUnit2);
	TRANSFER(RunSMSUnit);
	TRANSFER(RunTOBSUnit);
	TRANSFER(RunEMailUnit);
	TRANSFER(RunFAXUnit);
	TRANSFER(RunLinkUnit7);
	TRANSFER(RunLinkUnit8);
	TRANSFER(RunLinkUnit9);
	TRANSFER(RunLinkUnit10);


	TRANSFER(AllowSoftDecodeCoCo);
	TRANSFER(AllowSoftDecodeMiCo);
	TRANSFER(AllowHardDecodeCoCo);
	TRANSFER(AllowHardDecodeMiCo);

	TRANSFER(AllowLocalProcesses);

	TRANSFER(AllowSDICashSystems);
	TRANSFER(AllowSDIAccessControl);
	TRANSFER(AllowISDN2BChannels);
	TRANSFER(AllowTimers);
	TRANSFER(AllowMultiMonitor);
	TRANSFER(AllowHTMLmaps);
	TRANSFER(AllowSDIParkmanagment);
	TRANSFER(AllowFunctionality8);
	TRANSFER(AllowFunctionality9);
	TRANSFER(AllowFunctionality10);

	TRANSFER(AllowHTMLNotification);
	TRANSFER(AllowHTMLInformation);
	TRANSFER(AllowRouting);
	TRANSFER(AllowSDIIndustrialEquipment);
	TRANSFER(AllowSoftDecodePT);
	TRANSFER(AllowMotionDetection);
	TRANSFER(AllowAlarmOffSpans);
	TRANSFER(IsDemo);
	TRANSFER(RestrictedTo8CamerasPerBoard);
	TRANSFER(AllowFunctionality20);


	TRANSFER(AllowInternalAccess);
	TRANSFER(AllowEEPWrite);

#ifndef _HARDLOCK
	TRANSFER(RunTashaUnit1);
	TRANSFER(RunTashaUnit2);
	TRANSFER(RunTashaUnit3);
	TRANSFER(RunTashaUnit4);
#endif

// ~~~~~~~~~~~~~~~ Append new fields at the end,below
// ~~~~~~~~~~~~~~~ so they are compatible to burned dongle data
