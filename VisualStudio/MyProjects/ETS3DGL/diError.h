
   switch (hr)
   {
      case DI_BUFFEROVERFLOW: REPORT("The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.");
      case DI_DOWNLOADSKIPPED: REPORT("The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.");
      case DI_EFFECTRESTARTED: REPORT("The effect was stopped, the parameters were updated, and the effect was restarted.");
//      case DI_NOEFFECT: REPORT("The operation had no effect. This value is equal to the S_FALSE standard COM return value.");
//      case DI_NOTATTACHED: REPORT("The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value.");
      case DI_OK: REPORT("The operation completed successfully. This value is equal to the S_OK standard COM return value.");
      case DI_POLLEDDEVICE: REPORT("The device is a polled device. As a result, device buffering will not collect any data and event notifications will not be signaled until the IDirectInputDevice2::Poll method is called.");
//      case DI_PROPNOEFFECT: REPORT("The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value.");
      case DI_TRUNCATED: REPORT("The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.");
      case DI_TRUNCATEDANDRESTARTED: REPORT("Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.");
      case DIERR_ACQUIRED: REPORT("The operation cannot be performed while the device is acquired.");
      case DIERR_ALREADYINITIALIZED: REPORT("This object is already initialized");
      case DIERR_BADDRIVERVER: REPORT("The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.");
      case DIERR_BETADIRECTINPUTVERSION: REPORT("The application was written for an unsupported prerelease version of DirectInput.");
      case DIERR_DEVICEFULL: REPORT("The device is full.");
      case DIERR_DEVICENOTREG: REPORT("The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.");
//      case DIERR_EFFECTPLAYING: REPORT("The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.);
//      case DIERR_HASEFFECTS: REPORT("The device cannot be reinitialized because there are still effects attached to it.");
      case DIERR_GENERIC: REPORT("An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.");
      case DIERR_HANDLEEXISTS: REPORT("The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.");
      case DIERR_INCOMPLETEEFFECT: REPORT("The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.");
      case DIERR_INPUTLOST: REPORT("Access to the input device has been lost. It must be reacquired.");
      case DIERR_INVALIDPARAM: REPORT("An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.");
      case DIERR_MOREDATA: REPORT("Not all the requested information fitted into the buffer.");
      case DIERR_NOAGGREGATION: REPORT("This object does not support aggregation.");
      case DIERR_NOINTERFACE: REPORT("The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value.");
      case DIERR_NOTACQUIRED: REPORT("The operation cannot be performed unless the device is acquired.");
      case DIERR_NOTBUFFERED: REPORT("The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.");
      case DIERR_NOTDOWNLOADED: REPORT("The effect is not downloaded.");
      case DIERR_NOTEXCLUSIVEACQUIRED: REPORT("The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.");
      case DIERR_NOTFOUND: REPORT("The requested object does not exist.");
      case DIERR_NOTINITIALIZED: REPORT("This object has not been initialized.");
//      case DIERR_OBJECTNOTFOUND: REPORT("The requested object does not exist.");
      case DIERR_OLDDIRECTINPUTVERSION: REPORT("The application requires a newer version of DirectInput.");
//      case DIERR_OTHERAPPHASPRIO: REPORT("Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background.");
      case DIERR_OUTOFMEMORY: REPORT("The DirectInput subsystem couldn't allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.");
//      case DIERR_READONLY: REPORT("The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value.");
      case DIERR_REPORTFULL: REPORT("More information was requested to be sent than can be sent to the device.");
      case DIERR_UNPLUGGED: REPORT("The operation could not be completed because the device is not plugged in.");
      case DIERR_UNSUPPORTED: REPORT("The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.");
      case E_PENDING: REPORT("Data is not yet available.");
   }
