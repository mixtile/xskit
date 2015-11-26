#if 0
static void TestMemCpy()
{
	char dst[6] =
	{	0, 1, 2, 3, 4, 5};
	const char *src = "abcdef";

	xsMemSet(dst, 1, 5);
	XS_TEST(dst[0] == 1 && dst[4] == 1 && dst[5] == 5);

	xsMemMove(dst, src, 5);
	XS_TEST(dst[0] == src[0] && dst[4] == src[4] && dst[5] == 5 );

	xsMemCpy(dst, src, 5);
	XS_TEST(dst[0] == src[0] && dst[4] == src[4] && dst[5] == 5 );

	XS_TEST(xsMemCmp(&dst[0], &src[0], 1) == 0 && xsMemCmp(&dst[4], &src[4], 1) == 0 && xsMemCmp(&dst[5], &src[5], 1) != 0);

	char *dst1;
	dst1 = xsMallocNative(6);
	XS_TEST ( dst1[0] != 0 && dst1[5] != 0 );
	dst1 = xsCallocNative(6);
	XS_TEST ( dst1[0] == 0 && dst1[5] == 0 );
	xsFreeNative(dst1);
	XS_TEST ( dst1[0] != 0 );
	dst1 = xsCallocNative(6);
	xsReAllocNative(dst1,10);
	XS_TEST ( dst1[0] == 0 && dst1[5] == 0 && dst1[9] != 0);
}
#endif
