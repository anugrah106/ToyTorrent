export CFLAGS := -g -Wall

all: root_client client tracker

root_client:
	$(MAKE) -C ./root_client all

client:
	$(MAKE) -C ./client all

tracker:
	$(MAKE) -C ./tracker all

report.pdf: report.md
	pandoc --from=markdown --output=$@ $< --latex-engine=xelatex -S --toc

clean:
	$(MAKE) -C ./tracker clean
	$(MAKE) -C ./client clean
	$(MAKE) -C ./root_client clean

.PHONY: root_client tracker client
