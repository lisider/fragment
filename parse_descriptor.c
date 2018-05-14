#include <stdio.h>

//定义描述符中的低32位
struct seg_des_low_word
{
    unsigned int limit_0_15:16;
    unsigned int base_0_15 :16;

};
//定义描述符中的高32位(通用格式)
struct seg_des_high_word
{
    unsigned int base_16_23 :8;
    unsigned int type       :4;
    unsigned int s          :1;
    unsigned int dpl        :2;
    unsigned int p          :1;
    unsigned int limit_16_19:4;
    unsigned int avl        :1;
    unsigned int l          :1;//保留给64位处理器使用
    unsigned int d_b        :1;
    unsigned int g          :1;
    unsigned int base_24_31 :8;
};


void parse_seg_des(unsigned int low_dw, unsigned int hig_dw)
{

    struct seg_des_low_word *pl 
        =(struct seg_des_low_word*)&low_dw;

    struct seg_des_high_word *ph
        =(struct seg_des_high_word*)&hig_dw;

    unsigned int type = ph->type;

    int offset = (hig_dw & 0xFFFF0000) | (low_dw & 0x0000FFFF);


    unsigned int seg_base = 
        (ph->base_24_31<<24)|(ph->base_16_23<<16)|pl->base_0_15;//拼接基地址字段

    unsigned int seg_limit =
        seg_limit = (ph->limit_16_19<<16)|pl->limit_0_15;//拼接段限长字段

    int system = ph->s;

    //下面的字段输出是不是很方便？这就是位字段的好处之一
    printf("P = %d\n",ph->p);
    printf("DPL = %d\n",ph->dpl); //P和DPL是每个段描述符都有的

    if(system == 0) //system segment
    {
        if(type == 9 || type==11 || type==2) //==的优先级高于||
        {
            //print base address,limit,G
            printf("base address = %#X\n",seg_base);
            printf("limit = %#X ，",seg_limit);

            if(ph->g == 1)
                printf("以4KB为单位\n");
            else
                printf("以B为单位\n");
        }

        if(type == 9)
            printf("TSS段，不忙\n");
        else if(type == 11)
            printf("TSS段，忙\n"); 
        else if(type == 5)
            printf("任务门，TSS段选择子 = %#X\n",pl->base_0_15);
        else if(type == 6)
            printf("16位中断门，段选择子 = %#X，偏移 = %#X\n",pl->base_0_15,offset);
        else if(type == 14)
            printf("32位中断门，段选择子 = %#X，偏移 = %#X\n",pl->base_0_15,offset);
        else if(type == 7)
            printf("16位陷阱门，段选择子 = %#X，偏移 = %#X\n",pl->base_0_15,offset);
        else if(type == 15)
            printf("32位陷阱门，段选择子 = %#X，偏移 = %#X\n",pl->base_0_15,offset);
        else if(type == 12)
            printf("调用门，段选择子 = %#X，偏移 = %#X , 参数个数 = %d\n",pl->base_0_15,offset, hig_dw & 0x1F);
        else if(type == 2)
            printf("LDT描述符\n");
    }
    else //data or code segment
    {
        printf("base address = %#X\n",seg_base);
        printf("limit = %#X ，",seg_limit);

        if(ph->g == 1)
            printf("以4KB为单位\n");
        else
            printf("以B为单位\n");

        if(type & 0x08 ) //代码段，分析C，R，D      
        {
            printf("代码段，");

            if(ph->d_b == 1) //D=1
                printf("默认操作数大小为32位\n");
            else
                printf("默认操作数大小为16位\n");

            if(type & 0x04) //C=1
                printf("一致性，");
            else
                printf("非一致性，");

            if(type & 0x02) //R=1
                printf("可读\n");
            else
                printf("不可读\n");
        }
        else    //数据段，分析B，E，W
        {
            printf("数据段，");

            if(ph->d_b == 1) //B=1
                printf("栈段使用ESP，ESP最大为0xFFFFFFFF\n");
            else
                printf("栈段使用SP，SP最大为0xFFFF\n");

            if(type & 0x04) //E=1
                printf("向下扩展，");
            else
                printf("向上扩展，");

            if(type & 0x02) //W=1
                printf("可写\n");
            else
                printf("不可写\n");
        }
    }

}

int main(void)
{
    printf("please input the segment descriptor,format: [low dword] [high dword]\n");
    printf("example:\n");
    printf("000003ff 00c0fa00\n");
    printf("The Descriptor:\n");

    unsigned int l_dword = 0;
    unsigned int h_dword = 0;

    //请求用户输入描述符，先是低32位，再是高32位
    scanf("%x" "%x",&l_dword,&h_dword);
    printf("----------------------\n");
    parse_seg_des(l_dword,h_dword);

    return 0;
}