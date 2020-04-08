/*
 * rn8302b.h
 *
 *  Created on: 2019骞�7鏈�31鏃�
 *      Author: alien
 */

#ifndef COMPONENTS_RN8302B_RN8302B_H_
#define COMPONENTS_RN8302B_RN8302B_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "driver/spi_master.h"
// #include "nvs_flash.h"
#include "nvs.h"
/* Macros --------------------------------------------------------------------*/
#define RN8302B_CALI_REPEAT_CNT    (10)   //鏍″噯閲嶅璇诲彇娆℃暟

#define RN8302B_VOLTAGE_CHANNEL_CNT       (3)
#define RN8302B_CURRENT_CHANNEL_CNT       (4)
#define RN8302B_TOTAL_CHANNEL_CNT         (RN8302B_VOLTAGE_CHANNEL_CNT + RN8302B_CURRENT_CHANNEL_CNT)

#define RN8302B_BASE_DATA_CNT            30

#define RN8302B_POLL_WITHOUT_ENERGY
#ifndef RN8302B_POLL_WITHOUT_ENERGY
#define RN8302B_MAX_ENERGY_DATA_CNT     28
#define RN8302B_MAX_DATA_CNT            (RN8302B_BASE_DATA_CNT + RN8302B_MAX_ENERGY_DATA_CNT)
#define RN8302B_ENERGY_STORGAE_ESP32_NVS
#else
#define RN8302B_MAX_DATA_CNT            (RN8302B_BASE_DATA_CNT)
#endif

/* Types ---------------------------------------------------------------------*/
typedef struct rn8302b_ratio_
{
    uint16_t V[3]; ///澶栫疆PT鍙橀�佸櫒鍙樻瘮
    uint16_t I[4]; ///澶栫疆CT鍙橀�佸櫒鍙樻瘮
} rn8302b_ratio_t;

typedef struct rn8302b_deadband_
{
    uint16_t V[3]; ///鐢靛帇娴嬮噺鍊兼鍖�=鏈�灏忔祴閲忓�煎�嶆暟锛�0.01V锛�
    uint16_t I[4]; ///鐢垫祦娴嬮噺鍊兼鍖�=鏈�灏忔祴閲忓�煎�嶆暟锛�0.001A锛�
} rn8302b_deadband_t;


typedef struct rn8302b_gain_
{
    int16_t  V[3]; /// 鑺墖鍐呴儴鐢靛帇澧炵泭瀵勫瓨鍣ㄥ弬鏁�
    int16_t  I[4]; /// 鑺墖鍐呴儴鐢垫祦澧炵泭瀵勫瓨鍣ㄥ弬鏁�
} rn8302b_gain_t;

typedef struct rn8302b_phase_
{
    int16_t  P[3]; /// 鑺墖鍐呴儴鏈夊姛鐩镐綅鏍″噯瀵勫瓨鍣ㄥ弬鏁�
    int16_t  Q[3]; /// 鑺墖鍐呴儴鏃犲姛鐩镐綅鏍″噯瀵勫瓨鍣ㄥ弬鏁�
} rn8302b_phase_t;

typedef struct rn8302b_offset_
{
    int16_t I[4]; /// 鐢垫祦閫氶亾offset鏍℃鍙傛暟
} rn8302b_offset_t;

#ifndef RN8302B_POLL_WITHOUT_ENERGY
struct energy_data
{
    uint32_t data[RN8302B_MAX_ENERGY_DATA_CNT];
};
#endif


#pragma pack(push, 1)
typedef struct rn8302b_dev_param_
{
    uint16_t e_const; //鐢佃〃甯告暟
    uint16_t rated_vol;//棰濆畾鐢垫祦
    uint16_t rated_cur;//棰濆畾鐢靛帇
    uint32_t std_vol;  // 棰濆畾5A鐢垫祦鏈夋晥鍊煎瘎瀛樺櫒鏍囧噯鍊�
    uint32_t std_cur;   // 棰濆畾220V鐢靛帇鏈夋晥鍊煎瘎瀛樺櫒鏍囧噯鍊�
} rn8302b_conf_param_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct rn8302b_calc_param_
{
    uint16_t rms_vol;// 0.01V 鐢靛帇瀵瑰簲鐨勬湁鏁堝�煎瘎瀛樺櫒鏍囧噯鍊�
    uint16_t rms_cur;//0.001A 鐢垫祦瀵瑰簲鐨勬湁鏁堝�煎瘎瀛樺櫒鏍囧噯鍊�
    float rms_power;
} rn8302b_calc_param_t;
#pragma pack(pop)


struct rn8302b_dev  
{       
    char *namespace;//namesapce for nvs
    // nvs_handle nvs;
    spi_device_handle_t spi;
    /* parameters,must be initialized first coz used in modbus*/
    void *table;//鍐呭瓨鍖�
    rn8302b_ratio_t *ratio;
    rn8302b_deadband_t * deadband;

    /* parameters get from nvs,no need to initialize*/
    // rn8302b_conf_param_t *conf;
    // rn8302b_gain_t *gain;
    // rn8302b_offset_t *offset;
    // rn8302b_phase_t *phase;
    /* parameters runtime ,no need to initialize*/
    rn8302b_calc_param_t calc;
#ifndef RN8302B_POLL_WITHOUT_ENERGY
    struct energy_data *energy;
#endif
}__packed;

/* Constants -----------------------------------------------------------------*/
extern const char* rn8302b_key_ratio;
extern const char* rn8302b_key_deadband;
extern const char* rn8302b_key_offset;
extern const char* rn8302b_key_gain;
extern const char* rn8302b_key_phase;
extern const char* rn8302b_key_conf;

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
esp_err_t rn8302b_write(struct rn8302b_dev *dev,uint16_t addr,uint32_t val, uint16_t len);
esp_err_t rn8302b_read(struct rn8302b_dev *dev,uint16_t addr,uint32_t *val, uint16_t len);
esp_err_t rn8302b_dev_init(struct rn8302b_dev *dev);
esp_err_t rn8302b_write_protect_dis(struct rn8302b_dev *dev);
esp_err_t rn8302b_write_protect_en(struct rn8302b_dev *dev);

// esp_err_t rn8302b_get_param_from_nvs(struct rn8302b_dev *dev, const char* key, void* value, size_t* length);

int rn8302b_calibrate_gain(struct rn8302b_dev *dev,uint8_t voltage,uint8_t current);
int rn8302b_calibrate_phase(struct rn8302b_dev *dev,uint8_t voltage,uint8_t current);
int rn8302b_calibrate_offset(struct rn8302b_dev *dev,uint8_t voltage);

void rn8302b_poll_metric(struct rn8302b_dev *dev);
#ifndef RN8302B_POLL_WITHOUT_ENERGY
void rn8302b_poll_energy(struct rn8302b_dev *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_RN8302B_RN8302B_H_ */
