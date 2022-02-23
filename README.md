# X-NUCLEO-IKS02A1 Audio
Arduino library to support the [IMP34DT05](https://www.st.com/en/mems-and-sensors/imp34dt05.html) digital microphone.
Currently, this library works only with the X-NUCLEO-IKS02A1 and a NUCLEO-F401RE or a NUCLEO-L476RG.
It requires a [STM32 Core](https://github.com/stm32duino/Arduino_Core_STM32) equal to or greater than version 2.0.0.

## API
This library acquires PDM, converts PDM to PCM.

### PDM
Before acquire PDM it is required to initialize it:

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
