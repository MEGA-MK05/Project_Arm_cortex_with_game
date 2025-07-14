#include "device_driver.h"

#define TIM2_TICK         	(20) 				// usec
#define TIM2_FREQ 	  		(1000000/TIM2_TICK)	// Hz
#define TIME2_PLS_OF_1ms  	(1000/TIM2_TICK)
#define TIM2_MAX	  		(0xffffu)


#define TIM4_TICK	  		(20) 				// usec
#define TIM4_FREQ 	  		(1000000/TIM4_TICK) // Hz
#define TIME4_PLS_OF_1ms  	(1000/TIM4_TICK)
#define TIM4_MAX	  		(0xffffu)

#define TIM3_FREQ 	  		(8000000) 	      	// Hz
#define TIM3_TICK	  		(1000000/TIM3_FREQ)	// usec
#define TIME3_PLS_OF_1ms  	(1000/TIM3_TICK)


void TIM2_Stopwatch_Start(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->PSC = (unsigned int)(TIMXCLK/50000.0 + 0.5)-1;
	TIM2->ARR = TIM2_MAX;

	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Set_Bit(TIM2->CR1, 0);
}

unsigned int TIM2_Stopwatch_Stop(void)
{
	unsigned int time;

	Macro_Clear_Bit(TIM2->CR1, 0);
	time = (TIM2_MAX - TIM2->CNT) * TIM2_TICK;
	return time;
}

/* Delay Time Max = 65536 * 20use = 1.3sec */

#if 0

void TIM2_Delay(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
	TIM2->ARR = TIME2_PLS_OF_1ms * time;

	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Clear_Bit(TIM2->SR, 0);
	Macro_Set_Bit(TIM2->DIER, 0);
	Macro_Set_Bit(TIM2->CR1, 0);

	while(Macro_Check_Bit_Clear(TIM2->SR, 0));

	Macro_Clear_Bit(TIM2->CR1, 0);
	Macro_Clear_Bit(TIM2->DIER, 0);
}

#else

/* Delay Time Extended */

void TIM2_Delay(int time)
{
	int i;
	unsigned int t = TIME2_PLS_OF_1ms * time;

	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->ARR = 0xffff;
	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Set_Bit(TIM2->DIER, 0);

	for(i=0; i<(t/0xffffu); i++)
	{
		Macro_Set_Bit(TIM2->EGR,0);
		Macro_Clear_Bit(TIM2->SR, 0);
		Macro_Set_Bit(TIM2->CR1, 0);
		while(Macro_Check_Bit_Clear(TIM2->SR, 0));
	}

	TIM2->ARR = t % 0xffffu;
	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Clear_Bit(TIM2->SR, 0);
	Macro_Set_Bit(TIM2->CR1, 0);
	while (Macro_Check_Bit_Clear(TIM2->SR, 0));

	Macro_Clear_Bit(TIM2->CR1, 0);
	Macro_Clear_Bit(TIM2->DIER, 0);
}

#endif

void TIM4_Repeat(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 2);

	TIM4->CR1 = (1<<4)|(0<<3);
	TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
	TIM4->ARR = TIME4_PLS_OF_1ms * time - 1;

	Macro_Set_Bit(TIM4->EGR,0);
	Macro_Clear_Bit(TIM4->SR, 0);
	Macro_Set_Bit(TIM4->DIER, 0);
	Macro_Set_Bit(TIM4->CR1, 0);
}

int TIM4_Check_Timeout(void)
{
	if(Macro_Check_Bit_Set(TIM4->SR, 0))
	{
		Macro_Clear_Bit(TIM4->SR, 0);
		return 1;
	}
	else
	{
		return 0;
	}
}
int TIM2_Check_Timeout(void)
{
	if(Macro_Check_Bit_Set(TIM2->SR, 0))
	{
		Macro_Clear_Bit(TIM2->SR, 0);
		return 1;
	}
	else
	{
		return 0;
	}
}
void TIM4_Stop(void)
{
	Macro_Clear_Bit(TIM4->CR1, 0);
	Macro_Clear_Bit(TIM4->DIER, 0);
}

void TIM4_Change_Value(int time)
{
	TIM4->ARR = TIME4_PLS_OF_1ms * time;
}

