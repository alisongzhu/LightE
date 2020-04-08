/*
 * fft.c
 *
 *  Created on: 2020年3月26日
 *      Author: Administrator
 */
#include "include.h"
#include <math.h>
#include <string.h>


#define NUM_FFT 128
#define PI 3.1415926

typedef struct
{
//	s16 iRealArray[NUM_FFT];
//	s16 iMageArray[NUM_FFT];
	float iRealArray[NUM_FFT];
	float iMageArray[NUM_FFT];
	uint16_t	FU[3];			//---基波电压---NNN.N6
	uint32_t	FI[4];			//---基波电流NNNN.NNNN
	uint32_t HarmonicpercentU[3][51];
	uint32_t	HarmonicpercentI[3][51];			//---谐波含有率--NNN.N6
}HarmonicData_TypeDef;

typedef struct
{
    uint8_t  Channel;					 // 当前采样通道
    uint16_t  ReadAdress;				 // 读取地址
    uint8_t  StarFlag;					 // 开始进行谐波数据分析
    uint8_t  ADSPIBusy;					 // 进行谐波分析瞬时值数据采样
    uint16_t DataCount;					 // 当前采样点数
    uint16_t ReadAddres;				 // 采样点数据地址
    uint16_t	dwFreq;					 // 采样周期当前频率
    uint16_t TimeOutStamp;				 // 采样延时退出
    uint8_t InstantaneousData[384];      //当前采样128点瞬时数据
}Instantaneous_TypeDef;

typedef enum
{
	UAChannel		= 0x00,
	UBChannel		= 0x01,
	UCChannel		= 0x02,
	IAChannel		= 0x03,
	IBChannel		= 0x04,
	ICChannel		= 0x05
}ChannelFlag_TypeDef;

typedef struct
{
    uint8_t		ChkErrCnt;       //读错误计数1

    int32_t 	Pw[12];   		//{Pa Pb Pc P Qa Qb Qc Q Sa Sb Sc S}   48
    int32_t 	UI[7];       	//Ua Ub Uc Ia Ib Ic Inal   28
    int32_t		VectorU[9];		// 正序、负序、零序电压
    int32_t		VectorI[9];		// 正序、负序、零序电流
    int32_t  	Pf[4];       	//Pf Pfa Pfb Pfc      16
    uint32_t 	Frequency;   	//电网频率，单位：        4
    int32_t  	YUI[3],YUU[2];  //20

    int32_t		Pulse[15];		//前台高频脉冲48
    //---电能脉冲---
    int32_t		Pulse_EgTmp[20];	//高频脉冲{P,Q,Ps},{Pa,Qa,Psa},{Pb,Qb,Psb},{Pc,Qc,Psc}{Fp,Fq}{Fpa,Fqa}{Fpb,Fqb}{Fpc,Fqc}
    uint32_t		Pulse_Eg[20];  //低频脉冲数
	//---需量脉冲---
	int32_t		Pulse_NeedTmp[12];
  	uint16_t		Pulse_Need[12]; //{PNeed,QNeed,PsNeed},{PNeeda,QNeeda,PsNeeda},{PNeedb,QNeedb,PsNeedb},{PNeedc,QNeedc,PsNeedc}48


    uint16_t		Angle[9];
    uint16_t 	PDirect;   //4
    uint32_t 	ChkSum1;   //4
    uint32_t 	ChkSum2;   //4

	uint16_t		Temperature;	//温度4
	uint32_t		ClockBat;		//时钟电池4
	uint32_t		BackupBat;		//后备电池4

    uint16_t   CF1DelayStamp;
    uint16_t   CF2DelayStamp;

    uint16_t   CfIn_P;
    uint16_t   CfIn_q;

    uint16_t   CfTime_P;
    uint16_t   CfTime_q;

} sDl645FrontTmp_TypeDef;

const float Fftcoefficient[50]=					// 谐波计算时补偿系数
{
	1.0005,
	1.0027,
	1.0054,
	1.0087,
	1.0129,
	1.0175,
	1.0229,
	1.029,
	1.0362,
	1.0438,
	1.0524,
	1.0618,
	1.0718,
	1.0827,
	1.0944,
	1.1075,
	1.121,
	1.136,
	1.1517,
	1.1685,
	1.1862,
	1.2051,
	1.2255,
	1.2475,
	1.27,
	1.2942,
	1.3199,
	1.3468,
	1.3758,
	1.407,
	1.4421,
	1.4714,
	1.5107,
	1.5483,
	1.5873,
	1.6312,
	1.6727,
	1.7199,
	1.77,
	1.8231,
	1.8797,
	1.9448,
	2.0074,
	2.074,
	2.1534,
	2.2126,
	2.2919,
	2.3762,
	2.467,
	2.5639,
};


