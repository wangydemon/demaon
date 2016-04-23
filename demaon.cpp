/*
                                              守护进程（Demaon）
   守护进程（Daemon）是一种运行在后台的一种特殊的进程，它独立于控制终端并且周期性的执行某种任务或等待处理某些发生的事件。
由于在linux中，每个系统与用户进行交流的界面成为终端，每一个从此终端开始运行的进程都会依附于这个终端，这个终端被称为这些进程的控制终端，
当控制终端被关闭的时候，相应的进程都会自动关闭。但是守护进程却能突破这种限制，它脱离于终端并且在后台运行，并且它脱离终端的目的是为了
避免进程在运行的过程中的信息在任何终端中显示并且进程也不会被任何终端所产生的终端信息所打断。它从被执行的时候开始运转，知道整个系统关闭
才退出（当然可以认为的杀死相应的守护进程）。如果想让某个进程不因为用户或中断或其他变化而影响，那么就必须把这个进程变成一个守护进程

   守护进程实现的步骤：
   （1）  创建子进程，父进程退出（使子进程成为孤儿进程）：这是编写守护进程的第一步，由于守护进程是脱离终端的，
          因此完成第一步后就会在shell终端里造成一个程序已经运行完毕的假象。之后的所有工作在子进程中完成，而用户在shell终端里则可以执行
          其他命令，从而在形式上做到了与控制终端脱离。实现的语句如下：if(pid=fork()){exit(0);}是父进程就结束，然后子进程继续执行。
   （2） 在子进程中创建新的会话（脱离控制终端）：
         这步是创建守护进程中最重要的一步，虽然实现起来很简单，但是它的意义非常重要，在这里使用的是系统函数setsid（）来创建一个新的会
         话，并且担任该会话组的组长。在这里有两个概念需要解释一下，进程组合会话期。

        进程组：是一个或多个进程的集合。进程组有进程组ID来唯一标识。除了进程号（PID）之外，进程组ID也是一个进程的必备属性。
        每个进程组都有一个组长进程，其组长进程的进程号等于进程组ID。且该进程组ID不会因组长进程的退出而受到影响。
        会话周期：会话期是一个或者多个进程的集合。通常一个会话开始于用户的登录，终止于用户的退出，在此期间该用户运行的所有进
        程都属于这个会话期。
		
	 Setsid（）函数的相关内容：

         （1）  setsid（）函数的作用：创建一个新的会话，并且担任该会话组的组长。具体作用包括：让一个进程摆脱原会话的控制，
                让进程摆脱原进程的控制，让进程摆脱原控制终端的控制。

         （2）  创建守护进程要调用setsid（）函数的原因：由于创建守护进程的第一步是调用fork（）函数来创建子进程，再将父进程退出。
         由于在调用了fork（）函数的时候，子进程拷贝了父进程的会话期、进程组、控制终端等资源、虽然父进程退出了，
         但是会话期、进程组、控制终端等并没有改变，因此，需要用setsid（）函数来使该子进程完全独立出来，从而摆脱其他进程的控制。

   （3）  改变当前目录为根目录：
          使用fork（）创建的子进程是继承了父进程的当前工作目录，由于在进程运行中，当前目录所在的文件系统是不能卸载的，
          这对以后使用会造成很多的麻烦。因此通常的做法是让“/”作为守护进程的当前目录，当然也可以指定其他的别的目录来作为守护进程的工作目录。
   （4）  重设文件权限掩码：
          文件权限掩码是屏蔽掉文件权限中的对应位。由于使用fork（）函数新创建的子进程继承了父进程的文件权限掩码，
          这就给该子进程使用文件带了很多的麻烦（比如父进程中的文件没有执行文件的权限，然而在子进程中希望执行相应的文
          件这个时候就会出问题）。因此在子进程中要把文件的权限掩码设置成为0，即在此时有最大的权限，这样可以大大增强该守护进程的灵活性。
          设置的方法是：umask（0）。

   （5）  关闭文件描述符：
          同文件权限码一样，用fork（）函数新建的子进程会从父进程那里继承一些已经打开了的文件。这些文件被打开的文件可能永远不会被守
          护进程读写，如果不进行关闭的话将会浪费系统的资源，造成进程所在的文件系统无法卸下以及引起预料的错误。按照如下方法关闭它们：
          for(i=0;i 关闭打开的文件描述符close(i);
*/


// Demaon函数，只需main函数调用即可

void Daemon()
{
	//const int MAXFD = 64;
	if (fork() != 0) //父进程退出
		exit(0);
	setsid(); 
	//chdir("/");
	umask(0);
	//for (int i = 0; i < MAXFD; i++)
	//	close(i);
}

/*
//          守护进程简单实现
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXFILE 65535

int main()
{
    pid_t pc;
    int i,fd,len;
    char *buf="this is a dameon \n";
    len = strlen(buf);
    pc =fork();     //创建一个进程用来做守护进程
    if(pc<0)
        {
           printf("error fork \n");
            exit(1);
        }
    else if(pc>0)
        exit(0);    //结束父进程
    setsid();       //使子进程独立1.摆脱原会话控制 2.摆脱原进程组的控制 3.摆脱控制中端的控制
    chdir("/");     //改变当前工作目录，这也是为了摆脱父进程的影响
    umask(0);       //重设文件权限掩码
    for(i=0;i<MAXFILE;i++)  //关闭文件描述符(常说的输入，输出，报错3个文件)，
   //因为守护进程要失去了对所属的控制终端的联系，这三个文件要关闭
   close(i);
    while(1)
        {
        if((fd=open("/tmp/dameon.txt",O_CREAT|O_WRONLY|O_APPEND,0600))<0)
               {
                   printf("open file err \n");
                   exit(0);
               }
           write(fd,buf,len+1);
            close(fd);
            sleep(10);
        }
}


*/
