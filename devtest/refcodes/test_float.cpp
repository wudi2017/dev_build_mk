void testfloat()
{
	float x1 = 10000.11f;
	int y1 = 3;
	printf("test1 %f\n", x1*y1);

	float x2 = 10000.11f;
	double x2d = (double)x2;
	int y2 = 3;
	printf("test2 %f\n", x2d*y2);

	double x3 = 10000.11;
	int y3 = 3;
	printf("test3 %f\n", x3*y3);
}