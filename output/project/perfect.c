#include "HANS.h"
#include "../8023.h"
void main(){
    fun_start(//在此输入小车目前的状态
        4,//X坐标
        0,//Y坐标
        dir_up,//车头方向
        han_s,//手抓状态
        sjp_1,//升降位置
        tra_h,//平移位置
        dir_right);//回转方向
    // fun_setxy(//x轴坐标,y轴坐标,工位号,到达此坐标时需要的朝向,工位在小车的哪一侧
    //     0,4,4,dir_up,dir_right,
    //     0,9,2,dir_up,dir_right,
    //     2,7,3,dir_up,dir_left,
    //     3,9,1,dir_up,dir_right,
    //     5,dir_right,dir_down);
    // /*请从下一行开始写你的程序*/
    
    fun_setxy(//x轴坐标,y轴坐标,工位号,到达此坐标时需要的朝向,工位在小车的哪一侧
        1,7,4,DU,DR,
        2,10,2,DU,DL,
        3,9,3,DU,DL,
        3,10,1,DU,DR,
        5,DR,DR);
    /*START*/

/*    
    L1
W
    
    L2
W
    
    L3
W
    
    O4
W
    
    O5
W
    
    
    L1
W
    O4
W
    
    L3
W
    
    O5
W
    
    O4
W
    
    L1
W
    
    L2
W
    
    O4
W
    
    L1
W
    O5
W
    
    O4
W
    
    OE
    W
*/


    /*你的程序到此为止,请不要再继续改动*/
    fun_stop();
}
void Int0Interrupt() interrupt 0{
    ;
}
void Timer0Interrupt() interrupt 1 using 1{
    fun_timer0();
}
void Timer1Interrupt() interrupt 3 using 1{
    fun_timer1();
}