/*
 * rn8302.h
 *
 *  Created on: 2020Äê3ÔÂ26ÈÕ
 *      Author: Administrator
 */

#ifndef _RN8302_H_
#define _RN8302_H_

void Rn8302Read(uint16_t temp,uint32_t* val,uint8_t num);
void Rn8302Write(uint16_t wReg, uint8_t *pBuf, uint8_t DatLen);


#endif /* RN8302_RN8302_H_ */
