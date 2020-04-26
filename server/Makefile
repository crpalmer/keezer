SCRIPTS = lxde-autostart.sh run-ui.sh refresh-chromium.sh \
	  temp-controller-configs/ temp-controller.cfg

install:
	sudo mkdir -p /opt/keezer
	for i in $(SCRIPTS); \
	do \
	    sudo cp -r $$i /opt/keezer; \
	    sudo chmod 755 /opt/keezer/$$i; \
	done