Instantaneous_TypeDef Harmonictemp;
HarmonicData_TypeDef HarmonicData;
sDl645FrontTmp_TypeDef Dl645FrontTmp;

float sin_tab[NUM_FFT];
float cos_tab[NUM_FFT];


void fnDl645Fft_init(void)
{
	Harmonictemp.Channel=0x00;
	Harmonictemp.DataCount=0;
    Harmonictemp.StarFlag=0;
    Harmonictemp.ADSPIBusy = 0;
    memset(&Harmonictemp,0,sizeof(Harmonictemp));
    memset(&HarmonicData,0,sizeof(HarmonicData));
}


void InitForFFT()
{
	int i;

	for ( i=0;i<NUM_FFT;i++ )
	{
		sin_tab[i]=sin(PI*2*i/NUM_FFT);
		cos_tab[i]=cos(PI*2*i/NUM_FFT);
	}
}


/**function: CharToHex()
*** ACSII change to 16 hex
*** input:ACSII
***Return :Hex
**/
uint8_t HexToASCII(uint32_t *p,uint8_t *m,uint16_t num)
{
	uint8_t value[3];
	uint8_t j=0;

	for(uint16_t i=0;i<num;i++)
	{
		value[0]= p[i] % 10;
		value[1]= (p[i] / 10)%10;
		value[2]= p[i] / 100;
		m[j]=value[2]+0x30;
		m[j+1]=value[1]+0x30;
        m[j+2]=value[0]+0x30;
        j++;
//		if(value[0] >= 0x30 && value[0] <=0x39)
//		{
//			temp[j+i] -= 0x30;
//		}
//		else if(value[0] >= 0x41 && value[0] <= 0x46)
//		{
//			temp[j+i] -= 0x37;
//		}
//		else if(value[0] >= 0x61 && value[0] <= 0x66)
//		{
//			temp[j+i] -= 0x57;
//		}
//
//		if(value[1] >= 0x30 && value[1] <=0x39)
//		{
//			temp[j+1+i] -= 0x30;
//		}
//		else if(value[1] >= 0x41 && value[1] <= 0x46)
//		{
//			temp[j+1+i] -= 0x37;
//		}
//		else if(value[1] >= 0x61 && value[1] <= 0x66)
//		{
//			temp[j+1+i] -= 0x57;
//		}
//
//		if(value[2] >= 0x30 && value[2] <=0x39)
//		{
//			temp[j+2+i] -= 0x30;
//		}
//		else if(value[2] >= 0x41 && value[2] <= 0x46)
//		{
//			temp[j+2+i] -= 0x37;
//		}
//		else if(value[2] >= 0x61 && value[2] <= 0x66)
//		{
//			temp[j+2+i] -= 0x57;
//		}
//		j++;
//		k=j+2+i;
	}
	    m[j+2]=0x0D;
	    m[j+3]=0x0A;
	return (j+4);
}



uint32_t	fnHexToBcd_u32(uint32_t Dat)
{
	uint32_t result = 0;

	Dat = Dat % 100000000;
	result += (Dat / 10000000) * 0x10000000;
	Dat = Dat % 10000000;
	result += (Dat / 1000000) * 0x1000000;
	Dat = Dat % 1000000;
	result += (Dat / 100000) * 0x100000;
	Dat = Dat % 100000;
	result += (Dat / 10000) * 0x10000;
	Dat = Dat % 10000;
	result += (Dat / 1000) * 0x1000;
	Dat = Dat % 1000;
	result += (Dat / 100) * 0x100;
	Dat = Dat % 100;
	result += (Dat / 10) * 0x10;
	Dat = Dat % 10;
	result += Dat;

	return(result);
}