void TIM3_Out_Init(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 1);
	Macro_Set_Bit(RCC->APB2ENR, 3);
	Macro_Write_Block(GPIOB->CRL,0xf,0xb,0);
	Macro_Write_Block(TIM3->CCMR2,0x7,0x6,4);
	TIM3->CCER = (0<<9)|(1<<8);
}

void TIM3_Out_Freq_Generation(unsigned short freq)
{
	TIM3->PSC = (unsigned int)(TIMXCLK/(double)TIM3_FREQ + 0.5)-1;
	TIM3->ARR = (double)TIM3_FREQ/freq-1;
	TIM3->CCR3 = TIM3->ARR/2;

	Macro_Set_Bit(TIM3->EGR,0);
	TIM3->CR1 = (1<<4)|(0<<3)|(0<<1)|(1<<0);
}


void TIM3_Out_Stop(void)
{
	Macro_Clear_Bit(TIM3->CR1, 0);
	Macro_Clear_Bit(TIM3->DIER, 0);
}

void TIM4_Repeat_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 2);

		TIM4->CR1 = (1<<4)|(0<<3);
		TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
		TIM4->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM4->EGR,0); //MSEC 단위로 만들기 위한 밑작업.

		// TIM4->SR 레지스터에서 Timer Pending Clear
		TIM4->SR= (0<<0);
		// NVIC에서 30번 인터럽트 Pending Clear => NVIC용 Macro 사용
		NVIC_ClearPendingIRQ(30);

		// TIM4->DIER 레지스터에서 Timer 인터럽트 허용
        TIM4->DIER = (1<<0);
		// NVIC에서 30번 인터럽트를 허용으로 설정 => NVIC용 Macro 사용
        NVIC_EnableIRQ(30);
		// TIM4 Start
		Macro_Set_Bit(TIM4->CR1, 0);
	}

	else
	{
		NVIC_DisableIRQ(30);
		Macro_Clear_Bit(TIM4->CR1, 0);
		Macro_Clear_Bit(TIM4->DIER, 0);
	}
}

void TIM3_Repeat_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 1);

		TIM3->CR1 = (1<<4)|(0<<3);
		TIM3->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
		TIM3->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM3->EGR,0); //MSEC 단위로 만들기 위한 밑작업.

		// TIM4->SR 레지스터에서 Timer Pending Clear
		TIM3->SR= (0<<0);
		// NVIC에서 30번 인터럽트 Pending Clear => NVIC용 Macro 사용
		NVIC_ClearPendingIRQ(29);

		// TIM4->DIER 레지스터에서 Timer 인터럽트 허용
        TIM3->DIER = (1<<0);
		// NVIC에서 30번 인터럽트를 허용으로 설정 => NVIC용 Macro 사용
        NVIC_EnableIRQ(29);
		// TIM4 Start
		Macro_Set_Bit(TIM3->CR1, 0);
	}

	else
	{
		NVIC_DisableIRQ(29);
		Macro_Clear_Bit(TIM3->CR1, 0);
		Macro_Clear_Bit(TIM3->DIER, 0);
	}
}

void TIM2_Repeat_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 0);

		TIM2->CR1 = (1<<4)|(0<<3);
		TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
		TIM2->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM2->EGR,0); //MSEC 단위로 만들기 위한 밑작업.

		// TIM4->SR 레지스터에서 Timer Pending Clear
		TIM2->SR= (0<<0);
		// NVIC에서 30번 인터럽트 Pending Clear => NVIC용 Macro 사용
		NVIC_ClearPendingIRQ(28);

		// TIM4->DIER 레지스터에서 Timer 인터럽트 허용
        TIM2->DIER = (1<<0);
		// NVIC에서 30번 인터럽트를 허용으로 설정 => NVIC용 Macro 사용
        NVIC_EnableIRQ(28);
		// TIM4 Start
		Macro_Set_Bit(TIM2->CR1, 0);
	}

	else
	{
		NVIC_DisableIRQ(28);
		Macro_Clear_Bit(TIM2->CR1, 0);
		Macro_Clear_Bit(TIM2->DIER, 0);
	}
}


