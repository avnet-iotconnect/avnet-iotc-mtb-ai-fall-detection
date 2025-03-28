#include <stdbool.h>
#include "cyhal.h"
#include "cybsp.h"
#include "mtb_bmi270.h"
#include "fall_lib.h"

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"

#include "app.h"

/*
 We use this define to track changes that we made to the original app
 but we don't address unused variables and functions just so that re-integration
 with a new baseline is easier 
 */
#define AVNET_APP_CHANGES

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

//model class names
static const char* LABELS[IMAI_DATA_OUT_COUNT] = IMAI_SYMBOL_MAP;

// Record when fall is detected. was_fall_detected() will get and clear the flag.
static bool fall_detected = false;

// get and clear
const bool was_fall_detected(void) {
    const char* class_map[] = IMAI_SYMBOL_MAP;
    const char* ret = fall_detected ? class_map[1] : NULL;
    fall_detected = false; // Unless the next detection triggers at some time...
    return ret;
}

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

void fed_task(void *pvParameters) {
    (void) pvParameters;

    cy_rslt_t result;
    mtb_bmi270_data_t data;
    int dataOut[IMAI_DATA_OUT_COUNT];

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

	/* Initialize the Machine Learning model */
	IMAI_FED_init();

	printf("\r\n\n****************** IMAGIMOB Ready Model %s Code Example ****************** \r\n\n", LABELS[1]);

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(20));

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

        static int prediction_count = 0;

        switch (dequeue_result)
        {
            case IMAI_RET_SUCCESS:

                static int16_t success_flag = 1;
                prediction_count +=1;

                if (dataOut[1]==1)
                {
                    fall_detected = true;
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
                break;
            case IMAI_RET_NODATA:
                /* Sleep until more data is available */
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
