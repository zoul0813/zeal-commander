BIN=zc.bin

ifndef ZVB_SDK_PATH
	$(error "Failure: ZVB_SDK_PATH variable not found.  It must point to Zeal Video Board SDK path.")
endif


include $(ZVB_SDK_PATH)/sdcc/base_sdcc.mk

all::
	@echo "Creating sample data in " $(OUTPUT_DIR)
	@mkdir $(OUTPUT_DIR)/tmp
	@mkdir $(OUTPUT_DIR)/test1
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom > $(OUTPUT_DIR)/test1/file1a.txt
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test1/file1b.txt
	@cp ls.bin $(OUTPUT_DIR)/test1/simple.bin

	@mkdir $(OUTPUT_DIR)/test2
	@cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test2/file2a.txt
	@echo "Hello World" > $(OUTPUT_DIR)/test2/hello.txt
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/alpha
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/beta
	@cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin

	@touch $(OUTPUT_DIR)/001.txt
	@touch $(OUTPUT_DIR)/002.txt
	@touch $(OUTPUT_DIR)/003.txt
	@touch $(OUTPUT_DIR)/004.txt
	@touch $(OUTPUT_DIR)/005.txt
	@touch $(OUTPUT_DIR)/006.txt
	@touch $(OUTPUT_DIR)/007.txt
	@touch $(OUTPUT_DIR)/008.txt
	@touch $(OUTPUT_DIR)/009.txt
	@touch $(OUTPUT_DIR)/010.txt
	@touch $(OUTPUT_DIR)/011.txt
	@touch $(OUTPUT_DIR)/012.txt
	@touch $(OUTPUT_DIR)/013.txt
	@touch $(OUTPUT_DIR)/014.txt
	@touch $(OUTPUT_DIR)/015.txt
	@touch $(OUTPUT_DIR)/016.txt
	@touch $(OUTPUT_DIR)/017.txt
	@touch $(OUTPUT_DIR)/018.txt
	@touch $(OUTPUT_DIR)/019.txt
	@touch $(OUTPUT_DIR)/020.txt
	@touch $(OUTPUT_DIR)/021.txt
	@touch $(OUTPUT_DIR)/022.txt
	@touch $(OUTPUT_DIR)/023.txt
	@touch $(OUTPUT_DIR)/024.txt
	@touch $(OUTPUT_DIR)/025.txt
	@touch $(OUTPUT_DIR)/026.txt
	@touch $(OUTPUT_DIR)/027.txt
	@touch $(OUTPUT_DIR)/028.txt
	@touch $(OUTPUT_DIR)/029.txt
	@touch $(OUTPUT_DIR)/030.txt
	@touch $(OUTPUT_DIR)/031.txt
	@touch $(OUTPUT_DIR)/032.txt
	@touch $(OUTPUT_DIR)/033.txt
	@touch $(OUTPUT_DIR)/034.txt
	@touch $(OUTPUT_DIR)/035.txt
	@touch $(OUTPUT_DIR)/036.txt
	@touch $(OUTPUT_DIR)/037.txt
	@touch $(OUTPUT_DIR)/038.txt
	@touch $(OUTPUT_DIR)/039.txt
	@touch $(OUTPUT_DIR)/040.txt

run:
	$(ZEAL_NATIVE_BIN) -H bin -r $(ZEAL_NATIVE_ROM) -t tf.img -e eeprom.img

native: all run