/*추가가*/
void TIM2_Delay2(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);
	// TIM2 CR1 설정: down count, one pulse
	Macro_Set_Bit(TIM2->CR1,4);
	Macro_Clear_Bit(TIM2->CR1,3);
	// PSC 초기값 설정 => 20usec tick이 되도록 설계 (50KHz)
    TIM2->PSC = (HCLK/TIM2_FREQ)-1;
	// ARR 초기값 설정 => 요청한 time msec에 해당하는 초기값 설정
	TIM2->ARR= (time*(TIM2_FREQ/1000));
	// UG 이벤트 발생
	Macro_Set_Bit(TIM2->EGR,0);
	// Update Interrupt Pending Clear
	Macro_Clear_Bit(TIM2->SR,0);
	// Update Interrupt Enable

	// TIM2 start
    Macro_Set_Bit(TIM2->CR1,0);
}

void TIM3_Delay2(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 1);
	// TIM2 CR1 설정: down count, one pulse
	Macro_Set_Bit(TIM3->CR1,4);
	Macro_Clear_Bit(TIM3->CR1,3);
	// PSC 초기값 설정 => 20usec tick이 되도록 설계 (50KHz)
    TIM3->PSC = (HCLK/TIM2_FREQ)-1;
	// ARR 초기값 설정 => 요청한 time msec에 해당하는 초기값 설정
	TIM3->ARR= (time*(TIM2_FREQ/1000));
	// UG 이벤트 발생
	Macro_Set_Bit(TIM3->EGR,0);
	// Update Interrupt Pending Clear
	Macro_Clear_Bit(TIM3->SR,0);
	// Update Interrupt Enable

	// TIM2 start
    Macro_Set_Bit(TIM3->CR1,0);
}

void TIM4_PWM(int duty)
{
   Macro_Set_Bit(RCC->APB1ENR, 2);

   TIM4->CR1 = (0X1<<4);
   // Timer 주파수가 TIM3_FREQ가 되도록 PSC 설정
   TIM4->PSC= (unsigned int)(TIMXCLK/(double)TIM3_FREQ + 0.5)-1;
   // 요청한 주파수가 되도록 ARR 설정 (1000hz)
   TIM4->ARR = (TIM3_FREQ / 1000);

   // Manual Update(UG 발생)
   TIM4->CCR3= (int)(TIM4->ARR*(duty*0.1));
   Macro_Set_Bit(TIM4->EGR,0);
   Macro_Set_Bit(TIM4->CR1,0);
}

void TIM4_Change_Duty(int duty)
{
   TIM4->CCR3= (int)(TIM4->ARR*(duty*0.1)); //하드 코딩. 나중에 바꾸기.
}

void TIM4_Out_Init(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 2);
	Macro_Set_Bit(RCC->APB2ENR, 3);
	Macro_Write_Block(GPIOB->CRH,0xf,0xe,0);
	Macro_Write_Block(TIM4->CCMR2,0xff,0x68,0);
	TIM4->CCER = (0<<9)|(1<<8);
}

void TIM2_Delay_Itr(int time)
{
   Macro_Set_Bit(RCC->APB1ENR, 0);

   TIM2->CR1 = (1<<4)|(1<<3);
   TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
   TIM2->ARR = TIME2_PLS_OF_1ms * time;

   Macro_Set_Bit(TIM2->EGR,0);
   Macro_Clear_Bit(TIM2->SR, 0);
   NVIC_ClearPendingIRQ(28);
   Macro_Set_Bit(TIM2->DIER, 0);
   NVIC_EnableIRQ(28);
   Macro_Set_Bit(TIM2->CR1, 0);

}
void TIM3_Delay_Itr(int time)
{
   Macro_Set_Bit(RCC->APB1ENR, 1);

   TIM3->CR1 = (1<<4)|(1<<3);
   TIM3->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
   TIM3->ARR = TIME2_PLS_OF_1ms * time;

   Macro_Set_Bit(TIM3->EGR,0);
   Macro_Clear_Bit(TIM3->SR, 0);
   NVIC_ClearPendingIRQ(29);
   Macro_Set_Bit(TIM3->DIER, 0);
   NVIC_EnableIRQ(29);
	Macro_Set_Bit(TIM3->CR1, 0);

}