#define SYSCTL 0x400FE000U //this is the base HEX address for System controller
#define HB_GPIOE_BASE 0x4005C000U //Hex address for High performance Bus PORTE
#define HB_GPIOF_BASE 0x4005D000U //Hex Address for High performance Bus for PORTF
#define VTABLE_BASE 0xE000E000U //Hex address for Vector Table for Interrupts
#define ADC1_BASE 0x40039000U //No one wants to see the number 0x40039000 over and over again

//some offset for GPIO addresses defined to make reading the code easier
#define RCGCGPIO_OFFSET 0x608U 
#define GPIOHBCTL_OFFSET 0x06CU  
#define GPIO_DEN_OFFSET 0x51CU 
#define GPIO_DIR_OFFSET 0x400U 
#define GPIO_DATA_OFFSET 0x3FCU 
//

//All this code is for ADC particularly for ADC1 Sample Sequencer 3
#define ADC_OFFSET 0x638U // This offset needs to be 
#define AFSEL_OFFSET 0x420U 
#define GPIO_AMSEL_OFFSET 0x528U 
#define ADCACTS_OFFSET 0x000U 
#define ADCEMUX_OFFSET 0x014U 
#define SSPRI_OFFSET 0x020U 
#define ADCT1MUX3_OFFSET 0x0A0U 
#define ADCSSCTL3_OFFSET 0x0A4U //this offset is for initializing end of sequence and enable raw interrupts 
#define ADCIM_OFFSET 0x008U 
#define ADCISC_OFFSET 0x00CU //ADC interrupt service clear 
#define ADC1SS3FIFO_OFFSET 0x0A8//What I primarily know about FIFO is that its where ADC info is stored
#define ADC1SS3_Handler_OFFSET 0x104U

#define RCGCGPIO_PORT (*((unsigned int*)(SYSCTL + RCGCGPIO_OFFSET))) //this enables a GPIOPORT of my choice, but to be clearer I have the intentions of for PORTF
#define GPIOHBCTL_PORT (*((unsigned int*)(SYSCTL + GPIOHBCTL_OFFSET))) //enables me to use High performance Bus

#define HB_GPIOEDEN (*((unsigned int*)(HB_GPIOE_BASE + GPIO_DEN_OFFSET))) //Digitally enable AHB_PORFTE to be used
#define HB_GPIOEDIR (*((unsigned int*)(HB_GPIOE_BASE + GPIO_DIR_OFFSET))) // I'll have to enable the HBGPIOE to input
#define HB_GPIOE_AMSEL (*((unsigned int*)(HB_GPIOE_BASE + GPIO_AMSEL_OFFSET))) 
#define GPIOAFSEL_E (*((unsigned int*)(HB_GPIOE_BASE + AFSEL_OFFSET)))

#define ADC1_ACTSS3 (*(unsigned int*)(ADC1_BASE + ADCACTS_OFFSET))//use sample sequence 3
#define ADC1EMUX (*(unsigned int*)(ADC1_BASE + ADCEMUX_OFFSET))
#define ADC1SSPRI (*(unsigned int*)(ADC1_BASE + SSPRI_OFFSET))//sample sequence priority
#define ADC1SSMUX3 (*(unsigned int*)(ADC1_BASE + ADCT1MUX3_OFFSET))//use analog input to determine what to add in
#define ADCSSCTL3 (*(unsigned int*)(ADC1_BASE + ADCSSCTL3_OFFSET))
#define ADCSS3IM (*(unsigned int*)(ADC1_BASE + ADCIM_OFFSET))
#define ADCISC (*(unsigned int*)(ADC1_BASE + ADCISC_OFFSET))
#define ADC1SS3FIFO (*(unsigned int*)(ADC1_BASE + ADC1SS3FIFO_OFFSET))
#define ADC1SS3_Handler_EN (*(unsigned int*)(VTABLE_BASE + ADC1SS3_Handler_OFFSET))

