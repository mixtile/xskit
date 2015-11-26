#if 0
#if XS_EVKIT_ANSIONE
///ANSIONE test six functions,including xsStrLen,xsStrCpy,xsStrCpyN,xsStrCat,xsStrCatN and xsSnprintf.

#endif
#if XS_EVKIT_ANSITWO
/////ANSITWO test four functions,including xsStrCmp,xsStrCaseCmp,xsStrCmpN and xsStrCaseCmpN.

#endif
#if XS_EVKIT_ANSITHREE
/////ANSITHREE test four functions,including xsStrChr,xsStrStr,xsStrToInt,xsIntToStr,xsStrLwr and xsStrUpr.

#endif

static void TestANSIOne()
{
	printf("-------------------------xsStrLen Start---------------------------------------------------------\n");
	///////xsStrLen
	char *len ="abcd";
	XS_TEST(xsStrLen(len) == 4);
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrLen End---------------------------------------------------------\n");
	printf("-------------------------xsStrCpy Start---------------------------------------------------------\n");
	///////xsStrCpy//////////////////////////////////////////////////////////

	//可以free的
	char *cpyTo =(char *)malloc(sizeof(char)*5);
	char *cpyF = "aaaa";
	char *cpyR = xsStrCpy(cpyTo,cpyF);
	printf("cpyTo:%s\ncpyR:%s\n",cpyTo,cpyR);
	XS_TEST(xsStrLen(cpyTo) == 4);
	free(cpyTo);
	//cpyF不能为NULL
	/*cpyTo = (char *)malloc(5);
	 cpyF = NULL;
	 cpyR =  xsStrCpy(cpyTo,cpyF);*/
	cpyTo = (char *)malloc(5);
	cpyF = "";
	cpyR = xsStrCpy(cpyTo,cpyF);
	free(cpyTo);
	//不可以free的 越界了也能copy 因为有越界检查
	cpyTo = (char *)malloc(5);
	cpyF = "BBBBBB";
	cpyR = xsStrCpy(cpyTo,cpyF);
	XS_TEST(xsStrLen(cpyTo) == 6);
	printf("cpyTo1:%s\ncpyR1:%s\n",cpyTo,cpyR);
	//free(cpyTo); //出错,越界了无法释放内存
	////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCpy End---------------------------------------------------------\n");
	printf("-------------------------xsStrCpyN Start---------------------------------------------------------\n");
	///////xsStrCpyN/////////////////////////////////////////////////////////
	char * cpynT = (char*)malloc(5);
	char * cpynF = "dddddddd";
	char * cpynR = xsStrCpyN(cpynT,cpynF,4);
	XS_TEST(xsStrLen(cpynT) > 4);
	//由于cpynT没有以'\0'结尾 所以输出的是乱码 print输出一直到'\0'为止
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);

	//由于申请的空间小于max（即5<6）所以这行代码会出错
	//cpynR = xsStrCpyN(cpynT,cpynF,6);
	//max值不可以为负数，否则出错。
	//cpynR = xsStrCpyN(cpynT,cpynF,-1);
	free(cpynT);
	//全部拷贝（包含'\0'一起拷贝，否则一样有乱码）就没有乱码了
	cpynT = (char*)malloc(9);
	cpynR = xsStrCpyN(cpynT,cpynF,9);
	XS_TEST(xsStrLen(cpynT) == 8);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	free(cpynT);
	cpynT = (char*)malloc(10);
	cpynR = xsStrCpyN(cpynT,cpynF,10);
	XS_TEST(xsStrLen(cpynT) == 8);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	free(cpynT);
	cpynT = (char*)malloc(10);
	cpynR = xsStrCpyN(cpynT,cpynF,0);
	XS_TEST(xsStrLen(cpynT) > 0);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	//////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCpyN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCat Start---------------------------------------------------------\n");
	///////xsStrCat///////////////////////////////////////////////////////////////////////////////////////////
	char *catF = "dskk";
	char *temp = "dsd";
	char *catT = (char *)malloc(4);
	xsStrCpy(catT,temp);
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 7);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	//  free(catT);
	xsStrCpy(catT,"");
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 4);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	// free(catT);
	//catF不能为NULL
	/* xsStrCpy(catT,temp);
	 catF = NULL;
	 xsStrCat(catT,catF);*/
	xsStrCpy(catT,temp);
	catF ="";
	xsStrCat(catT,catF);
	printf("catT:%s\n",catT);
	XS_TEST(xsStrLen(catT) == 3);
	//////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCat End---------------------------------------------------------\n");
	printf("-------------------------xsStrCatN Start---------------------------------------------------------\n");
	///////////////xsStrCatN//////////////////////////////////////////////////////////////////////
	char *catnF = "djsd";
	char *catnT = (char*)malloc(4);
	temp = "sds";
	xsStrCpy(catnT,temp);
	xsStrCatN(catnT,catnF,-1);

	XS_TEST(xsStrLen(catnT) == 7);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,0);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,2);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,4);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	XS_TEST(xsStrLen(catnT) > 0);
	xsStrCatN(catnT,"",5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	//catF不能为NULL 否则出错
	/*xsStrCatN(catnT,NULL,5);
	 printf("catnT:%s\n",catnT);*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCatN End---------------------------------------------------------\n");
	printf("-------------------------xsSnprintf Start----------------------------------------------\n");
	////////////////////xsSnprintf///////////////////
	char test[10];
	int sn = xsSnprintf(test,8,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test:%s\n",test);
	char *test1 = (char*)malloc(9);
	sn = xsSnprintf(test1,8,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test,12,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test:%s\n",test);
	sn = xsSnprintf(test1,11,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test1,-1,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test1,0,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsSnprintf End---------------------------------------------------------\n");
}

static void TestANSITwo()
{
	printf("-------------------------xsStrCmp Start---------------------------------------------------------\n");
	/////////xsStrCmp/////////////////////////////////////////////////////////

	//相等
	char * cmp1 = "";
	char * cmp2 = "";
	int i = xsStrCmp(cmp1,cmp2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	//cmp1 > cmp2
	cmp1 = "b";
	cmp2 = "a";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	cmp1 = "b";
	cmp2 = "B";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	//cmp1 < cmp2
	cmp1 = "a";
	cmp2 = "b";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	cmp1 = "B";
	cmp2 = "b";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	//当参数为NULL时出错
	/*  cmp1 = NULL;  //或者cpm2为NULL或者两个参数都为NULL
	 cmp2 ="das";
	 i = xsStrCmp(cmp1,cmp2);
	 printf("i:%d\n",i);
	 XS_TEST(i == -1);*/
	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCmp End---------------------------------------------------------\n");
	printf("-------------------------xsStrCaseCmp Start---------------------------------------------------------\n");
	/////xsStrCaseCmp////////////////////////////////////////////////////////////////

	//相等
	char * casecmp1 = "";
	char * casecmp2 = "";
	i = xsStrCaseCmp(casecmp1,casecmp2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	casecmp1 = "b";
	casecmp2 = "B";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 0);

	// casecmp1 >  casecmp2
	casecmp1 = "b";
	casecmp2 = "a";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	// casecmp1 <  casecmp2
	casecmp1 = "a";
	casecmp2 = "b";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	//当参数为NULL时
	casecmp1 = NULL;
	casecmp2 ="das";
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);
	casecmp2 = NULL;
	casecmp1 ="das";
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);
	casecmp2 = NULL;
	casecmp1 =NULL;
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);

	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCaseCmp End---------------------------------------------------------\n");
	printf("-------------------------xsStrCmpN Start---------------------------------------------------------\n");
	/////////xsStrCmpN//////////////////////////////////////////////////////////////

	//返回的结果都是0，因为无论取多少都是相等的
	char *cmpn1 = "";
	char *cmpn2 = "";
	i = xsStrCmpN(cmpn1,cmpn2,-1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	//当max的值不为0时，返回结果都是-15(d的ASCII值减去s的ASCII值)，因为比较的只是字符串的首字母，返回值就是这两个首字母的ASCII值之间的差值。当取0时，就是什么都不取，所以返回值就是0
	cmpn1 = "dsds";
	cmpn2 = "sdds";
	i = xsStrCmpN(cmpn1,cmpn2,-1);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == -15);
	printf("i:%d\n",i);

	//当max的值不为0时，返回结果都是15(s的ASCII值减去d的ASCII值)，因为比较的只是字符串的首字母，返回值就是这两个首字母的ASCII值之间的差值。当取0时，就是什么都不取，所以返回值就是0
	cmpn1 = "ssds";
	cmpn2 = "ddds";
	i = xsStrCmpN(cmpn1,cmpn2,-4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == 15);
	printf("i:%d\n",i);

	cmpn1 = "ssds";
	cmpn2 = "dds";
	i = xsStrCmpN(cmpn1,cmpn2,-4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	//当两个字符串的值有NULL时，max只有取0，也就是什么都不取，函数才能执行不出错。
	cmpn1 = NULL;
	cmpn2 = "dds";
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	cmpn1 = "dss";
	cmpn2 = NULL;
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	cmpn1 = NULL;
	cmpn2 = NULL;
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	//当两个字符串的值有NULL时，max不取0，函数执行就会出错。
	/* cmpn1 = NULL;
	 cmpn2 = "dds";
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);
	 cmpn1 = "dss";
	 cmpn2 = NULL;
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);
	 cmpn1 = NULL;
	 cmpn2 = NULL;
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);*/
	////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCaseCmpN Start---------------------------------------------------------\n");
	///////xsStrCaseCmpN/////////////////////////////////////////////////////////////////////////
	char * casecmpn1 = "";
	char *casecmpn2 ="";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	casecmpn1 = "absf";
	casecmpn2 = "ABSD";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,4);
	XS_TEST(i == 2);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,6);
	XS_TEST(i == 2);
	printf("i:%d\n",i);

	casecmpn1 = "abs";
	casecmpn2 = "ABSp";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,3);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,4);
	XS_TEST(i < 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,6);
	XS_TEST(i < 0);
	printf("i:%d\n",i);

	casecmpn1 = NULL;
	casecmpn2 ="sdsa";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);

	casecmpn1 = "sdsa";
	casecmpn2 = NULL;
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);

	casecmpn1 = NULL;
	casecmpn2 = NULL;
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCaseCmpN End---------------------------------------------------------\n");
}

