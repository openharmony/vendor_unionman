# Copyright 2023 Unionman Technology Co., Ltd.
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
#!/bin/sh
chdir /data/log
notifier() {
	arg=$1
	if [ -z $arg ];then
		cem publish -e user.faultlog.dump -c 0 -d "succ"
	else
		cem publish -e user.faultlog.dump -c 1 -d $arg
	fi
}
while
do
	switch_on=$(param get persist.faultlog.on)
	#echo $switch_on
	if [ $switch_on = "true" ];then
		echo "switch on"
		path=$(param get persist.faultlog.path)
		echo "path=$path"
		if [ -d $path ];then
			timestamp=$(date +%Y-%m-%d-%H-%M-%S)
			tar -czf faultlog-$timestamp.gz faultlog
			if [ $? -ne 0 ];then
				notifier "tar"
			fi
			cp faultlog-$timestamp.gz $path
			if [ $? -ne 0 ];then
				notifier "copy"
			fi
			rm -f faultlog-$timestamp.gz
			param set persist.faultlog.on false
			notifier
		fi
	fi
	sleep 2
done
