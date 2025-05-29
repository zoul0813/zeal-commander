BIN=zc.bin

ifndef ZVB_SDK_PATH
	$(error "Failure: ZVB_SDK_PATH variable not found.  It must point to Zeal Video Board SDK path.")
endif


include $(ZVB_SDK_PATH)/sdcc/base_sdcc.mk

all::
	@echo "Creating sample data in " $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)/tmp
	@mkdir -p $(OUTPUT_DIR)/test1
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom > $(OUTPUT_DIR)/test1/file1a.txt
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test1/file1b.txt
	@cp ls.bin $(OUTPUT_DIR)/test1/simple.bin

	@mkdir -p $(OUTPUT_DIR)/test2
	@cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin
	@head -c $$(( (RANDOM % 255) + 1 )) </dev/urandom >  $(OUTPUT_DIR)/test2/file2a.txt
	@echo "Hello World" > $(OUTPUT_DIR)/test2/hello.txt
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/alpha
	@cp -R $(OUTPUT_DIR)/test1 $(OUTPUT_DIR)/test2/beta
	@cp $(OUTPUT_DIR)/zc.bin $(OUTPUT_DIR)/test2/test.bin

	@mkdir -p $(OUTPUT_DIR)/large

	@touch $(OUTPUT_DIR)/large/001.txt
	@touch $(OUTPUT_DIR)/large/002.txt
	@touch $(OUTPUT_DIR)/large/003.txt
	@touch $(OUTPUT_DIR)/large/004.txt
	@touch $(OUTPUT_DIR)/large/005.txt
	@touch $(OUTPUT_DIR)/large/006.txt
	@touch $(OUTPUT_DIR)/large/007.txt
	@touch $(OUTPUT_DIR)/large/008.txt
	@touch $(OUTPUT_DIR)/large/009.txt
	@touch $(OUTPUT_DIR)/large/010.txt
	@touch $(OUTPUT_DIR)/large/011.txt
	@touch $(OUTPUT_DIR)/large/012.txt
	@touch $(OUTPUT_DIR)/large/013.txt
	@touch $(OUTPUT_DIR)/large/014.txt
	@touch $(OUTPUT_DIR)/large/015.txt
	@touch $(OUTPUT_DIR)/large/016.txt
	@touch $(OUTPUT_DIR)/large/017.txt
	@touch $(OUTPUT_DIR)/large/018.txt
	@touch $(OUTPUT_DIR)/large/019.txt
	@touch $(OUTPUT_DIR)/large/020.txt
	@touch $(OUTPUT_DIR)/large/021.txt
	@touch $(OUTPUT_DIR)/large/022.txt
	@touch $(OUTPUT_DIR)/large/023.txt
	@touch $(OUTPUT_DIR)/large/024.txt
	@touch $(OUTPUT_DIR)/large/025.txt
	@touch $(OUTPUT_DIR)/large/026.txt
	@touch $(OUTPUT_DIR)/large/027.txt
	@touch $(OUTPUT_DIR)/large/028.txt
	@touch $(OUTPUT_DIR)/large/029.txt
	@touch $(OUTPUT_DIR)/large/030.txt
	@touch $(OUTPUT_DIR)/large/031.txt
	@touch $(OUTPUT_DIR)/large/032.txt
	@touch $(OUTPUT_DIR)/large/033.txt
	@touch $(OUTPUT_DIR)/large/034.txt
	@touch $(OUTPUT_DIR)/large/035.txt
	@touch $(OUTPUT_DIR)/large/036.txt
	@touch $(OUTPUT_DIR)/large/037.txt
	@touch $(OUTPUT_DIR)/large/038.txt
	@touch $(OUTPUT_DIR)/large/039.txt
	@touch $(OUTPUT_DIR)/large/040.txt
	@touch $(OUTPUT_DIR)/large/041.txt
	@touch $(OUTPUT_DIR)/large/042.txt
	@touch $(OUTPUT_DIR)/large/043.txt
	@touch $(OUTPUT_DIR)/large/044.txt
	@touch $(OUTPUT_DIR)/large/045.txt
	@touch $(OUTPUT_DIR)/large/046.txt
	@touch $(OUTPUT_DIR)/large/047.txt
	@touch $(OUTPUT_DIR)/large/048.txt
	@touch $(OUTPUT_DIR)/large/049.txt
	@touch $(OUTPUT_DIR)/large/050.txt
	@touch $(OUTPUT_DIR)/large/051.txt
	@touch $(OUTPUT_DIR)/large/052.txt
	@touch $(OUTPUT_DIR)/large/053.txt
	@touch $(OUTPUT_DIR)/large/054.txt
	@touch $(OUTPUT_DIR)/large/055.txt
	@touch $(OUTPUT_DIR)/large/056.txt
	@touch $(OUTPUT_DIR)/large/057.txt
	@touch $(OUTPUT_DIR)/large/058.txt
	@touch $(OUTPUT_DIR)/large/059.txt
	@touch $(OUTPUT_DIR)/large/060.txt
	@touch $(OUTPUT_DIR)/large/061.txt
	@touch $(OUTPUT_DIR)/large/062.txt
	@touch $(OUTPUT_DIR)/large/063.txt
	@touch $(OUTPUT_DIR)/large/064.txt
	@touch $(OUTPUT_DIR)/large/065.txt
	@touch $(OUTPUT_DIR)/large/066.txt
	@touch $(OUTPUT_DIR)/large/067.txt
	@touch $(OUTPUT_DIR)/large/068.txt
	@touch $(OUTPUT_DIR)/large/069.txt
	@touch $(OUTPUT_DIR)/large/070.txt
	@touch $(OUTPUT_DIR)/large/071.txt
	@touch $(OUTPUT_DIR)/large/072.txt
	@touch $(OUTPUT_DIR)/large/073.txt
	@touch $(OUTPUT_DIR)/large/074.txt
	@touch $(OUTPUT_DIR)/large/075.txt
	@touch $(OUTPUT_DIR)/large/076.txt
	@touch $(OUTPUT_DIR)/large/077.txt
	@touch $(OUTPUT_DIR)/large/078.txt
	@touch $(OUTPUT_DIR)/large/079.txt
	@touch $(OUTPUT_DIR)/large/080.txt
	@touch $(OUTPUT_DIR)/large/081.txt
	@touch $(OUTPUT_DIR)/large/082.txt
	@touch $(OUTPUT_DIR)/large/083.txt
	@touch $(OUTPUT_DIR)/large/084.txt
	@touch $(OUTPUT_DIR)/large/085.txt
	@touch $(OUTPUT_DIR)/large/086.txt
	@touch $(OUTPUT_DIR)/large/087.txt
	@touch $(OUTPUT_DIR)/large/088.txt
	@touch $(OUTPUT_DIR)/large/089.txt
	@touch $(OUTPUT_DIR)/large/090.txt
	@touch $(OUTPUT_DIR)/large/091.txt
	@touch $(OUTPUT_DIR)/large/092.txt
	@touch $(OUTPUT_DIR)/large/093.txt
	@touch $(OUTPUT_DIR)/large/094.txt
	@touch $(OUTPUT_DIR)/large/095.txt
	@touch $(OUTPUT_DIR)/large/096.txt
	@touch $(OUTPUT_DIR)/large/097.txt
	@touch $(OUTPUT_DIR)/large/098.txt
	@touch $(OUTPUT_DIR)/large/099.txt


	@touch $(OUTPUT_DIR)/large/100.txt
	@touch $(OUTPUT_DIR)/large/101.txt
	@touch $(OUTPUT_DIR)/large/102.txt
	@touch $(OUTPUT_DIR)/large/103.txt
	@touch $(OUTPUT_DIR)/large/104.txt
	@touch $(OUTPUT_DIR)/large/105.txt
	@touch $(OUTPUT_DIR)/large/106.txt
	@touch $(OUTPUT_DIR)/large/107.txt
	@touch $(OUTPUT_DIR)/large/108.txt
	@touch $(OUTPUT_DIR)/large/109.txt
	@touch $(OUTPUT_DIR)/large/110.txt
	@touch $(OUTPUT_DIR)/large/111.txt
	@touch $(OUTPUT_DIR)/large/112.txt
	@touch $(OUTPUT_DIR)/large/113.txt
	@touch $(OUTPUT_DIR)/large/114.txt
	@touch $(OUTPUT_DIR)/large/115.txt
	@touch $(OUTPUT_DIR)/large/116.txt
	@touch $(OUTPUT_DIR)/large/117.txt
	@touch $(OUTPUT_DIR)/large/118.txt
	@touch $(OUTPUT_DIR)/large/119.txt
	@touch $(OUTPUT_DIR)/large/120.txt
	@touch $(OUTPUT_DIR)/large/121.txt
	@touch $(OUTPUT_DIR)/large/122.txt
	@touch $(OUTPUT_DIR)/large/123.txt
	@touch $(OUTPUT_DIR)/large/124.txt
	@touch $(OUTPUT_DIR)/large/125.txt
	@touch $(OUTPUT_DIR)/large/126.txt
	@touch $(OUTPUT_DIR)/large/127.txt
	@touch $(OUTPUT_DIR)/large/128.txt
	@touch $(OUTPUT_DIR)/large/129.txt
	@touch $(OUTPUT_DIR)/large/130.txt
	@touch $(OUTPUT_DIR)/large/131.txt
	@touch $(OUTPUT_DIR)/large/132.txt
	@touch $(OUTPUT_DIR)/large/133.txt
	@touch $(OUTPUT_DIR)/large/134.txt
	@touch $(OUTPUT_DIR)/large/135.txt
	@touch $(OUTPUT_DIR)/large/136.txt
	@touch $(OUTPUT_DIR)/large/137.txt
	@touch $(OUTPUT_DIR)/large/138.txt
	@touch $(OUTPUT_DIR)/large/139.txt
	@touch $(OUTPUT_DIR)/large/140.txt
	@touch $(OUTPUT_DIR)/large/141.txt
	@touch $(OUTPUT_DIR)/large/142.txt
	@touch $(OUTPUT_DIR)/large/143.txt
	@touch $(OUTPUT_DIR)/large/144.txt
	@touch $(OUTPUT_DIR)/large/145.txt
	@touch $(OUTPUT_DIR)/large/146.txt
	@touch $(OUTPUT_DIR)/large/147.txt
	@touch $(OUTPUT_DIR)/large/148.txt
	@touch $(OUTPUT_DIR)/large/149.txt
	@touch $(OUTPUT_DIR)/large/150.txt
	@touch $(OUTPUT_DIR)/large/151.txt
	@touch $(OUTPUT_DIR)/large/152.txt
	@touch $(OUTPUT_DIR)/large/153.txt
	@touch $(OUTPUT_DIR)/large/154.txt
	@touch $(OUTPUT_DIR)/large/155.txt
	@touch $(OUTPUT_DIR)/large/156.txt
	@touch $(OUTPUT_DIR)/large/157.txt
	@touch $(OUTPUT_DIR)/large/158.txt
	@touch $(OUTPUT_DIR)/large/159.txt
	@touch $(OUTPUT_DIR)/large/160.txt
	@touch $(OUTPUT_DIR)/large/161.txt
	@touch $(OUTPUT_DIR)/large/162.txt
	@touch $(OUTPUT_DIR)/large/163.txt
	@touch $(OUTPUT_DIR)/large/164.txt
	@touch $(OUTPUT_DIR)/large/165.txt
	@touch $(OUTPUT_DIR)/large/166.txt
	@touch $(OUTPUT_DIR)/large/167.txt
	@touch $(OUTPUT_DIR)/large/168.txt
	@touch $(OUTPUT_DIR)/large/169.txt
	@touch $(OUTPUT_DIR)/large/170.txt
	@touch $(OUTPUT_DIR)/large/171.txt
	@touch $(OUTPUT_DIR)/large/172.txt
	@touch $(OUTPUT_DIR)/large/173.txt
	@touch $(OUTPUT_DIR)/large/174.txt
	@touch $(OUTPUT_DIR)/large/175.txt
	@touch $(OUTPUT_DIR)/large/176.txt
	@touch $(OUTPUT_DIR)/large/177.txt
	@touch $(OUTPUT_DIR)/large/178.txt
	@touch $(OUTPUT_DIR)/large/179.txt
	@touch $(OUTPUT_DIR)/large/180.txt
	@touch $(OUTPUT_DIR)/large/181.txt
	@touch $(OUTPUT_DIR)/large/182.txt
	@touch $(OUTPUT_DIR)/large/183.txt
	@touch $(OUTPUT_DIR)/large/184.txt
	@touch $(OUTPUT_DIR)/large/185.txt
	@touch $(OUTPUT_DIR)/large/186.txt
	@touch $(OUTPUT_DIR)/large/187.txt
	@touch $(OUTPUT_DIR)/large/188.txt
	@touch $(OUTPUT_DIR)/large/189.txt
	@touch $(OUTPUT_DIR)/large/190.txt
	@touch $(OUTPUT_DIR)/large/191.txt
	@touch $(OUTPUT_DIR)/large/192.txt
	@touch $(OUTPUT_DIR)/large/193.txt
	@touch $(OUTPUT_DIR)/large/194.txt
	@touch $(OUTPUT_DIR)/large/195.txt
	@touch $(OUTPUT_DIR)/large/196.txt
	@touch $(OUTPUT_DIR)/large/197.txt
	@touch $(OUTPUT_DIR)/large/198.txt
	@touch $(OUTPUT_DIR)/large/199.txt

	@touch $(OUTPUT_DIR)/large/200.txt
	@touch $(OUTPUT_DIR)/large/201.txt
	@touch $(OUTPUT_DIR)/large/202.txt
	@touch $(OUTPUT_DIR)/large/203.txt
	@touch $(OUTPUT_DIR)/large/204.txt
	@touch $(OUTPUT_DIR)/large/205.txt
	@touch $(OUTPUT_DIR)/large/206.txt
	@touch $(OUTPUT_DIR)/large/207.txt
	@touch $(OUTPUT_DIR)/large/208.txt
	@touch $(OUTPUT_DIR)/large/209.txt
	@touch $(OUTPUT_DIR)/large/210.txt
	@touch $(OUTPUT_DIR)/large/211.txt
	@touch $(OUTPUT_DIR)/large/212.txt
	@touch $(OUTPUT_DIR)/large/213.txt
	@touch $(OUTPUT_DIR)/large/214.txt
	@touch $(OUTPUT_DIR)/large/215.txt
	@touch $(OUTPUT_DIR)/large/216.txt
	@touch $(OUTPUT_DIR)/large/217.txt
	@touch $(OUTPUT_DIR)/large/218.txt
	@touch $(OUTPUT_DIR)/large/219.txt
	@touch $(OUTPUT_DIR)/large/220.txt
	@touch $(OUTPUT_DIR)/large/221.txt
	@touch $(OUTPUT_DIR)/large/222.txt
	@touch $(OUTPUT_DIR)/large/223.txt
	@touch $(OUTPUT_DIR)/large/224.txt
	@touch $(OUTPUT_DIR)/large/225.txt
	@touch $(OUTPUT_DIR)/large/226.txt
	@touch $(OUTPUT_DIR)/large/227.txt
	@touch $(OUTPUT_DIR)/large/228.txt
	@touch $(OUTPUT_DIR)/large/229.txt
	@touch $(OUTPUT_DIR)/large/230.txt
	@touch $(OUTPUT_DIR)/large/231.txt
	@touch $(OUTPUT_DIR)/large/232.txt
	@touch $(OUTPUT_DIR)/large/233.txt
	@touch $(OUTPUT_DIR)/large/234.txt
	@touch $(OUTPUT_DIR)/large/235.txt
	@touch $(OUTPUT_DIR)/large/236.txt
	@touch $(OUTPUT_DIR)/large/237.txt
	@touch $(OUTPUT_DIR)/large/238.txt
	@touch $(OUTPUT_DIR)/large/239.txt
	@touch $(OUTPUT_DIR)/large/240.txt
	@touch $(OUTPUT_DIR)/large/241.txt
	@touch $(OUTPUT_DIR)/large/242.txt
	@touch $(OUTPUT_DIR)/large/243.txt
	@touch $(OUTPUT_DIR)/large/244.txt
	@touch $(OUTPUT_DIR)/large/245.txt
	@touch $(OUTPUT_DIR)/large/246.txt
	@touch $(OUTPUT_DIR)/large/247.txt
	@touch $(OUTPUT_DIR)/large/248.txt
	@touch $(OUTPUT_DIR)/large/249.txt
	@touch $(OUTPUT_DIR)/large/250.txt
	@touch $(OUTPUT_DIR)/large/251.txt
	@touch $(OUTPUT_DIR)/large/252.txt
	@touch $(OUTPUT_DIR)/large/253.txt
	@touch $(OUTPUT_DIR)/large/254.txt
	@touch $(OUTPUT_DIR)/large/255.txt



run:
	$(ZEAL_NATIVE_BIN) -H bin -r $(ZEAL_NATIVE_ROM) -t tf.img -e eeprom.img

native: all run

