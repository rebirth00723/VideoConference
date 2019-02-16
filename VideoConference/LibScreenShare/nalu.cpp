#include "nalu_defines.h"
NALU_t *allocNalu(int maxSize)
{
	NALU_t *u = (NALU_t*)calloc(1, sizeof(NALU_t));
	u->buf = (byte*)calloc(maxSize, sizeof(byte));
	u->max_size = maxSize;
	if (!u || !u->buf)
	{
		puts("Allocated nalu structure has failed.");
		return NULL;
	}
	else
		return u;
}
static void dumpNalu(NALU_t *u)
{
	static FILE *trace = NULL;
	char showNaluUnitTypeString[30], showNalRefIdcString[30] = { 0 };
	if (!trace)
		trace = fopen("trace.txt", "w");
	switch (u->nal_unit_type)
	{
	case NALU_TYPE_SLICE:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_SLICE");
		break;
	case NALU_TYPE_DPA:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_DPA");
		break;
	case NALU_TYPE_DPB:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_DPB");
		break;
	case NALU_TYPE_DPC:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_DPC");
		break;
	case NALU_TYPE_IDR:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_IDR");
		break;
	case NALU_TYPE_SEI:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_SEI");
		break;
	case NALU_TYPE_SPS:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_SPS");
		break;
	case NALU_TYPE_PPS:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_PPS");
		break;
	case NALU_TYPE_AUD:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_AUD");
		break;
	case NALU_TYPE_EOSEQ:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_EOSEQ");
		break;
	case NALU_TYPE_EOSTREAM:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_EOSTREAM");
		break;
	case NALU_TYPE_FILL:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_FILL");
		break;
	case NALU_TYPE_PREFIX:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_PREFIX");
		break;
	case NALU_TYPE_SUB_SPS:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_SUB_SPS");
		break;
	case NALU_TYPE_SLC_EXT:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_SLC_EXT");
		break;
	case NALU_TYPE_VDRD:
		strcpy(showNaluUnitTypeString, "NALU_TYPE_VDRD");
		break;
	}
	switch (u->nal_reference_idc)
	{
	case NALU_PRIORITY_HIGHEST:
		strcpy(showNalRefIdcString, "NALU_PRIORITY_HIGHEST");
		break;
	case NALU_PRIORITY_HIGH:
		strcpy(showNalRefIdcString, "NALU_PRIORITY_HIGH");
		break;
	case NALU_PRIORITY_LOW:
		strcpy(showNalRefIdcString, "NALU_PRIORITY_LOW");
		break;
	case NALU_PRIORITY_DISPOSABLE:
		strcpy(showNalRefIdcString, "NALU_PRIORITY_DISPOSABLE");
		break;
	}
	puts("-----------A NALU Header-----------\n");
	printf("nal_ref_idc:%s(%d)\n", showNalRefIdcString, u->nal_reference_idc);
	printf("nal_unit_type:%s(%d)\n", showNaluUnitTypeString, u->nal_unit_type);
	printf("nalu length:%d(byte)\n\n", u->len);

	fputs("-----------A NALU Header-----------\n", trace);
	fprintf(trace, "nal_ref_idc:%s(%d)\n", showNalRefIdcString, u->nal_reference_idc);
	fprintf(trace, "nal_unit_type:%s(%d)\n", showNaluUnitTypeString, u->nal_unit_type);
	fprintf(trace, "nalu length:%d(byte)\n\n", u->len);
}
static int FindNaluStartCode3Bytes(const char *buf)
{
	if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x01)
		return 1;
	return 0;
}
static int FindNaluStartCode4Bytes(const char *buf)
{
	if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x01)
		return 1;
	return 0;
}
int addstartcode_prefix(char *buf ,int len)
{
	switch (len)
	{
	case 3:
		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x01;
		return 3;
	case 4:
		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x00;
		buf[3] = 0x01;
		return 4;
	default:
		printf("Bad start code length.\n");
		return -1;
	}
}
int addstartcode_prefix2(char *buf, NaluType nalutype)
{
	switch (nalutype)
	{
	case NALU_TYPE_SEI:
	case NALU_TYPE_IDR:
		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x01;
		return 3;
	case NALU_TYPE_SLICE:
	case NALU_TYPE_SPS:
	case NALU_TYPE_PPS:
		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x00;
		buf[3] = 0x01;
		return 4;
	default:
		return 0;
	}
}
int GetNextNalu(FILE *file, NALU_t *u)
{
	int pos, rewind = 0;
	static byte *buf = NULL;
	if (!buf)
	{
		buf = (byte*)calloc(u->max_size, sizeof(byte));
	}
	fread(buf, 1, 3, file);
	if (FindNaluStartCode3Bytes((char*)buf))
	{
		pos = u->startcodeprefix_len = 3;
	}
	else
	{
		fread(buf + 3, 1, 1, file);
		if (FindNaluStartCode4Bytes((char*)buf))
		{
			pos = u->startcodeprefix_len = 4;
		}
		else
		{
			puts("Could not find the nalu header of this file!");
			return 0;
		}
	}
	while (rewind == 0)
	{
		if (feof(file))
		{
			u->len = pos + rewind - u->startcodeprefix_len;
			memcpy(u->buf, &buf[u->startcodeprefix_len], u->len);
			u->forbidden_bit = (u->buf[0] >> 7) & 0x01;
			u->nal_reference_idc = (NalRefIdc)((u->buf[0] >> 5) & 0x03);
			u->nal_unit_type = (NaluType)(u->buf[0] & 0x1F);
			return EOF;
		}
		buf[pos++] = fgetc(file);
		if (FindNaluStartCode4Bytes((char*)( &buf[pos - 4])))
			rewind = -4;
		else
		{
			if (FindNaluStartCode3Bytes((char*)(&buf[pos - 3])))
				rewind = -3;
		}
	}
	fseek(file, rewind, SEEK_CUR);
	u->len = pos + rewind - u->startcodeprefix_len;
	memcpy(u->buf, &buf[u->startcodeprefix_len], u->len);
	u->forbidden_bit = (u->buf[0] >> 7) & 0x01;
	u->nal_reference_idc = (NalRefIdc)( (u->buf[0] >> 5) & 0x03 );
	u->nal_unit_type = (NaluType)(u->buf[0] & 0x1F);
	return pos + rewind;
}