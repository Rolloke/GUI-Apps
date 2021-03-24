<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="con-dil" urn="urn:adsk.eagle:library:140">
<description>&lt;b&gt;DIL Ribbon Cable Connectors&lt;/b&gt;&lt;p&gt;
Based on Harting material.&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="IC24P" urn="urn:adsk.eagle:footprint:6592/1" library_version="2">
<description>&lt;b&gt;DIL CABLE CONNECTOR&lt;/b&gt;</description>
<wire x1="-15.367" y1="4.064" x2="-16.637" y2="4.064" width="0.1524" layer="21"/>
<wire x1="-15.367" y1="4.064" x2="-15.367" y2="-4.064" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="-4.064" x2="-15.367" y2="-4.064" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="4.064" x2="-16.637" y2="2.286" width="0.1524" layer="21"/>
<wire x1="-16.002" y1="2.286" x2="-16.637" y2="2.286" width="0.1524" layer="21"/>
<wire x1="-16.002" y1="2.286" x2="-16.002" y2="-2.286" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="-2.286" x2="-16.002" y2="-2.286" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="-2.286" x2="-16.637" y2="-4.064" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="9.398" x2="-16.637" y2="4.064" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="-4.064" x2="-16.637" y2="-8.89" width="0.1524" layer="21"/>
<wire x1="16.637" y1="8.89" x2="16.637" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="-16.637" y1="-8.89" x2="-16.129" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="-16.129" y1="-9.398" x2="-14.605" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="16.129" y1="9.398" x2="14.605" y2="9.398" width="0.1524" layer="21"/>
<wire x1="16.129" y1="9.398" x2="16.637" y2="8.89" width="0.1524" layer="21"/>
<wire x1="14.605" y1="-9.398" x2="14.605" y2="-12.7" width="0.1524" layer="21"/>
<wire x1="14.605" y1="-9.398" x2="16.637" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="-12.7" x2="-14.605" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="-9.398" x2="14.605" y2="-9.398" width="0.1524" layer="21"/>
<wire x1="14.605" y1="9.398" x2="14.605" y2="9.779" width="0.1524" layer="21"/>
<wire x1="14.605" y1="9.398" x2="-14.605" y2="9.398" width="0.1524" layer="21"/>
<wire x1="14.605" y1="9.779" x2="-14.605" y2="9.779" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="9.779" x2="-14.605" y2="9.398" width="0.1524" layer="21"/>
<wire x1="-14.605" y1="9.398" x2="-16.637" y2="9.398" width="0.1524" layer="21"/>
<wire x1="14.605" y1="-12.7" x2="8.89" y2="-12.065" width="0.1524" layer="21"/>
<wire x1="8.89" y1="-12.065" x2="3.81" y2="-12.7" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-12.7" x2="-7.62" y2="-12.065" width="0.1524" layer="21"/>
<wire x1="-7.62" y1="-12.065" x2="-14.605" y2="-12.7" width="0.1524" layer="21"/>
<pad name="1" x="-13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="-1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="-3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="21" x="-6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="22" x="-8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="23" x="-11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="24" x="-13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<text x="-15.113" y="-7.62" size="1.27" layer="21" ratio="10" rot="R180">1</text>
<text x="6.35" y="-1.27" size="1.778" layer="25" ratio="10" rot="R180">&gt;NAME</text>
<text x="6.35" y="3.81" size="1.778" layer="27" ratio="10" rot="R180">&gt;VALUE</text>
<text x="-12.7" y="-11.43" size="1.27" layer="21" ratio="10">Ribbon cables 24P</text>
</package>
</packages>
<packages3d>
<package3d name="IC24P" urn="urn:adsk.eagle:package:6622/1" type="box" library_version="2">
<description>DIL CABLE CONNECTOR</description>
<packageinstances>
<packageinstance name="IC24P"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="IC24P" urn="urn:adsk.eagle:symbol:6591/1" library_version="2">
<wire x1="3.81" y1="15.24" x2="-3.81" y2="15.24" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="-17.78" x2="-3.81" y2="15.24" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="-17.78" x2="3.81" y2="-17.78" width="0.4064" layer="94"/>
<wire x1="3.81" y1="15.24" x2="3.81" y2="-17.78" width="0.4064" layer="94"/>
<wire x1="-2.54" y1="12.7" x2="-1.27" y2="12.7" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="10.16" x2="-1.27" y2="10.16" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="7.62" x2="-1.27" y2="7.62" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="-1.27" y2="5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-1.27" y2="2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.27" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-1.27" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-5.08" x2="-1.27" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-12.7" x2="-1.27" y2="-12.7" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-10.16" x2="-1.27" y2="-10.16" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-7.62" x2="-1.27" y2="-7.62" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-15.24" x2="-1.27" y2="-15.24" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-15.24" x2="1.27" y2="-15.24" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-12.7" x2="1.27" y2="-12.7" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-10.16" x2="1.27" y2="-10.16" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-7.62" x2="1.27" y2="-7.62" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-5.08" x2="1.27" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="1.27" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="2.54" y1="0" x2="1.27" y2="0" width="0.6096" layer="94"/>
<wire x1="2.54" y1="2.54" x2="1.27" y2="2.54" width="0.6096" layer="94"/>
<wire x1="2.54" y1="5.08" x2="1.27" y2="5.08" width="0.6096" layer="94"/>
<wire x1="2.54" y1="7.62" x2="1.27" y2="7.62" width="0.6096" layer="94"/>
<wire x1="2.54" y1="10.16" x2="1.27" y2="10.16" width="0.6096" layer="94"/>
<wire x1="2.54" y1="12.7" x2="1.27" y2="12.7" width="0.6096" layer="94"/>
<text x="-3.81" y="-20.32" size="1.778" layer="96">&gt;VALUE</text>
<text x="-3.81" y="16.002" size="1.778" layer="95">&gt;NAME</text>
<pin name="11" x="-7.62" y="-12.7" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="9" x="-7.62" y="-7.62" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="7" x="-7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="5" x="-7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="3" x="-7.62" y="7.62" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="1" x="-7.62" y="12.7" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="10" x="-7.62" y="-10.16" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="8" x="-7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="6" x="-7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="4" x="-7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="2" x="-7.62" y="10.16" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="12" x="-7.62" y="-15.24" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="13" x="7.62" y="-15.24" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="14" x="7.62" y="-12.7" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="15" x="7.62" y="-10.16" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="16" x="7.62" y="-7.62" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="17" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="18" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="19" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="20" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="21" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="22" x="7.62" y="7.62" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="23" x="7.62" y="10.16" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="24" x="7.62" y="12.7" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="IC24P" urn="urn:adsk.eagle:component:6657/2" prefix="SV" uservalue="yes" library_version="2">
<description>&lt;b&gt;DIL CABLE CONNECTOR&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="IC24P" x="0" y="0"/>
</gates>
<devices>
<device name="" package="IC24P">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="10" pad="10"/>
<connect gate="1" pin="11" pad="11"/>
<connect gate="1" pin="12" pad="12"/>
<connect gate="1" pin="13" pad="13"/>
<connect gate="1" pin="14" pad="14"/>
<connect gate="1" pin="15" pad="15"/>
<connect gate="1" pin="16" pad="16"/>
<connect gate="1" pin="17" pad="17"/>
<connect gate="1" pin="18" pad="18"/>
<connect gate="1" pin="19" pad="19"/>
<connect gate="1" pin="2" pad="2"/>
<connect gate="1" pin="20" pad="20"/>
<connect gate="1" pin="21" pad="21"/>
<connect gate="1" pin="22" pad="22"/>
<connect gate="1" pin="23" pad="23"/>
<connect gate="1" pin="24" pad="24"/>
<connect gate="1" pin="3" pad="3"/>
<connect gate="1" pin="4" pad="4"/>
<connect gate="1" pin="5" pad="5"/>
<connect gate="1" pin="6" pad="6"/>
<connect gate="1" pin="7" pad="7"/>
<connect gate="1" pin="8" pad="8"/>
<connect gate="1" pin="9" pad="9"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:6622/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="switch-dil" urn="urn:adsk.eagle:library:375">
<description>&lt;b&gt;DIL Switches and Code Switches&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DIP03YL" urn="urn:adsk.eagle:footprint:27168/1" library_version="2">
<description>&lt;b&gt;DIL/CODE SWITCH&lt;/b&gt;&lt;p&gt;</description>
<wire x1="5.08" y1="-3.81" x2="5.08" y2="3.81" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="3.81" x2="-5.08" y2="-3.81" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-1.27" x2="-1.905" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-1.905" y1="-1.27" x2="-1.905" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="-1.905" y1="1.27" x2="-3.175" y2="1.27" width="0.254" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="-3.175" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.254" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="0.635" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="0.635" y1="1.27" x2="-0.635" y2="1.27" width="0.254" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-0.635" y2="-0.635" width="0.254" layer="21"/>
<wire x1="1.905" y1="-1.27" x2="3.175" y2="-1.27" width="0.254" layer="21"/>
<wire x1="3.175" y1="-1.27" x2="3.175" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="3.175" y1="1.27" x2="1.905" y2="1.27" width="0.254" layer="21"/>
<wire x1="1.905" y1="1.27" x2="1.905" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-3.175" y1="-0.9398" x2="-1.905" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="-3.175" y1="-0.9398" x2="-3.175" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-1.905" y1="-0.9398" x2="-1.905" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-3.175" y1="-0.635" x2="-1.905" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-3.175" y1="-0.635" x2="-3.175" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="-1.905" y1="-0.635" x2="-1.905" y2="1.27" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-0.9398" x2="0.635" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-0.9398" x2="-0.635" y2="-1.27" width="0.254" layer="21"/>
<wire x1="0.635" y1="-0.9398" x2="0.635" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-0.635" x2="0.635" y2="-0.635" width="0.254" layer="21"/>
<wire x1="-0.635" y1="-0.635" x2="-0.635" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="0.635" y1="-0.635" x2="0.635" y2="1.27" width="0.254" layer="21"/>
<wire x1="1.905" y1="-0.9398" x2="3.175" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="1.905" y1="-0.9398" x2="1.905" y2="-1.27" width="0.254" layer="21"/>
<wire x1="3.175" y1="-0.9398" x2="3.175" y2="-0.635" width="0.254" layer="21"/>
<wire x1="1.905" y1="-0.635" x2="3.175" y2="-0.635" width="0.254" layer="21"/>
<wire x1="1.905" y1="-0.635" x2="1.905" y2="-0.9398" width="0.254" layer="21"/>
<wire x1="3.175" y1="-0.635" x2="3.175" y2="1.27" width="0.254" layer="21"/>
<wire x1="5.08" y1="-3.81" x2="3.302" y2="-3.81" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-3.81" x2="-3.302" y2="-3.81" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="3.81" x2="-3.302" y2="3.81" width="0.1524" layer="21"/>
<wire x1="5.08" y1="3.81" x2="3.302" y2="3.81" width="0.1524" layer="21"/>
<wire x1="-3.302" y1="-3.81" x2="-1.778" y2="-3.81" width="0.1524" layer="51"/>
<wire x1="-1.778" y1="-3.81" x2="-0.762" y2="-3.81" width="0.1524" layer="21"/>
<wire x1="-0.762" y1="-3.81" x2="0.762" y2="-3.81" width="0.1524" layer="51"/>
<wire x1="0.762" y1="-3.81" x2="1.778" y2="-3.81" width="0.1524" layer="21"/>
<wire x1="1.778" y1="-3.81" x2="3.302" y2="-3.81" width="0.1524" layer="51"/>
<wire x1="3.302" y1="3.81" x2="1.778" y2="3.81" width="0.1524" layer="51"/>
<wire x1="1.778" y1="3.81" x2="0.762" y2="3.81" width="0.1524" layer="21"/>
<wire x1="0.762" y1="3.81" x2="-0.762" y2="3.81" width="0.1524" layer="51"/>
<wire x1="-0.762" y1="3.81" x2="-1.778" y2="3.81" width="0.1524" layer="21"/>
<wire x1="-1.778" y1="3.81" x2="-3.302" y2="3.81" width="0.1524" layer="51"/>
<circle x="-4.064" y="-2.794" radius="0.254" width="0.1524" layer="21"/>
<pad name="1" x="-2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="0" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="2.54" y="-3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="0" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-2.54" y="3.81" drill="0.8128" shape="long" rot="R90"/>
<text x="-2.921" y="-3.048" size="1.27" layer="51" ratio="10">1</text>
<text x="-0.381" y="-3.048" size="1.27" layer="51" ratio="10">2</text>
<text x="2.159" y="-3.048" size="1.27" layer="51" ratio="10">3</text>
<text x="-5.4102" y="-3.7592" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="4.7752" y="-3.1496" size="1.27" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<text x="-2.921" y="1.7018" size="1.27" layer="51" ratio="10">ON</text>
</package>
</packages>
<packages3d>
<package3d name="DIP03YL" urn="urn:adsk.eagle:package:27256/2" type="model" library_version="2">
<description>DIL/CODE SWITCH</description>
<packageinstances>
<packageinstance name="DIP03YL"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="DIP03" urn="urn:adsk.eagle:symbol:27095/1" library_version="2">
<wire x1="4.445" y1="5.08" x2="4.445" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="4.445" y1="5.08" x2="-4.445" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-4.445" y1="5.08" x2="-4.445" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-4.445" y1="-5.08" x2="4.445" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-2.032" y1="-2.54" x2="-3.048" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-3.048" y1="2.54" x2="-2.032" y2="2.54" width="0.1524" layer="94"/>
<wire x1="-2.032" y1="2.54" x2="-2.032" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-3.048" y1="-2.54" x2="-3.048" y2="2.54" width="0.1524" layer="94"/>
<wire x1="0.508" y1="-2.54" x2="-0.508" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-0.508" y1="2.54" x2="0.508" y2="2.54" width="0.1524" layer="94"/>
<wire x1="0.508" y1="2.54" x2="0.508" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-0.508" y1="-2.54" x2="-0.508" y2="2.54" width="0.1524" layer="94"/>
<wire x1="3.048" y1="-2.54" x2="2.032" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="2.032" y1="2.54" x2="3.048" y2="2.54" width="0.1524" layer="94"/>
<wire x1="3.048" y1="2.54" x2="3.048" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="2.032" y1="-2.54" x2="2.032" y2="2.54" width="0.1524" layer="94"/>
<text x="-5.08" y="-5.08" size="1.778" layer="95" rot="R90">&gt;NAME</text>
<text x="6.985" y="-5.08" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<text x="-2.794" y="-4.064" size="0.9906" layer="94" ratio="14">1</text>
<text x="-0.381" y="-4.064" size="0.9906" layer="94" ratio="14">2</text>
<text x="2.159" y="-4.064" size="0.9906" layer="94" ratio="14">3</text>
<text x="-3.302" y="3.048" size="0.9906" layer="94" ratio="14">ON</text>
<rectangle x1="-2.794" y1="-2.286" x2="-2.286" y2="0" layer="94"/>
<rectangle x1="-0.254" y1="-2.286" x2="0.254" y2="0" layer="94"/>
<rectangle x1="2.286" y1="-2.286" x2="2.794" y2="0" layer="94"/>
<pin name="4" x="2.54" y="7.62" visible="pad" length="short" direction="pas" rot="R270"/>
<pin name="5" x="0" y="7.62" visible="pad" length="short" direction="pas" rot="R270"/>
<pin name="6" x="-2.54" y="7.62" visible="pad" length="short" direction="pas" rot="R270"/>
<pin name="1" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="2" x="0" y="-7.62" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="3" x="2.54" y="-7.62" visible="pad" length="short" direction="pas" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="DIP03YL" urn="urn:adsk.eagle:component:27357/2" prefix="S" uservalue="yes" library_version="2">
<description>&lt;b&gt;DIL/CODE SWITCH&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="DIP03" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DIP03YL">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="2" pad="2"/>
<connect gate="1" pin="3" pad="3"/>
<connect gate="1" pin="4" pad="4"/>
<connect gate="1" pin="5" pad="5"/>
<connect gate="1" pin="6" pad="6"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:27256/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="con-lstb" urn="urn:adsk.eagle:library:162">
<description>&lt;b&gt;Pin Headers&lt;/b&gt;&lt;p&gt;
Naming:&lt;p&gt;
MA = male&lt;p&gt;
# contacts - # rows&lt;p&gt;
W = angled&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="MA03-2" urn="urn:adsk.eagle:footprint:8265/1" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-3.175" y1="2.54" x2="-1.905" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="2.54" x2="-1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="1.905" x2="-0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="2.54" x2="0.635" y2="2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="2.54" x2="1.27" y2="1.905" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="2.54" x2="-3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="1.27" y1="1.905" x2="1.905" y2="2.54" width="0.1524" layer="21"/>
<wire x1="1.905" y1="2.54" x2="3.175" y2="2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="2.54" x2="3.81" y2="1.905" width="0.1524" layer="21"/>
<wire x1="3.81" y1="1.905" x2="3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-1.905" x2="-1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-1.905" x2="0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-2.54" x2="-0.635" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-2.54" x2="-1.27" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="1.905" x2="-3.81" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-1.905" x2="-3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-2.54" x2="-3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-1.905" x2="3.175" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-2.54" x2="1.905" y2="-2.54" width="0.1524" layer="21"/>
<wire x1="1.905" y1="-2.54" x2="1.27" y2="-1.905" width="0.1524" layer="21"/>
<pad name="1" x="-2.54" y="-1.27" drill="1.016"/>
<pad name="3" x="0" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="2.54" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="2" x="-2.54" y="1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="0" y="1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="2.54" y="1.27" drill="1.016" shape="octagon"/>
<text x="-3.175" y="-4.191" size="1.27" layer="21" ratio="10">1</text>
<text x="-3.81" y="2.921" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="4.064" y="0.635" size="1.27" layer="21" ratio="10">6</text>
<text x="-1.27" y="-4.191" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-1.524" x2="0.254" y2="-1.016" layer="51"/>
<rectangle x1="-2.794" y1="-1.524" x2="-2.286" y2="-1.016" layer="51"/>
<rectangle x1="2.286" y1="-1.524" x2="2.794" y2="-1.016" layer="51"/>
<rectangle x1="-2.794" y1="1.016" x2="-2.286" y2="1.524" layer="51"/>
<rectangle x1="-0.254" y1="1.016" x2="0.254" y2="1.524" layer="51"/>
<rectangle x1="2.286" y1="1.016" x2="2.794" y2="1.524" layer="51"/>
</package>
<package name="MA05-1" urn="urn:adsk.eagle:footprint:8283/1" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-5.715" y1="1.27" x2="-4.445" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="1.27" x2="-3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="-0.635" x2="-4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="0.635" x2="-3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="-1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="1.27" x2="-1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-1.27" x2="-3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-1.27" x2="-3.81" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="0.635" x2="-6.35" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-5.715" y1="1.27" x2="-6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-6.35" y1="-0.635" x2="-5.715" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="-1.27" x2="-5.715" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="-1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-0.635" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.81" y1="0.635" x2="4.445" y2="1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="1.27" x2="5.715" y2="1.27" width="0.1524" layer="21"/>
<wire x1="5.715" y1="1.27" x2="6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="6.35" y1="0.635" x2="6.35" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-0.635" x2="5.715" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="5.715" y1="-1.27" x2="4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="-1.27" x2="3.81" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="1.27" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-1.27" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<pad name="1" x="-5.08" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="-2.54" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="0" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="4" x="2.54" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="5" x="5.08" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-6.35" y="1.651" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-5.715" y="-2.921" size="1.27" layer="21" ratio="10">1</text>
<text x="4.445" y="1.651" size="1.27" layer="21" ratio="10">5</text>
<text x="-2.54" y="-2.921" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="-2.794" y1="-0.254" x2="-2.286" y2="0.254" layer="51"/>
<rectangle x1="-5.334" y1="-0.254" x2="-4.826" y2="0.254" layer="51"/>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
<rectangle x1="4.826" y1="-0.254" x2="5.334" y2="0.254" layer="51"/>
<rectangle x1="2.286" y1="-0.254" x2="2.794" y2="0.254" layer="51"/>
</package>
<package name="MA04-1" urn="urn:adsk.eagle:footprint:8285/1" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-4.445" y1="1.27" x2="-3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="-2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-0.635" x2="-3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="0.635" x2="-1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="1.27" x2="-0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="0" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="-0.635" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="-1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-1.27" x2="-2.54" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="0.635" x2="-5.08" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="1.27" x2="-5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-0.635" x2="-4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-1.27" x2="-4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0" y1="0.635" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-0.635" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="-1.27" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="0" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="4.445" y2="1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="1.27" x2="5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="0.635" x2="5.08" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-0.635" x2="4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-0.635" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="-1.27" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<pad name="1" x="-3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="-1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="4" x="3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-5.08" y="1.651" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-6.223" y="-0.635" size="1.27" layer="21" ratio="10">1</text>
<text x="0.635" y="1.651" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="5.334" y="-0.635" size="1.27" layer="21" ratio="10">4</text>
<rectangle x1="-1.524" y1="-0.254" x2="-1.016" y2="0.254" layer="51"/>
<rectangle x1="-4.064" y1="-0.254" x2="-3.556" y2="0.254" layer="51"/>
<rectangle x1="1.016" y1="-0.254" x2="1.524" y2="0.254" layer="51"/>
<rectangle x1="3.556" y1="-0.254" x2="4.064" y2="0.254" layer="51"/>
</package>
</packages>
<packages3d>
<package3d name="MA03-2" urn="urn:adsk.eagle:package:8334/1" type="box" library_version="2">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="MA03-2"/>
</packageinstances>
</package3d>
<package3d name="MA05-1" urn="urn:adsk.eagle:package:8332/1" type="box" library_version="2">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="MA05-1"/>
</packageinstances>
</package3d>
<package3d name="MA04-1" urn="urn:adsk.eagle:package:8337/1" type="box" library_version="2">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="MA04-1"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="MA03-2" urn="urn:adsk.eagle:symbol:8264/1" library_version="2">
<wire x1="3.81" y1="-5.08" x2="-3.81" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="5.08" x2="-3.81" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-5.08" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-3.81" y1="5.08" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-1.27" y2="2.54" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.27" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="-2.54" x2="-1.27" y2="-2.54" width="0.6096" layer="94"/>
<text x="-3.81" y="-7.62" size="1.778" layer="96">&gt;VALUE</text>
<text x="-3.81" y="5.842" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="5" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="-7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="4" x="-7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1"/>
<pin name="6" x="-7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="MA05-1" urn="urn:adsk.eagle:symbol:8282/1" library_version="2">
<wire x1="3.81" y1="-7.62" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="2.54" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-1.27" y1="7.62" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-1.27" y1="7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="5.08" x2="2.54" y2="5.08" width="0.6096" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<text x="-1.27" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-1.27" y="8.382" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="4" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="5" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
<symbol name="MA04-1" urn="urn:adsk.eagle:symbol:8284/1" library_version="2">
<wire x1="3.81" y1="-7.62" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="2.54" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-1.27" y1="5.08" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-7.62" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-1.27" y1="5.08" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<text x="-1.27" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-1.27" y="5.842" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="4" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="MA03-2" urn="urn:adsk.eagle:component:8382/2" prefix="SV" uservalue="yes" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="MA03-2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MA03-2">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="2" pad="2"/>
<connect gate="1" pin="3" pad="3"/>
<connect gate="1" pin="4" pad="4"/>
<connect gate="1" pin="5" pad="5"/>
<connect gate="1" pin="6" pad="6"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:8334/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
<attribute name="POPULARITY" value="15" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MA05-1" urn="urn:adsk.eagle:component:8379/2" prefix="SV" uservalue="yes" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="MA05-1" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MA05-1">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:8332/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
<attribute name="POPULARITY" value="29" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MA04-1" urn="urn:adsk.eagle:component:8375/2" prefix="SV" uservalue="yes" library_version="2">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="MA04-1" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MA04-1">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="2" pad="2"/>
<connect gate="1" pin="3" pad="3"/>
<connect gate="1" pin="4" pad="4"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:8337/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
<attribute name="POPULARITY" value="41" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="SV1" library="con-dil" library_urn="urn:adsk.eagle:library:140" deviceset="IC24P" device="" package3d_urn="urn:adsk.eagle:package:6622/1"/>
<part name="S1" library="switch-dil" library_urn="urn:adsk.eagle:library:375" deviceset="DIP03YL" device="" package3d_urn="urn:adsk.eagle:package:27256/2"/>
<part name="SV2" library="con-lstb" library_urn="urn:adsk.eagle:library:162" deviceset="MA03-2" device="" package3d_urn="urn:adsk.eagle:package:8334/1"/>
<part name="SV3" library="con-lstb" library_urn="urn:adsk.eagle:library:162" deviceset="MA05-1" device="" package3d_urn="urn:adsk.eagle:package:8332/1"/>
<part name="SV4" library="con-lstb" library_urn="urn:adsk.eagle:library:162" deviceset="MA04-1" device="" package3d_urn="urn:adsk.eagle:package:8337/1"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="SV1" gate="1" x="35.56" y="58.42" smashed="yes">
<attribute name="VALUE" x="31.75" y="38.1" size="1.778" layer="96"/>
<attribute name="NAME" x="31.75" y="74.422" size="1.778" layer="95"/>
</instance>
<instance part="S1" gate="1" x="71.12" y="68.58" smashed="yes" rot="R90">
<attribute name="NAME" x="76.2" y="63.5" size="1.778" layer="95" rot="R180"/>
<attribute name="VALUE" x="76.2" y="75.565" size="1.778" layer="96" rot="R180"/>
</instance>
<instance part="SV2" gate="1" x="71.12" y="48.26" smashed="yes">
<attribute name="VALUE" x="67.31" y="40.64" size="1.778" layer="96"/>
<attribute name="NAME" x="67.31" y="54.102" size="1.778" layer="95"/>
</instance>
<instance part="SV3" gate="G$1" x="12.7" y="50.8" smashed="yes">
<attribute name="VALUE" x="11.43" y="40.64" size="1.778" layer="96"/>
<attribute name="NAME" x="11.43" y="59.182" size="1.778" layer="95"/>
</instance>
<instance part="SV4" gate="1" x="7.62" y="50.8" smashed="yes" rot="R180">
<attribute name="VALUE" x="8.89" y="60.96" size="1.778" layer="96" rot="R180"/>
<attribute name="NAME" x="8.89" y="44.958" size="1.778" layer="95" rot="R180"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="TX0" class="0">
<segment>
<pinref part="SV1" gate="1" pin="1"/>
<wire x1="27.94" y1="71.12" x2="25.4" y2="71.12" width="0.1524" layer="91"/>
<label x="25.4" y="71.12" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="RX1" class="0">
<segment>
<pinref part="SV1" gate="1" pin="2"/>
<wire x1="27.94" y1="68.58" x2="25.4" y2="68.58" width="0.1524" layer="91"/>
<label x="25.4" y="68.58" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="SV1" gate="1" pin="3"/>
<wire x1="27.94" y1="66.04" x2="25.4" y2="66.04" width="0.1524" layer="91"/>
<label x="25.4" y="66.04" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
<segment>
<pinref part="SV1" gate="1" pin="4"/>
<wire x1="27.94" y1="63.5" x2="25.4" y2="63.5" width="0.1524" layer="91"/>
<label x="25.4" y="63.5" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
<segment>
<pinref part="SV1" gate="1" pin="23"/>
<wire x1="43.18" y1="68.58" x2="45.72" y2="68.58" width="0.1524" layer="91"/>
<label x="45.72" y="68.58" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="S1" gate="1" pin="3"/>
<wire x1="78.74" y1="71.12" x2="81.28" y2="71.12" width="0.1524" layer="91"/>
<wire x1="81.28" y1="71.12" x2="81.28" y2="68.58" width="0.1524" layer="91"/>
<pinref part="S1" gate="1" pin="2"/>
<wire x1="81.28" y1="68.58" x2="78.74" y2="68.58" width="0.1524" layer="91"/>
<pinref part="S1" gate="1" pin="1"/>
<wire x1="78.74" y1="66.04" x2="81.28" y2="66.04" width="0.1524" layer="91"/>
<wire x1="81.28" y1="66.04" x2="81.28" y2="68.58" width="0.1524" layer="91"/>
<junction x="81.28" y="68.58"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="6"/>
<wire x1="63.5" y1="50.8" x2="60.96" y2="50.8" width="0.1524" layer="91"/>
<label x="60.96" y="50.8" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="PIN4" class="0">
<segment>
<pinref part="SV1" gate="1" pin="7"/>
<label x="25.4" y="55.88" size="1.778" layer="95" rot="R180" align="center-left"/>
<pinref part="SV3" gate="G$1" pin="5"/>
<wire x1="27.94" y1="55.88" x2="20.32" y2="55.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN5" class="0">
<segment>
<pinref part="SV1" gate="1" pin="8"/>
<label x="25.4" y="53.34" size="1.778" layer="95" rot="R180" align="center-left"/>
<pinref part="SV3" gate="G$1" pin="4"/>
<wire x1="27.94" y1="53.34" x2="20.32" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN6" class="0">
<segment>
<pinref part="SV1" gate="1" pin="9"/>
<label x="25.4" y="50.8" size="1.778" layer="95" rot="R180" align="center-left"/>
<pinref part="SV3" gate="G$1" pin="3"/>
<wire x1="27.94" y1="50.8" x2="20.32" y2="50.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN7" class="0">
<segment>
<pinref part="SV1" gate="1" pin="10"/>
<label x="25.4" y="48.26" size="1.778" layer="95" rot="R180" align="center-left"/>
<pinref part="SV3" gate="G$1" pin="2"/>
<wire x1="27.94" y1="48.26" x2="20.32" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN8" class="0">
<segment>
<pinref part="SV1" gate="1" pin="11"/>
<label x="25.4" y="45.72" size="1.778" layer="95" rot="R180" align="center-left"/>
<pinref part="SV3" gate="G$1" pin="1"/>
<wire x1="27.94" y1="45.72" x2="20.32" y2="45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN9" class="0">
<segment>
<pinref part="SV1" gate="1" pin="12"/>
<wire x1="27.94" y1="43.18" x2="25.4" y2="43.18" width="0.1524" layer="91"/>
<label x="25.4" y="43.18" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
<segment>
<pinref part="SV4" gate="1" pin="1"/>
<wire x1="0" y1="55.88" x2="-2.54" y2="55.88" width="0.1524" layer="91"/>
<label x="-2.54" y="55.88" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="RAW" class="0">
<segment>
<pinref part="SV1" gate="1" pin="24"/>
<wire x1="43.18" y1="71.12" x2="45.72" y2="71.12" width="0.1524" layer="91"/>
<label x="45.72" y="71.12" size="1.778" layer="95" align="center-left"/>
</segment>
</net>
<net name="RESET" class="0">
<segment>
<pinref part="SV1" gate="1" pin="22"/>
<wire x1="43.18" y1="66.04" x2="45.72" y2="66.04" width="0.1524" layer="91"/>
<label x="45.72" y="66.04" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="5"/>
<wire x1="78.74" y1="50.8" x2="81.28" y2="50.8" width="0.1524" layer="91"/>
<label x="81.28" y="50.8" size="1.778" layer="95" align="center-left"/>
</segment>
</net>
<net name="VCC" class="0">
<segment>
<pinref part="SV1" gate="1" pin="21"/>
<wire x1="43.18" y1="63.5" x2="45.72" y2="63.5" width="0.1524" layer="91"/>
<label x="45.72" y="63.5" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="2"/>
<wire x1="63.5" y1="45.72" x2="60.96" y2="45.72" width="0.1524" layer="91"/>
<label x="60.96" y="45.72" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="A3" class="0">
<segment>
<pinref part="SV1" gate="1" pin="20"/>
<wire x1="43.18" y1="60.96" x2="45.72" y2="60.96" width="0.1524" layer="91"/>
<label x="45.72" y="60.96" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="S1" gate="1" pin="4"/>
<wire x1="63.5" y1="71.12" x2="60.96" y2="71.12" width="0.1524" layer="91"/>
<label x="60.96" y="71.12" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="A2" class="0">
<segment>
<pinref part="SV1" gate="1" pin="19"/>
<wire x1="43.18" y1="58.42" x2="45.72" y2="58.42" width="0.1524" layer="91"/>
<label x="45.72" y="58.42" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="S1" gate="1" pin="5"/>
<wire x1="63.5" y1="68.58" x2="60.96" y2="68.58" width="0.1524" layer="91"/>
<label x="60.96" y="68.58" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="A1" class="0">
<segment>
<pinref part="SV1" gate="1" pin="18"/>
<wire x1="43.18" y1="55.88" x2="45.72" y2="55.88" width="0.1524" layer="91"/>
<label x="45.72" y="55.88" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="S1" gate="1" pin="6"/>
<wire x1="63.5" y1="66.04" x2="60.96" y2="66.04" width="0.1524" layer="91"/>
<label x="60.96" y="66.04" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="A0" class="0">
<segment>
<pinref part="SV1" gate="1" pin="17"/>
<wire x1="43.18" y1="53.34" x2="45.72" y2="53.34" width="0.1524" layer="91"/>
<label x="45.72" y="53.34" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV4" gate="1" pin="2"/>
<wire x1="0" y1="53.34" x2="-2.54" y2="53.34" width="0.1524" layer="91"/>
<label x="-2.54" y="53.34" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="SCLK" class="0">
<segment>
<pinref part="SV1" gate="1" pin="16"/>
<wire x1="43.18" y1="50.8" x2="45.72" y2="50.8" width="0.1524" layer="91"/>
<label x="45.72" y="50.8" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="3"/>
<wire x1="78.74" y1="48.26" x2="81.28" y2="48.26" width="0.1524" layer="91"/>
<label x="81.28" y="48.26" size="1.778" layer="95" align="center-left"/>
</segment>
</net>
<net name="MISO" class="0">
<segment>
<pinref part="SV1" gate="1" pin="15"/>
<wire x1="43.18" y1="48.26" x2="45.72" y2="48.26" width="0.1524" layer="91"/>
<label x="45.72" y="48.26" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="1"/>
<wire x1="78.74" y1="45.72" x2="81.28" y2="45.72" width="0.1524" layer="91"/>
<label x="81.28" y="45.72" size="1.778" layer="95" align="center-left"/>
</segment>
</net>
<net name="MOSI" class="0">
<segment>
<pinref part="SV1" gate="1" pin="14"/>
<wire x1="43.18" y1="45.72" x2="45.72" y2="45.72" width="0.1524" layer="91"/>
<label x="45.72" y="45.72" size="1.778" layer="95" align="center-left"/>
</segment>
<segment>
<pinref part="SV2" gate="1" pin="4"/>
<wire x1="63.5" y1="48.26" x2="60.96" y2="48.26" width="0.1524" layer="91"/>
<label x="60.96" y="48.26" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="PIN10" class="0">
<segment>
<pinref part="SV1" gate="1" pin="13"/>
<wire x1="43.18" y1="43.18" x2="45.72" y2="43.18" width="0.1524" layer="91"/>
<label x="45.72" y="43.18" size="1.778" layer="95" align="center-left"/>
</segment>
</net>
<net name="PIN3" class="0">
<segment>
<pinref part="SV1" gate="1" pin="6"/>
<wire x1="27.94" y1="58.42" x2="25.4" y2="58.42" width="0.1524" layer="91"/>
<label x="25.4" y="58.42" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
<segment>
<pinref part="SV4" gate="1" pin="4"/>
<wire x1="0" y1="48.26" x2="-2.54" y2="48.26" width="0.1524" layer="91"/>
<label x="-2.54" y="48.26" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
<net name="PIN2" class="0">
<segment>
<pinref part="SV1" gate="1" pin="5"/>
<wire x1="27.94" y1="60.96" x2="25.4" y2="60.96" width="0.1524" layer="91"/>
<label x="25.4" y="60.96" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
<segment>
<pinref part="SV4" gate="1" pin="3"/>
<wire x1="0" y1="50.8" x2="-2.54" y2="50.8" width="0.1524" layer="91"/>
<label x="-2.54" y="50.8" size="1.778" layer="95" rot="R180" align="center-left"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
<note version="9.0" severity="warning">
Since Version 9.0, EAGLE supports the align property for labels. 
Labels in schematic will not be understood with this version. Update EAGLE to the latest version 
for full support of labels. 
</note>
</compatibility>
</eagle>
