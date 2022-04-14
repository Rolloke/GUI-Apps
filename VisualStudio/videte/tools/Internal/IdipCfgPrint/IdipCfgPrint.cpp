

#include "IdipCfgPrint.h"
#include "stdio.h"

#include "wk_profile.h"
#include "SecID.h"
#include "Permission.h"
#include "User.h"
#include "Host.h"

static void DumpPermissions()
{
	printf (">>> Permissions: <<<\n");

	CPermissionArray permissions;
	CWK_Profile wkp;
	permissions.Load(wkp);

	for (int i=0;i<permissions.GetSize();i++) {
		const CPermission &onePermission = * permissions[i];
		printf ("Permission:  '%s' password '%s'\n",
			onePermission.GetName(),
			onePermission.GetPassword()
			);


	}
}

static void DumpUsers()
{
	printf (">>> Users: <<<\n");

	CWK_Profile wkp;
	CUserArray users;
	users.Load(wkp);
	for (int i=0;i<users.GetSize();i++) {
		const CUser &oneUser= * users[i];
		printf ("User: '%s' password '%s'\n",
			oneUser.GetName(),
			oneUser.GetPassword()
			);
		// NOT YET from permissionID to permissionName
	}
}

static void DumpHosts()
{
	printf (">>> Hosts: <<<\n");

	CHostArray hosts;
	CWK_Profile wkp;
	hosts.Load(wkp);
	for (int i=0;i<hosts.GetSize();i++) {
		const CHost &oneHost= * hosts[i];
		printf ("Hosts: '%s','%s'\n",
			oneHost.GetName(),
			oneHost.GetNumber()
			);

	}
}


int main(int argc, char ** argv)
{
	// NOT YET any command line switches

	DumpPermissions();
	DumpUsers();
	DumpHosts();

	return 0;
}