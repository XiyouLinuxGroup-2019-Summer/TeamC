/*************************************************************************
	> File Name: mysql_test.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月02日 星期五 19时43分21秒
 ************************************************************************/

#include<stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    MYSQL mysql;
    MYSQL_RES  *result=NULL;
    MYSQL_ROW  row;
    MYSQL_FIELD *field;
    mysql_init(&mysql);//初始化mysql句柄
    mysql_real_connect(&mysql,"127.0.0.1","root","200713","chatroom",0,NULL,0);//将mysql库的信息存入到mysql句柄
    
    int ret;
    int num_fields,i;
    //ret=mysql_query(&mysql,"insert into know values(\"ly\",19)");   //query insert 
   // ret = mysql_query(&mysql,"select *from know where name = \"lt\"");//query select
    char buff[100];
    char lt[20] = "jieni";
    sprintf(buff, "select *from user where name = \"%s\"", lt);//连接两个字符串
    ret = mysql_query(&mysql,buff);       
    printf("%s\n",buff);
    if(!ret)
    {
        result=mysql_store_result(&mysql);    //返回查询结果
        if(!result)
        {
            printf("ssssssssssssss");
             return -1;
        }
       //返回列数
        num_fields=mysql_num_fields(result);
        while((field = mysql_fetch_field(result))) //返回表头
        {
            printf("%-20s",field->name);

        }

        printf("\n");
        while(row=mysql_fetch_row(result))     //遍历行
        {
           for(i=0;i<num_fields;i++)  //打印列
            {
                printf("%-20s",row[i]);
            }
            printf("\n");
           /* if(strcmp(lt, row[0]) == 0) //比较密码
            {
                printf("ssssss\n");
            }
            int a = 18;
            if(a == atoi(row[1]))    //atoi 字符串转int
            {
                printf("aaaaaaaaaaa\n");
            }*/

        }
        

    }
    mysql_close(&mysql);
}
