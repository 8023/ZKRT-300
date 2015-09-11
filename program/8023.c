#include "./output/8023.h"
xdata struct str_state str_begin,str_now,str_next;//分别为:起始状态/当前状态/目标状态
xdata struct str_parameter str_cod={
    /*ui str_cod.mlinerqd*/25000,//默认主函数巡线软起动路程为25000
    /*ui str_cod.mlineqc*/200,   //默认主函数巡线前冲时间为500毫秒

    /*ui str_cod.sj1bzw*/58,     //升降标准位延时
    /*ui str_cod.sj1zjw*/800,    //升降中间位延时

    /*ui str_cod.py1bz*/20,      //fun_py1标准位延时
    /*ui str_cod.py1qkq*/666,    //fun_py1前到靠前延时参数
    /*ui str_cod.py1kqz*/684,    //fun_py1靠前到中间延时
    /*ui str_cod.py1zkh*/684,    //fun_py1中间到靠后延时
    /*ui str_cod.py1khh*/666,    //fun_py1靠后到后延时
    /*ui str_cod.py1qz*/500,     //fun_py1前到中间延时
    /*ui str_cod.py1zh*/500,     //fun_py1中间到后延时
    /*ui str_cod.py1kqkh*/1480,  //fun_py1靠前到靠后延时
    /*ui str_cod.py1qkh*/2122,   //fun_py1前到靠后延时
    /*ui str_cod.py1kqh*/2122,   //fun_py1靠前到后延时

    /*ui str_cod.hz1bz*/20,      //fun_hz1标准位延时

    /*ui turn90;*/500,           //90度转弯屏蔽延时
    /*ui turn180;*/2000          //180度转弯屏蔽延时
};
xdata struct str_timerfolline str_tfl;
ul var_timer=0;
void fun_delay(ui par_value,enum varENU_del par_model){
    ui loc_con=par_value;
    switch(par_model){
        case del_us://微秒级延时
            while(loc_con-->0){
                _nop_();
                _nop_();
            }
            return;
        case del_ms://毫秒级延时
            while(loc_con-->0){
                uc loc_i, loc_j;
                _nop_();
                _nop_();
                loc_i=12;
                loc_j=168;
                do{
                    while(--loc_j);
                }while(--loc_i);
            }
            return;
        case del_s://秒级延时
            while(loc_con-->0){
                uc loc_i, loc_j, loc_k;
                loc_i=46;
                loc_j=153;
                loc_k=245;
                do{
                    do{
                        while(--loc_k);
                    }while(--loc_j);
                }while(--loc_i);
            }
            return;
        default:return;
    }
}//延时
void fun_timer0init(){
    AUXR|=0x80;   //定时器时钟1T模式
    TMOD&=0xF0;   //定时器模式:16位
    TMOD|=0x01;   //定时器模式:16位
    TL0=0x20;     //定时初值:1ms
    TH0=0xD1;     //定时初值:1ms
    TF0=0;        //清除TF0标志
    ET0=1;
    EA=1;         //开启总中断
}//1毫秒定时器0初始化
void fun_timer1init(){
    AUXR&=0xBF;     //定时器时钟12T模式
    TMOD&=0x0F;     //定时器模式:16位
    TMOD|=0x10;     //定时器模式:16位
    TL1=0xE0;       //定时初值:20ms
    TH1=0xB1;       //定时初值:20ms
    TF1=0;          //清除TF1标志
    ET1=1;
    EA=1;           //开启总中断
}//20毫秒定时器1初始化
void fun_timer0(){
    TL0=0x20;
    TH0=0xD1;//定时器初值恢复至1ms
    var_timer++;//全局计时器开始计时
    if(str_tfl.doing==tf_ture)//如果正在移动
        fun_timermove();//定时器移动
}//1毫秒定时器0处理函数
void fun_timer1(){
    TL1=0xE0;
    TH1=0xB1;
}//20毫秒定时器1处理函数
void fun_wait(){
    while(in_start==1);
    fun_delay(20,del_ms);
    while(in_start==0);
    fun_delay(256,del_ms);
}//等待按键
void fun_pwminit(){
    CCON=0x00;//PAC寄存控制器
    CH=0;//重置PAC计时器
    CL=0;
    CMOD=0x08;//不分频
}//PWM初始化
void fun_pwmr(uc par_value){
    CCAP0H=CCAP0L=par_value*25/10;//控制输出的占空比
    CCAPM0=0X42;//8位PWM输出，无中断
    PCA_PWM0=0x00;
}//右路PWM输出
void fun_pwml(uc par_value){
    CCAP1H=CCAP1L=par_value*25/10;//控制输出的占空比
    CCAPM1=0X42;//8位PWM输出，无中断
    PCA_PWM1=0x00;
}//左路PWM输出
void fun_motors14(enum varENU_mot par_model,char par_speed){
    if(par_speed>100)
        par_speed=100;
    else if(par_speed<-100)
        par_speed=-100;//速度最多100,最少-100
    switch(par_model){
        case mot_sz://正转为抓紧，反转为松开
            if(str_begin.szsd==par_speed)
                return;
            else
                str_begin.szsd=par_speed;
            if(par_speed==0){
                out_motorselect=1;out_dir1=0;out_en1=1;
            }
            else if(par_speed>0){
                out_motorselect=1;out_dir1=1;out_en1=0;
            }
            else if(par_speed<0){
                out_motorselect=1;out_dir1=0;out_en1=0;
            }
            break;
        case mot_py://正转是向无电机一方转,反转为向有电机一方转
            if(str_begin.pysd==par_speed)
                return;
            else
                str_begin.pysd=par_speed;
            if(par_speed==0){
                out_motorselect=1;out_dir2=0;out_en2=1;
            }
            else if(par_speed>0){
                out_motorselect=1;out_dir2=1;out_en2=0;
            }
            else if(par_speed<0){
                out_motorselect=1;out_dir2=0;out_en2=0;
            }
            break;
        case mot_sj://向上为正转,向下为反转
            if(str_begin.sjsd==par_speed)
                return;
            else
                str_begin.sjsd=par_speed;
            if(par_speed==0){
                out_motorselect=0;out_dir1=0;out_en1=1;
            }
            else if(par_speed>0){
                out_motorselect=0;out_dir1=0;out_en1=0;
            }
            else if(par_speed<0){
                out_motorselect=0;out_dir1=1;out_en1=0;
            }
            break;
        case mot_hz://顺时针为正转,逆时针为反转
            if(str_begin.hzsd==par_speed)
                return;
            else
                str_begin.hzsd=par_speed;
            if(par_speed==0){
                out_motorselect=0;out_dir2=0;out_en2=1;
            }
            else if(par_speed>0){
                out_motorselect=0;out_dir2=0;out_en2=0;
            }
            else if(par_speed<0){
                out_motorselect=0;out_dir2=1;out_en2=0;
            }
            break;
        default:
            break;
    }
}//主函数操作14电机
void fun_motorsrl(enum varENU_mot par_model,char par_speed){
    if(par_speed>100)
        par_speed=100;
    else if(par_speed<-100)
        par_speed=-100;//速度最多100,最少-100
    switch(par_model){
        case mot_l://左轮电机
            if(str_begin.leftsd==par_speed)
                return;
            else
                str_begin.leftsd=par_speed;
            if(par_speed==0){
                fun_pwml(0);out_pwml=0;
            }
            else if(par_speed>0){
                CR=1;fun_pwml(par_speed);out_pwml=0;
            }
            else if(par_speed<0){
                CR=1;fun_pwml(cabs(par_speed));out_pwml=1;
            }
            break;
        case mot_r://右轮电机
            if(str_begin.rightsd==par_speed)
                return;
            else
                str_begin.rightsd=par_speed;
            if(par_speed==0){
                fun_pwmr(0);out_pwmr=0;
            }
            else if(par_speed>0){
                CR=1;fun_pwmr(par_speed);out_pwmr=0;
            }
            else if(par_speed<0){
                CR=1;fun_pwmr(cabs(par_speed));out_pwmr=1;
            }
            break;
        case mot_rl://左右轮同步
            if((str_begin.leftsd==par_speed)&&(str_begin.rightsd==par_speed))
                return;
            else
                str_begin.leftsd=str_begin.rightsd=par_speed;
            if(par_speed==0){
                CR=0;
                fun_pwmr(0);out_pwmr=0;
                fun_pwml(0);out_pwml=0;
            }
            else if(par_speed>0){
                CR=1;
                fun_pwml(par_speed);out_pwml=0;
                fun_pwmr(par_speed);out_pwmr=0;
            }
            else if(par_speed<0){
                CR=1;
                fun_pwml(cabs(par_speed));out_pwml=1;
                fun_pwmr(cabs(par_speed));out_pwmr=1;
            }
            break;
        default:
            break;
    }
}//定时器操作左右轮
void fun_sz1(enum varENU_han par_model){
    if(str_begin.szzt==par_model)
        return;
    if(par_model==han_s){//手抓松
        fun_motors14(mot_sz,-100);
        while(in_s==1);
    }
    else{//手抓紧
        fun_motors14(mot_sz,100);
        while(in_j==1);
    }
    fun_delay(20,del_ms);
    fun_motors14(mot_sz,0);
    str_begin.szzt=par_model;//存储运行结果
}//手抓单步运动
void fun_py1(enum varENU_tra par_model){
    if(str_begin.pywz==par_model)
        return;
    switch(par_model){
        case tra_q://前平移(没有电机的呢个方向)
            fun_motors14(mot_py,100);
            while(in_qpy==1);
            fun_delay(str_cod.py1bz,del_ms);
            break;
        case tra_kq:
            switch(str_begin.pywz){
                case tra_q:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1qkq,del_ms);
                    break;
                case tra_z:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1kqz,del_ms);
                    break;
                case tra_kh:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1kqkh,del_ms);
                    break;
                case tra_h:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1kqh,del_ms);
                    break;
            }
            break;
        case tra_z:
            switch(str_begin.pywz){
                case tra_q:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1qz,del_ms);
                    break;
                case tra_kq:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1kqz,del_ms);
                    break;
                case tra_kh:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1zkh,del_ms);
                    break;
                case tra_h:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1zh,del_ms);
                    break;
            }
            break;
        case tra_kh:
            switch(str_begin.pywz){
                case tra_q:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1qkh,del_ms);
                    break;
                case tra_kq:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1kqkh,del_ms);
                    break;
                case tra_z:
                    fun_motors14(mot_py,-100);
                    fun_delay(str_cod.py1zkh,del_ms);
                    break;
                case tra_h:
                    fun_motors14(mot_py,100);
                    fun_delay(str_cod.py1khh,del_ms);
                    break;
            }
            break;
        case tra_h://后平移(有电机的呢个方向)
            fun_motors14(mot_py,-100);
            while(in_hpy==1);
            fun_delay(str_cod.py1bz,del_ms);
            break;
        default:
            break;
    }
    fun_motors14(mot_py,0);
    str_begin.pywz=par_model;//存储运行结果
}//平移单步运动
void fun_sj1(enum varENU_sjp par_model){
    if(str_begin.sjwz==par_model)
       return;
    switch(par_model){
        case sjp_1://升降位置1(最上位)
            fun_motors14(mot_sj,100);
            def_select(sel_58);
            while(in_wz1==1);
            fun_delay(str_cod.sj1bzw,del_ms);
            break;
        case sjp_12:
            if(par_model>str_begin.sjwz){
                fun_motors14(mot_sj,-100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            else{//要去的地方在上面，向上走
                fun_sj1(sjp_2);
                fun_motors14(mot_sj,100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            break;
        case sjp_2://升降位置2
            fun_motors14(mot_sj,par_model<str_begin.sjwz?100:-100);
            def_select(sel_58);
            while(in_wz2==1);
            fun_delay(str_cod.sj1bzw,del_ms);
            break;
        case sjp_23:
            if(par_model>str_begin.sjwz){
                fun_sj1(sjp_2);
                fun_motors14(mot_sj,-100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            else{//要去的地方在上面，向上走
                fun_sj1(sjp_3);
                fun_motors14(mot_sj,100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            break;
        case sjp_3://升降位置3
            fun_motors14(mot_sj,par_model<str_begin.sjwz?100:-100);
            def_select(sel_58);
            while(in_wz3==1);
            fun_delay(str_cod.sj1bzw,del_ms);
            break;
        case sjp_34:
            if(par_model>str_begin.sjwz){
                fun_sj1(sjp_3);
                fun_motors14(mot_sj,-100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            else{//要去的地方在上面，向上走
                fun_sj1(sjp_4);
                fun_motors14(mot_sj,100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            break;
        case sjp_4://升降位置4
            fun_motors14(mot_sj,par_model<str_begin.sjwz?100:-100);
            def_select(sel_58);
            while(in_wz4==1);
            fun_delay(str_cod.sj1bzw,del_ms);
            break;
        case sjp_45:
            if(par_model>str_begin.sjwz){
                fun_sj1(sjp_4);
                fun_motors14(mot_sj,-100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            else{//要去的地方在上面，向上走
                fun_motors14(mot_sj,100);
                fun_delay(str_cod.sj1zjw,del_ms);
            }
            break;
        case sjp_5://升降位置5
            fun_motors14(mot_sj,-100);
            def_select(sel_912);
            while(in_wz5==1);
            fun_delay(str_cod.sj1bzw,del_ms);
            break;
        default:
            break;
    }
    fun_motors14(mot_sj,0);
    str_begin.sjwz=par_model;//存储运行结果
}//升降单步运动
void fun_hz1(enum varENU_dir par_model){
    if(str_begin.hzfx==par_model)
        return;
    def_select(sel_912);
    switch(par_model){
        case dir_up://回转至前方
            switch(str_begin.hzfx){
                case dir_down://现在在下方
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    while(in_hz==0);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                case dir_left://现在在左边
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                case dir_right://现在在右边
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                default:
                    break;
            }
            break;
        case dir_down://要去下面
            switch(str_begin.hzfx){
                case dir_up://现在在上面
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    while(in_hz==0);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                case dir_left://现在在左面
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                    break;
                case dir_right://现在在右面
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                default:
                    break;
            }
            break;
        case dir_left://要去左边
            switch(str_begin.hzfx){
                case dir_up://现在在上面
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                    break;
                case dir_down://现在在下面
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                    break;
                case dir_right://现在在右面
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    while(in_hz==0);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                default:
                    break;
            }
            break;
        case dir_right://要去右面
            switch(str_begin.hzfx){
                case dir_up://现在在前面
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                case dir_down://现在在下面
                    fun_motors14(mot_hz,-100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                case dir_left://现在在左面
                    fun_motors14(mot_hz,100);
                    fun_delay(1,del_s);
                    while(in_hz==1);
                    while(in_hz==0);
                    while(in_hz==1);
                    fun_delay(str_cod.hz1bz,del_ms);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    fun_motors14(mot_hz,0);
    str_begin.hzfx=par_model;//存储运行结果
}//回转单步运动

//void fun_pyhz2(enum varENU_tra par_pymodel,enum varENU_dir par_hzmodel){//平移回转同步运动
//    switch(){
//        ;
//    }
//}

void fun_jtjp(){
    while(1){
        if((!in_ls1&&!in_ls2&&in_ls4&&in_ls5&&!in_ls7&&!in_ls8)&&((in_ls3&&in_ls6)||(!in_ls3&&!in_ls6))){
            fun_delay(10,del_ms);
            fun_motors14(mot_rl,0);
            return;
        }
        if(in_ls2||in_ls1){
            fun_motors14(mot_l,16);
            fun_motors14(mot_r,-16);
        }
        else if(in_ls7||in_ls8){
            fun_motors14(mot_l,-16);
            fun_motors14(mot_r,16);
        }
        else{
            if(!in_ls4){
                fun_motors14(mot_l,-12);
                fun_motors14(mot_r,12);
            }
            else if(!in_ls5){
                fun_motors14(mot_l,12);
                fun_motors14(mot_r,-12);
            }
            else{
                if(in_ls6&&!in_ls3){
                    fun_motors14(mot_l,-8);
                    fun_motors14(mot_r,8);
                }
                if(in_ls3&&!in_ls6){
                    fun_motors14(mot_l,8);
                    fun_motors14(mot_r,-8);
                }
            }
        }
    }
}//静态纠偏
void fun_timermove(){
    static uc loc_con=0;//需要分步做的动作需要用到此静态私有分步标志位
    uc loc_sdl=str_tfl.gospeed,loc_sdr=str_tfl.gospeed;
    switch(*str_tfl.run){//选择运行方式
        case def_end://运行结束
            fun_motorsrl(mot_rl,0);//再次停止电机运动
            str_tfl.doing=tf_false;//移动结束
            memset(str_tfl.step,0,sizeof(str_tfl.step));//清空step数组
            str_tfl.run=str_tfl.step;//指针指向step的第一个元素
            break;
        case tfl_line://运行巡线
            if(*(str_tfl.run+1)){//如果参数为非0数
                if((in_ls1||in_ls8)&&(in_ls2&&in_ls3&&in_ls4&&in_ls5&in_ls6&&in_ls7)){//如果中间6个灯全亮,左右亮任意一个
                    str_tfl.online=tf_ture;//标志位记录在线
                    fun_motorsrl(mot_rl,str_tfl.gospeed);//按照常规速度过线
                }
                else if(str_tfl.online==tf_ture){//如果标志位记录在线,而且不符合在线特征
                    str_tfl.online=tf_false;//标志位记录不在线
                    (*(str_tfl.run+1))--;//参数值减1,记录已经走了一条线
                }
                else{
                    if(in_ls1&&!in_ls8){//1亮8不亮
                        loc_sdl*=0.5;//左减速
                        loc_sdr*=1.5;//右加速
                    }//向左转
                    if(in_ls8&&!in_ls1){//8亮1不亮
                        loc_sdl*=1.5;//左加速
                        loc_sdr*=0.5;//右减速
                    }//向右转
                    if(in_ls2&&!in_ls7){//2亮7不亮
                        loc_sdl*=0.7;//左减速
                        loc_sdr*=1.3;//右加速
                    }//向左转
                    if(in_ls7&&!in_ls2){//7亮2不亮
                        loc_sdl*=1.3;//左加速
                        loc_sdr*=0.7;//右减速
                    }//向右转
                    if(in_ls3&&!in_ls6){//3亮6不亮
                        loc_sdl*=0.9;//左减速
                        loc_sdr*=1.1;//右加速
                    }//向左转
                    if(in_ls6&&!in_ls3){//6亮3不亮
                        loc_sdl*=1.1;//左加速
                        loc_sdr*=0.9;//右减速
                    }//向右转
                    fun_motorsrl(mot_r,loc_sdr);
                    fun_motorsrl(mot_l,loc_sdl);
                }
            }
            else{//如果线走完了
                str_tfl.run+=2;//指针指向下一组过程
                fun_motorsrl(mot_rl,0);//停止电机运动
            }
            break;
        case tfl_turn://运行转弯
            switch(loc_con){//分步运行标志位
                case 0://第一步
                    switch(*(str_tfl.run+1)){//获取参数
                        case tur_r90://右转90
                            fun_motorsrl(mot_l,str_tfl.turnspeed);//左轮向前
                            fun_motorsrl(mot_r,-str_tfl.turnspeed);//右轮向后
                            str_tfl.delay=var_timer+str_cod.turn90;//计时
                            break;
                        case tur_l90://左转90
                            fun_motorsrl(mot_l,-str_tfl.turnspeed);//左轮向后
                            fun_motorsrl(mot_r,str_tfl.turnspeed);//右轮向前
                            str_tfl.delay=var_timer+str_cod.turn90;//计时
                            break;
                        case tur_r180://右转180
                            fun_motorsrl(mot_l,str_tfl.turnspeed);//左轮向前
                            fun_motorsrl(mot_r,-str_tfl.turnspeed);//右轮向后
                            str_tfl.delay=var_timer+str_cod.turn180;//计时
                            break;
                        case tur_l180://左转180
                            fun_motorsrl(mot_l,-str_tfl.turnspeed);//左轮向后
                            fun_motorsrl(mot_r,str_tfl.turnspeed);//左轮向前
                            str_tfl.delay=var_timer+str_cod.turn180;//计时
                            break;
                    }
                    loc_con++;//执行下一步
                    break;
                case 1://第二步
                    if(str_tfl.delay<=var_timer)//计时时间到
                        loc_con++;//执行下一步
                    break;
                case 2://第三步
                    if(in_ls4&&in_ls5){//如果中间两个灯亮
                        fun_motorsrl(mot_rl,0);//停止电机
                        str_tfl.run+=2;//指针指向下一组过程
                        str_tfl.delay=0;//延时计数器归零
                        loc_con=0;//分步运行标志位归零
                    }
                    break;
            }
            break;
        case tfl_cache://运行前冲
            switch(loc_con){
                case 0://第一步
                    str_tfl.delay=var_timer+((*(str_tfl.run+1))*10);//延时时间(毫秒)为参数的10倍
                    loc_con++;//执行下一步
                    break;
                case 1://第二步
                    if(str_tfl.delay<=var_timer){//计时时间到
                        fun_motorsrl(mot_rl,0);//停止电机
                        str_tfl.run+=2;//指针指向下一组过程
                        str_tfl.delay=0;//延时计数器归零
                        loc_con=0;//分步运行标志位归零
                    }
                    else{
                        if(in_ls1&&!in_ls8){//1亮8不亮
                            loc_sdl*=0.5;//左减速
                            loc_sdr*=1.5;//右加速
                        }//向左转
                        if(in_ls8&&!in_ls1){//8亮1不亮
                            loc_sdl*=1.5;//左加速
                            loc_sdr*=0.5;//右减速
                        }//向右转
                        if(in_ls2&&!in_ls7){//2亮7不亮
                            loc_sdl*=0.7;//左减速
                            loc_sdr*=1.3;//右加速
                        }//向左转
                        if(in_ls7&&!in_ls2){//7亮2不亮
                            loc_sdl*=1.3;//左加速
                            loc_sdr*=0.7;//右减速
                        }//向右转
                        if(in_ls3&&!in_ls6){//3亮6不亮
                            loc_sdl*=0.9;//左减速
                            loc_sdr*=1.1;//右加速
                        }//向左转
                        if(in_ls6&&!in_ls3){//6亮3不亮
                            loc_sdl*=1.1;//左加速
                            loc_sdr*=0.9;//右减速
                        }//向右转
                        fun_motorsrl(mot_r,loc_sdr);
                        fun_motorsrl(mot_l,loc_sdl);
                    }
                    break;
            }
            break;
        case tfl_start://运行带加速的前冲

            break;
        case tfl_end://运行带减速的前冲

            break;
    }
}//定时器移动
void fun_stope2prom(){
    IAP_CONTR = 0;                  //关闭IAP功能
    IAP_CMD = 0;                    //清除命令
    IAP_TRIG = 0;                   //清除触发寄存器
    IAP_ADDRH = 0x80;               //数据指针指向非EEPROM区
    IAP_ADDRL = 0;                  //清除IAP地址
}//关闭EEPROM功能(IapIdle)
uc fun_reade2prom(ui par_add){
    uc loc_dat;                     //数据缓冲区
    IAP_CONTR = 0x83;               //打开EEPROM功能,设置等待时间
    IAP_CMD = 1;                    //设置EEPROM读命令
    IAP_ADDRL = par_add;            //设置EEPROM地址低八位
    IAP_ADDRH = par_add >> 8;       //设置EEPROM地址高八位
    IAP_TRIG = 0x5a;                //触发
    IAP_TRIG = 0xa5;                //再次触发
    fun_delay(10,del_us);           //稍等一会儿
    loc_dat = IAP_DATA;             //读出EEPROM中的数据
    fun_stope2prom();               //关闭EEPROM功能
    return loc_dat;                 //返回读取结果
}//读取EEPROM数据
void fun_writee2prom(ui par_add,uc par_dat){
    IAP_CONTR = 0x83;               //打开EEPROM功能,设置等待时间
    IAP_CMD = 2;                    //设置EEPROM写入命令
    IAP_ADDRL = par_add;            //设置EEPROM地址低八位
    IAP_ADDRH = par_add >> 8;       //设置EEPROM地址高八位
    IAP_DATA = par_dat;             //写入数据
    IAP_TRIG = 0x5a;                //触发
    IAP_TRIG = 0xa5;                //再次触发
    fun_delay(10,del_us);           //稍等一会儿
    fun_stope2prom();               //关闭EEPROM功能
}//写EEPROM数据
void fun_cleane2prom(ui par_add){
    IAP_CONTR = 0x83;               //打开EEPROM功能,设置等待时间
    IAP_CMD = 3;                    //设置EEPROM擦除命令
    IAP_ADDRL = par_add;            //设置EEPROM地址低八位
    IAP_ADDRH = par_add >> 8;       //设置EEPROM地址高八位
    IAP_TRIG = 0x5a;                //触发
    IAP_TRIG = 0xa5;                //再次触发
    fun_delay(10,del_us);           //稍等一会儿
    fun_stope2prom();               //关闭EEPROM功能
}//清除EEPROM数据
void fun_calibration(){
    // fun_sj1(sjp_1);//升到最上位
    ul loc_time;
    fun_py1(tra_q);//移动到最前端
    fun_py1(tra_h);//移动到最后端
    loc_time=var_timer;
    fun_py1(tra_q);//移动到前端并计时
    fun_py1(tra_h);//移动到后端
    loc_time=var_timer-loc_time;
    loc_time*=0.5;
    str_cod.py1zh=loc_time*0.5;//从后到中间
    str_cod.py1qz=loc_time*0.5;//从前到中间
    str_cod.py1qkh=loc_time*0.75;//从前到靠后
    str_cod.py1kqh=loc_time*0.564;//从靠前到后
    str_cod.py1qkq=loc_time*0.34;//从前到靠前
    str_cod.py1khh=loc_time*0.2701;//从后到靠后
    str_cod.py1kqz=loc_time*0.189;//从靠前到中间
    str_cod.py1zkh=loc_time*0.20;//从靠后到中间.
    str_cod.py1kqkh=loc_time*0.5;//从靠前到靠后
}//自动校准平移参数
void fun_port(){
    PCON|=0x80;     //使能波特率倍速位SMOD
    SCON=0x50;      //8位数据,可变波特率
    AUXR|=0x04;     //独立波特率发生器时钟为Fosc,即1T
    BRT=0xD9;       //设定独立波特率发生器重装值
    AUXR|=0x01;     //串口1选择独立波特率发生器为波特率发生器
    AUXR|=0x10;     //启动独立波特率发生器
    TI=1;           //打开串口传输功能
}//串口初始化
void fun_test(){
    fun_sz1(han_j);
    fun_delay(1,del_s);
    fun_sz1(han_s);
    fun_delay(1,del_s);
    fun_sz1(han_j);
    fun_delay(1,del_s);
    fun_sz1(han_s);
    fun_delay(1,del_s);

    str_begin.pywz=tra_h;
    fun_py1(tra_q);
    fun_delay(1,del_s);
    fun_py1(tra_h);
    fun_delay(1,del_s);
    fun_py1(tra_q);
    fun_delay(1,del_s);
    fun_py1(tra_h);
    fun_delay(1,del_s);

    fun_sj1(sjp_1);
    fun_delay(1,del_s);
    fun_sj1(sjp_2);
    fun_delay(1,del_s);
    fun_sj1(sjp_3);
    fun_delay(1,del_s);
    fun_sj1(sjp_4);
    fun_delay(1,del_s);
    fun_sj1(sjp_5);
    fun_delay(1,del_s);
    fun_sj1(sjp_4);
    fun_delay(1,del_s);
    fun_sj1(sjp_3);
    fun_delay(1,del_s);
    fun_sj1(sjp_2);
    fun_delay(1,del_s);
    fun_sj1(sjp_1);

    str_now.hzfx=str_begin.hzfx;
    str_begin.hzfx=dir_up;
    fun_hz1(dir_right);
    fun_delay(1,del_s);
    fun_hz1(dir_up);
    fun_delay(1,del_s);
    fun_hz1(dir_right);
    fun_delay(1,del_s);
    fun_hz1(dir_up);
    fun_delay(1,del_s);
    str_begin.hzfx=str_now.hzfx;

    fun_motors14(mot_rl,60);
    fun_delay(1,del_s);
    fun_motors14(mot_rl,0);
    fun_delay(1,del_s);

    fun_motors14(mot_rl,-60);
    fun_delay(1,del_s);
    fun_motors14(mot_rl,0);
    fun_delay(1,del_s);

    fun_motors14(mot_r,40);
    fun_motors14(mot_l,-40);
    fun_delay(1,del_s);
    fun_motors14(mot_rl,0);
    fun_delay(1,del_s);

    fun_motors14(mot_r,-40);
    fun_motors14(mot_l,40);
    fun_delay(1,del_s);
    fun_motors14(mot_rl,0);
}//测试程序
uc fun_min(uc par_num,...){
    va_list loc_argp;//保存参数结构
    uc loc_min=par_num;//现在的最小值就是第一个
    uc loc_shu;//当前参数
    va_start(loc_argp,par_num);//loc_argp指向传入的第一个可选参数，par_num是最后一个确定的参数
    loc_shu=va_arg(loc_argp,uc);//取出下一个参数
    do{
        if(loc_shu<loc_min)
            loc_min=loc_shu;
        loc_shu=va_arg(loc_argp,uc);//取出下一个参数
    }while(loc_shu!=def_end);
    va_end(loc_argp);//结束
    return loc_min;//退出
}//求最小值
void fun_coordinate(){
    
}//自动巡线之坐标