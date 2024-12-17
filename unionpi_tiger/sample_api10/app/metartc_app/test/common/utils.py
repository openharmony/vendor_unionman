import platform
import signal
import subprocess
import logging
from datetime import datetime
import os
COMMAND_TIME_OUT = 30

def get_decode(stream):
    if isinstance(stream,str):
        return stream
    if not isinstance(stream,bytes):
        return str(stream)
    try:
        ret = stream.decode("utf-8",errors="ignore")
    except (ValueError,AttributeError,TypeError):
        ret = str(stream)
    return ret

def exec_cmd(cmd,timeout=COMMAND_TIME_OUT,error_print=True,join_result=False,wait_out=False):
    sys_type = platform.system()
    if sys_type == "Linux" or sys_type == "Darwin":
        proc = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,
                                shell=False,preexec_fn=os.setsid)
    else:
        proc = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    if wait_out:
        try:
            (out,err) = proc.communicate(timeout=timeout)
            err = get_decode(err).strip()
            out = get_decode(out).strip()
            if err and error_print:
                logging.exception(err,exc_info=False)
            if "Not support std mode" in out and not out.strip().endwith("std mode"):
                out = out[out.index("\n")+1:len(out)]
            if join_result:
                return "exec_cmd result = %s\n %s" % (out,err) if err else out
            else:
                return err if err else out
        except(TimeoutError,KeyboardInterrupt,AttributeError,ValueError,subprocess.TimeoutExpired,
               EOFError,IOError) as e:
            logging.error("exec_cmd exception = %s" % e)
            sys_type = platform.system()
            if sys_type == "Linux" or sys_type == "Darwin":
                os.killpg(proc.pid,signal.SIGTERM)
            else:
                os.kill(proc.pid,signal.SIGINT)

def creat_dir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def synch_time(hdc_shell):
    date_cmd = "%s date \'%s\'" % (hdc_shell,datetime.now())
    process = subprocess.run(date_cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=30)
    logging.info("result = "+ process.stdout.decode().strip())

def set_env(hdc_shell):
    logging.info("init test environment")
    synch_time(hdc_shell)
    log_cmd_array = [
        "%s rm -rf /data/log/faultlog/*.tmp" % hdc_shell,
        "%s rm -rf /data/log/faultlog/temp/*" % hdc_shell,
        "%s rm -rf /data/log/faultlog/faultlogger/*" % hdc_shell,
        "%s rm -rf /data/log/faultlog/debug/*" % hdc_shell,
        '%s "power-shell wakeup"' % hdc_shell,
        '%s "uinput -T -m 594 2117 594 864 400"' % hdc_shell,
        "%s hilog -w stop" % hdc_shell,
        "%s rm -rf /data/log/hilog/*" % hdc_shell,
        '%s hilog -w start -t kmsg -f "kmsg" -n 10' % hdc_shell,
        '%s hilog -w start -f "hilog" -n 20' % hdc_shell,
        "%s hilog -b D" % hdc_shell,
        "%s hilog -Q pidoff" % hdc_shell,
        '%s "param set persist.ark.properties 0x3c"' % hdc_shell,
        '%s "power-shell setmode 602"' % hdc_shell,
        '%s "echo 300 > /sys/class/leds/lcd_backlight0/brightness"' % hdc_shell,
    ]
    for command in log_cmd_array:
        exec_cmd(command,wait_out=True)





    