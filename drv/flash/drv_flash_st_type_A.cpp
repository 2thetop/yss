////////////////////////////////////////////////////////////////////////////////////////
//
// 저작권 표기 License_ver_2.0
// 본 소스코드의 소유권은 yss Embedded Operating System 네이버 카페 관리자와 운영진에게 있습니다.
// 운영진이 임의로 코드의 권한을 타인에게 양도할 수 없습니다.
// 본 소스코드는 아래 사항에 동의할 경우에 사용 가능합니다.
// 아래 사항에 대해 동의하지 않거나 이해하지 못했을 경우 사용을 금합니다.
// 본 소스코드를 사용하였다면 아래 사항을 모두 동의하는 것으로 자동 간주 합니다.
// 본 소스코드의 상업적 또는 비상업적 이용이 가능합니다.
// 본 소스코드의 내용을 임의로 수정하여 재배포하는 행위를 금합니다.
// 본 소스코드의 내용을 무단 전재하는 행위를 금합니다.
// 본 소스코드의 사용으로 인해 발생하는 모든 사고에 대해서 어떤한 법적 책임을 지지 않습니다.
//
//	Home Page : http://cafe.naver.com/yssoperatingsystem
//	Copyright 2020.	yss Embedded Operating System all right reserved.
//  
//  주담당자 : 아이구 (mymy49@nate.com) 2016.04.30 ~ 현재
//  부담당자 : -
//
////////////////////////////////////////////////////////////////////////////////////////

#if	defined(STM32F746xx) ||	defined(STM32F745xx) ||	\
	defined(STM32F765xx) ||	defined(STM32F767xx) ||	defined(STM32F768xx) ||	defined(STM32F769xx)

#include <drv/peripherals.h>
#include <drv/flash/drv_st_flash_type_A_register.h>

#if defined(FLASH)
drv::Flash flash(0, 0);
#endif

namespace drv
{
	Flash::Flash(void (*clockFunc)(bool en), void (*nvicFunc)(bool en)) :  Drv(clockFunc, nvicFunc)
	{

	}

	void Flash::setLatency(unsigned long freq, unsigned char vcc)
	{
		unsigned long div, wait;

		if(vcc > 27)
		{
			div = 30;
		}
		else if(vcc	> 24)
		{
			div = 24;
		}
		else if(vcc	> 21)
		{
			div = 22;
		}
		else
		{
			div = 20;
		}

		freq /= 1000000;
		wait = freq / div;
		if(!(freq % div))
			wait--;

		setFlashLatency(wait);
	}

	void Flash::setPrefetchEn(bool en)
	{
		if(en)
			FLASH->ACR |= FLASH_ACR_PRFTEN_Msk;
		else
			FLASH->ACR &= ~FLASH_ACR_PRFTEN_Msk;
	}

    void Flash::setArtEn(bool en)
    {
		if(en)
			FLASH->ACR |= FLASH_ACR_ARTEN_Msk;
		else
			FLASH->ACR &= ~FLASH_ACR_ARTEN_Msk;
    }

	void Flash::eraseSector(unsigned char sector)
	{
		while(getFlashBusy())
			thread::switchContext();

		if(getFlashLock())
		{
			setFlashKey(0x45670123);
			setFlashKey(0xcdef89ab);
		}

		while(getFlashLock())
			thread::switchContext();

		setFlashSectorErase(true);
		setFlashSectorNumber(sector);
		setFlashEraseStart();

		thread::delay(10);
		while(getFlashBusy());

		thread::delay(10);
		setFlashSectorErase(false);
		setFlashLock();
	}

	void Flash::program(unsigned long *des, unsigned long *src, unsigned long size)
	{
		size += 3;
		size >>= 2;

		while(getFlashBusy())
			thread::switchContext();

		if(getFlashLock())
		{
			setFlashKey(0x45670123);
			setFlashKey(0xcdef89ab);
		}

		while(getFlashLock())
			thread::switchContext();

		setFlashProgramSize(2);
		setFlashProgramming(true);

		thread::delay(10);
		for(unsigned long i=0;i<size;i++)
		{
			des[i] = src[i];
			thread::switchContext();
			while(getFlashBusy());
		}

		setFlashProgramming(false);
		setFlashLock();
	}
}
#endif
