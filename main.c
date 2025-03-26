#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "mtb_bmi270.h"
#include "fall_lib.h"

mtb_bmi270_t imu;

//i2c config
cyhal_i2c_t i2c;
cyhal_i2c_cfg_t i2c_cfg =
{
    .is_slave        = false,
    .address         = 0,
    .frequencyhal_hz = 100000
};

#define IMU_I2C_SDA (CYBSP_I2C_SDA)
#define IMU_I2C_SCL (CYBSP_I2C_SCL)

//convert imu int data to float
#define IMU_MODEL_CONVERSION_FACTOR     4096.0f

#define IMU_SCAN_RATE         50
#define IMU_TIMER_FREQUENCY   100000
#define IMU_TIMER_PERIOD      (IMU_TIMER_FREQUENCY/IMU_SCAN_RATE)
#define IMU_TIMER_PRIORITY    3
//flag for IMU
volatile bool IMU_flag;
//model class names
static const char* LABELS[IMAI_DATA_OUT_COUNT] = IMAI_SYMBOL_MAP;

/*System tick variables*/
unsigned long tick = 0.0;
void systick_isr(void)
{
    tick++; // This increments every time the SysTick counter decrements to 0.
}

void getTimeFromMilliseconds(unsigned long milliseconds, char* output);

/* timer used for getting data */
cyhal_timer_t imu_timer;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void imu_interrupt_handler(void* callback_arg, cyhal_timer_event_t event);
cy_rslt_t imu_timer_init(void);



//update imu config
static void update_sensor_bmi270(struct bmi2_dev* sensor)
{
	struct bmi2_sens_config config = {0};
	/* Select the Output data rate, range of accelerometer sensor */
	config.type = BMI2_ACCEL;
	config.cfg.acc.range = BMI2_ACC_RANGE_8G;
	config.cfg.acc.odr = BMI2_ACC_ODR_50HZ;

	int8_t status = bmi2_set_sensor_config(&config, 1, sensor);

	if (BMI2_OK != status)
	{
		printf("Error updating the IMU sensor configuration.\n");
	}
}
int main(void)
{
    cy_rslt_t result;
    mtb_bmi270_data_t data;
    int dataOut[IMAI_DATA_OUT_COUNT];

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /* Initialize I2C for interfacing with sensor */
    result = cyhal_i2c_init(&i2c, IMU_I2C_SDA, IMU_I2C_SCL, NULL);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }
    result = cyhal_i2c_configure(&i2c, &i2c_cfg);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /* Initialize the sensor and configure default settings */
    result = mtb_bmi270_init_i2c(&imu, &i2c, MTB_BMI270_ADDRESS_DEFAULT);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }
    result = mtb_bmi270_config_default(&imu);
    if (CY_RSLT_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /*  update bmi270  * */
    update_sensor_bmi270(&(imu.sensor));

	IMU_flag = false;
	/* Timer for data collection */
	result = imu_timer_init();
	if(CY_RSLT_SUCCESS != result)
	{
		CY_ASSERT(0);
	}

	/* Initialize the User LED */
	result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
								 CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/*timer set up*/
	Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_IMO , (8000000/1000)-1);
	Cy_SysTick_SetCallback(0, systick_isr);        // point to SysTick ISR to increment the 1ms count

	/* Initialize the Machine Learning model */
	IMAI_FED_init();

	/*starting time of model initialization */
    unsigned long start_t = tick;
	int ledon_t = 0;

	printf("\r\n\n****************** IMAGIMOB Ready Model %s Code Example ****************** \r\n\n", LABELS[1]);

    for (;;)
    {
        //cyhal_system_delay_ms(20);
        if (IMU_flag)
        {
        	IMU_flag = false;

            /* Get the accelerometer*/
            result = mtb_bmi270_read(&imu, &data);
            float dataIn[3] =
			{
				data.sensor_data.acc.x / (float)IMU_MODEL_CONVERSION_FACTOR,
				-(data.sensor_data.acc.y / (float)IMU_MODEL_CONVERSION_FACTOR),
				-(data.sensor_data.acc.z / (float)IMU_MODEL_CONVERSION_FACTOR),
			 };

			int enqueue_reslut = IMAI_FED_enqueue(dataIn);
			if (IMAI_RET_SUCCESS != enqueue_reslut)
			{
				printf("Insufficient memory to enqueue sensor data. Inferencing is not keeping up.\n");
			}

			int dequeue_result = IMAI_FED_dequeue(dataOut);

			/*LED variables*/
			static int led_off = 0;
			static int prediction_count = 0;

			switch (dequeue_result)
			{
				case IMAI_RET_SUCCESS:

					static int16_t success_flag = 1;
					prediction_count +=1;
					if (dataOut[1]==1)
					{
						cyhal_gpio_write((cyhal_gpio_t) CYBSP_USER_LED, CYBSP_LED_STATE_ON);
						ledon_t= tick;

						/*new line when a detection is triggered*/
						if (led_off == 1)
						{
							printf("\r\n");
							led_off = 0;

						}

						/*print triggered class and the triggered time since IMAI Initial.*/
						unsigned long t = tick - start_t;
						char timeString[9];
						getTimeFromMilliseconds(t, timeString);
						printf("%s %s\r\n",LABELS[1],timeString);
						printf("%s\r\n",LABELS[1]);

					}
					else
					{
						/*only print non-label class very 10 predictions*/
						if (prediction_count>9)
						{
							printf(".");
							fflush( stdout );
							prediction_count =0;
						}

					}
					/*turn off fall LED after the LED is on for 10s*/
					if((tick - ledon_t)>10000)
					{
						cyhal_gpio_write((cyhal_gpio_t) CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
						led_off = 1;
					}
					break;
				case IMAI_RET_NODATA:
					/* Sleep until more data is available */
					cyhal_syspm_sleep();
					break;
				case IMAI_RET_NOMEM:
					/* Something went wrong, stop the program */
					printf("Unable to perform inference. Internal memory error.\n");
					break;
				case IMAI_RET_TIMEDOUT:
					if (success_flag == 1)
					{
						printf("The evaluation period has ended. Please rerun the evaluation or purchase a license for the ready model.\r\n");
					}
					break;
				default:
					/* Something went wrong, stop the program */
					printf("Unable to perform inference. Unknown error occurred.\n");
					break;
			}
        }

    }
}

