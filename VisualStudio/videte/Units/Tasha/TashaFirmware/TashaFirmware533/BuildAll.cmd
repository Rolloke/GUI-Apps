Rem ############### DSP 0 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=0 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF0.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF0.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 1 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=0 -D SLAVE=1 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF1.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF1.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 2 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=2 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF2.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF2.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 3 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=1 -D SLAVE=3 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF3.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF3.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 4 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=4 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF4.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF4.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 5 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=2 -D SLAVE=5 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF5.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF5.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 6 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=6 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF6.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF6.ldr ..\..\TashaUnit /R /C /F /Y

Rem ############### DSP 7 ######################
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\codec.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\codec.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" ..\Include\cycles.asm -proc ADSP-BF533 -o .\Release\cycles.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\detection.asm -proc ADSP-BF533 -o .\Release\detection.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\easmblkfn.exe" .\filter.asm -proc ADSP-BF533 -o .\Release\filter.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\i2c_function.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\i2c_function.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\initialisation.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\initialisation.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\isr.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\isr.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\main.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\main.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\ppi.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\ppi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\spi.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\spi.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\sport.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\sport.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" -c .\submain.cpp -c++ -O1 -Ov100 -D CHANNEL=3 -D SLAVE=7 -proc ADSP-BF533 -o .\Release\submain.doj
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\ccblkfn.exe" .\Release\codec.doj .\Release\cycles.doj .\Release\filter.doj .\Release\detection.doj .\Release\i2c_function.doj .\Release\initialisation.doj .\Release\isr.doj .\Release\main.doj .\lib\MPEG4_SP_ENC_EZ533.dlb .\Release\ppi.doj .\Release\spi.doj .\Release\sport.doj .\Release\submain.doj -T .\TashaFirmware533.ldf -L .\Release -flags-link -od,.\Release -o .\Release\TashaFirmware533.dxe -proc ADSP-BF533
"C:\Programme\Developer\Analog Devices\VisualDSP3.5\elfloader.exe" .\Release\TashaFirmware533.dxe -b SPI -f BINARY -Width 8 -o .\Bin\TashaFirmware533_BF7.ldr -proc ADSP-BF533
xcopy .\Bin\TashaFirmware533_BF7.ldr ..\..\TashaUnit /R /C /F /Y

