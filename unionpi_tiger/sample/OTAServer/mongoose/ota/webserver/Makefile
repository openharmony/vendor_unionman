# Copyright (c) 2023 Unionman Technology Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -g -DMG_ENABLE_OPENSSL -D_WIN32 -DMG_ENABLE_WINSOCK -I.
LDFLAGS = -lwsock32 -L. -lsslMD -lcryptoMD 

all: clean webserver
	$(RUN) ./webserver $(ARGS)

webserver: main.o mongoose.o server_process.o cJSON.o
	$(CC) $(CFLAGS) main.o mongoose.o server_process.o cJSON.o $(LDFLAGS) -o webserver

main.o: main.c mongoose.h
	$(CC) $(CFLAGS) -c main.c

mongoose.o: mongoose.c mongoose.h
	$(CC) $(CFLAGS) -c mongoose.c
	
cJSON.o: cJSON.c cJSON.h
	$(CC) $(CFLAGS) -c cJSON.c

clean:
	del /Q *.o webserver
