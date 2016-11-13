#using <System.dll>
#include <Windows.h>
#include <iostream>
#include <string>
#include <regex>
#include <stdio.h>		
#include <sstream>		// ��� ostringstream

using namespace std;

using namespace System;
using namespace System::Net;
using namespace System::Net::Mail;
using namespace System::Net::Mime;
using namespace System::Threading;
using namespace System::ComponentModel;


string splitFileName(string, string&);
int renameFile(string, string);


class Letter
{
private:
	static const int MXFILES = 15;
	string filesList[15];
	string email;
	int fCounter;
public:
	Letter() : fCounter(0), email("")
	{}
	void addFile(string sIncFile) {
		filesList[fCounter] = sIncFile;
		fCounter++;
	}
	string getMail() {return email;}
	void setMail(string incMail) {email = incMail;}
	void resetCounter() {fCounter = 0;}
	void sendMail();
	void printReport();
	string delFiles();
};

int main()
{

	const string strFILESPATH = "C:/uNkind/testFilesForSend/";
	const TCHAR tcharFILESPATH[] = "C:\\uNkind\\testFilesForSend\\*.*\0";

	string incFileName, newFileName;
	string incMail = "";	// � ����� ���� �����
	int renameErrors;
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	// ����� ���������, ��� ����� ����� � ���������� ��������� ����� ����������������.
	// ����� ���������� � ����� ����� ������� ��������
	int opNumber = 0;

	// ����� ������� �����, �� ������� ����� �����.
	hf = FindFirstFile(tcharFILESPATH, &FindFileData);
	
	if (hf != INVALID_HANDLE_VALUE) {
		Letter letter;
		do {
			// ������� ������ � ������ �����
			incFileName = string(FindFileData.cFileName);
			if (incFileName != "." && incFileName != "..") {
				++opNumber;
				ostringstream strOpNumber;
				strOpNumber << opNumber;
				// �������� ��� ����� �� ����� � ��� � ������� �� � ����������:
				newFileName = strOpNumber.str() + "_" + splitFileName(incFileName, incMail);
				// ����������� ���� � ��������
				renameErrors = renameFile((strFILESPATH + incFileName), (strFILESPATH + newFileName));
				if (renameErrors == 0 && incMail != "") {
					if (letter.getMail() != incMail && letter.getMail() != "") {
						letter.sendMail();
						letter.printReport();
						cout << letter.delFiles() << endl;
						letter.resetCounter();
					}
					letter.setMail(incMail);
					letter.addFile(strFILESPATH + newFileName);
				} else {
					cout << "sending FAILED" << endl;
				}
			}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
		if (letter.getMail() != "") {
			letter.sendMail();
			letter.printReport();
			cout << letter.delFiles() << endl;
			letter.resetCounter();
		}
	}
	else {
		cout << "������, ������ � ����� ���.";
	}

	system("pause");
	return 0;
}

string splitFileName(string incFileName, string& incMail) {
	cmatch res;
	regex post_fName("(.*)___(.*)");
	regex_search(incFileName.c_str(), res, post_fName);
	incMail = res[1];
	return res[2];
}

// ������� ��������������� ����, ���������� 0, ���� ��� ������
int renameFile(string strOldName, string strNewName) {
	int rename_return_code;
	const char *oldname, *newname;
	oldname = strOldName.c_str();
	newname = strNewName.c_str();
	rename_return_code = rename(oldname, newname);
	return rename_return_code;
}

void Letter::printReport() {
	cout << "mail: " << email << " --- files: \n";
	for (int i = 0; i < fCounter; i++)
		cout << i << ". " << filesList[i] << endl;
}

string Letter::delFiles() {
	Sleep(2000);
	string delReport;
	for (int i = 0; i < fCounter; i++) {
		const char *strfile;
		strfile = filesList[i].c_str();

		if (remove(strfile) == -1) {
			delReport += "\nFail to delete file " + filesList[i];
		} else {
			delReport += "\nFile " + filesList[i] + " successfully deleted. ";
		}
	}
	return delReport;
}

void Letter::sendMail() {
	// ���� ����������, ���������� ����������:
	String^ userMail = gcnew String(email.c_str());

	SmtpClient ^Smtp = gcnew SmtpClient("smtp.yandex.ru", 25);
	// � ��� ������, ���� ��� ��������� ��������� ��� ���� ��� �� ���������� �����������, 
	// �� ����� ����������� ����� �������� 465 ��� 587

	Smtp->Credentials = gcnew NetworkCredential("t9535395@yandex.ru", "c7161X3119");
	Smtp->EnableSsl = true;

	MailMessage ^Message = gcnew MailMessage();
	Message->From = gcnew MailAddress("t9535395@yandex.ru");
	Message->To->Add(gcnew MailAddress(userMail));
	Message->Subject = "�������� �� C++, ������� �����... ����1";
	Message->Body = "������, � ����";

	Attachment^ dan;

	for (int i = 0; i < fCounter; i++) {
		String^ fileFullPath = gcnew String(filesList[i].c_str());
		dan = gcnew Attachment(fileFullPath, MediaTypeNames::Application::Octet);
		Message->Attachments->Add(dan);
	}

	Smtp->Send(Message);

	dan->~Attachment();
	Smtp->~SmtpClient();	

}