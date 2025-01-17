[1mdiff --git a/Core/Src/fdcan.c b/Core/Src/fdcan.c[m
[1mindex f82b9b5..da19426 100644[m
[1m--- a/Core/Src/fdcan.c[m
[1m+++ b/Core/Src/fdcan.c[m
[36m@@ -125,9 +125,16 @@[m [mvoid writeMessage(uint32_t identifier, uint8_t* data, uint32_t len, uint8_t bus)[m
 [m
 void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)[m
 {[m
[31m-  uint32_t msg = hfdcan->msgRam->RxFIFO0SA;[m
[32m+[m[41m    [m
   // read from buffer[m
 [m
[32m+[m[32m  uint32_t dataLength = hfdcan->DataLength;[m[41m[m
[32m+[m[32m  uint32_t msgPtr = hfdcan->msgRam->RxFIFO0SA;[m[41m[m
[32m+[m[32m  uint32_t data[dataLength];[m[41m[m
[32m+[m[32m  for (int i = 0; i < dataLength; i++) {[m[41m[m
[32m+[m[32m    data[i] = *(hfdcan->msgRam->RxFIFO0SA + i);[m[41m[m
[32m+[m[32m  }[m[41m[m
[32m+[m[41m[m
   // update info[m
 [m
   // change state[m
[1mdiff --git a/ECU-25.ioc b/ECU-25.ioc[m
[1mindex 7e8c94d..7708e8d 100644[m
[1m--- a/ECU-25.ioc[m
[1m+++ b/ECU-25.ioc[m
[36m@@ -25,14 +25,20 @@[m [mADC3.master=1[m
 CAD.formats=[][m
 CAD.pinconfig=Dual[m
 CAD.provider=[m
[31m-FDCAN1.CalculateBaudRateNominal=3333333[m
[31m-FDCAN1.CalculateTimeBitNominal=300[m
[32m+[m[32mFDCAN1.CalculateBaudRateNominal=1000000[m
[32m+[m[32mFDCAN1.CalculateTimeBitNominal=1000[m
 FDCAN1.CalculateTimeQuantumNominal=100.0[m
[31m-FDCAN1.IPParameters=CalculateTimeQuantumNominal,CalculateTimeBitNominal,CalculateBaudRateNominal[m
[31m-FDCAN2.CalculateBaudRateNominal=3333333[m
[31m-FDCAN2.CalculateTimeBitNominal=300[m
[32m+[m[32mFDCAN1.FrameFormat=FDCAN_FRAME_FD_BRS[m
[32m+[m[32mFDCAN1.IPParameters=CalculateTimeQuantumNominal,CalculateTimeBitNominal,CalculateBaudRateNominal,NominalTimeSeg1,NominalTimeSeg2,FrameFormat[m
[32m+[m[32mFDCAN1.NominalTimeSeg1=5[m
[32m+[m[32mFDCAN1.NominalTimeSeg2=4[m
[32m+[m[32mFDCAN2.CalculateBaudRateNominal=1000000[m
[32m+[m[32mFDCAN2.CalculateTimeBitNominal=1000[m
 FDCAN2.CalculateTimeQuantumNominal=100.0[m
[31m-FDCAN2.IPParameters=CalculateTimeQuantumNominal,CalculateTimeBitNominal,CalculateBaudRateNominal[m
[32m+[m[32mFDCAN2.FrameFormat=FDCAN_FRAME_FD_BRS[m
[32m+[m[32mFDCAN2.IPParameters=CalculateTimeQuantumNominal,CalculateTimeBitNominal,CalculateBaudRateNominal,FrameFormat,NominalTimeSeg1,NominalTimeSeg2[m
[32m+[m[32mFDCAN2.NominalTimeSeg1=5[m
[32m+[m[32mFDCAN2.NominalTimeSeg2=4[m
 File.Version=6[m
 GPIO.groupedBy=Group By Peripherals[m
 KeepUserPlacement=false[m
[36m@@ -82,8 +88,7 @@[m [mMcu.Pin7=PA3[m
 Mcu.Pin8=PA5[m
 Mcu.Pin9=PA6[m
 Mcu.PinsNb=31[m
[31m-Mcu.ThirdParty0=emotas.I-CUBE-CANOPEN.1.3.0[m
[31m-Mcu.ThirdPartyNb=1[m
[32m+[m[32mMcu.ThirdPartyNb=0[m
 Mcu.UserConstants=[m
 Mcu.UserName=STM32G474RETx[m
 MxCube.Version=6.13.0[m
[36m@@ -302,5 +307,3 @@[m [mVP_SYS_VS_DBSignals.Signal=SYS_VS_DBSignals[m
 VP_SYS_VS_Systick.Mode=SysTick[m
 VP_SYS_VS_Systick.Signal=SYS_VS_Systick[m
 board=custom[m
[31m-emotas.I-CUBE-CANOPEN.1.3.0.CANopenJjemotas_Checked=false[m
[31m-emotas.I-CUBE-CANOPEN.1.3.0_SwParameter=CANopenJjemotas\:true;[m