#define GPIOAFSEL_F (*((unsigned int*)(HB_GPIOF_BASE + AFSEL_OFFSET)))
#define HB_GPIOFDEN (*((unsigned int*)(HB_GPIOF_BASE + GPIO_DEN_OFFSET))) //Digitally enable AHB_PORFTE with some Data it can use
#define HB_GPIOFDIR (*((unsigned int*)(HB_GPIOF_BASE  + GPIO_DIR_OFFSET))) 
#define HB_GPIOFDATA (*(((unsigned int*)(HB_GPIOF_BASE + GPIO_DATA_OFFSET))))


#define RCGCADC (*(unsigned int*)(SYSCTL + ADC_OFFSET)) 

#define SetEn_0_31_OFFSET 0x100U //enables vector Interrupt numbers from 0 to 31, for this project our vector interrupt number for timer0A is 19


#define RED (1U << 1)
#define BLUE (1U << 2)
#define GREEN (1U << 3)

volatile static unsigned long int PE1_Dig_Input = 0;


void ADC1SS3_Handler ( void )
{
  ADCISC = (1U << 3); //clear interrupt before using it

}


int main()
{

  RCGCADC = (1U << 1); //enable me to use ADC   (X)
  RCGCGPIO_PORT = (1U << 4) | (1U << 5); // I've enabled GPIOPORTE and PortF aswell  (X)
  GPIOHBCTL_PORT = (1U << 4) | (1U << 5); // High performance Bus PORTE and PortF    (X)

  HB_GPIOFDEN |= (RED | BLUE | GREEN); // enabled RED
  HB_GPIOFDIR |= (RED | BLUE | GREEN); // I've set RED as an output
  GPIOAFSEL_F = 0x00U;
  
  //This is for ADC, using PORTE1
  HB_GPIOEDIR = (0U << 1); // HBGPIOE_Pin_1 is now an input, shifting 1 bits to the left indicates I wish to use HBGPIOE1 as an input (X)
  GPIOAFSEL_E = (1U << 1); //This is the Alternative function select, it allows me to use PE1  (X)
  HB_GPIOEDEN = (0U << 1); //using HB_GPIOE 1 and disabling the Digital Enable  (X)
  HB_GPIOE_AMSEL = (1U << 1); //analog mode select, I shifted 1 to the left becauseI wanted to use PE1, which has analog input of 2, so 1 << 1 = 10 which 2 in binary (X)

  
  
  ADC1_ACTSS3 = (0U << 3);//clear the bits (X)
  ADC1EMUX |= (0xF << 12); //This is the trigger select 0xF mean continous(X)
  ADC1SSPRI = (0x3U << 12); //(X) supposed to be lowest priority
  ADC1SSMUX3 = 2; 
  ADCSSCTL3 = 0x6;//0x6 or 3'b110 (X)
  ADCSS3IM = (1U << 3); //enable my type of interrupts to be used (X)
  

  ADC1_ACTSS3 |= (1U << 3); //enable analog digital converstion Sample sequencer 3
  ADCISC = (1U << 3); //clear interrupt before using it

  ADC1SS3_Handler_EN = (1U << 19);
  

  
  while(1)
  {
    PE1_Dig_Input = ADC1SS3FIFO;
    if(PE1_Dig_Input >= 1000 && PE1_Dig_Input < 2000)

        HB_GPIOFDATA = GREEN;

      else if(PE1_Dig_Input < 1000)
      {
        HB_GPIOFDATA = RED;
        
      }
      else if (PE1_Dig_Input > 2000 && PE1_Dig_Input < 3000)
      {
        HB_GPIOFDATA = BLUE;
      }
      else if (PE1_Dig_Input > 3000 && PE1_Dig_Input < 4000)
      {
        HB_GPIOFDATA = BLUE|RED;
      }
      else if (PE1_Dig_Input > 4000)
      {
        HB_GPIOFDATA = BLUE|GREEN;
      }
  }
  
}