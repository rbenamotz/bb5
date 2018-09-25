#include <Arduino.h>
#include "user_config.h"

void setupCommon();
void write_to_log(String line, ...);
String read_log_buffer();