void FFT(float dataR[NUM_FFT],float dataI[NUM_FFT])
{
	int x0,x1,x2,x3,x4,x5,x6,xx;
	int i,j,k,b,p,L;
	float TR,TI,temp;

	/********** following code invert sequence ************/
	for ( i=0;i<NUM_FFT;i++ )
	{
		x0=x1=x2=x3=x4=x5=x6=0;
		x0=i&0x01; x1=(i/2)&0x01; x2=(i/4)&0x01; x3=(i/8)&0x01;x4=(i/16)&0x01; x5=(i/32)&0x01; x6=(i/64)&0x01;
		xx=x0*64+x1*32+x2*16+x3*8+x4*4+x5*2+x6;
		dataI[xx]=dataR[i];
	}
	for ( i=0;i<NUM_FFT;i++ )
	{
		dataR[i]=dataI[i]; dataI[i]=0;
	}

	/************** following code FFT *******************/
	for ( L=1;L<=7;L++ )
	{ /* for(1) */
		b=1; i=L-1;
		while ( i>0 )
		{
			b=b*2; i--;
		} /* b= 2^(L-1) */
		for ( j=0;j<=b-1;j++ ) /* for (2) */
		{
			p=1; i=7-L;
			while ( i>0 ) /* p=pow(2,7-L)*j; */
			{
				p=p*2; i--;
			}
			p=p*j;
			for ( k=j;k<128;k=k+2*b ) /* for (3) */
			{
				TR=dataR[k]; TI=dataI[k]; temp=dataR[k+b];
				dataR[k]=dataR[k]+dataR[k+b]*cos_tab[p]+dataI[k+b]*sin_tab[p];
				dataI[k]=dataI[k]-dataR[k+b]*sin_tab[p]+dataI[k+b]*cos_tab[p];
				dataR[k+b]=TR-dataR[k+b]*cos_tab[p]-dataI[k+b]*sin_tab[p];
				dataI[k+b]=TI+temp*sin_tab[p]-dataI[k+b]*cos_tab[p];
			} /* END for (3) */
		} /* END for (2) */
	} /* END for (1) */
} /* END FFT */


void SampleDataModifyF(float *piRetValue)
{
	uint8_t i;
	uint32_t	Temp;
   	for(i=0; i<NUM_FFT; i++)
   	{
	   	piRetValue[i] = 0;
	   	Temp = 0;
	   	memcpy((uint8_t *)&Temp , &(Harmonictemp.InstantaneousData[i*3]) , 3);
   		if(Temp &0x800000)
   		{
   			Temp = (0xffffff - Temp) + 1;
   			piRetValue[i] = -(float)Temp/8388608;
   		}
   		else piRetValue[i] = (float)Temp/8388608;
   	}
}


