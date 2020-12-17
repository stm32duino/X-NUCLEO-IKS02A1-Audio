# X-NUCLEO-IKS02A1 Audio
Arduino library to support the [IMP34DT05](https://www.st.com/en/mems-and-sensors/imp34dt05.html) digital microphone.

## Setting SAI & I2S
Up to [STM32 core](https://github.com/stm32duino/Arduino_Core_STM32) version 2.0.0 it is necessary to apply the following steps:

Check where the Arduino `preferences.txt` file are in `Arduino IDE -> Preferences`.

	For Example: `C:\Users\Duino\AppData\Local\Arduino15` (Windows)

Then go to `..\packages\STM32\hardware\stm32\<release version>\cores\arduino\stm32\stm32yyxx_hal_conf.h` and open the file.

In the file add these lines of code:

```C
	#if !defined(HAL_I2S_MODULE_DISABLED)
  		#define HAL_I2S_MODULE_ENABLED
	#else
	  #undef HAL_I2S_MODULE_ENABLED
	#endif

	#if !defined(HAL_SAI_MODULE_DISABLED)
	  #define HAL_SAI_MODULE_ENABLED
	#else
	  #undef HAL_SAI_MODULE_ENABLED
	#endif
```

## API
This library acquires PDM, converts PDM to PCM.

### PDM
Before acquire PDM it is required to initialized it:

`   PDM.begin();`

Thanks  to `PDM` instance, it is possible to start to acquire the data, stop, pause the acquisition and resume.
- Start to acquire the PDM data:
    
`   PDM.Record(buffer);`
- Stop to acquire the PDM data:
    
`   PDM.Stop();`
- Pause the acquisition:
    
`   PDM.Pause();`
- Resume the acquisition.
    
`       PDM.Resume();`
- To perform a function to process the data (foo in the example), it can call the function below:
    
`       PDM.onReceive(foo);`

### PDM2PCM

Before encoding the data from PDM to PCM, it must be initialized following the procedure below:

`   pdm2pcm_init();`
  
  Set the volume of the microphone.

```C++
    pdm2pcm_volume(volume);
    
    pdm2pcm(BufIn, BufOut, BLOCK_SIZE);
```


# Documentation

You can find the source files at
https://github.com/stm32duino/X-NUCLEO-IKS02A1-Audio

The X-NUCLEO-IKS02A1 datasheet is available at
https://www.st.com/content/st_com/en/products/mems-and-sensors/mems-microphones/imp34dt05.html

