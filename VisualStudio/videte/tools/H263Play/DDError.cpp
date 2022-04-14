// DDError.cpp

#include "stdafx.h"

#include "DDError.h"

////////////////////////////////////////////////////////////////////
CString GetDDError(HRESULT ddrval)
{
	CString sError;

	switch (ddrval)
	{
	case DDERR_ALREADYINITIALIZED:
		sError = "This object is already initialized\n";
		break;

	case DDERR_CANNOTATTACHSURFACE:
		sError = "This surface can not be attached to the requested surface.\n";
		break;
		
	case DDERR_CANNOTDETACHSURFACE:
		sError = "This surface can not be detached from the requested surface.\n";
		break;
		
	case DDERR_CURRENTLYNOTAVAIL:
		sError = "Support is currently not available.\n";
		break;
		
	case DDERR_EXCEPTION:
		sError = "An exception was encountered while performing the requested operation\n";
		break;
		
	case DDERR_GENERIC:
		sError = "Generic failure.\n";
		break;
		
	case DDERR_HEIGHTALIGN:
		sError = "Height of rectangle provided is not a multiple of reqd alignment\n";
		break;
		
	case DDERR_INCOMPATIBLEPRIMARY:
		sError = "Unable to match primary surface creation request with existing primary surface.\n";
		break;
											 
	case DDERR_INVALIDCAPS:
		sError = "One or more of the caps bits passed to the callback are incorrect.\n";
		break;
									 
	case DDERR_INVALIDCLIPLIST:
		sError = "DirectDraw does not support provided Cliplist.\n";
		break;
										 
	case DDERR_INVALIDMODE:
		sError = "DirectDraw does not support the requested mode\n";
		break;
									 
	case DDERR_INVALIDOBJECT:
		sError = "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\n";
		break;
										 
	case DDERR_INVALIDPARAMS:
		sError = "One or more of the parameters passed to the callback function are incorrect.\n";
		break;
	
	case DDERR_INVALIDPIXELFORMAT:
		sError = "pixel format was invalid as specified\n";
		break;
										 
	case DDERR_INVALIDRECT:
		sError = "Rectangle provided was invalid.\n";
		break;
									 
	case DDERR_LOCKEDSURFACES:
		sError = "Operation could not be carried out because one or more surfaces are locked\n";
		break;
										 
	case DDERR_NO3D:
		sError = "There is no 3D present.\n";
		break;
								 
	case DDERR_NOALPHAHW:
		sError = "Operation could not be carried out because there is no alpha accleration hardware present or available.\n";
		break;
										 
	case DDERR_NOCLIPLIST:
		sError = "no clip list available\n";
		break;
									 
	case DDERR_NOCOLORCONVHW:
		sError = "Operation could not be carried out because there is no color conversion hardware present or available.\n";
		break;
										 
	case DDERR_NOCOOPERATIVELEVELSET:
		sError = "Create function called without DirectDraw object method SetCooperativeLevel being called.\n";
		break;
											 
	case DDERR_NOCOLORKEY:
		sError = "Surface doesn't currently have a color key\n";
		break;
									 
	case DDERR_NOCOLORKEYHW:
		sError = "Operation could not be carried out because there is no hardware support of the dest color key.\n";
		break;
									 
	case DDERR_NODIRECTDRAWSUPPORT:
		sError = "No DirectDraw support possible with current display driver\n";
		break;
										 
	case DDERR_NOEXCLUSIVEMODE:
		sError = "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\n";
		break;
										 
	case DDERR_NOFLIPHW:
		sError = "Flipping visible surfaces is not supported.\n";
		break;
									 
	case DDERR_NOGDI:
		sError = "There is no GDI present.\n";
		break;
									 
	case DDERR_NOMIRRORHW:
		sError = "Operation could not be carried out because there is no hardware present or available.\n";
		break;
									 
	case DDERR_NOTFOUND:
		sError = "Requested item was not found\n";
		break;
									 
	case DDERR_NOOVERLAYHW:
		sError = "Operation could not be carried out because there is no overlay hardware present or available.\n";
		break;
									 
	case DDERR_NORASTEROPHW:
		sError = "Operation could not be carried out because there is no appropriate raster op hardware present or available.\n";
		break;
								 
	case DDERR_NOROTATIONHW:
		sError = "Operation could not be carried out because there is no rotation hardware present or available.\n";
		break;
									 
	case DDERR_NOSTRETCHHW:
		sError = "Operation could not be carried out because there is no hardware support for stretching\n";
		break;
									 
	case DDERR_NOT4BITCOLOR:
		sError = "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\n";
		break;
									 
	case DDERR_NOT4BITCOLORINDEX:
		sError = "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\n";
		break;
											 
	case DDERR_NOT8BITCOLOR:
		sError = "DirectDraw Surface is not in 8 bit color mode and the requested operation requires 8 bit color.\n";
		break;
									 
	case DDERR_NOTEXTUREHW:
		sError = "Operation could not be carried out because there is no texture mapping hardware present or available.\n";
		break;
									 
	case DDERR_NOVSYNCHW:
		sError = "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\n";
		break;
										 
	case DDERR_NOZBUFFERHW:
		sError = "Operation could not be carried out because there is no hardware support for zbuffer blting.\n";
		break;
									 
	case DDERR_NOZOVERLAYHW:
		sError = "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\n";
		break;
									 
	case DDERR_OUTOFCAPS:
		sError = "The hardware needed for the requested operation has already been allocated.\n";
		break;
										 
	case DDERR_OUTOFMEMORY:
		sError = "DirectDraw does not have enough memory to perform the operation.\n";
		break;
	
	case DDERR_OUTOFVIDEOMEMORY:
		sError = "DirectDraw does not have enough memory to perform the operation.\n";
		break;
										 
	case DDERR_OVERLAYCANTCLIP:
		sError = "hardware does not support clipped overlays\n";
		break;

	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		sError = "Can only have ony color key active at one time for overlays\n";
		break;
											 
	case DDERR_PALETTEBUSY:
		sError = "Access to this palette is being refused because the palette is already locked by another thread.\n";
		break;
									 
	case DDERR_COLORKEYNOTSET:
		sError = "No src color key specified for this operation.\n";
		break;
										 
	case DDERR_SURFACEALREADYATTACHED:
		sError = "This surface is already attached to the surface it is being attached to.\n";
		break;
										 
	case DDERR_SURFACEALREADYDEPENDENT:
		sError = "This surface is already a dependency of the surface it is being made a dependency of.\n";
		break;
											 
	case DDERR_SURFACEBUSY:
		sError = "Access to this surface is being refused because the surface is already locked by another thread.\n";
		break;
									 
	case DDERR_CANTLOCKSURFACE:
		sError = "Access to this surface is being refused because no driver exists which can supply a pointer to the surface.\n";
		sError += "This is most likely to happen when attempting to lock the primary\n";
		sError += "surface when no DCI provider is present.\n";
		sError += "Will also happen on attempts to lock an optimized surface.\n";
		break;
												  
	case DDERR_SURFACEISOBSCURED:
		sError = "Access to Surface refused because Surface is obscured.\n";
		break;
											 
	case DDERR_SURFACELOST:
		sError = "Access to this surface is being refused because the surface is gone.\n";
		sError += "The DIRECTDRAWSURFACE object representing this surface should\n";
		sError += "have Restore called on it.\n";
		break;
									 
	case DDERR_SURFACENOTATTACHED:
		sError = "The requested surface is not attached.\n";
		break;
										 
	case DDERR_TOOBIGHEIGHT:
		sError = "Height requested by DirectDraw is too large.\n";
		break;
									 
	case DDERR_TOOBIGSIZE:
		sError = "Size requested by DirectDraw is too large --	 The individual height and width are OK.\n";
		break;
									 
	case DDERR_TOOBIGWIDTH:
		sError = "Width requested by DirectDraw is too large.\n";
		break;
									 
	case DDERR_UNSUPPORTED:
		sError = "Action not supported.\n";
		break;
	
	case DDERR_UNSUPPORTEDFORMAT:
		sError = "FOURCC format requested is unsupported by DirectDraw\n";
		break;
											 
	case DDERR_UNSUPPORTEDMASK:
		sError = "Bitmask in the pixel format requested is unsupported by DirectDraw\n";
		break;
									 
	case DDERR_VERTICALBLANKINPROGRESS:
		sError = "vertical blank is in progress\n";
		break;
											 
	case DDERR_WASSTILLDRAWING:
		sError = "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\n";
		break;
										 
	case DDERR_XALIGN:
		sError = "Rectangle provided was not horizontally aligned on reqd. boundary\n";
		break;
									 
	case DDERR_INVALIDDIRECTDRAWGUID:
		sError = "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\n";
		break;
											 
	case DDERR_DIRECTDRAWALREADYCREATED:
		sError = "A DirectDraw object representing this driver has already been created for this process.\n";
		break;
										 
	case DDERR_NODIRECTDRAWHW:
		sError = "A hardware only DirectDraw object creation was attempted but the driver did not support any hardware.\n";
		break;
										 
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		sError = "this process already has created a primary surface\n";
		break;
											 
	case DDERR_NOEMULATION:
		sError = "software emulation not available.\n";
		break;
									 
	case DDERR_REGIONTOOSMALL:
		sError = "region passed to Clipper::GetClipList is too small.\n";
		break;
										 
	case DDERR_CLIPPERISUSINGHWND:
		sError = "an attempt was made to set a clip list for a clipper objec that is already monitoring an hwnd.\n";
		break;
										 
	case DDERR_NOCLIPPERATTACHED:
		sError = "No clipper object attached to surface object\n";
		break;
											 
	case DDERR_NOHWND:
		sError = "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\n";
		break;
									 
	case DDERR_HWNDSUBCLASSED:
		sError = "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\n";
		break;
										 
	case DDERR_HWNDALREADYSET:
		sError = "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\n";
		break;
										 
	case DDERR_NOPALETTEATTACHED:
		sError = "No palette object attached to this surface.\n";
		break;
											 
	case DDERR_NOPALETTEHW:
		sError = "No hardware support for 16 or 256 color palettes.\n";
		break;
								 
	case DDERR_BLTFASTCANTCLIP:
		sError = "If a clipper object is attached to the source surface passed into a BltFast call.\n";
		break;
										 
	case DDERR_NOBLTHW:
		sError = "No blter.\n";
		break;
								 
	case DDERR_NODDROPSHW:
		sError = "No DirectDraw ROP hardware.\n";
		break;
									 
	case DDERR_OVERLAYNOTVISIBLE:
		sError = "returned when GetOverlayPosition is called on a hidden overlay\n";
		break;
											 
	case DDERR_NOOVERLAYDEST:
		sError = "returned when GetOverlayPosition is called on a overlay that UpdateOverlay has never been called on to establish a destionation.\n";
		break;
										 
	case DDERR_INVALIDPOSITION:
		sError = "returned when the position of the overlay on the destionation is no longer legal for that destionation.\n";
		break;
										 
	case DDERR_NOTAOVERLAYSURFACE:
		sError = "returned when an overlay member is called for a non-overlay surface\n";
		break;
									 
	case DDERR_EXCLUSIVEMODEALREADYSET:
		sError = "An attempt was made to set the cooperative level when it was already set to exclusive.\n";
		break;
											 
	case DDERR_NOTFLIPPABLE:
		sError = "An attempt has been made to flip a surface that is not flippable.\n";
		break;
									 
	case DDERR_CANTDUPLICATE:
		sError = "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\n";
		break;

	case DDERR_NOTLOCKED:
		sError = "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\n";
		break;
										 
	case DDERR_CANTCREATEDC:
		sError = "Windows can not create any more DCs\n";
		break;
									 
	case DDERR_NODC:
		sError = "No DC was ever created for this surface.\n";
		break;
								 
	case DDERR_WRONGMODE:
		sError = "This surface can not be restored because it was created in a different mode.\n";
		break;
										 
	case DDERR_IMPLICITLYCREATED:
		sError = "This surface can not be restored because it is an implicitly created surface.\n";
		break;
											 
	case DDERR_NOTPALETTIZED:
		sError = "The surface being used is not a palette-based surface\n";
		break;
										 
	case DDERR_UNSUPPORTEDMODE:
		sError = "The display is currently in an unsupported mode\n";
		break;
										 
	case DDERR_NOMIPMAPHW:
		sError = "Operation could not be carried out because there is no mip-map texture mapping hardware present or available.\n";
		break;
								 
	case DDERR_INVALIDSURFACETYPE:
		sError = "The requested action could not be performed because the surface was of the wrong type.\n";
		break;
												  
	case DDERR_NOOPTIMIZEHW:
		sError = "Device does not support optimized surfaces, therefore no video memory optimized surfaces\n";
		break;
												  
	case DDERR_NOTLOADED:
		sError = "Surface is an optimized surface, but has not yet been allocated any memory\n";
		break;
												  
	case DDERR_DCALREADYCREATED:
		sError = "A DC has already been returned for this surface. Only one DC can be retrieved per surface.\n";
		break;
										 
	case DDERR_NONONLOCALVIDMEM:
		sError = "An attempt was made to allocate non-local video memory from a device that does not support non-local video memory.\n";
		break;
												  
	case DDERR_CANTPAGELOCK:
		sError = "The attempt to page lock a surface failed.\n";
		break;
								 
	case DDERR_CANTPAGEUNLOCK:
		sError = "The attempt to page unlock a surface failed.\n";
		break;
										 
	case DDERR_NOTPAGELOCKED:
		sError = "An attempt was made to page unlock a surface with no outstanding page locks.\n";
		break;
										 
	case DDERR_MOREDATA:
		sError = "There is more data available than the specified buffer size could hold\n";
		break;
											 
	case DDERR_VIDEONOTACTIVE:
		sError = "The video port is not active\n";
		break;
											 
	case DDERR_DEVICEDOESNTOWNSURFACE:
		sError = "Surfaces created by one direct draw device cannot be used directly by another direct draw device.\n";
		break;
												 
	case DDERR_NOTINITIALIZED:
		sError = "An attempt was made to invoke an interface member of a DirectDraw object created by CoCreateInstance() before it was initialized.\n";
		break;
	}

	return sError;
}