static void TestANSIThree()
{
	printf("-------------------------xsStrChr Start---------------------------------------------------------\n");
	//////////////////////////////////////xsStrChr//////////////////////////////////////////////////////
	char *chr = "assdsd";
	char * resultC = xsStrChr(chr,'s');
	printf("resultC:%s\n",resultC);
	XS_TEST(xsStrLen(resultC) == 5);
	resultC = xsStrChr(chr,'f');
	printf("resultC:%s\n",resultC);
	XS_TEST(resultC == 0);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrChr End---------------------------------------------------------\n");
	printf("-------------------------xsStrStr Start---------------------------------------------------------\n");
	/////////////////////xsStrStr///////////////////////////////////////////////////////////

	char *str = "sfsfsfsfsa";
	char *resultS = xsStrStr(str,"sfsf");
	XS_TEST(xsStrLen(resultS) == 10);
	printf("resutS:%s\n",resultS);
	resultS = xsStrStr(str,"dfsf");
	XS_TEST(resultS == NULL);
	printf("resutS:%s\n",resultS);

	//substring不能是NULL，否则出错
	/* resultS = xsStrStr(str,NULL);
	 printf("resultS:%s\n",resultS);
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrStr End---------------------------------------------------------\n");
	printf("-------------------------xsStrToInt Start---------------------------------------------------------\n");
	//////////xsStrToInt////////////////////////////////////////////////////////////////////////

	int integer = xsStrToInt("3234242");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3234242);
	//超过了最大整数值,得不到正确的值
	integer = xsStrToInt("2147483649");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 2147483649);
	//串中有字母与数字  返回值只是数字
	integer = xsStrToInt("3232jjj");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3232);
	//串中有只有字母  返回值只是0
	integer = xsStrToInt("dsfsjjj");
	XS_TEST(integer == 0);
	printf("integer:%d\n",integer);
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrToInt End---------------------------------------------------------\n");
	printf("-------------------------xsIntToStr Start---------------------------------------------------------\n");
	////////xsIntToStr///////////////////////////////////////////////////////////////////////////
	char *tostr = (char *)malloc(11);
	char *resultI = xsIntToStr(22323,tostr,10);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	XS_TEST(resultI);
	free(tostr);
	tostr = (char *)malloc(11);
	resultI = xsIntToStr(-22323,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	//buffer 不能低于11bytes 否则出错
	/* char *tostr1 = (char*)malloc(5);
	 resultI = xsIntToStr(22323,tostr1,10);
	 printf("tostr1:%s\nresultI:%s\n",tostr1,resultI);
	 free(tostr1);*/
	//int 不能超过最大的整型值
	tostr = (char *)malloc(20);
	resultI = xsIntToStr(2147483649,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(11);
	resultI = xsIntToStr(2147483647,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(32);
	resultI = xsIntToStr(2147483647,tostr,2);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(12);
	resultI = xsIntToStr(-2147483648,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(12);
	resultI = xsIntToStr(2147483647,tostr,8);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsIntToStr End---------------------------------------------------------\n");
	printf("-------------------------xsStrLwr Start---------------------------------------------------------\n");
	/////////////////xsStrLwr//////////////////////////////////////////////////////////////////////////
	char * strlwr = (char*)malloc(sizeof(10));
	xsStrCpy(strlwr,"sfsfsSSSSSsd");
	char * resultL =xsStrLwr(strlwr);
	XS_TEST(resultL);
	printf("strlwr:%s\n",strlwr);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrLwr End---------------------------------------------------------\n");
	printf("-------------------------xsStrUpr Start---------------------------------------------------------\n");
	/////////xsStrUpr////////////////////////////////////////////////////////////////////////////////////////
	char *strupr = (char*)malloc(sizeof(char)*6);
	xsStrCpy(strupr,"sdsfsfAA");
	char * resultU = xsStrUpr(strupr);
	XS_TEST(resultU);
	printf("strupr:%s\n",strupr);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrUpr End------------------------------------------------\n");
}

static void TestWideCharOne()
{
	printf("-------------------------xsWcsLen Start---------------------------------------------------------\n");
	///xsWcsLen//////////////////////////////////////////////////////////////////
	xsWChar wcsLen[5] =
	{	's','d','d','d',0};
	xsWChar *wcsLen1 = xsT("vccvccx");
	size_t t = xsWcsLen(wcsLen);
	size_t st = xsWcsLen(wcsLen1);
	int ii = (int)t;
	int iii = (int)st;
	XS_TEST(ii == 4);
	printf("ii:%d\niii:%d\n",ii,iii);

	///////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsLen End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCpy Start---------------------------------------------------------\n");
	////////////////xsWcsCpy/////////////////////////////////////////////////////////

	xsWChar * wcscpyF =xsT("dsdsdsd");
	xsWChar wcscpyT[8];
	xsWcsCpy(wcscpyT,wcscpyF);
	t = xsWcsLen(wcscpyT);
	XS_TEST(t == 7);
	xsWcsCpy(wcscpyT,xsT(""));
	t = xsWcsLen(wcscpyT);
	XS_TEST(t == 0);

	//wcspyF不能为NULL,否则出错
	/* wcscpyF = NULL;
	 xsWcsCpy(wcscpyT,wcscpyF);*/
	//wcscpyT1必须是数组（也就是必须有存储空间）否则出错
	/* xsWChar *wcscpyT1;
	 xsWcsCpy(wcscpyT1,wcscpyF);*/
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCpy End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCpyN Start---------------------------------------------------------\n");
	///////xsWcsCpyN////////////////////////////////////

	xsWChar *wcscpynT = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWChar *wcscpynF = xsT("d");
	//max不能为负数
	/* xsWcsCpyN(wcscpynT,wcscpynF,-1);
	 printf("wcscpynT:%s\n",wcscpynT);*/
	/*  xsWcsCpyN(wcscpynT,wcscpynF,0);
	 XS_TEST(xsWcsLen(wcscpynT) > 9);*/
	xsWcsCpyN(wcscpynT,wcscpynF,1);
	t = xsWcsLen(wcscpynT);
	int i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),2);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),0);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),4);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	free(wcscpynT);
	//wcscpynF不能为NULL
	// xsWcsCpyN(wcscpynT,NULL,9);
	///////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCpyN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCat Start---------------------------------------------------------\n");
	///////xsStrCat///////////////////////////////////////////////////////////////////////////////////////////
	char *catF = "dskk";
	char *temp = "dsd";
	char *catT = (char *)malloc(4);
	xsStrCpy(catT,temp);
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 7);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	//  free(catT);
	xsStrCpy(catT,"");
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 4);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	// free(catT);
	//catF不能为NULL
	/* xsStrCpy(catT,temp);
	 catF = NULL;
	 xsStrCat(catT,catF);*/
	xsStrCpy(catT,temp);
	catF ="";
	xsStrCat(catT,catF);
	printf("catT:%s\n",catT);
	XS_TEST(xsStrLen(catT) == 3);
	//////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCat End---------------------------------------------------------\n");
	printf("-------------------------xsStrCatN Start---------------------------------------------------------\n");
	///////////////xsStrCatN//////////////////////////////////////////////////////////////////////
	char *catnF = "djsd";
	char *catnT = (char*)malloc(4);
	temp = "sds";
	xsStrCpy(catnT,temp);
	xsStrCatN(catnT,catnF,-1);

	XS_TEST(xsStrLen(catnT) == 7);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,0);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,2);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,4);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	XS_TEST(xsStrLen(catnT) > 0);
	xsStrCatN(catnT,"",5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	//catF不能为NULL 否则出错
	/*xsStrCatN(catnT,NULL,5);
	 printf("catnT:%s\n",catnT);*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCatN End---------------------------------------------------------\n");
}

static void TestWideCharTwo()
{
	printf("-------------------------xsWcsCmp Start---------------------------------------------------------\n");
	////////xsWcsCmp/////////////////////////////////////////////////////////////////
	//比较的字符串不能为""或NULL
	/*xsWChar wcscmp1 = "";
	 xsWChar wcscmp2 = "";
	 i = xsWcsCmp(wcscmp1,wcscmp2);
	 printf("i:%d\n",i);
	 i = xsWcsCmp(NULL,NULL);*/
	int i = xsWcsCmp(xsT("dsds"),xsT("dsds"));
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsWcsCmp(xsT("d"),xsT("D"));
	XS_TEST(i == 1);
	printf("i:%d\n",i);
	i = xsWcsCmp(xsT("D"),xsT("d"));
	XS_TEST(i == -1);
	printf("i:%d\n",i);
	///////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCmp End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCaseCmp Start---------------------------------------------------------\n");
	/////////////////xsWcsCaseCmp/////////////////////////////
	i = xsWcsCaseCmp(xsT(""),xsT(""));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT(""),xsT("a"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(NULL,NULL);
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(NULL,xsT("fdgfdg"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("A"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("BBBB"), xsT("bbbb"));
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("B"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("B"),xsT("a"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("a"));
	printf("i:%d\n",i);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCaseCmp End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCmpN Start---------------------------------------------------------\n");
	//////////////xsWcsCmpN//////////////////////////////////////////////////////////
	i = xsWcsCmpN(xsT(""),xsT(""),3);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT(""),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("a"),xsT(""),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsf"),xsT("adsfs"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("a"),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsf"),xsT("adsfs"),0);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),-1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),5);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),6);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("addsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("addsd"),xsT("adfdsa"),2);
	printf("i:%d\n",i);
	//参数不能为NULL
	/*  i = xsWcsCmpN(NULL,NULL,3);
	 printf("i:%d\n",i);*/
	/////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCaseCmpN Start---------------------------------------------------------\n");
	///////////////////xsWcsCaseCmpN///////////////////////
	i = xsWcsCaseCmpN(xsT(""),xsT(""),3);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT(""),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("a"),xsT(""),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsf"),xsT("adsfs"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("d"),xsT("D"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsf"),xsT("adsfs"),0);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),-1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("DFdsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),5);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),6);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("fddsd"),3);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("addsd"),xsT("dfdsa"),3);
	printf("i:%d\n",i);
	///////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCaseCmpN End---------------------------------------------------------\n");
}

