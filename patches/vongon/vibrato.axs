<patch-1.0 appVersion="1.0.12">
   <obj type="patch/inlet a" uuid="b577fe41e0a6bc7b5502ce33cb8a3129e2e28ee5" name="inlet_1" x="378" y="168">
      <params/>
      <attribs/>
   </obj>
   <obj type="delay/write" uuid="bd73958e3830021807ee97a8cff4500a72a5710d" name="delay" x="476" y="168">
      <params/>
      <attribs>
         <combo attributeName="size" selection="2048 (42.66ms)"/>
      </attribs>
   </obj>
   <obj type="ctrl/dial p" uuid="cc5d2846c3d50e425f450c4b9851371b54f4d674" name="speed" x="154" y="266">
      <params>
         <frac32.u.map name="value" onParent="true" value="7.80000000000002"/>
      </params>
      <attribs/>
   </obj>
   <obj type="math/*c" uuid="7d5ef61c3bcd571ee6bbd8437ef3612125dfb225" name="*c_2" x="252" y="266">
      <params>
         <frac32.u.map name="amp" value="0.019999999552965164"/>
      </params>
      <attribs/>
   </obj>
   <obj type="osc/sine lin" uuid="6a4458d598c9b8634b469d1a6e7f87971b5932f" name="sine_2" x="378" y="266">
      <params>
         <frac32.u.map name="freq" value="0.0"/>
      </params>
      <attribs/>
   </obj>
   <obj type="conv/bipolar2unipolar" uuid="5b5f9405be365706c1ad37266d9df07b9a1570ee" name="bipolar2unipolar_1" x="504" y="266">
      <params/>
      <attribs/>
   </obj>
   <obj type="math/*c" uuid="7a66f52a9594e7e9eb31328ea725cb3641a80b55" name="depth" x="658" y="266">
      <params>
         <frac32.u.map name="amp" onParent="true" value="5.000000000000001"/>
      </params>
      <attribs/>
   </obj>
   <obj type="delay/read interp" uuid="e3d8b8823ab551c588659520bf6cc25c630466c7" name="read_1" x="770" y="266">
      <params>
         <frac32.u.map name="time" value="0.0"/>
      </params>
      <attribs>
         <objref attributeName="delayname" obj="delay"/>
      </attribs>
   </obj>
   <obj type="patch/outlet a" uuid="abd8c5fd3b0524a6630f65cad6dc27f6c58e2a3e" name="outlet_1" x="952" y="266">
      <params/>
      <attribs/>
   </obj>
   <nets>
      <net>
         <source obj="bipolar2unipolar_1" outlet="o"/>
         <dest obj="depth" inlet="in"/>
      </net>
      <net>
         <source obj="depth" outlet="out"/>
         <dest obj="read_1" inlet="time"/>
      </net>
      <net>
         <source obj="read_1" outlet="out"/>
         <dest obj="outlet_1" inlet="outlet"/>
      </net>
      <net>
         <source obj="sine_2" outlet="wave"/>
         <dest obj="bipolar2unipolar_1" inlet="i"/>
      </net>
      <net>
         <source obj="speed" outlet="out"/>
         <dest obj="*c_2" inlet="in"/>
      </net>
      <net>
         <source obj="*c_2" outlet="out"/>
         <dest obj="sine_2" inlet="freq"/>
      </net>
      <net>
         <source obj="inlet_1" outlet="inlet"/>
         <dest obj="delay" inlet="in"/>
      </net>
   </nets>
   <settings>
      <subpatchmode>no</subpatchmode>
   </settings>
   <notes><![CDATA[]]></notes>
   <windowPos>
      <x>676</x>
      <y>288</y>
      <width>1664</width>
      <height>975</height>
   </windowPos>
</patch-1.0>