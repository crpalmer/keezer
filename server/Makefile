SCRIPTS = lxde-autostart.sh run-ui.sh refresh-chromium.sh
install:
	sudo cp owfs.conf /etc/
	sudo mkdir -p /opt/keezer
	for i in $(SCRIPTS); do sudo cp $$i /opt/keezer; sudo chmod 755 /opt/keezer/$$i; done
