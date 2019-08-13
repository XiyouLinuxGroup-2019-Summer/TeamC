void Interface() {
	time_t Time = time(NULL);
	char *buf = ctime(&Time);
	system("clear");
	printf("%s", buf);
	printf("|****************************|\n");
	printf("|          欢迎使用          |\n");
	printf("|****************************|\n");
	printf("|       1、用户注册          |\n");
	printf("|       2、用户登录          |\n");
	printf("|       3、退出              |\n");
	printf("|****************************|\n");
	printf("请输入您要进行的操作:\n");
}
void uesr_menu() {
	time_t Time = time(NULL);
	char *buf = ctime(&Time);
	system("clear");
	printf("%s", buf);
	printf("|****************************|\n");
	printf("|           主菜单           |\n");
	printf("|****************************|\n");
	printf("|      1、显示在线好友       |\n");
	printf("|      2、添加好友           |\n");
	printf("|      3、删除好友           |\n");
	printf("|      4、创建群聊           \n|");
	printf("|      5、加入群聊           |\n");
	printf("|      6、退出群聊           |\n");
	printf("|      7、查看聊天记录       |\n");
	printf("|      8、用户注销           |\n");
	printf("|      9、文件传输           |\n");
	printf("|      10、退出               |\n");
	printf("|****************************|\n");
	printf("请输入您要进行的操作:\n");
}
