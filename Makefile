
MOD	:= i2c.pd_linux
SRC	:= i2c.c

$(MOD): $(SRC)
	 gcc -Wall -fPIC -shared $? -o $@

clean:
	rm -f $(MOD)