void Fft_Harmonic_Exec(void)
{
	float ftemp_N,ftemp_P;
	float fBase;  //基波的平方
	int16_t i;
	uint8_t RN8302DataComm[1];
	uint32_t temp[2];
  /**** 启动波形采样  *****/
	if(Harmonictemp.StarFlag==0)
	{
		Harmonictemp.StarFlag =1;

		RN8302DataComm[0] = 0xe5; // 写使能位
		Rn8302Write(0x0180, RN8302DataComm, 1);
		RN8302DataComm[0] = 0xa0; // 启动采样值写缓存
		Rn8302Write(0x0163,RN8302DataComm,1);
		RN8302DataComm[0] = 0xDC; // 写保护
		Rn8302Write(0x0180,RN8302DataComm,1);

		Harmonictemp.Channel=0x00;
		Harmonictemp.ReadAdress = 0x0200;
		InitForFFT();

//		uint8_t temp[1];
//		Rn8302Read(0x0163,temp,1) ;
//		HAL_UART_Transmit(&huart1, temp, 1, 1000);

//		uint8_t temp[1]={0xdc};
//		uint8_t temp1[100];
//		uint8_t n;
//		n=HexToASCII(temp,temp1,1);
//		HAL_UART_Transmit(&huart1, temp1, n, 1000);

		return;
	}

	if(Harmonictemp.StarFlag == 0x01)
	{
		Harmonictemp.StarFlag = 0x02;
		Rn8302Read(0x0163,temp,1);
		if(temp[0] & 0x30)  return;


		uint32_t std_vol=32000000;
		uint16_t rated_vol=220;
		uint16_t rms_vol;

		uint32_t std_cur=16000000;
		uint16_t rated_cur=5;
		uint16_t rms_cur;

		rms_vol=std_vol/(rated_vol*10);
		rms_cur=std_cur/(rated_cur);

//		rms_vol=std_vol/(rated_vol*100);
//		rms_cur=std_cur/(rated_cur*1000);

//		for(i=0;i<7;i++)
		for(i=0;i<6;i++)
		{
			Dl645FrontTmp.UI[i] = 0 ;
			Rn8302Read( 0x0058+i , (uint32_t *)&Dl645FrontTmp.UI[i] , 4 ) ;  //基波电压有效值
		}
		for(i=0;i<3;i++)
		{
			HarmonicData.FU[i] = (uint32_t)(Dl645FrontTmp.UI[i]/rms_vol);    //基波电压有效值
		}
		for(i=0;i<3;i++)
		{
			HarmonicData.FI[i] =(int32_t)(Dl645FrontTmp.UI[i+3]/rms_cur);    //基波电压有效值
		}

	/****基波寄存器打印   *****/
//		if(Dl645FrontTmp.UI[0]>1000000)
//		{
//			uint8_t temp2[4];
//			temp2[3]=(uint8_t)Dl645FrontTmp.UI[0];
//			temp2[2]=(uint8_t)(Dl645FrontTmp.UI[0]>>8);
//			temp2[1]=(uint8_t)(Dl645FrontTmp.UI[0]>>16);
//			temp2[0]=(uint8_t)(Dl645FrontTmp.UI[0]>>24);
//			HAL_UART_Transmit(&huart1, temp2, 4, 1000);
//		}

//		if(HarmonicData.FU[0]>100)
//		{
//			uint8_t temp1[2];
//			temp1[1]=(uint8_t)HarmonicData.FU[0];
//			temp1[0]=(uint8_t)(HarmonicData.FU[0]>>8);
//			HAL_UART_Transmit(&huart1, temp1, 2, 1000);
//		}

		return;
	}

	if(Harmonictemp.StarFlag == 0x02)
	{
		for(i=0;i<128;i++)
		{
//			fnRN8302_ReadBuf(((Harmonictemp.ReadAdress)+i*16),(uint8_t *)&Harmonictemp.InstantaneousData[i*48]);
			Rn8302Read((Harmonictemp.ReadAdress+i),(uint32_t *)&Harmonictemp.InstantaneousData[i*3],3);
			//读取128个采样点
//			HAL_UART_Transmit(&huart1, &Harmonictemp.InstantaneousData[i], 1, 1000);
		}

		// 采样完成后进行谐波计算
		SampleDataModifyF(&HarmonicData.iRealArray[0]);	//采样数据修正
		for(i=0;i<NUM_FFT;i++)  HarmonicData.iMageArray[i]=0;
		FFT(HarmonicData.iRealArray, HarmonicData.iMageArray);

		ftemp_N = (float)(((float)HarmonicData.iRealArray[1] *(float)HarmonicData.iRealArray[1]) + ((float)HarmonicData.iMageArray[1] * (float)HarmonicData.iMageArray[1]));
		ftemp_N = sqrt(ftemp_N);
		fBase	= ftemp_N;
		ftemp_P = 0;
		for(i=0;i<50;i++)
	    {
	    	ftemp_N = (float)(((float)HarmonicData.iRealArray[i+2] *(float)HarmonicData.iRealArray[i+2]) + ((float)HarmonicData.iMageArray[i+2] * (float)HarmonicData.iMageArray[i+2]));
	    	ftemp_N = sqrt(ftemp_N);
	    	if(Harmonictemp.Channel<0x03)
	    	{
//	    		HarmonicData.HarmonicpercentU[Harmonictemp.Channel][i+1] = (fnHexToBcd_u32((uint32_t)(( ftemp_N * 1000000*Fftcoefficient[i]) /fBase))) ;
	    		HarmonicData.HarmonicpercentU[Harmonictemp.Channel][i+1] = (uint32_t)(( ftemp_N * 1000000*Fftcoefficient[i]) /fBase) ;
	    		ftemp_P += (((float)HarmonicData.HarmonicpercentU[Harmonictemp.Channel][i+1]) * ((float)HarmonicData.HarmonicpercentU[Harmonictemp.Channel][i+1]));
	    	}
	    	else
	    	{
//		    	HarmonicData.HarmonicpercentI[Harmonictemp.Channel-3][i+1] = (fnHexToBcd_u32((uint32_t)(( ftemp_N * 1000000*Fftcoefficient[i]) /fBase))) ;
	    		HarmonicData.HarmonicpercentI[Harmonictemp.Channel-3][i+1] = (uint32_t)(( ftemp_N * 1000000*Fftcoefficient[i]) /fBase) ;
	    	}
	    }

		if(Harmonictemp.Channel<0x03)
		{
			ftemp_P = sqrt(ftemp_P);
			HarmonicData.HarmonicpercentU[Harmonictemp.Channel][0] = (uint16_t)(ftemp_P);
		}
		HarmonicData.HarmonicpercentI[Harmonictemp.Channel-3][0] = 0x999999;
		HarmonicData.HarmonicpercentU[Harmonictemp.Channel][0] = 0x999999;
		Harmonictemp.Channel++;
		Harmonictemp.ReadAdress+=128;

//		for(i=0;i<50;i++)
//		{
//					/***  谐波含量打印  ***/
//
//			//			uint8_t temp3[2]={0XFF,0XFF};
//			//			HAL_UART_Transmit(&huart1, temp3, 1, 1000);
//			uint8_t temp2[4];
//			temp2[3]=(uint8_t)HarmonicData.HarmonicpercentU[0][i];
//			temp2[2]=(uint8_t)(HarmonicData.HarmonicpercentU[0][i]>>8);
//			temp2[1]=(uint8_t)(HarmonicData.HarmonicpercentU[0][i]>>16);
//			temp2[0]=(uint8_t)(HarmonicData.HarmonicpercentU[0][i]>>24);
//			HAL_UART_Transmit(&huart1, temp2, 4, 1000);
//		}


		for(i=Harmonictemp.Channel;i<6;i++)
		{
			Harmonictemp.Channel++;
			Harmonictemp.ReadAdress+=128;
		}

		if(Harmonictemp.Channel > 5 )
		{
			Harmonictemp.StarFlag = 0;
			return;
		}

	}
//	if(fnStamp_Through(Harmonictemp.TimeOutStamp)>8000)
//	{
//		Harmonictemp.StarFlag = 0;
//	}
}

