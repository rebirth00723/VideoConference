#include<xml.h>

void str2xml(char* xml, char* tableName, char* col[], char* data[], int n)
{//col=欄位 data=欄位資料 n=欄位數

	sprintf(xml, "<%s ", tableName);
	for (int i = 0; i < n; i++)
	{
		sprintf(xml + strlen(xml), "<%s>\"%s\"</%s>\n", col[i], data[i], col[i]);
	}
	sprintf(xml + strlen(xml), "</%s>\n", tableName);

	//return xml;
}
void ex_xml()
{
	char *xml = (char*)malloc(1000 * sizeof(char));

	char* table = "jerry";
	char* col[2] = { "gender", "bd" };
	char* data[2] = { "male", "619" };
	str2xml(xml, table, col, data, 2);
	printf("%s", xml);
	free(xml);
}