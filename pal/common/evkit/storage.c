#if 0
static void TestCreateFile()
{
	xsFile file = xsOpenFile(xsT("evkit_test.txt"), XS_OF_CREATE);
	XS_TEST(xsFileHandleValid(file));

	xsCloseFile(file);
}
#endif
