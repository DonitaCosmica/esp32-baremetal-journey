CHIP ?= esp32s3
PORT ?= /dev/ttyACM0
BUILD := build/$(CHIP)

ifeq ($(CHIP),esp32s3)
  CC := xtensa-esp32s3-elf-gcc
  FLASH_OFF := 0x0
  FLASH_SIZE := 8MB
else
  CC := xtensa-esp32-elf-gcc
  FLASH_OFF  := 0x1000
  FLASH_SIZE := 4MB
endif

CFLAGS := -Os -g -Wall -ffreestanding -mlongcalls -mtext-section-literals -DTARGET_$(CHIP)
LDFLAGS := -nostdlib -nostartfiles -Wl,-T,ld/$(CHIP).ld -Wl,-Map,$(BUILD)/blink.map -Wl,--gc-sections

all: $(BUILD)/blink.bin

$(BUILD)/blink.elf: start.S main.c ld/$(CHIP).ld
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) start.S main.c -o $@

$(BUILD)/blink.bin: $(BUILD)/blink.elf
	esptool.py --chip $(CHIP) elf2image -fm dio -ff 40m -fs $(FLASH_SIZE) -o $@ $<

flash: $(BUILD)/blink.bin
	esptool.py --chip $(CHIP) -p $(PORT) write_flash $(FLASH_OFF) $<

clean:
	rm -rf build