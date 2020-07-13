SAMPLE = sample

demo:
	make -C $(SAMPLE)

.PHONY: clean

clean:
	make clean -C $(SAMPLE)
