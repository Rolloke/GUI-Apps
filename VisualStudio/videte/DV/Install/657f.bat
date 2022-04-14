@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

# force "flush output"
$|=1;

$DEST_DIR="p:\\programme\\intern\\dts\\patches\\657f";
$WISE_CMD="d:\\apps\\wise\\wise32.exe";
if (! -f $WISE_CMD)
{
	$WISE_CMD = "c:\\Programme\\wise\\wise32.exe"
}
$WISE_DIR="d:\\dts_657_project\\dv\\install";

# oem DTS is Original
print "$WISE_CMD /c \"$WISE_DIR\\657f.wse\" \n";
`$WISE_CMD /c \"$WISE_DIR\\657f.wse\" `;

&DoConfiguration("dts",0);
&DoConfiguration("vdr",1);
&DoConfiguration("sdr",3);

sub DoConfiguration
{ 
	local($oemName,$oemNumber)=@_;
	
	if (-d "$DEST_DIR\\Update")
	{
		print `rmdir /S /Q $DEST_DIR\\Update`;
	}

	print "v:\\bin\\tools\\MakeUpdateDisk.exe $DEST_DIR $oemNumber 657\n";
	`v:\\bin\\tools\\MakeUpdateDisk.exe $DEST_DIR $oemNumber 657`;

	if (-d "$DEST_DIR\\$oemName")
	{
		print `rmdir /S /Q $DEST_DIR\\$oemName`;
	}
	if (!(-d "$DEST_DIR\\$oemName"))
	{
		print "$DEST_DIR\\$oemName not found, OK\n";
		`mkdir $DEST_DIR\\$oemName`;
	}
	`copy $DEST_DIR\\Update\\Disk1\\disk $DEST_DIR\\$oemName\\disk`;
	`copy $DEST_DIR\\Update\\Disk1\\update $DEST_DIR\\$oemName\\update`;
}


__END__
:endofperl

