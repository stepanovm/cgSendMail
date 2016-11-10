#using <System.dll>
#include <Windows.h>
#include <iostream>
#include <string>
#include <regex>
#include <stdio.h>

using namespace std;

using namespace System;
using namespace System::Net;
using namespace System::Net::Mail;
using namespace System::Net::Mime;
using namespace System::Threading;
using namespace System::ComponentModel;


string splitFileName(string&, string&);
int renameFile(string, string);
void sendMail(string, string);

int main()
{

	const string strFILESPATH = "C:/uNkind/testFilesForSend/";
	const TCHAR tcharFILESPATH[] = "C:\\uNkind\\testFilesForSend\\*.*\0";

	string incFileName, newFileName;
	string incMail = "error";	// в почте пока пусто
	int renameErrors;
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	// Здесь указать папку, из которой брать файлы.
	hf = FindFirstFile(tcharFILESPATH, &FindFileData);

	if (hf != INVALID_HANDLE_VALUE) {
		do {
			// Получим строку с именем файла
			incFileName = string(FindFileData.cFileName);
			if (incFileName != "." && incFileName != "..") {
				// запишем в переменные новое имя файла и почту:
				newFileName = splitFileName(incFileName, incMail);
				// Переименуем файл в каталоге
				renameErrors = renameFile((strFILESPATH + incFileName), (strFILESPATH + newFileName));
				if (renameErrors == 0 && incMail != "error") {
					//sendMail((strFILESPATH + newFileName), incMail);
					cout << "success send FILE: " << newFileName << " TO MAIL: " << incMail << endl;
				} else {
					cout << "sending FAILED" << endl;
				}
			}
		} while (FindNextFile(hf, &FindFileData) != 0);
		FindClose(hf);
	}
	else {
		cout << "Ошибка, файлов в папке нет.";
	}
	

	system("pause");
	return 0;
	
}

string splitFileName(string& incFileName, string& incMail) {
	cmatch res;
	regex post_fName("(.*)___(.*)");
	regex_search(incFileName.c_str(), res, post_fName);
	incMail = res[1];
	return res[2];
}

// Функция переименовывает файл, возвращает 0, если нет ошибок
int renameFile(string strOldName, string strNewName) {
	// Переименовать файл:
	int rename_return_code;
	const char *oldname, *newname;
	oldname = strOldName.c_str();
	newname = strNewName.c_str();
	rename_return_code = rename(oldname, newname);
	return rename_return_code;
}

void sendMail(string file, string usermail) {
	// Куда отправлять, подготовим переменную:
	String^ userMail = gcnew String(usermail.c_str());

	SmtpClient ^Smtp = gcnew SmtpClient("smtp.yandex.ru", 25);
	// В том случае, если ваш провайдер блокирует это порт или не получается соединиться, 
	// то можно попробовать такие варианты 465 или 587

	Smtp->Credentials = gcnew NetworkCredential("t9535395@yandex.ru", "c7161X3119");
	Smtp->EnableSsl = true;

	MailMessage ^Message = gcnew MailMessage();
	Message->From = gcnew MailAddress("t9535395@yandex.ru");
	Message->To->Add(gcnew MailAddress(userMail));
	Message->Subject = "Тест8 отправки из С++";
	Message->Body = "Привет, я тест";

	String^ fileFullPath = gcnew String(file.c_str());
	Attachment^ dan = gcnew Attachment(fileFullPath, MediaTypeNames::Application::Octet);
	Message->Attachments->Add(dan);

	Smtp->Send(Message);
}