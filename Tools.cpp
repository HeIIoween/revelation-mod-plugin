#include "Tools.h"

using namespace std;

int randNumber(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}

string ws2s(wstring str)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);

	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &str[0], (int)str.size(), &strTo[0], size_needed, NULL, NULL);

	return strTo;
}

int dirNotExisted(const char *dir)
{
	struct stat info;

	if (stat(dir, &info) != 0) // File not found
	{
		return 1;
	}
	else if (!(info.st_mode & S_IFDIR)) // Not dir but not a file
	{
		return 2;
	}

	return 0; // There is the dir
}

const char *putDirReadmeContent = "Revelation Mod Data Storage Directory\r\n" \
"==========================================\r\n" \
"1. Please backup this folder and all data in it ALONG with MultiPlayer.\r\n" \
"2. Do NOT attempt to modify data manually as it may causes unrecoverable data corruption and make server no longer running normal.";
void putDirReadme(const char *dir)
{
	char path[MAX_PATH];

	// If the resulting path is too long
	if (strlen(dir) + 12 > MAX_PATH) {
		return;
	}

	strcat_s(path, dir);
	strcat_s(path, "\\Readme.txt");

	ofstream outfile(path);

	if (!outfile.is_open()) {
		return;
	}

	outfile << putDirReadmeContent << endl;
	outfile << endl;
	outfile << dir << endl;

	outfile.close();
}

bool createStorageDir(const char *dir)
{
	switch (dirNotExisted(dir))
	{
	case 0:
		return true;

	case 2:
		remove(dir);
	case 1:
		if (CreateDirectory(dir, 0)) {
			putDirReadme(dir);

			return true;
		}

	default:
		return false;
	}

	return false;
}

HK_ERROR editPlayerCharFile(wstring wscCharname, IniEditItemSet &inis)
{
	wstring wscDir = L"", wscCharFile = L"";
	string opeartingFilePath = "", scCharFile = "";
	
	if (HkGetAccountDirName(wscCharname, wscDir) != HKE_OK || HkGetCharFileName(wscCharname, wscCharFile) != HKE_OK)
	{
		return HKE_CHAR_DOES_NOT_EXIST;
	}
	
	scCharFile = scAcctPath + wstos(wscDir) + "\\" + wstos(wscCharFile) + ".fl";

	if (HkIsEncoded(scCharFile))
	{
		opeartingFilePath.append(scCharFile).append(".editing.tmp.ini");

		if (!flc_decode(scCharFile.c_str(), opeartingFilePath.c_str()))
		{
			return HKE_COULD_NOT_DECODE_CHARFILE;
		}
	} 
	else
	{
		opeartingFilePath = scCharFile;
	}

	IniEditItemSet::iterator iter;
	for (iter = inis.begin(); iter != inis.end(); ++iter)
	{
		switch (iter->Type)
		{
		case IniEditItems::WSTRING:
			IniWriteW(opeartingFilePath, iter->Node, iter->Key, iter->NewWideValue);
			break;

		case IniEditItems::STRING:
		default:
			IniWrite(opeartingFilePath, iter->Node, iter->Key, iter->NewValue);
			break;
		}
	}

	// Must be flc_decoded so it not the same
	if (opeartingFilePath == scCharFile)
	{
		return HKE_OK;
	}

	if (!flc_encode(opeartingFilePath.c_str(), scCharFile.c_str()))
	{
		return HKE_COULD_NOT_ENCODE_CHARFILE;
	}
	else
	{
		DeleteFile(opeartingFilePath.c_str());
	}

	return HKE_OK;
}

void lTrim(string &str)
{
	unsigned int length = str.length(), loop = 0, endLoop = 0;

	for (loop = 0; loop < length; ++loop)
	{
		if (str[loop] == ' ' || str[loop] == '\t')
		{
			continue;
		}

		endLoop = loop;

		break;
	}

	str = str.substr(endLoop, length);
}

void rTrim(string &str)
{
	unsigned int length = str.length(), loop = 0, endLoop = 0;

	for (loop = length - 1; loop >= 0; --loop)
	{
		if (str[loop] == ' ' || str[loop] == '\t')
		{
			continue;
		}

		endLoop = loop;

		break;
	}

	str = str.substr(0, endLoop + 1);
}

void trim(string &str)
{
	lTrim(str);
	rTrim(str);
}

void lTrim(wstring &str)
{
	unsigned int length = str.length(), loop = 0, endLoop = 0;

	for (loop = 0; loop < length; ++loop)
	{
		if (str[loop] == L' ' || str[loop] == L'\t')
		{
			continue;
		}

		endLoop = loop;

		break;
	}

	str = str.substr(endLoop, length);
}

void rTrim(wstring &str)
{
	unsigned int length = str.length(), loop = 0, endLoop = 0;

	for (loop = length - 1; loop >= 0; --loop)
	{
		if (str[loop] == L' ' || str[loop] == L'\t')
		{
			continue;
		}

		endLoop = loop;

		break;
	}

	str = str.substr(0, endLoop + 1);
}

void trim(wstring &str)
{
	lTrim(str);
	rTrim(str);
}