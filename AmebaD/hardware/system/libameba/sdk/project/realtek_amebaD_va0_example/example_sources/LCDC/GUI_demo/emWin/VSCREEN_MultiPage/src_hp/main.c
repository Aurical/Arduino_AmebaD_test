
#include "ameba_soc.h"
#include "main.h"
#include "GUI.h"
#include "WM.h"
#include "WIDGET.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"

#define INT_PIN    _PA_17

void gpio_touch_irq_handler (uint32_t id, gpio_irq_event event)
{
	GUI_TOUCH_Exec();
}


int main(void)
{
  #if GUI_SUPPORT_MEMDEV
	WM_SetCreateFlags(WM_CF_MEMDEV);
  #endif
	GUI_Init();

	/*touch interrupt pin*/
	gpio_irq_t gpio_btn;

	gpio_irq_init(&gpio_btn, INT_PIN, gpio_touch_irq_handler, (uint32_t)(&gpio_btn));
	gpio_irq_set(&gpio_btn, IRQ_FALL, 1);   
	gpio_irq_enable(&gpio_btn);
  
	if (xTaskCreate((TaskFunction_t )MainTask,(const char*)"MainTask", 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(MainTask) failed", __FUNCTION__);
	
	//DBG_8195A("M4U:%d \n", RTIM_GetCount(TIMM05));
	/* Enable Schedule, Start Kernel */
	vTaskStartScheduler();
}
