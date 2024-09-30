# RP2040 HOTAS/HOSAS

This will be rewritten in the future (some time)


[![PlatformIO CI](https://github.com/MNS26/RP2040-HOTAS/actions/workflows/build.yml/badge.svg)](https://github.com/MNS26/RP2040-HOTAS/actions/workflows/build.yml)


create a file called `WIFI_CR` in ther main folder and paste
```
[wifi]

AP=
; LINUX
	-D STASSID=\"AP\"
	-D STAPSK=\"PASSWORD\"

; WINDOWS
;	-D STASSID="AP"
;	-D STAPSK="PASSWORD"
```
in it
change `AP` to the wifi name you want it to connect to and same with `PASSWORD` 
