编译时找不到头文件，需要在wscript的build时include相关的库路径(eclipse编译需要更改隐藏文件.cproject的头文件包含)
link时找不到gtk函数，在configure时使用ctx.check_cfg方法找库路径并保存在uselib_store指定的环境变量里，然后在build时使用uselib选项调用该路径
(	
	ctx.check_cfg(package='gtk+-2.0', args=['--cflags', '--libs'] , uselib_store='GCC', mandatory=True) 
	print(ctx.env.LIB_GCC)
)
