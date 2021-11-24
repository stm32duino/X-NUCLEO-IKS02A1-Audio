#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#if __has_include("app_config_custom.h")
  #include "app_config_custom.h"
#else
  #include "app_config_default.h"
#endif

#endif /* __APP_CONFIG_H__ */