/*******************************************************************************
* Function Name: getTimeFromMilliseconds
*********************************************************************************/
void getTimeFromMilliseconds(unsigned long milliseconds, char* timeString) {
  unsigned int seconds = (milliseconds / 1000) % 60;
  unsigned int minutes = (milliseconds / (1000 * 60)) % 60;
  unsigned int hours = (milliseconds / (1000 * 60 * 60));
  sprintf(timeString, "%02u:%02u:%02u",hours, minutes, seconds);
}
/*******************************************************************************
* Function Name: imu_interrupt_handler
********************************************************************************
* Summary:
*   Interrupt handler for timer. Interrupt handler will get called at 50Hz and
*   sets a flag that can be checked in main.
*
* Parameters:
*     callback_arg: not used
*     event: not used
*
*
*******************************************************************************/
void imu_interrupt_handler(void *callback_arg, cyhal_timer_event_t event)
{
    (void) callback_arg;
    (void) event;

    IMU_flag = true;
}
/*******************************************************************************
* Function Name: imu_timer_init
********************************************************************************
* Summary:
*   Sets up an interrupt that triggers at the desired frequency.
*
* Returns:
*   The status of the initialization.
*
*
*******************************************************************************/
cy_rslt_t imu_timer_init(void)
{
    cy_rslt_t rslt;
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = IMU_TIMER_PERIOD,         /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run the timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };

    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */
    rslt = cyhal_timer_init(&imu_timer, NC, NULL);
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }

    /* Apply timer configuration such as period, count direction, run mode, etc. */
    rslt = cyhal_timer_configure(&imu_timer, &timer_cfg);
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }

    /* Set the frequency of timer to 100KHz */
    rslt = cyhal_timer_set_frequency(&imu_timer, IMU_TIMER_FREQUENCY);
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&imu_timer, imu_interrupt_handler, NULL);
    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&imu_timer, CYHAL_TIMER_IRQ_TERMINAL_COUNT, IMU_TIMER_PRIORITY, true);
    /* Start the timer with the configured settings */
    rslt = cyhal_timer_start(&imu_timer);
    if (CY_RSLT_SUCCESS != rslt)
    {
        return rslt;
    }

    return CY_RSLT_SUCCESS;
}
