import prompt from '@ohos.prompt';
import gpio_hdf from '@ohos.gpio_hdf';

export default {
    data: {
        pin: 380,                       //pinNum;[380,389]
        status: [0,0,0,0,0,0,0,0,0,0],  //灯号状态
        delay: 100,                     //间隔
        time: 10,                       //次数
        full: false,                    //是否全选
        blank: " ",                     //空格
        timerID1: null,                 //setTimeout返回值
        timerID2: null,                 //setTimeout返回值
        type: "无",                      //效果运行类型
        temp: 0,                        //临时变量
        isinf: true,                    //是否无穷次数
    },

    //初始化
    onInit() {
        //进入程序灭掉全部灯，保证与初始状态全为0相同
        for(var i = 380; i <= 389; i++){
            this.pin = i
            this.removeLedEffect()//因IDE不能调用接口，使previewer试图变空白，注释掉即可正常预览试图
        }

    },

    //改变gpio口
    changeGpio(pinNum) {
        this.full = false
        this.pin = pinNum + 380
        prompt.showToast({
            message: "选择" + this.pin
        })
    },
    //改变间隔时间
    changeDelay(delay) {
        this.delay = delay
        prompt.showToast({
            message: "选择" + this.delay + "ms"
        })
    },
    //改变次数
    changeTime(time) {
        this.isinf = false
        this.time = time
        prompt.showToast({
            message: "选择" + this.time + "次"
        })
    },
    //改变是否全选
    changeFull() {
        this.full = true
        prompt.showToast({
            message: "全选"
        })
    },
    //改变次数是否无穷
    changeIsinf() {
        this.isinf = true
        prompt.showToast({
            message: "选择∞"
        })
    },


    //开灯
    addLedEffect() {
        //若全选执行全开灯
        if(this.full){
            this.fullAddLedEffect()
            this.full = true
        }
        //单个pin开灯,使用napi接口
        gpio_hdf.setLedStatus(this.pin, gpio_hdf.LED_ON).then((result) => {
                if (result === 0) {
                console.info("Turned on LED." + result)
                    this.status.splice(this.pin - 380,1,1)  //更新状态
                    prompt.showToast({
                        message: "pin."+this.pin+"开灯"
                    })
                } else {
                    console.info("Turned on LED failed." + result)
                    prompt.showToast({
                        message: "pin."+this.pin+"开灯失败" + result
                    })
                }
        })

    },

    //关灯
    removeLedEffect() {
        //若全选执行全关灯
        if(this.full){
            this.fullRemoveLedEffect()
            this.full = true
        }
        //单个pin关灯,使用napi接口
        gpio_hdf.setLedStatus(this.pin, gpio_hdf.LED_OFF).then((result) => {
                if (result === 0) {
                    console.info("Turned off LED." + result)
                    this.status.splice(this.pin - 380,1,0)  //更新状态
                    prompt.showToast({
                        message: "pin."+this.pin+"关灯"
                    })
                } else {
                    console.info("Turned off LED failed." + result)
                    prompt.showToast({
                        message: "pin."+this.pin+"关灯失败" + result
                    })
                }
            })
    },
    fullAddLedEffect() {
        for(var i = 0; i <= 9; i++){
            this.changeGpio(i)
            this.addLedEffect()
            this.status.splice(i,1,1)   //更新状态；若删除则只有i = 9更新
        }
    },
    fullRemoveLedEffect() {
        for(var i = 0; i <= 9; i++){
            this.changeGpio(i)
            this.removeLedEffect()
            this.status.splice(i,1,0)   //更新状态；若删除则只有i = 9更新
        }
    },


    //中断
    shutdown(){
        this.type = "无"
        clearInterval(this.timerID1)
        clearInterval(this.timerID2)
        prompt.showToast({
            message: "关闭程序"
        })
    },

    //效果
    sparkle(){
        //清除setTimeout
        clearInterval(this.timerID1)
        clearInterval(this.timerID2)
        /*!this.isinf表示选择的是次数有限
        当1.次数小于0;2.次数有限;不执行函数*/
        if(this.time <= 0 && !this.isinf){
            this.type = "无"
            prompt.showToast({
                message: "次数为0"
            })
            return
        }
        //若不是无穷次数，次数减少1
        if(!this.isinf){
            this.time--
        }
        //type更新
        this.type = "闪烁"
        //执行开灯
        this.addLedEffect()
        //delay时间后执行关灯
        this.timerID1 = setTimeout(() => {
            this.removeLedEffect()
        },this.delay)
        //2 * delay时间后执行递归
        this.timerID2 = setTimeout(() => {
            this.sparkle()
        },this.delay * 2)
    },

    flow() {
        //清除setTimeout
        clearInterval(this.timerID1)
        clearInterval(this.timerID2)
        /*!this.isinf表示选择的是次数有限
        当1.次数小于0;2.次数有限;不执行函数*/
        if(this.time <= 0 && !this.isinf){
            this.type = "无"
            prompt.showToast({
                message: "次数为0"
            })
            return
        }
        //若不是无穷次数，次数减少1
        if(!this.isinf){
            this.time--
        }
        //type更新
        if(this.type != "射点")   this.temp = 9
        if(this.type == "射点" && this.temp == -1)   this.temp = 9    //更新temp取代while循环，用if判断变量和递归
        this.type = "射点"
        if(this.temp >= 0){
            this.changeGpio(this.temp)
            this.addLedEffect()
            this.timerID1 = setTimeout(() => {
                this.removeLedEffect()
            },this.delay)
            this.timerID2 = setTimeout(() => {
                this.flow()
            },this.delay * 2)
            this.temp--
        }


    },
    flow2() {
        //清除setTimeout
        clearInterval(this.timerID1)
        clearInterval(this.timerID2)
        /*!this.isinf表示选择的是次数有限
        当1.次数小于0;2.次数有限;不执行函数*/
        if(this.time <= 0 && !this.isinf){
            this.type = "无"
            prompt.showToast({
                message: "次数为0"
            })
            return
        }
        //若不是无穷次数，次数减少1
        if(!this.isinf){
            this.time--
        }
        //type更新
        if(this.type != "波浪")   this.temp = 9
        if(this.type == "波浪" && this.temp == -11)   this.temp = 9   //更新temp取代while循环，用if判断变量和递归
        this.type = "波浪"
        if(this.temp >= 0){
            this.changeGpio(this.temp)
            this.addLedEffect()
            this.timerID1 = setTimeout(() => {
                this.flow2()
            },this.delay)
            this.temp--
        }
        else{
            this.changeGpio(this.temp + 10)
            this.removeLedEffect()
            this.timerID1 = setTimeout(() => {
                this.flow2()
            },this.delay)
            this.temp--
        }


    },
    random() {
        //清除setTimeout
        clearInterval(this.timerID1)
        clearInterval(this.timerID2)
        /*!this.isinf表示选择的是次数有限
        当1.次数小于0;2.次数有限;不执行函数*/
        if(this.time <= 0 && !this.isinf){
            this.type = "无"
            prompt.showToast({
                message: "次数为0"
            })
            return
        }
        //若不是无穷次数，次数减少1
        if(!this.isinf){
            this.time--
        }
        this.type = "随机"
        var i = Math.floor(Math.random()*10)
        this.changeGpio(i)
        this.addLedEffect()
        this.timerID1 = setTimeout(() => {
            this.removeLedEffect()
        },this.delay)
        this.timerID2 = setTimeout(() => {
            this.random()
        },this.delay * 2)

    },
    specification(){
        prompt.showToast({
            message: "蓝色按钮：参数控制按钮，分别有灯号、间隔、次数"+"\n"+
                    "开灯、关灯：由”灯号“控制"+"\n"+
                    "闪烁：由”灯号、间隔、次数“控制"+"\n"+
                    "射点、波浪、随机：由”间隔、次数“控制"+"\n"+
                    "中断：中断”效果“执行过程"+"\n"+
                    "右下方状态实时同步显示"
        })
    }
}