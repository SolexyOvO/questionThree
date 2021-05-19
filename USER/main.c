#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "math.h"
#include "ad9958.h"
#include "exti.h"

#define C        2//��ͨ�˲��Ŵ���
#define ANGLE    57.2957795130//ת��Ϊ�Ƕ�ֵ  180 / +3.1415
#define ADC_STEP 1//����Ҫ�ˣ�ֱ������������������
#define MOSTVOL   4//��ߵĵ�ѹ��4v���������л�ͼ


void draw_background();
u32 SINGLE_FRE = 20000000;
u8 modeflag = 0;//0��ɨƵ��1�ǵ��
u32 SOURCE_AMP =1.00; //1v ad9958�ķ���

//PF0~4 AD9958�Ŀ���
//ADC����ͨ��ADC1_0 pA0 ADC2_1 PA1
//����dmaͨ����������

//����ֵ��������ʾ   


void get_pha_amp(u16 adc_data1,u16 adc_data2,u16 * transdata)//������ADC��ֵ����������Ҫ֪������Դ �ķ����Լ���������ķŴ���
{
	double vol;
	double  pha;

	vol = 2 * sqrt( (adc_data1 * adc_data1 + adc_data2 * adc_data2)) / (SOURCE_AMP * C);
	pha = atan(adc_data2 /adc_data1) * ANGLE;//���b
	vol = 20 * log(vol / SOURCE_AMP) / log(10);
	*transdata = vol;
	transdata ++;
	*transdata = pha; 
	//printf("vol :%lf ,�Ƕȣ� %lf\r\n",vol,amp);
}

void sweeperfre(void)
{
	while(1)
	{
		//int ampdata[2];
		u16 draw[2];
		u16 amp[2];
		int fre;
		int i = 0;
	
		//�Ȼ�����,��������
		LCD_Clear(WHITE);
		draw_background();
	

		//ɨƵ����
		for(fre = 100000,i = 0 ; fre <= 40000000;fre += 100000)
		{

			//ad9958���Ʋ�����·��
			ad9958_out(2,fre,0,840);
		
			//��ʾ��ǰ��Ƶ��
		
		
			//adc �ɼ�
			amp[0] = get_adc1(5);
			amp[1] = get_adc2(5);
			get_pha_amp(amp[0],amp[1],amp);//�Բɼ����ĵ��������
			printf("amp:%d,%d\r\n",amp[0],amp[1]);
			//��ʼ��
			if(i == 0)
			{
				draw[0] = 0;
				draw[1] = 0;
			}
			POINT_COLOR=BLUE;//��������Ϊ��ɫ 
			LCD_DrawLine(i + 50,700 - draw[0] / 20 * 40,i + 51,700 - amp[0] / 20 * 40);//���껹δ������Ҫ��������д���
			printf("draw:%d,%d\r\n",draw[0],draw[1]);
			
			POINT_COLOR=YELLOW;//��������Ϊ��ɫ 
			LCD_DrawLine(i + 50,700 - ((int)draw[1] % 90) / 90 * 400,i + 51,700 - ((int)amp[1] % 90) / 90 * 400);
			POINT_COLOR=RED;//��������Ϊ��ɫ
			draw[0] = amp[0];
			draw[1] = amp[1];
			i++;
		}

	}
	
	
	
}

void draw_background()
{
	u8 cir = 0;
	for(cir = 0;cir < 11;cir ++)
	{
		POINT_COLOR=RED;//��������Ϊ��ɫ
		LCD_DrawLine(50,700 - cir * 40,450,700 - cir * 40);
		LCD_DrawLine(50 + 40 * cir,700,50 + 40 * cir,300);
		
	}
	cir = 0;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"0");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"4M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"8M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"12M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"16M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"20M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"24M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"28M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"32M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"36M");cir ++;
	LCD_ShowString(42 + 40 * cir,720,24,16,16,"40M");cir ++;
	
	cir = 1;
	LCD_ShowString(0,720- 40 * cir,32,16,16,"0.4v");cir ++;
}

void draw_backgroud1(void)//���ã�����Ļ�ϲ��ÿ���
{
	LCD_ShowString(10,50,24 * 3,24,24,"+1");
	LCD_ShowString(10,110,24 * 3,24,24,"+10");
	LCD_ShowString(10,170,24 * 3,24,24,"+100");
	LCD_ShowString(10,230,24 * 3,24,24,"+1k");
	LCD_ShowString(10,290,24 * 3,24,24,"+10k");
	LCD_ShowString(10,350,24 * 3,24,24,"+100k");
	LCD_ShowString(10,410,24 * 3,24,24,"+1M");
	LCD_ShowString(10,470,24 * 3,24,24,"+10M");

	LCD_ShowString(350,50,24 * 3,24,24,"-1");
	LCD_ShowString(350,110,24 * 3,24,24,"-10");
	LCD_ShowString(350,170,24 * 3,24,24,"-100");
	LCD_ShowString(350,230,24 * 3,24,24,"-1k");
	LCD_ShowString(350,290,24 * 3,24,24,"-10k");
	LCD_ShowString(350,350,24 * 3,24,24,"-100k");
	LCD_ShowString(350,410,24 * 3,24,24,"-1M");
	LCD_ShowString(350,470,24 * 3,24,24,"-10M");





}



void single_fre(void)
{
	u16 data[2];//�ȷֱ������������������������ʡ�ռ�
	u16 fre;

{
	while(1)
	{
		int past[2];
		int current[2];
		int x,y;
		u8 flag = 1;//��һ�δ���
		tp_dev.scan(0);
		
		for(t=0;t<5;t++)
		{
			if((tp_dev.sta)&(1<<t))
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
						x = lastpos[t][0];
						y = lastpos[t][1];
                        if(flag)
                        {
							current[0] = 0;
                        	current[1] = 0;
							past[0] = tp_dev.x[0];
                        	past[1] = tp_dev.y[0];
                            flag = 0;
                        }
						else
						{
							current[0] = tp_dev.x[0];
                        	current[1] = tp_dev.y[0];
						}
                       
					}
					if((current[0] - past[0] > 10)  | (current[0] - past[0] < -10) | (current[1] - past[1] > 10) | (current[1] - past[1] < -10))
					{
						current[1] = past[1];
						current[0] = past[0];
						if((y < 500 )//�Ӽ���
						{
							if(x < 180)
							{
								if(y > 470)
								{
									

								}

							}
							else 
							{


							}



						}
						
				    }
					
					
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
	}



}
	//����
	LCD_Clear(WHITE);
	
	//����һ������ ���ƽ���
	draw_backgroud1();
	
	
	//ad9958������
	ad9958_out(2,SINGLE_FRE,0,1023);
	
	
	
	
	
	//�ɼ���Ϣ
	data[0] = get_adc1(5);
	data[1] = get_adc2(5);
	//��������
	get_pha_amp(data[0],data[1],data);
	
	
	//��ʾ���
	LCD_ShowString(4,700,48,24,24,"AMP:");
	LCD_ShowxNum(60,700,data[0],5,24,0);
	
	
	LCD_ShowString(4,730,48,24,24,"PHA:");
	LCD_ShowxNum(60,730,data[1],5,24,0);


	
}
  


 int main(void)
 {	 
	u16 adcx1;
	u16 adcx2;
	float temp;
	float temp1;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	EXTIX_Init();
	LED_Init();
	LCD_Init();			 	
 	adc_init();		  		//ADC��ʼ��
	ad9958_init_1();


	POINT_COLOR=RED;//��������Ϊ��ɫ 

	draw_background(); 
	sweeperfre();	

	while(1)
	{
		
	}
 }

