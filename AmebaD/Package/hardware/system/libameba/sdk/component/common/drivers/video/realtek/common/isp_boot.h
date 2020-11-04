#ifndef ISP_BOOT_H
#define ISP_BOOT_H

#define ISP_FORMAT_YUV420_SEMIPLANAR 0x01
#define ISP_FORMAT_YUV422_SEMIPLANAR 0x02
#define ISP_FORMAT_BAYER_PATTERN     0x03

#define HW_SLOT_NUM 2
//#define MAX_FPS 30

#define ISP_TIMEOUT 2000
#define ISP_FAST_BOOT   (1)
#define ISP_NORMAL_BOOT (0)

#define ISP_INTERFACE_PARALLEL (0)
#define ISP_INTERFACE_MIPI     (1)

typedef struct isp_boot_stream_cfg{
	uint32_t pin_idx;
	uint32_t isp_id;
	uint32_t hw_slot_num;
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t format;
	uint32_t interface;
        uint32_t mode;
        uint32_t clk;
        uint32_t ldc;
        uint32_t sensor_fps;
}isp_boot_stream_t;

typedef struct isp_boot_cfg{
	isp_boot_stream_t isp_config;
	uint32_t isp_buffer[HW_SLOT_NUM];
	uint32_t boot_enable;
	uint32_t mcu_fw_addr;
}isp_boot_cfg_t;

#endif