void Three_Phase_Unbalance(void)
{
	int32_t temp1[3];
	int32_t temp2[3];
	float average_U;
	float Unbalance_U;
	float Unbalance_I;

	for(int i=0;i<6;i++)
	{
		Dl645FrontTmp.UI[i] = 0 ;
		Rn8302Read( 0x0007+i , (uint32_t *)&Dl645FrontTmp.UI[i] , 4 ) ;  //基波电压有效值
	}

	if(Dl645FrontTmp.UI[0]>Dl645FrontTmp.UI[1])
	{
		temp1[1]=Dl645FrontTmp.UI[0];
		temp1[2]=Dl645FrontTmp.UI[1];
	}else{
		temp1[1]=Dl645FrontTmp.UI[1];
		temp1[2]=Dl645FrontTmp.UI[0];
	}

	if(Dl645FrontTmp.UI[2]>temp1[1])
	{
		temp1[0]=Dl645FrontTmp.UI[2];
	}else if(Dl645FrontTmp.UI[2]>temp1[2]){
		temp1[0]=temp1[1];
		temp1[1]=Dl645FrontTmp.UI[2];
	}else{
		temp1[0]=temp1[1];
		temp1[1]=temp1[2];
		temp1[2]=Dl645FrontTmp.UI[2];
	}

	if(Dl645FrontTmp.UI[3]>Dl645FrontTmp.UI[4])
	{
		temp2[1]=Dl645FrontTmp.UI[3];
		temp2[2]=Dl645FrontTmp.UI[4];
	}else{
		temp2[1]=Dl645FrontTmp.UI[4];
		temp2[2]=Dl645FrontTmp.UI[3];
	}

	if(Dl645FrontTmp.UI[5]>temp2[4])
	{
		temp2[0]=Dl645FrontTmp.UI[5];
	}else if(Dl645FrontTmp.UI[5]>temp2[2]){
		temp2[0]=temp2[1];
		temp2[1]=Dl645FrontTmp.UI[5];
	}else{
		temp2[0]=temp2[1];
		temp2[1]=temp2[2];
		temp2[2]=Dl645FrontTmp.UI[5];
	}

	average_U=(temp1[0]+temp1[1]+temp1[2])/3;
	Unbalance_U=(temp1[0]-average_U)/average_U;
	Unbalance_I=(temp2[0]-temp2[2])/temp2[0];

	return;
}
