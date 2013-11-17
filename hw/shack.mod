PCBNEW-LibModule-V1  17/11/2013 00:14:54
# encoding utf-8
Units mm
$INDEX
Alps-EC11-Vert
PIN_ARRAY_4x1_BPAD
PIN_ARRAY_4x1_KEYED_BPAD
PIN_ARRAY_5x2_BPAD
TinyMega
pin_array_16x1
pin_array_16x1_BPAD
$EndINDEX
$MODULE Alps-EC11-Vert
Po 0 0 0 15 52874058 00000000 F~
Li Alps-EC11-Vert
Sc 0
AR /52642BB8
Op 0 0 0
T0 0 5 1 1 0 0.15 N V 21 N "S1"
T1 0 -5 1 1 0 0.15 N V 21 N "ENCODER_SW"
DS 6 -6 6 6 0.15 21
DS 6 6 -6 6 0.15 21
DS -6 6 -6 -6 0.15 21
DS -6 -6 6 -6 0.15 21
DC 0 0 3 0 0.15 21
$PAD
Sh "6" R 2.5 3.5 0 0 0
Dr 1.5 0 0 O 1.5 2.6
At STD N 00E0FFFF
Ne 0 ""
Po 5.5 0
$EndPAD
$PAD
Sh "2" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 1 "/ENC_A"
Po -2.5 7.5
$EndPAD
$PAD
Sh "1" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 4 "GND"
Po 0 7.5
$EndPAD
$PAD
Sh "3" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 2 "/ENC_B"
Po 2.5 7.5
$EndPAD
$PAD
Sh "4" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 3 "/ENC_PUSH"
Po -2.5 -7
$EndPAD
$PAD
Sh "5" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 4 "GND"
Po 2.5 -7
$EndPAD
$PAD
Sh "6" R 2.5 3.5 0 0 0
Dr 1.5 0 0 O 1.5 2.6
At STD N 00E0FFFF
Ne 0 ""
Po -5.5 0
$EndPAD
$EndMODULE Alps-EC11-Vert
$MODULE PIN_ARRAY_4x1_BPAD
Po 0 0 0 15 52868C21 00000000 ~~
Li PIN_ARRAY_4x1_BPAD
Cd Double rangee de contacts 2 x 5 pins
Kw CONN
Sc 0
AR /526400BF/52640185
Op 0 0 0
T0 0 -2.54 1.016 1.016 0 0.2032 N V 21 N "K1"
T1 0 2.54 1.016 1.016 0 0.2032 N I 21 N "CONN_4"
DS 5.08 1.27 -5.08 1.27 0.254 21
DS 5.08 -1.27 -5.08 -1.27 0.254 21
DS -5.08 -1.27 -5.08 1.27 0.254 21
DS 5.08 1.27 5.08 -1.27 0.254 21
$PAD
Sh "1" R 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 3 "GND"
Po -3.81 0
$EndPAD
$PAD
Sh "2" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 1 "/Key Jacks/pwr0"
Po -1.27 0
$EndPAD
$PAD
Sh "3" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 2 "/Key Jacks/scio0"
Po 1.27 0
$EndPAD
$PAD
Sh "4" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 4 "N-0000061"
Po 3.81 0
$EndPAD
$SHAPE3D
Na "pin_array\\pins_array_4x1.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE PIN_ARRAY_4x1_BPAD
$MODULE PIN_ARRAY_4x1_KEYED_BPAD
Po 0.05 0 0 15 5287FC50 00000000 ~~
Li PIN_ARRAY_4x1_KEYED_BPAD
Cd Double rangee de contacts 2 x 5 pins
Kw CONN
Sc 0
AR /5275125C
Op 0 0 0
T0 0 -2.54 1.016 1.016 0 0.2032 N V 21 N "SMAUL1"
T1 0 2.54 1.016 1.016 0 0.2032 N I 21 N "CONN_4"
DS -3.175 1.905 -3.175 2.54 0.254 21
DS -3.175 2.54 3.175 2.54 0.254 21
DS 3.175 2.54 3.175 1.905 0.254 21
DS 5.08 1.905 5.08 -1.905 0.254 21
DS -5.08 -1.905 -5.08 1.905 0.254 21
DS 5.08 1.905 -5.08 1.905 0.254 21
DS 5.08 -1.905 -5.08 -1.905 0.254 21
$PAD
Sh "1" R 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 3 "GND"
Po -3.81 0
$EndPAD
$PAD
Sh "2" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 4 "N-0000046"
Po -1.27 0
$EndPAD
$PAD
Sh "3" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 2 "/SMAUL_BTN"
Po 1.27 0
$EndPAD
$PAD
Sh "4" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 1 "/BUZZER"
Po 3.81 0
$EndPAD
$SHAPE3D
Na "pin_array\\pins_array_4x1.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE PIN_ARRAY_4x1_KEYED_BPAD
$MODULE PIN_ARRAY_5x2_BPAD
Po 0 0 0 15 52868E0F 00000000 ~~
Li PIN_ARRAY_5x2_BPAD
Cd Double rangee de contacts 2 x 5 pins
Kw CONN
Sc 0
AR /5275C658
Op 0 0 0
T0 0.635 -3.81 1.016 1.016 0 0.2032 N V 21 N "JTAG1"
T1 0 -3.81 1.016 1.016 0 0.2032 N I 21 N "CONN_5X2"
DS -6.35 -2.54 6.35 -2.54 0.3048 21
DS 6.35 -2.54 6.35 2.54 0.3048 21
DS 6.35 2.54 -6.35 2.54 0.3048 21
DS -6.35 2.54 -6.35 -2.54 0.3048 21
$PAD
Sh "1" R 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 2 "/TCK"
Po -5.08 1.27
$EndPAD
$PAD
Sh "2" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 6 "GND"
Po -5.08 -1.27
$EndPAD
$PAD
Sh "3" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 4 "/TDO"
Po -2.54 1.27
$EndPAD
$PAD
Sh "4" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 7 "VDD"
Po -2.54 -1.27
$EndPAD
$PAD
Sh "5" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 5 "/TMS"
Po 0 1.27
$EndPAD
$PAD
Sh "6" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 1 "/!RESET"
Po 0 -1.27
$EndPAD
$PAD
Sh "7" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po 2.54 1.27
$EndPAD
$PAD
Sh "8" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po 2.54 -1.27
$EndPAD
$PAD
Sh "9" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 3 "/TDI"
Po 5.08 1.27
$EndPAD
$PAD
Sh "10" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 6 "GND"
Po 5.08 -1.27
$EndPAD
$SHAPE3D
Na "pin_array/pins_array_5x2.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE PIN_ARRAY_5x2_BPAD
$MODULE TinyMega
Po 0 0 0 15 52874766 00000000 ~~
Li TinyMega
Cd TinyMega board
Kw TINYMEGA
Sc 0
AR /5262DBCC
Op 0 0 0
T0 0 -6.35 1.778 1.143 0 0.28575 N V 21 N "U1"
T1 0 6.35 1.778 1.778 0 0.3048 N V 21 N "TINYMEGA"
DS -21.59 -2.54 -13.97 -2.54 0.15 21
DS -13.97 -2.54 -13.97 3.81 0.15 21
DS -21.59 -2.54 -21.59 3.81 0.15 21
DS -21.59 3.81 -13.97 3.81 0.15 21
DS -20.32 -11.43 -20.32 11.43 0.15 21
DS -20.32 11.43 20.32 11.43 0.15 21
DS 20.32 11.43 20.32 -11.43 0.15 21
DS 20.32 -11.43 -20.32 -11.43 0.15 21
$PAD
Sh "30" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 21 "/TCK"
Po -17.78 8.89
$EndPAD
$PAD
Sh "29" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 7 "/Key Jacks/LED_LATCH"
Po -15.24 8.89
$EndPAD
$PAD
Sh "1" R 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 26 "VDD"
Po -17.78 -8.89
$EndPAD
$PAD
Sh "2" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 25 "GND"
Po -15.24 -8.89
$EndPAD
$PAD
Sh "3" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -12.7 -8.89
$EndPAD
$PAD
Sh "4" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -10.16 -8.89
$EndPAD
$PAD
Sh "5" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 4 "/ENC_PUSH"
Po -7.62 -8.89
$EndPAD
$PAD
Sh "6" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 8 "/Key Jacks/LED_SCK"
Po -5.08 -8.89
$EndPAD
$PAD
Sh "7" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 9 "/Key Jacks/LED_SIN"
Po -2.54 -8.89
$EndPAD
$PAD
Sh "8" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 2 "/ENC_A"
Po 0 -8.89
$EndPAD
$PAD
Sh "9" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 3 "/ENC_B"
Po 2.54 -8.89
$EndPAD
$PAD
Sh "10" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 1 "/!RESET"
Po 5.08 -8.89
$EndPAD
$PAD
Sh "11" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 5 "/Key Jacks/KEY_PWR"
Po 7.62 -8.89
$EndPAD
$PAD
Sh "12" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 6 "/Key Jacks/KEY_SCIO"
Po 10.16 -8.89
$EndPAD
$PAD
Sh "13" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 17 "/M_RX"
Po 12.7 -8.89
$EndPAD
$PAD
Sh "14" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 18 "/M_TX"
Po 15.24 -8.89
$EndPAD
$PAD
Sh "15" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 10 "/LCD_D0"
Po 17.78 -8.89
$EndPAD
$PAD
Sh "16" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 11 "/LCD_D1"
Po 17.78 8.89
$EndPAD
$PAD
Sh "17" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 12 "/LCD_D2"
Po 15.24 8.89
$EndPAD
$PAD
Sh "18" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 13 "/LCD_D3"
Po 12.7 8.89
$EndPAD
$PAD
Sh "19" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 19 "/SMAUL_BTN"
Po 10.16 8.89
$EndPAD
$PAD
Sh "20" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 20 "/SMAUL_LED"
Po 7.62 8.89
$EndPAD
$PAD
Sh "21" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 15 "/LCD_LED"
Po 5.08 8.89
$EndPAD
$PAD
Sh "22" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 14 "/LCD_E"
Po 2.54 8.89
$EndPAD
$PAD
Sh "23" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 16 "/LCD_RS"
Po 0 8.89
$EndPAD
$PAD
Sh "24" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 19 "/SMAUL_BTN"
Po -2.54 8.89
$EndPAD
$PAD
Sh "25" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 22 "/TDI"
Po -5.08 8.89
$EndPAD
$PAD
Sh "26" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 23 "/TDO"
Po -7.62 8.89
$EndPAD
$PAD
Sh "27" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 24 "/TMS"
Po -10.16 8.89
$EndPAD
$PAD
Sh "28" C 1.397 1.397 0 0 0
Dr 0.8128 0 0
At STD N 00E0FFFF
Ne 21 "/TCK"
Po -12.7 8.89
$EndPAD
$SHAPE3D
Na "dil/dil_28-w600.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE TinyMega
$MODULE pin_array_16x1
Po 0 0 0 15 5275B2E2 00000000 ~~
Li pin_array_16x1
Cd 1 x 16 pins connector
Kw CONN
Sc 0
AR /527525FD
Op 0 0 0
T0 -7.62 -2.54 1.016 1.016 0 0.2032 N V 21 N "LCD1"
T1 7.62 -2.54 1.016 1.016 0 0.2032 N V 21 N "LCD_TC1602A"
DS -20.32 1.27 20.32 1.27 0.15 21
DS 20.32 1.27 20.32 -1.27 0.15 21
DS -20.32 1.27 -20.32 -1.27 0.15 21
DS -20.32 -1.27 20.32 -1.27 0.15 21
$PAD
Sh "1" R 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 8 "GND"
Po -19.05 0
$EndPAD
$PAD
Sh "2" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 11 "VDD"
Po -16.51 0
$EndPAD
$PAD
Sh "3" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 9 "N-0000042"
Po -13.97 0
$EndPAD
$PAD
Sh "4" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 6 "/LCD_RS"
Po -11.43 0
$EndPAD
$PAD
Sh "5" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 7 "/LCD_RW"
Po -8.89 0
$EndPAD
$PAD
Sh "6" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 5 "/LCD_E"
Po -6.35 0
$EndPAD
$PAD
Sh "7" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -3.81 0
$EndPAD
$PAD
Sh "8" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -1.27 0
$EndPAD
$PAD
Sh "9" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 1.27 0
$EndPAD
$PAD
Sh "10" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 3.81 0
$EndPAD
$PAD
Sh "11" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 1 "/LCD_D0"
Po 6.35 0
$EndPAD
$PAD
Sh "12" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 2 "/LCD_D1"
Po 8.89 0
$EndPAD
$PAD
Sh "13" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 3 "/LCD_D2"
Po 11.43 0
$EndPAD
$PAD
Sh "14" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 4 "/LCD_D3"
Po 13.97 0
$EndPAD
$PAD
Sh "15" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 10 "N-0000043"
Po 16.51 0
$EndPAD
$PAD
Sh "16" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E0FFFF
Ne 8 "GND"
Po 19.05 0
$EndPAD
$SHAPE3D
Na "pin_array/pins_array_13x2.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE pin_array_16x1
$MODULE pin_array_16x1_BPAD
Po 0 0 0 15 52868EC7 00000000 ~~
Li pin_array_16x1_BPAD
Cd 1 x 16 pins connector
Kw CONN
Sc 0
AR /527525FD
Op 0 0 0
T0 -7.62 -2.54 1.016 1.016 0 0.2032 N V 21 N "LCD1"
T1 7.62 -2.54 1.016 1.016 0 0.2032 N V 21 N "LCD_TC1602A"
DS -20.32 1.27 20.32 1.27 0.15 21
DS 20.32 1.27 20.32 -1.27 0.15 21
DS -20.32 1.27 -20.32 -1.27 0.15 21
DS -20.32 -1.27 20.32 -1.27 0.15 21
$PAD
Sh "1" R 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 8 "GND"
Po -19.05 0
$EndPAD
$PAD
Sh "2" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 11 "VDD"
Po -16.51 0
$EndPAD
$PAD
Sh "3" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 10 "N-0000044"
Po -13.97 0
$EndPAD
$PAD
Sh "4" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 6 "/LCD_RS"
Po -11.43 0
$EndPAD
$PAD
Sh "5" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 7 "/LCD_RW"
Po -8.89 0
$EndPAD
$PAD
Sh "6" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 5 "/LCD_E"
Po -6.35 0
$EndPAD
$PAD
Sh "7" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po -3.81 0
$EndPAD
$PAD
Sh "8" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po -1.27 0
$EndPAD
$PAD
Sh "9" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po 1.27 0
$EndPAD
$PAD
Sh "10" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 0 ""
Po 3.81 0
$EndPAD
$PAD
Sh "11" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 1 "/LCD_D0"
Po 6.35 0
$EndPAD
$PAD
Sh "12" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 2 "/LCD_D1"
Po 8.89 0
$EndPAD
$PAD
Sh "13" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 3 "/LCD_D2"
Po 11.43 0
$EndPAD
$PAD
Sh "14" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 4 "/LCD_D3"
Po 13.97 0
$EndPAD
$PAD
Sh "15" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 9 "N-0000033"
Po 16.51 0
$EndPAD
$PAD
Sh "16" C 1.524 1.524 0 0 0
Dr 1.016 0 0
At STD N 00E00001
Ne 8 "GND"
Po 19.05 0
$EndPAD
$SHAPE3D
Na "pin_array/pins_array_13x2.wrl"
Sc 1 1 1
Of 0 0 0
Ro 0 0 0
$EndSHAPE3D
$EndMODULE pin_array_16x1_BPAD
$EndLIBRARY
