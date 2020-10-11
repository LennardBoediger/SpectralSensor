make: src/main.c
		gcc -Wall ./src/main.c ./lib/wiringPi_AS726X_Libary/AS726X.c lib/wiringPi_AS726X_Libary/AS726X.h lib/influxDB_http_Libary/influxdb.c lib/influxDB_http_Libary/influxdb.h src/measurement.c src/measurement.h src/welcome.c src/welcome.h -o AS726X -lwiringPi -pthread -lm
	./AS726X
clean:
	rm AS726X