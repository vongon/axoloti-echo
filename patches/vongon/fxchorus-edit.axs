<patch-1.0 appVersion="1.0.12">
   <obj type="delay/write" uuid="bd73958e3830021807ee97a8cff4500a72a5710d" name="dela" x="84" y="0">
      <params/>
      <attribs>
         <combo attributeName="size" selection="2048 (42.66ms)"/>
      </attribs>
   </obj>
   <obj type="ctrl/dial p" uuid="cc5d2846c3d50e425f450c4b9851371b54f4d674" name="depth" x="84" y="56">
      <params>
         <frac32.u.map name="value" onParent="true" value="0.9499998092651367"/>
      </params>
      <attribs/>
   </obj>
   <obj type="patch/inlet a" uuid="b577fe41e0a6bc7b5502ce33cb8a3129e2e28ee5" name="in" x="0" y="70">
      <params/>
      <attribs/>
   </obj>
   <obj type="lfo/sine" uuid="75f7330c26a13953215dccc3b7b9008545c9daa9" name="speed" x="84" y="140">
      <params>
         <frac32.s.map name="pitch" onParent="true" value="0.5999999046325684"/>
      </params>
      <attribs/>
   </obj>
   <obj type="conv/bipolar2unipolar" uuid="f6f63d71053d572d3c795f83c7ec11dfbbce82dd" name="bipolar2unipolar1" x="266" y="140">
      <params/>
      <attribs/>
   </obj>
   <obj type="math/*" uuid="922423f2db9f222aa3e5ba095778288c446da47a" name="vca_1" x="406" y="140">
      <params/>
      <attribs/>
   </obj>
   <obj type="conv/interp" uuid="d68c1a8709d8b55e3de8715d727ec0a2d8569d9a" name="interp_1" x="476" y="140">
      <params/>
      <attribs/>
   </obj>
   <obj type="delay/read interp" uuid="e3d8b8823ab551c588659520bf6cc25c630466c7" name="read_1" x="560" y="140">
      <params>
         <frac32.u.map name="time" value="1.0"/>
      </params>
      <attribs>
         <objref attributeName="delayname" obj="dela"/>
      </attribs>
   </obj>
   <obj type="patch/outlet a" uuid="abd8c5fd3b0524a6630f65cad6dc27f6c58e2a3e" name="L" x="728" y="140">
      <params/>
      <attribs/>
   </obj>
   <obj type="math/inv" uuid="565521d3699b36d8095aa1c79b9ad0046fb133ce" name="inv1" x="196" y="196">
      <params/>
      <attribs/>
   </obj>
   <obj type="conv/bipolar2unipolar" uuid="f6f63d71053d572d3c795f83c7ec11dfbbce82dd" name="bipolar2unipolar1_" x="266" y="196">
      <params/>
      <attribs/>
   </obj>
   <obj type="math/*" uuid="922423f2db9f222aa3e5ba095778288c446da47a" name="vca_1_" x="406" y="196">
      <params/>
      <attribs/>
   </obj>
   <obj type="conv/interp" uuid="d68c1a8709d8b55e3de8715d727ec0a2d8569d9a" name="interp_2" x="476" y="196">
      <params/>
      <attribs/>
   </obj>
   <obj type="delay/read interp" uuid="e3d8b8823ab551c588659520bf6cc25c630466c7" name="read_2" x="560" y="238">
      <params>
         <frac32.u.map name="time" value="1.0"/>
      </params>
      <attribs>
         <objref attributeName="delayname" obj="dela"/>
      </attribs>
   </obj>
   <obj type="patch/outlet a" uuid="abd8c5fd3b0524a6630f65cad6dc27f6c58e2a3e" name="R" x="728" y="238">
      <params/>
      <attribs/>
   </obj>
   <obj type="lfo/saw down lin" uuid="15c1502dc35ff4a3807d832441b23493f3dd95ff" name="saw_1" x="154" y="294">
      <params/>
      <attribs/>
   </obj>
   <obj type="audio/in left" uuid="d40e60b7641fe75af4d7c91b45bb038aacafc52e" name="in_1" x="14" y="350">
      <params/>
      <attribs/>
   </obj>
   <obj type="audio/out left" uuid="b11a3c09b2fdd575ea8212f2ce7743d5269253b1" name="out_1" x="336" y="378">
      <params/>
      <attribs/>
   </obj>
   <nets>
      <net>
         <source obj="read_1" outlet="out"/>
         <dest obj="L" inlet="outlet"/>
         <dest obj="out_1" inlet="wave"/>
      </net>
      <net>
         <source obj="read_2" outlet="out"/>
         <dest obj="R" inlet="outlet"/>
      </net>
      <net>
         <source obj="bipolar2unipolar1" outlet="o"/>
         <dest obj="vca_1" inlet="a"/>
      </net>
      <net>
         <source obj="inv1" outlet="out"/>
         <dest obj="bipolar2unipolar1_" inlet="i"/>
      </net>
      <net>
         <source obj="bipolar2unipolar1_" outlet="o"/>
         <dest obj="vca_1_" inlet="a"/>
      </net>
      <net>
         <source obj="depth" outlet="out"/>
         <dest obj="vca_1" inlet="b"/>
         <dest obj="vca_1_" inlet="b"/>
      </net>
      <net>
         <source obj="vca_1" outlet="result"/>
         <dest obj="interp_1" inlet="i"/>
      </net>
      <net>
         <source obj="interp_1" outlet="o"/>
         <dest obj="read_1" inlet="time"/>
      </net>
      <net>
         <source obj="vca_1_" outlet="result"/>
         <dest obj="interp_2" inlet="i"/>
      </net>
      <net>
         <source obj="interp_2" outlet="o"/>
         <dest obj="read_2" inlet="time"/>
      </net>
      <net>
         <source obj="in_1" outlet="wave"/>
         <dest obj="dela" inlet="in"/>
      </net>
      <net>
         <source obj="speed" outlet="wave"/>
         <dest obj="bipolar2unipolar1" inlet="i"/>
         <dest obj="inv1" inlet="in"/>
      </net>
   </nets>
   <settings>
      <subpatchmode>normal</subpatchmode>
      <MidiChannel>1</MidiChannel>
      <NPresets>8</NPresets>
      <NPresetEntries>32</NPresetEntries>
      <NModulationSources>8</NModulationSources>
      <NModulationTargetsPerSource>8</NModulationTargetsPerSource>
      <Author>jt</Author>
      <License>viral!</License>
   </settings>
   <notes><![CDATA[A simple stereo chorus, based on two out-of-phase sine-wave modulated linear interpolating delayline readers.
]]></notes>
   <windowPos>
      <x>578</x>
      <y>419</y>
      <width>1243</width>
      <height>906</height>
   </windowPos>
</patch-1.0>