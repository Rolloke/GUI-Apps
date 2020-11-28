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
<layer number="96" name="Values" color="7" fill="1" visible="no" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="switch-omron" urn="urn:adsk.eagle:library:377">
<description>&lt;b&gt;Omron Switches&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="B3F-10XX" urn="urn:adsk.eagle:footprint:27476/1" library_version="3">
<description>&lt;b&gt;OMRON SWITCH&lt;/b&gt;</description>
<wire x1="3.302" y1="-0.762" x2="3.048" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="3.302" y1="-0.762" x2="3.302" y2="0.762" width="0.1524" layer="21"/>
<wire x1="3.048" y1="0.762" x2="3.302" y2="0.762" width="0.1524" layer="21"/>
<wire x1="3.048" y1="1.016" x2="3.048" y2="2.54" width="0.1524" layer="51"/>
<wire x1="-3.302" y1="0.762" x2="-3.048" y2="0.762" width="0.1524" layer="21"/>
<wire x1="-3.302" y1="0.762" x2="-3.302" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="-3.048" y1="-0.762" x2="-3.302" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="3.048" y1="2.54" x2="2.54" y2="3.048" width="0.1524" layer="51"/>
<wire x1="2.54" y1="-3.048" x2="3.048" y2="-2.54" width="0.1524" layer="51"/>
<wire x1="3.048" y1="-2.54" x2="3.048" y2="-1.016" width="0.1524" layer="51"/>
<wire x1="-2.54" y1="3.048" x2="-3.048" y2="2.54" width="0.1524" layer="51"/>
<wire x1="-3.048" y1="2.54" x2="-3.048" y2="1.016" width="0.1524" layer="51"/>
<wire x1="-2.54" y1="-3.048" x2="-3.048" y2="-2.54" width="0.1524" layer="51"/>
<wire x1="-3.048" y1="-2.54" x2="-3.048" y2="-1.016" width="0.1524" layer="51"/>
<wire x1="-1.27" y1="1.27" x2="-1.27" y2="-1.27" width="0.0508" layer="51"/>
<wire x1="1.27" y1="-1.27" x2="-1.27" y2="-1.27" width="0.0508" layer="51"/>
<wire x1="1.27" y1="-1.27" x2="1.27" y2="1.27" width="0.0508" layer="51"/>
<wire x1="-1.27" y1="1.27" x2="1.27" y2="1.27" width="0.0508" layer="51"/>
<wire x1="-1.27" y1="3.048" x2="-1.27" y2="2.794" width="0.0508" layer="21"/>
<wire x1="1.27" y1="2.794" x2="-1.27" y2="2.794" width="0.0508" layer="21"/>
<wire x1="1.27" y1="2.794" x2="1.27" y2="3.048" width="0.0508" layer="21"/>
<wire x1="1.143" y1="-2.794" x2="-1.27" y2="-2.794" width="0.0508" layer="21"/>
<wire x1="1.143" y1="-2.794" x2="1.143" y2="-3.048" width="0.0508" layer="21"/>
<wire x1="-1.27" y1="-2.794" x2="-1.27" y2="-3.048" width="0.0508" layer="21"/>
<wire x1="2.54" y1="-3.048" x2="2.159" y2="-3.048" width="0.1524" layer="51"/>
<wire x1="-2.54" y1="-3.048" x2="-2.159" y2="-3.048" width="0.1524" layer="51"/>
<wire x1="-2.159" y1="-3.048" x2="-1.27" y2="-3.048" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="3.048" x2="-2.159" y2="3.048" width="0.1524" layer="51"/>
<wire x1="2.54" y1="3.048" x2="2.159" y2="3.048" width="0.1524" layer="51"/>
<wire x1="2.159" y1="3.048" x2="1.27" y2="3.048" width="0.1524" layer="21"/>
<wire x1="1.27" y1="3.048" x2="-1.27" y2="3.048" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="3.048" x2="-2.159" y2="3.048" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-3.048" x2="1.143" y2="-3.048" width="0.1524" layer="21"/>
<wire x1="1.143" y1="-3.048" x2="2.159" y2="-3.048" width="0.1524" layer="21"/>
<wire x1="3.048" y1="-0.762" x2="3.048" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="3.048" y1="0.762" x2="3.048" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-3.048" y1="-0.762" x2="-3.048" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-3.048" y1="0.762" x2="-3.048" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="-2.159" x2="1.27" y2="-2.159" width="0.1524" layer="51"/>
<wire x1="1.27" y1="2.286" x2="-1.27" y2="2.286" width="0.1524" layer="51"/>
<wire x1="-2.413" y1="1.27" x2="-2.413" y2="0.508" width="0.1524" layer="51"/>
<wire x1="-2.413" y1="-0.508" x2="-2.413" y2="-1.27" width="0.1524" layer="51"/>
<wire x1="-2.413" y1="0.508" x2="-2.159" y2="-0.381" width="0.1524" layer="51"/>
<circle x="0" y="0" radius="1.778" width="0.1524" layer="21"/>
<circle x="-2.159" y="-2.159" radius="0.508" width="0.1524" layer="51"/>
<circle x="2.159" y="-2.032" radius="0.508" width="0.1524" layer="51"/>
<circle x="2.159" y="2.159" radius="0.508" width="0.1524" layer="51"/>
<circle x="-2.159" y="2.159" radius="0.508" width="0.1524" layer="51"/>
<circle x="0" y="0" radius="0.635" width="0.0508" layer="51"/>
<circle x="0" y="0" radius="0.254" width="0.1524" layer="21"/>
<pad name="1" x="-3.2512" y="2.2606" drill="1.016" shape="long"/>
<pad name="3" x="-3.2512" y="-2.2606" drill="1.016" shape="long"/>
<pad name="2" x="3.2512" y="2.2606" drill="1.016" shape="long"/>
<pad name="4" x="3.2512" y="-2.2606" drill="1.016" shape="long"/>
<text x="-3.048" y="3.683" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-3.048" y="-5.08" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<text x="-4.318" y="1.651" size="1.27" layer="51" ratio="10">1</text>
<text x="3.556" y="1.524" size="1.27" layer="51" ratio="10">2</text>
<text x="-4.572" y="-2.794" size="1.27" layer="51" ratio="10">3</text>
<text x="3.556" y="-2.794" size="1.27" layer="51" ratio="10">4</text>
</package>
</packages>
<packages3d>
<package3d name="B3F-10XX" urn="urn:adsk.eagle:package:27496/1" type="box" library_version="3">
<description>OMRON SWITCH</description>
<packageinstances>
<packageinstance name="B3F-10XX"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="TS2" urn="urn:adsk.eagle:symbol:27469/1" library_version="3">
<wire x1="0" y1="1.905" x2="0" y2="2.54" width="0.254" layer="94"/>
<wire x1="-4.445" y1="1.905" x2="-3.175" y2="1.905" width="0.254" layer="94"/>
<wire x1="-4.445" y1="-1.905" x2="-3.175" y2="-1.905" width="0.254" layer="94"/>
<wire x1="-4.445" y1="1.905" x2="-4.445" y2="0" width="0.254" layer="94"/>
<wire x1="-4.445" y1="0" x2="-4.445" y2="-1.905" width="0.254" layer="94"/>
<wire x1="-2.54" y1="0" x2="-1.905" y2="0" width="0.1524" layer="94"/>
<wire x1="-1.27" y1="0" x2="-0.635" y2="0" width="0.1524" layer="94"/>
<wire x1="-4.445" y1="0" x2="-3.175" y2="0" width="0.1524" layer="94"/>
<wire x1="2.54" y1="2.54" x2="0" y2="2.54" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="0" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.27" y2="1.905" width="0.254" layer="94"/>
<circle x="0" y="-2.54" radius="0.127" width="0.4064" layer="94"/>
<circle x="0" y="2.54" radius="0.127" width="0.4064" layer="94"/>
<text x="-6.35" y="-2.54" size="1.778" layer="95" rot="R90">&gt;NAME</text>
<text x="-3.81" y="3.175" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="P" x="0" y="-5.08" visible="pad" length="short" direction="pas" swaplevel="2" rot="R90"/>
<pin name="S" x="0" y="5.08" visible="pad" length="short" direction="pas" swaplevel="1" rot="R270"/>
<pin name="S1" x="2.54" y="5.08" visible="pad" length="short" direction="pas" swaplevel="1" rot="R270"/>
<pin name="P1" x="2.54" y="-5.08" visible="pad" length="short" direction="pas" swaplevel="2" rot="R90"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="10-XX" urn="urn:adsk.eagle:component:27498/2" prefix="S" uservalue="yes" library_version="3">
<description>&lt;b&gt;OMRON SWITCH&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="TS2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="B3F-10XX">
<connects>
<connect gate="1" pin="P" pad="3"/>
<connect gate="1" pin="P1" pad="4"/>
<connect gate="1" pin="S" pad="1"/>
<connect gate="1" pin="S1" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:27496/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="B3F-1000" constant="no"/>
<attribute name="OC_FARNELL" value="176432" constant="no"/>
<attribute name="OC_NEWARK" value="36M3542" constant="no"/>
<attribute name="POPULARITY" value="72" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="diode" urn="urn:adsk.eagle:library:210">
<description>&lt;b&gt;Diodes&lt;/b&gt;&lt;p&gt;
Based on the following sources:
&lt;ul&gt;
&lt;li&gt;Motorola : www.onsemi.com
&lt;li&gt;Fairchild : www.fairchildsemi.com
&lt;li&gt;Philips : www.semiconductors.com
&lt;li&gt;Vishay : www.vishay.de
&lt;/ul&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DO35-10" urn="urn:adsk.eagle:footprint:43092/1" library_version="8">
<description>&lt;B&gt;DIODE&lt;/B&gt;&lt;p&gt;
diameter 2 mm, horizontal, grid 10.16 mm</description>
<wire x1="5.08" y1="0" x2="4.191" y2="0" width="0.508" layer="51"/>
<wire x1="-5.08" y1="0" x2="-4.191" y2="0" width="0.508" layer="51"/>
<wire x1="-0.635" y1="0" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="1.016" y1="0.635" x2="1.016" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.016" y1="-0.635" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="1.524" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="1.016" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="0.635" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="0" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="2.032" y1="1.016" x2="2.286" y2="0.762" width="0.1524" layer="21" curve="-90"/>
<wire x1="-2.286" y1="0.762" x2="-2.032" y2="1.016" width="0.1524" layer="21" curve="-90"/>
<wire x1="-2.286" y1="-0.762" x2="-2.032" y2="-1.016" width="0.1524" layer="21" curve="90"/>
<wire x1="2.032" y1="-1.016" x2="2.286" y2="-0.762" width="0.1524" layer="21" curve="90"/>
<wire x1="2.286" y1="-0.762" x2="2.286" y2="0.762" width="0.1524" layer="21"/>
<wire x1="-2.286" y1="0.762" x2="-2.286" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="-2.032" y1="1.016" x2="2.032" y2="1.016" width="0.1524" layer="21"/>
<wire x1="-2.032" y1="-1.016" x2="2.032" y2="-1.016" width="0.1524" layer="21"/>
<pad name="C" x="-5.08" y="0" drill="0.8128" shape="long"/>
<pad name="A" x="5.08" y="0" drill="0.8128" shape="long"/>
<text x="-2.159" y="1.27" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.159" y="-2.667" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="-1.905" y1="-1.016" x2="-1.397" y2="1.016" layer="21"/>
<rectangle x1="2.286" y1="-0.254" x2="4.191" y2="0.254" layer="21"/>
<rectangle x1="-4.191" y1="-0.254" x2="-2.286" y2="0.254" layer="21"/>
</package>
<package name="DO35-7" urn="urn:adsk.eagle:footprint:43093/1" library_version="8">
<description>&lt;B&gt;DIODE&lt;/B&gt;&lt;p&gt;
diameter 2 mm, horizontal, grid 7.62 mm</description>
<wire x1="3.81" y1="0" x2="2.921" y2="0" width="0.508" layer="51"/>
<wire x1="-3.81" y1="0" x2="-2.921" y2="0" width="0.508" layer="51"/>
<wire x1="-0.635" y1="0" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="1.016" y1="0.635" x2="1.016" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="1.016" y1="-0.635" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="1.524" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="1.016" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="0.635" x2="0" y2="0" width="0.1524" layer="21"/>
<wire x1="0" y1="0" x2="0" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="2.032" y1="1.016" x2="2.286" y2="0.762" width="0.1524" layer="21" curve="-90"/>
<wire x1="2.286" y1="0.762" x2="2.286" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="2.032" y1="-1.016" x2="2.286" y2="-0.762" width="0.1524" layer="21" curve="90"/>
<wire x1="-2.286" y1="0.762" x2="-2.032" y2="1.016" width="0.1524" layer="21" curve="-90"/>
<wire x1="-2.286" y1="-0.762" x2="-2.032" y2="-1.016" width="0.1524" layer="21" curve="90"/>
<wire x1="-2.032" y1="-1.016" x2="2.032" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-2.286" y1="0.762" x2="-2.286" y2="-0.762" width="0.1524" layer="21"/>
<wire x1="-2.032" y1="1.016" x2="2.032" y2="1.016" width="0.1524" layer="21"/>
<pad name="C" x="-3.81" y="0" drill="0.8128" shape="long"/>
<pad name="A" x="3.81" y="0" drill="0.8128" shape="long"/>
<text x="-2.286" y="1.27" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.286" y="-2.667" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="-1.905" y1="-1.016" x2="-1.397" y2="1.016" layer="21"/>
<rectangle x1="2.286" y1="-0.254" x2="2.921" y2="0.254" layer="21"/>
<rectangle x1="-2.921" y1="-0.254" x2="-2.286" y2="0.254" layer="21"/>
</package>
</packages>
<packages3d>
<package3d name="DO35-10" urn="urn:adsk.eagle:package:43344/2" type="model" library_version="8">
<description>DIODE
diameter 2 mm, horizontal, grid 10.16 mm</description>
<packageinstances>
<packageinstance name="DO35-10"/>
</packageinstances>
</package3d>
<package3d name="DO35-7" urn="urn:adsk.eagle:package:43339/2" type="model" library_version="8">
<description>DIODE
diameter 2 mm, horizontal, grid 7.62 mm</description>
<packageinstances>
<packageinstance name="DO35-7"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="D" urn="urn:adsk.eagle:symbol:43091/2" library_version="8">
<wire x1="-1.27" y1="-1.27" x2="1.27" y2="0" width="0.254" layer="94"/>
<wire x1="1.27" y1="0" x2="-1.27" y2="1.27" width="0.254" layer="94"/>
<wire x1="1.27" y1="1.27" x2="1.27" y2="0" width="0.254" layer="94"/>
<wire x1="-1.27" y1="1.27" x2="-1.27" y2="0" width="0.254" layer="94"/>
<wire x1="-1.27" y1="0" x2="-1.27" y2="-1.27" width="0.254" layer="94"/>
<wire x1="1.27" y1="0" x2="1.27" y2="-1.27" width="0.254" layer="94"/>
<wire x1="-1.27" y1="0" x2="-2.54" y2="0" width="0.254" layer="94"/>
<wire x1="2.54" y1="0" x2="1.27" y2="0" width="0.254" layer="94"/>
<text x="2.54" y="0.4826" size="1.778" layer="95">&gt;NAME</text>
<text x="2.54" y="-2.3114" size="1.778" layer="96">&gt;VALUE</text>
<text x="-2.54" y="0" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<text x="2.54" y="0" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
<pin name="A" x="-2.54" y="0" visible="off" length="point" direction="pas"/>
<pin name="C" x="2.54" y="0" visible="off" length="point" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="1N4148" urn="urn:adsk.eagle:component:43496/5" prefix="D" library_version="8">
<description>&lt;B&gt;DIODE&lt;/B&gt;&lt;p&gt;
high speed (Philips)</description>
<gates>
<gate name="G$1" symbol="D" x="0" y="0"/>
</gates>
<devices>
<device name="DO35-10" package="DO35-10">
<connects>
<connect gate="G$1" pin="A" pad="A"/>
<connect gate="G$1" pin="C" pad="C"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:43344/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="21" constant="no"/>
</technology>
</technologies>
</device>
<device name="DO35-7" package="DO35-7">
<connects>
<connect gate="G$1" pin="A" pad="A"/>
<connect gate="G$1" pin="C" pad="C"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:43339/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="58" constant="no"/>
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
<part name="F1" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="ENTER" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="DOWN" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="LEFT" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="ESC" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="RIGHT" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="UP" library="switch-omron" library_urn="urn:adsk.eagle:library:377" deviceset="10-XX" device="" package3d_urn="urn:adsk.eagle:package:27496/1"/>
<part name="D3" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D4" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D6" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D7" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D8" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D9" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D10" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D11" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
<part name="D12" library="diode" library_urn="urn:adsk.eagle:library:210" deviceset="1N4148" device="DO35-7" package3d_urn="urn:adsk.eagle:package:43339/2"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="F1" gate="1" x="33.02" y="109.22" smashed="yes">
<attribute name="NAME" x="26.67" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="29.21" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="ENTER" gate="1" x="45.72" y="109.22" smashed="yes">
<attribute name="NAME" x="39.37" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="41.91" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="DOWN" gate="1" x="58.42" y="109.22" smashed="yes">
<attribute name="NAME" x="52.07" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="54.61" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="LEFT" gate="1" x="71.12" y="109.22" smashed="yes">
<attribute name="NAME" x="64.77" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="67.31" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="ESC" gate="1" x="83.82" y="109.22" smashed="yes">
<attribute name="NAME" x="77.47" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="80.01" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="RIGHT" gate="1" x="96.52" y="109.22" smashed="yes">
<attribute name="NAME" x="90.17" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="92.71" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="UP" gate="1" x="109.22" y="109.22" smashed="yes">
<attribute name="NAME" x="102.87" y="106.68" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="105.41" y="112.395" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="D3" gate="G$1" x="58.42" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="58.9026" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="56.1086" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D4" gate="G$1" x="53.34" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="53.8226" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="51.0286" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D6" gate="G$1" x="83.82" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="84.3026" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="81.5086" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D7" gate="G$1" x="78.74" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="79.2226" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="76.4286" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D8" gate="G$1" x="96.52" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="97.0026" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="94.2086" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D9" gate="G$1" x="91.44" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="91.9226" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="89.1286" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D10" gate="G$1" x="104.14" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="104.6226" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="101.8286" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D11" gate="G$1" x="109.22" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="109.7026" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="106.9086" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="D12" gate="G$1" x="114.3" y="121.92" smashed="yes" rot="R270">
<attribute name="NAME" x="114.7826" y="119.38" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="111.9886" y="119.38" size="1.778" layer="96" rot="R270"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<pinref part="F1" gate="1" pin="P"/>
<wire x1="33.02" y1="104.14" x2="33.02" y2="99.06" width="0.1524" layer="91"/>
<pinref part="ENTER" gate="1" pin="P"/>
<wire x1="33.02" y1="99.06" x2="45.72" y2="99.06" width="0.1524" layer="91"/>
<wire x1="45.72" y1="99.06" x2="45.72" y2="104.14" width="0.1524" layer="91"/>
<pinref part="DOWN" gate="1" pin="P"/>
<wire x1="45.72" y1="99.06" x2="58.42" y2="99.06" width="0.1524" layer="91"/>
<wire x1="58.42" y1="99.06" x2="58.42" y2="104.14" width="0.1524" layer="91"/>
<junction x="45.72" y="99.06"/>
<pinref part="LEFT" gate="1" pin="P"/>
<wire x1="58.42" y1="99.06" x2="71.12" y2="99.06" width="0.1524" layer="91"/>
<wire x1="71.12" y1="99.06" x2="71.12" y2="104.14" width="0.1524" layer="91"/>
<junction x="58.42" y="99.06"/>
<pinref part="ESC" gate="1" pin="P"/>
<wire x1="71.12" y1="99.06" x2="83.82" y2="99.06" width="0.1524" layer="91"/>
<wire x1="83.82" y1="99.06" x2="83.82" y2="104.14" width="0.1524" layer="91"/>
<junction x="71.12" y="99.06"/>
<pinref part="RIGHT" gate="1" pin="P"/>
<wire x1="83.82" y1="99.06" x2="96.52" y2="99.06" width="0.1524" layer="91"/>
<wire x1="96.52" y1="99.06" x2="96.52" y2="104.14" width="0.1524" layer="91"/>
<junction x="83.82" y="99.06"/>
<pinref part="UP" gate="1" pin="P"/>
<wire x1="96.52" y1="99.06" x2="109.22" y2="99.06" width="0.1524" layer="91"/>
<wire x1="109.22" y1="99.06" x2="109.22" y2="104.14" width="0.1524" layer="91"/>
<junction x="96.52" y="99.06"/>
<label x="25.4" y="99.06" size="1.778" layer="95" rot="R180"/>
<pinref part="F1" gate="1" pin="P1"/>
<wire x1="35.56" y1="104.14" x2="33.02" y2="104.14" width="0.1524" layer="91"/>
<junction x="33.02" y="104.14"/>
<pinref part="DOWN" gate="1" pin="P1"/>
<wire x1="58.42" y1="104.14" x2="60.96" y2="104.14" width="0.1524" layer="91"/>
<junction x="58.42" y="104.14"/>
<pinref part="LEFT" gate="1" pin="P1"/>
<wire x1="71.12" y1="104.14" x2="73.66" y2="104.14" width="0.1524" layer="91"/>
<junction x="71.12" y="104.14"/>
<pinref part="ESC" gate="1" pin="P1"/>
<wire x1="83.82" y1="104.14" x2="86.36" y2="104.14" width="0.1524" layer="91"/>
<junction x="83.82" y="104.14"/>
<pinref part="RIGHT" gate="1" pin="P1"/>
<wire x1="96.52" y1="104.14" x2="99.06" y2="104.14" width="0.1524" layer="91"/>
<junction x="96.52" y="104.14"/>
<pinref part="UP" gate="1" pin="P1"/>
<wire x1="109.22" y1="104.14" x2="111.76" y2="104.14" width="0.1524" layer="91"/>
<junction x="109.22" y="104.14"/>
<pinref part="ENTER" gate="1" pin="P1"/>
<wire x1="45.72" y1="104.14" x2="48.26" y2="104.14" width="0.1524" layer="91"/>
<junction x="45.72" y="104.14"/>
<wire x1="33.02" y1="99.06" x2="27.94" y2="99.06" width="0.1524" layer="91"/>
<junction x="33.02" y="99.06"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="D4" gate="G$1" pin="C"/>
<wire x1="53.34" y1="119.38" x2="53.34" y2="116.84" width="0.1524" layer="91"/>
<pinref part="D3" gate="G$1" pin="C"/>
<pinref part="DOWN" gate="1" pin="S"/>
<wire x1="58.42" y1="119.38" x2="58.42" y2="116.84" width="0.1524" layer="91"/>
<wire x1="58.42" y1="116.84" x2="58.42" y2="114.3" width="0.1524" layer="91"/>
<wire x1="53.34" y1="116.84" x2="58.42" y2="116.84" width="0.1524" layer="91"/>
<junction x="58.42" y="116.84"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="D7" gate="G$1" pin="C"/>
<wire x1="78.74" y1="119.38" x2="78.74" y2="116.84" width="0.1524" layer="91"/>
<pinref part="D6" gate="G$1" pin="C"/>
<pinref part="ESC" gate="1" pin="S"/>
<wire x1="83.82" y1="119.38" x2="83.82" y2="116.84" width="0.1524" layer="91"/>
<wire x1="83.82" y1="116.84" x2="83.82" y2="114.3" width="0.1524" layer="91"/>
<wire x1="78.74" y1="116.84" x2="83.82" y2="116.84" width="0.1524" layer="91"/>
<junction x="83.82" y="116.84"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="D9" gate="G$1" pin="C"/>
<wire x1="91.44" y1="119.38" x2="91.44" y2="116.84" width="0.1524" layer="91"/>
<pinref part="D8" gate="G$1" pin="C"/>
<pinref part="RIGHT" gate="1" pin="S"/>
<wire x1="96.52" y1="119.38" x2="96.52" y2="116.84" width="0.1524" layer="91"/>
<wire x1="96.52" y1="116.84" x2="96.52" y2="114.3" width="0.1524" layer="91"/>
<wire x1="91.44" y1="116.84" x2="96.52" y2="116.84" width="0.1524" layer="91"/>
<junction x="96.52" y="116.84"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="D11" gate="G$1" pin="C"/>
<wire x1="109.22" y1="119.38" x2="109.22" y2="116.84" width="0.1524" layer="91"/>
<pinref part="D12" gate="G$1" pin="C"/>
<wire x1="114.3" y1="119.38" x2="114.3" y2="116.84" width="0.1524" layer="91"/>
<wire x1="114.3" y1="116.84" x2="109.22" y2="116.84" width="0.1524" layer="91"/>
<junction x="109.22" y="116.84"/>
<pinref part="D10" gate="G$1" pin="C"/>
<pinref part="UP" gate="1" pin="S"/>
<wire x1="104.14" y1="119.38" x2="104.14" y2="116.84" width="0.1524" layer="91"/>
<wire x1="104.14" y1="116.84" x2="109.22" y2="116.84" width="0.1524" layer="91"/>
<wire x1="109.22" y1="116.84" x2="109.22" y2="114.3" width="0.1524" layer="91"/>
<junction x="109.22" y="116.84"/>
</segment>
</net>
<net name="PIN_BIT0" class="0">
<segment>
<label x="27.94" y="127" size="1.778" layer="95" rot="R180"/>
<pinref part="D3" gate="G$1" pin="A"/>
<wire x1="58.42" y1="124.46" x2="58.42" y2="127" width="0.1524" layer="91"/>
<wire x1="58.42" y1="127" x2="35.56" y2="127" width="0.1524" layer="91"/>
<pinref part="D6" gate="G$1" pin="A"/>
<wire x1="35.56" y1="127" x2="27.94" y2="127" width="0.1524" layer="91"/>
<wire x1="83.82" y1="124.46" x2="83.82" y2="127" width="0.1524" layer="91"/>
<wire x1="83.82" y1="127" x2="58.42" y2="127" width="0.1524" layer="91"/>
<junction x="58.42" y="127"/>
<pinref part="D10" gate="G$1" pin="A"/>
<wire x1="104.14" y1="124.46" x2="104.14" y2="127" width="0.1524" layer="91"/>
<wire x1="104.14" y1="127" x2="83.82" y2="127" width="0.1524" layer="91"/>
<junction x="83.82" y="127"/>
<pinref part="F1" gate="1" pin="S1"/>
<wire x1="35.56" y1="114.3" x2="35.56" y2="127" width="0.1524" layer="91"/>
<junction x="35.56" y="127"/>
</segment>
</net>
<net name="PIN_BIT1" class="0">
<segment>
<wire x1="45.72" y1="129.54" x2="27.94" y2="129.54" width="0.1524" layer="91"/>
<label x="27.94" y="129.54" size="1.778" layer="95" rot="R180"/>
<pinref part="D4" gate="G$1" pin="A"/>
<wire x1="53.34" y1="124.46" x2="53.34" y2="129.54" width="0.1524" layer="91"/>
<wire x1="53.34" y1="129.54" x2="45.72" y2="129.54" width="0.1524" layer="91"/>
<junction x="45.72" y="129.54"/>
<pinref part="D8" gate="G$1" pin="A"/>
<wire x1="96.52" y1="124.46" x2="96.52" y2="129.54" width="0.1524" layer="91"/>
<wire x1="96.52" y1="129.54" x2="53.34" y2="129.54" width="0.1524" layer="91"/>
<junction x="53.34" y="129.54"/>
<pinref part="D11" gate="G$1" pin="A"/>
<wire x1="109.22" y1="124.46" x2="109.22" y2="129.54" width="0.1524" layer="91"/>
<wire x1="109.22" y1="129.54" x2="96.52" y2="129.54" width="0.1524" layer="91"/>
<junction x="96.52" y="129.54"/>
<pinref part="ENTER" gate="1" pin="S"/>
<wire x1="45.72" y1="129.54" x2="45.72" y2="114.3" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PIN_BIT2" class="0">
<segment>
<wire x1="71.12" y1="132.08" x2="27.94" y2="132.08" width="0.1524" layer="91"/>
<label x="27.94" y="132.08" size="1.778" layer="95" rot="R180"/>
<pinref part="D7" gate="G$1" pin="A"/>
<wire x1="78.74" y1="124.46" x2="78.74" y2="132.08" width="0.1524" layer="91"/>
<wire x1="78.74" y1="132.08" x2="71.12" y2="132.08" width="0.1524" layer="91"/>
<junction x="71.12" y="132.08"/>
<pinref part="D9" gate="G$1" pin="A"/>
<wire x1="91.44" y1="124.46" x2="91.44" y2="132.08" width="0.1524" layer="91"/>
<wire x1="91.44" y1="132.08" x2="78.74" y2="132.08" width="0.1524" layer="91"/>
<junction x="78.74" y="132.08"/>
<pinref part="D12" gate="G$1" pin="A"/>
<wire x1="114.3" y1="124.46" x2="114.3" y2="132.08" width="0.1524" layer="91"/>
<wire x1="114.3" y1="132.08" x2="91.44" y2="132.08" width="0.1524" layer="91"/>
<junction x="91.44" y="132.08"/>
<pinref part="LEFT" gate="1" pin="S"/>
<wire x1="71.12" y1="132.08" x2="71.12" y2="114.3" width="0.1524" layer="91"/>
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
</compatibility>
</eagle>
