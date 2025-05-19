BIN=zc.bin

EMU_NATIVE_BIN=../../zeal8bit/Zeal-NativeEmulator/builddir/zeal.elf
EMU_NATIVE_ROM=../../zeal8bit/Zeal-NativeEmulator/roms/default.img

ifndef ZVB_SDK_PATH
	$(error "Failure: ZVB_SDK_PATH variable not found.  It must point to Zeal Video Board SDK path.")
endif


include $(ZVB_SDK_PATH)/sdcc/base_sdcc.mk

all::
	mkdir $(OUTPUT_DIR)/tmp
	mkdir $(OUTPUT_DIR)/test1
	head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom > $(OUTPUT_DIR)/test1/file1a.txt
	head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test1/file1b.txt
	cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test1/simple.bin

	mkdir $(OUTPUT_DIR)/test2
	cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin
	head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test2/file2a.txt
	echo "Hello World" > $(OUTPUT_DIR)/test2/hello.txt
	cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2
	cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/alpha
	cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/beta
	cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin

run:
	$(EMU_NATIVE_BIN) -H bin -r $(EMU_NATIVE_ROM) -t tf.img -e eeprom.img

native: all run

