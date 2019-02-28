
asmlinkage long sys_mysycall(char * srcFile, char * desFile);
asmlinkage long sys_mysycall(char * srcFile, char * desFile)
{
    int src = sys_open(srcFile, O_RDONLY, 0);
    int des = sys_open(desFile, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    char buff[4096];
    mm_segment_t fs;
    fs = get_fs();
    set_fs(get_ds());
    int i;
    if(src > 0 && des > 0)
    {
        do{
            i = sys_read(src, buff, 4096);
            sys_write(des, buff, i);
        }while(i);
    }
    else
    {
        printk("error!");
    }
    sys_close(src);
    sys_close(des);
    set_fs(fs);
    return 0;
}