static void TestWideCharThree()
{
	printf("-------------------------xsWcsChr Start---------------------------------------------------------\n");
	/////////////////////xsWcsChr///////////////////////////////////////////////
	xsWChar * resultChr =xsWcsChr(xsT("fsfsfsf"),'d');
	XS_TEST(resultChr == NULL);
	resultChr =xsWcsChr(xsT("fsfsfsf"),'f');
	XS_TEST(xsWcsLen(resultChr) == 7);
	resultChr =xsWcsChr(xsT("fsfsfsf"),'s');
	XS_TEST(xsWcsLen(resultChr) == 6);
	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsChr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsStr Start---------------------------------------------------------\n");
	/////////////////////////xsWcsStr//////////////////////////////////////////
	xsWChar * resultStr =xsWcsStr(xsT("fsfsfsf"),xsT("dfsfls"));
	XS_TEST(resultStr == NULL);
	resultStr =xsWcsStr(xsT("fsfsfsf"),xsT("fs"));
	XS_TEST(xsWcsLen(resultStr) == 7);
	resultStr =xsWcsStr(xsT("fsfdsdde"),xsT("ds"));
	XS_TEST(xsWcsLen(resultStr) == 5);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsStr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToInt Start---------------------------------------------------------\n");
	/////////////////////////////////xsWcsToInt/////////////////////////////////////////////////
	int integer = xsWcsToInt(xsT("3234242"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3234242);
	//超过了最大整数值,得不到正确的值
	integer = xsWcsToInt(xsT("2147483649"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 2147483649);
	//串中有字母与数字  返回值只是数字
	integer = xsWcsToInt(xsT("3232jjj"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3232);
	//串中有只有字母  返回值只是0
	integer = xsWcsToInt(xsT("dsfsjjj"));
	XS_TEST(integer == 0);
	printf("integer:%d\n",integer);
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToInt End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToInt Start---------------------------------------------------------\n");
	///////////////////////xsWcsLwr//////////////////////////////
	xsWChar * wcslwr = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcslwr,xsT("SfsfsSSSSSsd"));
	xsWChar * resultLwr =xsWcsLwr(wcslwr);
	XS_TEST(resultLwr);
	/////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToInt End---------------------------------------------------------\n");
	printf("-------------------------xsWcsUpr Start---------------------------------------------------------\n");
	/////////////////xsWcsUpr/////////////////////////////
	xsWChar * wcsupr = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcsupr,xsT("sdsdsaKKKJLdff"));
	xsWChar * resultUpr=xsWcsUpr(wcsupr);
	XS_TEST(resultUpr);
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsUpr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToMbs Start---------------------------------------------------------\n");
	////////////xsWcsToMbs/////////////////////////////////////////////
	char * src = (char*)malloc(sizeof(char)*10);
	size_t t = xsWcsToMbs(src,xsT("dsdsfdsfds"),4);
	int i = xsStrLen(src);
	printf("src:%s\nt:%d\ni:%d\n",src,t,i);
	///////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToMbs End---------------------------------------------------------\n");
	printf("-------------------------xsMbsToWcs Start---------------------------------------------------------\n");
	////////////////////////xsMbsToWcs////////////////////
	xsWChar * wcs = (xsWChar*)malloc(sizeof(xsWChar)*10);
	t = xsMbsToWcs(wcs,"sdfsdfsf",5);
	size_t count = xsWcsLen(wcs);
	printf("t:%d\ncount:%d\n",t,count);
}

static void TestString()
{
	printf("-------------------------xsStrLen Start---------------------------------------------------------\n");
	///////xsStrLen
	char *len ="abcd";
	XS_TEST(xsStrLen(len) == 4);
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrLen End---------------------------------------------------------\n");
	printf("-------------------------xsStrCpy Start---------------------------------------------------------\n");
	///////xsStrCpy//////////////////////////////////////////////////////////

	//可以free的
	char *cpyTo =(char *)malloc(sizeof(char)*5);
	char *cpyF = "aaaa";
	char *cpyR = xsStrCpy(cpyTo,cpyF);
	printf("cpyTo:%s\ncpyR:%s\n",cpyTo,cpyR);
	XS_TEST(xsStrLen(cpyTo) == 4);
	free(cpyTo);
	//cpyF不能为NULL
	/*cpyTo = (char *)malloc(5);
	 cpyF = NULL;
	 cpyR =  xsStrCpy(cpyTo,cpyF);*/
	cpyTo = (char *)malloc(5);
	cpyF = "";
	cpyR = xsStrCpy(cpyTo,cpyF);
	free(cpyTo);
	//不可以free的 越界了也能copy 因为有越界检查
	cpyTo = (char *)malloc(5);
	cpyF = "BBBBBB";
	cpyR = xsStrCpy(cpyTo,cpyF);
	XS_TEST(xsStrLen(cpyTo) == 6);
	printf("cpyTo1:%s\ncpyR1:%s\n",cpyTo,cpyR);
	//free(cpyTo); //出错,越界了无法释放内存
	////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCpy End---------------------------------------------------------\n");
	printf("-------------------------xsStrCpyN Start---------------------------------------------------------\n");
	///////xsStrCpyN/////////////////////////////////////////////////////////
	char * cpynT = (char*)malloc(5);
	char * cpynF = "dddddddd";
	char * cpynR = xsStrCpyN(cpynT,cpynF,4);
	XS_TEST(xsStrLen(cpynT) > 4);
	//由于cpynT没有以'\0'结尾 所以输出的是乱码 print输出一直到'\0'为止
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);

	//由于申请的空间小于max（即5<6）所以这行代码会出错
	//cpynR = xsStrCpyN(cpynT,cpynF,6);
	//max值不可以为负数，否则出错。
	//cpynR = xsStrCpyN(cpynT,cpynF,-1);
	free(cpynT);
	//全部拷贝（包含'\0'一起拷贝，否则一样有乱码）就没有乱码了
	cpynT = (char*)malloc(9);
	cpynR = xsStrCpyN(cpynT,cpynF,9);
	XS_TEST(xsStrLen(cpynT) == 8);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	free(cpynT);
	cpynT = (char*)malloc(10);
	cpynR = xsStrCpyN(cpynT,cpynF,10);
	XS_TEST(xsStrLen(cpynT) == 8);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	free(cpynT);
	cpynT = (char*)malloc(10);
	cpynR = xsStrCpyN(cpynT,cpynF,0);
	XS_TEST(xsStrLen(cpynT) > 0);
	printf("cpynT:%s\ncpynR:%s\n",cpynT,cpynR);
	//////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCpyN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCmp Start---------------------------------------------------------\n");
	/////////xsStrCmp/////////////////////////////////////////////////////////

	//相等
	char * cmp1 = "";
	char * cmp2 = "";
	int i = xsStrCmp(cmp1,cmp2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	//cmp1 > cmp2
	cmp1 = "b";
	cmp2 = "a";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	cmp1 = "b";
	cmp2 = "B";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	//cmp1 < cmp2
	cmp1 = "a";
	cmp2 = "b";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	cmp1 = "B";
	cmp2 = "b";
	i = xsStrCmp(cmp1,cmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	//当参数为NULL时出错
	/*  cmp1 = NULL;  //或者cpm2为NULL或者两个参数都为NULL
	 cmp2 ="das";
	 i = xsStrCmp(cmp1,cmp2);
	 printf("i:%d\n",i);
	 XS_TEST(i == -1);*/
	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCmp End---------------------------------------------------------\n");
	printf("-------------------------xsStrCaseCmp Start---------------------------------------------------------\n");
	/////xsStrCaseCmp////////////////////////////////////////////////////////////////

	//相等
	char * casecmp1 = "";
	char * casecmp2 = "";
	i = xsStrCaseCmp(casecmp1,casecmp2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	casecmp1 = "b";
	casecmp2 = "B";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 0);

	// casecmp1 >  casecmp2
	casecmp1 = "b";
	casecmp2 = "a";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == 1);

	// casecmp1 <  casecmp2
	casecmp1 = "a";
	casecmp2 = "b";
	i = xsStrCaseCmp( casecmp1, casecmp2);
	printf("i:%d\n",i);
	XS_TEST(i == -1);

	//当参数为NULL时
	casecmp1 = NULL;
	casecmp2 ="das";
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);
	casecmp2 = NULL;
	casecmp1 ="das";
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);
	casecmp2 = NULL;
	casecmp1 =NULL;
	i = xsStrCaseCmp( casecmp1, casecmp2);// 结果为2147483647（即为2的31次方减1 也就是最大的长整型）
	printf("i:%d\n",i);
	XS_TEST(i == 2147483647);

	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCaseCmp End---------------------------------------------------------\n");
	printf("-------------------------xsStrCmpN Start---------------------------------------------------------\n");
	/////////xsStrCmpN//////////////////////////////////////////////////////////////

	//返回的结果都是0，因为无论取多少都是相等的
	char *cmpn1 = "";
	char *cmpn2 = "";
	i = xsStrCmpN(cmpn1,cmpn2,-1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,2);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	//当max的值不为0时，返回结果都是-15(d的ASCII值减去s的ASCII值)，因为比较的只是字符串的首字母，返回值就是这两个首字母的ASCII值之间的差值。当取0时，就是什么都不取，所以返回值就是0
	cmpn1 = "dsds";
	cmpn2 = "sdds";
	i = xsStrCmpN(cmpn1,cmpn2,-1);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == -15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == -15);
	printf("i:%d\n",i);

	//当max的值不为0时，返回结果都是15(s的ASCII值减去d的ASCII值)，因为比较的只是字符串的首字母，返回值就是这两个首字母的ASCII值之间的差值。当取0时，就是什么都不取，所以返回值就是0
	cmpn1 = "ssds";
	cmpn2 = "ddds";
	i = xsStrCmpN(cmpn1,cmpn2,-4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == 15);
	printf("i:%d\n",i);

	cmpn1 = "ssds";
	cmpn2 = "dds";
	i = xsStrCmpN(cmpn1,cmpn2,-4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,1);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,3);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,4);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	i = xsStrCmpN(cmpn1,cmpn2,5);
	XS_TEST(i == 15);
	printf("i:%d\n",i);
	//当两个字符串的值有NULL时，max只有取0，也就是什么都不取，函数才能执行不出错。
	cmpn1 = NULL;
	cmpn2 = "dds";
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	cmpn1 = "dss";
	cmpn2 = NULL;
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	cmpn1 = NULL;
	cmpn2 = NULL;
	i = xsStrCmpN(cmpn1,cmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	//当两个字符串的值有NULL时，max不取0，函数执行就会出错。
	/* cmpn1 = NULL;
	 cmpn2 = "dds";
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);
	 cmpn1 = "dss";
	 cmpn2 = NULL;
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);
	 cmpn1 = NULL;
	 cmpn2 = NULL;
	 i = xsStrCmpN(cmpn1,cmpn2,1);
	 printf("i:%d\n",i);*/
	////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCaseCmpN Start---------------------------------------------------------\n");
	///////xsStrCaseCmpN/////////////////////////////////////////////////////////////////////////
	char * casecmpn1 = "";
	char *casecmpn2 ="";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);

	casecmpn1 = "absf";
	casecmpn2 = "ABSD";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,4);
	XS_TEST(i == 2);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,6);
	XS_TEST(i == 2);
	printf("i:%d\n",i);

	casecmpn1 = "abs";
	casecmpn2 = "ABSp";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,3);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,4);
	XS_TEST(i < 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,6);
	XS_TEST(i < 0);
	printf("i:%d\n",i);

	casecmpn1 = NULL;
	casecmpn2 ="sdsa";
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);

	casecmpn1 = "sdsa";
	casecmpn2 = NULL;
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);

	casecmpn1 = NULL;
	casecmpn2 = NULL;
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,-1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,0);
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsStrCaseCmpN(casecmpn1,casecmpn2,1);
	XS_TEST(i == 2147483647);
	printf("i:%d\n",i);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCaseCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsStrCat Start---------------------------------------------------------\n");
	///////xsStrCat///////////////////////////////////////////////////////////////////////////////////////////
	char *catF = "dskk";
	char *temp = "dsd";
	char *catT = (char *)malloc(4);
	xsStrCpy(catT,temp);
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 7);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	//  free(catT);
	xsStrCpy(catT,"");
	xsStrCat(catT,catF);
	XS_TEST(xsStrLen(catT) == 4);
	printf("catT:%s\n",catT);
	//内存越界，free出错
	// free(catT);
	//catF不能为NULL
	/* xsStrCpy(catT,temp);
	 catF = NULL;
	 xsStrCat(catT,catF);*/
	xsStrCpy(catT,temp);
	catF ="";
	xsStrCat(catT,catF);
	printf("catT:%s\n",catT);
	XS_TEST(xsStrLen(catT) == 3);
	//////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCat End---------------------------------------------------------\n");
	printf("-------------------------xsStrCatN Start---------------------------------------------------------\n");
	///////////////xsStrCatN//////////////////////////////////////////////////////////////////////
	char *catnF = "djsd";
	char *catnT = (char*)malloc(4);
	temp = "sds";
	xsStrCpy(catnT,temp);
	xsStrCatN(catnT,catnF,-1);

	XS_TEST(xsStrLen(catnT) == 7);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,0);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,2);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,4);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	xsStrCatN(catnT,catnF,5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	XS_TEST(xsStrLen(catnT) > 0);
	xsStrCatN(catnT,"",5);
	XS_TEST(xsStrLen(catnT) > 0);
	printf("catnT:%s\n",catnT);
	//catF不能为NULL 否则出错
	/*xsStrCatN(catnT,NULL,5);
	 printf("catnT:%s\n",catnT);*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrCatN End---------------------------------------------------------\n");
	printf("-------------------------xsStrChr Start---------------------------------------------------------\n");
	//////////////////////////////////////xsStrChr//////////////////////////////////////////////////////
	char *chr = "assdsd";
	char * resultC = xsStrChr(chr,'s');
	printf("resultC:%s\n",resultC);
	XS_TEST(xsStrLen(resultC) == 5);
	resultC = xsStrChr(chr,'f');
	printf("resultC:%s\n",resultC);
	XS_TEST(resultC == 0);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrChr End---------------------------------------------------------\n");
	printf("-------------------------xsStrStr Start---------------------------------------------------------\n");
	/////////////////////xsStrStr///////////////////////////////////////////////////////////

	char *str = "sfsfsfsfsa";
	char *resultS = xsStrStr(str,"sfsf");
	XS_TEST(xsStrLen(resultS) == 10);
	printf("resutS:%s\n",resultS);
	resultS = xsStrStr(str,"dfsf");
	XS_TEST(resultS == NULL);
	printf("resutS:%s\n",resultS);

	//substring不能是NULL，否则出错
	/* resultS = xsStrStr(str,NULL);
	 printf("resultS:%s\n",resultS);
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrStr End---------------------------------------------------------\n");
	printf("-------------------------xsStrToInt Start---------------------------------------------------------\n");
	//////////xsStrToInt////////////////////////////////////////////////////////////////////////

	int integer = xsStrToInt("3234242");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3234242);
	//超过了最大整数值,得不到正确的值
	integer = xsStrToInt("2147483649");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 2147483649);
	//串中有字母与数字  返回值只是数字
	integer = xsStrToInt("3232jjj");
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3232);
	//串中有只有字母  返回值只是0
	integer = xsStrToInt("dsfsjjj");
	XS_TEST(integer == 0);
	printf("integer:%d\n",integer);
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrToInt End---------------------------------------------------------\n");
	printf("-------------------------xsIntToStr Start---------------------------------------------------------\n");
	////////xsIntToStr///////////////////////////////////////////////////////////////////////////
	char *tostr = (char *)malloc(11);
	char *resultI = xsIntToStr(22323,tostr,10);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	XS_TEST(resultI);
	free(tostr);
	tostr = (char *)malloc(11);
	resultI = xsIntToStr(-22323,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	//buffer 不能低于11bytes 否则出错
	/* char *tostr1 = (char*)malloc(5);
	 resultI = xsIntToStr(22323,tostr1,10);
	 printf("tostr1:%s\nresultI:%s\n",tostr1,resultI);
	 free(tostr1);*/
	//int 不能超过最大的整型值
	tostr = (char *)malloc(20);
	resultI = xsIntToStr(2147483649,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(11);
	resultI = xsIntToStr(2147483647,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(32);
	resultI = xsIntToStr(2147483647,tostr,2);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(12);
	resultI = xsIntToStr(-2147483648,tostr,10);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	tostr = (char *)malloc(12);
	resultI = xsIntToStr(2147483647,tostr,8);
	XS_TEST(resultI);
	printf("tostr:%s\nresultI:%s\n",tostr,resultI);
	free(tostr);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsIntToStr End---------------------------------------------------------\n");
	printf("-------------------------xsStrLwr Start---------------------------------------------------------\n");
	/////////////////xsStrLwr//////////////////////////////////////////////////////////////////////////
	char * strlwr = (char*)malloc(sizeof(10));
	xsStrCpy(strlwr,"sfsfsSSSSSsd");
	char * resultL =xsStrLwr(strlwr);
	XS_TEST(resultL);
	printf("strlwr:%s\n",strlwr);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrLwr End---------------------------------------------------------\n");
	printf("-------------------------xsStrUpr Start---------------------------------------------------------\n");
	/////////xsStrUpr////////////////////////////////////////////////////////////////////////////////////////
	char *strupr = (char*)malloc(sizeof(char)*6);
	xsStrCpy(strupr,"sdsfsfAA");
	char * resultU = xsStrUpr(strupr);
	XS_TEST(resultU);
	printf("strupr:%s\n",strupr);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsStrUpr End------------------------------------------------\n");
	printf("-------------------------xsSnprintf Start----------------------------------------------\n");
	////////////////////xsSnprintf///////////////////
	char test[10];
	int sn = xsSnprintf(test,8,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test:%s\n",test);
	char *test1 = (char*)malloc(9);
	sn = xsSnprintf(test1,8,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test,12,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test:%s\n",test);
	sn = xsSnprintf(test1,11,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test1,-1,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
	sn = xsSnprintf(test1,0,"dshdshdsdhsdjsajdsj");
	XS_TEST(sn);
	printf("sn:%d\n",sn);
	printf("test1:%s\n",test1);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsSnprintf End---------------------------------------------------------\n");
	printf("-------------------------xsWcsLen Start---------------------------------------------------------\n");
	///xsWcsLen//////////////////////////////////////////////////////////////////
	xsWChar wcsLen[5] =
	{	's','d','d','d',0};
	xsWChar *wcsLen1 = xsT("vccvccx");
	size_t t = xsWcsLen(wcsLen);
	size_t st = xsWcsLen(wcsLen1);
	int ii = (int)t;
	int iii = (int)st;
	XS_TEST(ii == 4);
	printf("ii:%d\niii:%d\n",ii,iii);

	///////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsLen End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCpy Start---------------------------------------------------------\n");
	////////////////xsWcsCpy/////////////////////////////////////////////////////////

	xsWChar * wcscpyF =xsT("dsdsdsd");
	xsWChar wcscpyT[8];
	xsWcsCpy(wcscpyT,wcscpyF);
	t = xsWcsLen(wcscpyT);
	XS_TEST(t == 7);
	xsWcsCpy(wcscpyT,xsT(""));
	t = xsWcsLen(wcscpyT);
	XS_TEST(t == 0);

	//wcspyF不能为NULL,否则出错
	/* wcscpyF = NULL;
	 xsWcsCpy(wcscpyT,wcscpyF);*/
	//wcscpyT1必须是数组（也就是必须有存储空间）否则出错
	/* xsWChar *wcscpyT1;
	 xsWcsCpy(wcscpyT1,wcscpyF);*/
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCpy End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCpyN Start---------------------------------------------------------\n");
	///////xsWcsCpyN////////////////////////////////////

	xsWChar *wcscpynT = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWChar *wcscpynF = xsT("d");
	//max不能为负数
	/* xsWcsCpyN(wcscpynT,wcscpynF,-1);
	 printf("wcscpynT:%s\n",wcscpynT);*/
	/*  xsWcsCpyN(wcscpynT,wcscpynF,0);
	 XS_TEST(xsWcsLen(wcscpynT) > 9);*/
	xsWcsCpyN(wcscpynT,wcscpynF,1);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),2);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),0);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	xsWcsCpyN(wcscpynT,xsT("fdfddfd"),4);
	t = xsWcsLen(wcscpynT);
	i = (int)xsWcsLen(wcscpynT);

	if(i == t)
	printf("%d\n",i);
	free(wcscpynT);
	//wcscpynF不能为NULL
	// xsWcsCpyN(wcscpynT,NULL,9);
	///////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCpyN End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCmp Start---------------------------------------------------------\n");
	////////xsWcsCmp/////////////////////////////////////////////////////////////////
	//比较的字符串不能为""或NULL
	/*xsWChar wcscmp1 = "";
	 xsWChar wcscmp2 = "";
	 i = xsWcsCmp(wcscmp1,wcscmp2);
	 printf("i:%d\n",i);
	 i = xsWcsCmp(NULL,NULL);*/
	i = xsWcsCmp(xsT("dsds"),xsT("dsds"));
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsWcsCmp(xsT("d"),xsT("D"));
	XS_TEST(i == 1);
	printf("i:%d\n",i);
	i = xsWcsCmp(xsT("D"),xsT("d"));
	XS_TEST(i == -1);
	printf("i:%d\n",i);
	///////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCmp End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCaseCmp Start---------------------------------------------------------\n");
	/////////////////xsWcsCaseCmp/////////////////////////////
	i = xsWcsCaseCmp(xsT(""),xsT(""));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT(""),xsT("a"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(NULL,NULL);
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(NULL,xsT("fdgfdg"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("A"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("BBBB"), xsT("bbbb"));
	XS_TEST(i == 0);
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("B"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("B"),xsT("a"));
	printf("i:%d\n",i);
	i = xsWcsCaseCmp(xsT("a"),xsT("a"));
	printf("i:%d\n",i);
///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCaseCmp End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCmpN Start---------------------------------------------------------\n");
	//////////////xsWcsCmpN//////////////////////////////////////////////////////////
	i = xsWcsCmpN(xsT(""),xsT(""),3);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT(""),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("a"),xsT(""),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsf"),xsT("adsfs"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("a"),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsf"),xsT("adsfs"),0);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),-1);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),5);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("afdsd"),6);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("dfdsa"),xsT("addsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCmpN(xsT("addsd"),xsT("adfdsa"),2);
	printf("i:%d\n",i);
	//参数不能为NULL
	/*  i = xsWcsCmpN(NULL,NULL,3);
	 printf("i:%d\n",i);*/
	/////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCaseCmpN Start---------------------------------------------------------\n");
	///////////////////xsWcsCaseCmpN///////////////////////
	i = xsWcsCaseCmpN(xsT(""),xsT(""),3);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT(""),xsT("a"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("a"),xsT(""),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsf"),xsT("adsfs"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("d"),xsT("D"),1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsf"),xsT("adsfs"),0);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),-1);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("DFdsd"),2);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),5);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("afdsd"),6);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("dfdsa"),xsT("fddsd"),3);
	printf("i:%d\n",i);
	i = xsWcsCaseCmpN(xsT("addsd"),xsT("dfdsa"),3);
	printf("i:%d\n",i);
	///////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCaseCmpN End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCat Start---------------------------------------------------------\n");
	//////////////////////////////xsWcsCat/////////////////////////////////////////////
	xsWChar *wcscatT = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcscatT,xsT("fdJJ"));
	xsWcsCat(wcscatT,xsT("sdsfsff"));
	t = xsWcsLen(wcscatT);
	XS_TEST(t == 11);
	xsWcsCpy(wcscatT,xsT(""));
	xsWcsCat(wcscatT,xsT("sdsfsff"));
	t = xsWcsLen(wcscatT);
	XS_TEST(t == 7);
	//////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCat End---------------------------------------------------------\n");
	printf("-------------------------xsWcsCatN Start---------------------------------------------------------\n");
	//////////////////////////////xsWcsCatN/////////////////////////////////////////////////////////
	xsWChar *wcscatnT = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcscatnT,xsT("fdJJ"));
	xsWcsCatN(wcscatnT,xsT("sdsfsff"),-1);
	t = xsWcsLen(wcscatT);
	printf("%d\n",(int)t);
	XS_TEST(t > 4);
	xsWcsCpy(wcscatT,xsT(""));
	xsWcsCatN(wcscatT,xsT("sdsfsff"),0);
	t = xsWcsLen(wcscatT);
	XS_TEST(t == 0);
	xsWcsCatN(wcscatT,xsT("sdsfsff"),3);
	t = xsWcsLen(wcscatT);
	XS_TEST(t == 3);
	xsWcsCpy(wcscatT,xsT(""));
	xsWcsCatN(wcscatT,xsT("sdsfsff"),8);
	t = xsWcsLen(wcscatT);
	XS_TEST(t == 7);
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsCatN End---------------------------------------------------------\n");
	printf("-------------------------xsWcsChr Start---------------------------------------------------------\n");
	/////////////////////xsWcsChr///////////////////////////////////////////////
	xsWChar * resultChr =xsWcsChr(xsT("fsfsfsf"),'d');
	XS_TEST(resultChr == NULL);
	resultChr =xsWcsChr(xsT("fsfsfsf"),'f');
	XS_TEST(xsWcsLen(resultChr) == 7);
	resultChr =xsWcsChr(xsT("fsfsfsf"),'s');
	XS_TEST(xsWcsLen(resultChr) == 6);
	////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsChr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsStr Start---------------------------------------------------------\n");
	/////////////////////////xsWcsStr//////////////////////////////////////////
	xsWChar * resultStr =xsWcsStr(xsT("fsfsfsf"),xsT("dfsfls"));
	XS_TEST(resultStr == NULL);
	resultStr =xsWcsStr(xsT("fsfsfsf"),xsT("fs"));
	XS_TEST(xsWcsLen(resultStr) == 7);
	resultStr =xsWcsStr(xsT("fsfdsdde"),xsT("ds"));
	XS_TEST(xsWcsLen(resultStr) == 5);
	/////////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsStr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToInt Start---------------------------------------------------------\n");
	/////////////////////////////////xsWcsToInt/////////////////////////////////////////////////
	integer = xsWcsToInt(xsT("3234242"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3234242);
	//超过了最大整数值,得不到正确的值
	integer = xsWcsToInt(xsT("2147483649"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 2147483649);
	//串中有字母与数字  返回值只是数字
	integer = xsWcsToInt(xsT("3232jjj"));
	printf("integer:%d\n",integer);
	XS_TEST(integer == 3232);
	//串中有只有字母  返回值只是0
	integer = xsWcsToInt(xsT("dsfsjjj"));
	XS_TEST(integer == 0);
	printf("integer:%d\n",integer);
	///////////////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToInt End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToInt Start---------------------------------------------------------\n");
	///////////////////////xsWcsLwr//////////////////////////////
	xsWChar * wcslwr = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcslwr,xsT("SfsfsSSSSSsd"));
	xsWChar * resultLwr =xsWcsLwr(wcslwr);
	XS_TEST(resultLwr);
	/////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToInt End---------------------------------------------------------\n");
	printf("-------------------------xsWcsUpr Start---------------------------------------------------------\n");
	/////////////////xsWcsUpr/////////////////////////////
	xsWChar * wcsupr = (xsWChar*)malloc(sizeof(xsWChar)*10);
	xsWcsCpy(wcsupr,xsT("sdsdsaKKKJLdff"));
	xsWChar * resultUpr=xsWcsUpr(wcsupr);
	XS_TEST(resultUpr);
	/////////////////////////////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsUpr End---------------------------------------------------------\n");
	printf("-------------------------xsWcsToMbs Start---------------------------------------------------------\n");
	////////////xsWcsToMbs/////////////////////////////////////////////
	char * src = (char*)malloc(sizeof(char)*10);
	t = xsWcsToMbs(src,xsT("dsdsfdsfds"),4);
	i = xsStrLen(src);
	printf("src:%s\nt:%d\ni:%d\n",src,t,i);
	///////////////////////////////////////////////////////////////
	printf("-------------------------xsWcsToMbs End---------------------------------------------------------\n");
	printf("-------------------------xsMbsToWcs Start---------------------------------------------------------\n");
	////////////////////////xsMbsToWcs////////////////////
	xsWChar * wcs = (xsWChar*)malloc(sizeof(xsWChar)*10);
	t = xsMbsToWcs(wcs,"sdfsdfsf",5);
	size_t count = xsWcsLen(wcs);
	printf("t:%d\ncount:%d\n",t,count);
}
#endif
