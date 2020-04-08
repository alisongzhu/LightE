#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
//#include "delay.h"
//#include "sys.h"
//#include "lcd.h"
#include "main.h"
#include "include.h"

extern void led_set(u8 sta);
extern void test_fun(void(*ledset)(u8),u8 sta);										  
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"uint32_t read_addr(uint32_t addr)",
	(void*)write_addr,"void write_addr(uint32_t addr,uint32_t val)",
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(uint32_t nus)",
	(void*)LED,"uint8_t LED(u8 temp1,u8 temp2,u8 temp3)",
	(void*)Fft_Harmonic_Exec,"void Fft_Harmonic_Exec(void)",
	(void*)Three_Phase_Unbalance,"void Three_Phase_Unbalance(void)",
//	(void*)led_set,"void led_set(u8 sta)",
//	(void*)test_fun,"void test_fun(void(*ledset)(u8),u8 sta)",
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	0,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















