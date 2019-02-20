#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../alertMe/curl/curl.h"
#include <windows.data.json.h>
//#include "firebase/app.h"

#define CURL_STATICLIB

#ifdef _DEBUG
#	pragma comment (lib, "../alertMe/curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "../alertMe/curl/libcurl_a.lib")
#endif

//Pre declaration of functions, are defined under main function
std::string getCoords();
std::string writeLink(std::string fullLink);
void delLink();

int main()
{


	//Important variable declarations
	std::string preURL = "https://api.darksky.net/forecast/";
	std::string usrCoords;
	std::string line_;
	std::string fullURL;

	std::ifstream file_("../alertMe/debugOld/infLog.txt");
	if (file_.is_open()) {
		while (getline(file_, line_)) {
			std::cout << line_ << "\n";
		}
		file_.close();
	}
	else {
		std::cout << "file is not open" << "\n";
		std::cin.get();
	}
	usrCoords = getCoords();

	//Generates link with user coordinates
	fullURL = preURL + line_ + "/" + usrCoords;
	//Writes the link to a file
	std::cout << writeLink(fullURL);
	//New line
	std::cout << std::endl;


	//********************************************
	//********************cURL********************

	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL; //init to NULL is important
	//headers = curl_slist_append(headers, "Accept: application/json");
	//curl_slist_append(headers, "Content-Type: application/json");
	//curl_slist_append(headers, "charsets: utf-8");

	//curl = curl_easy_init();


	//Waits for input and deletes file with link
	//delLink();
}//Main function


std::string getCoords() {
	bool t = false;

	//Get user input loop
	do{
		float usrLat = 0; 
		float usrLong = 0;
		std::cout << "Input latitude: ";
		std::cin >> usrLat;
		std::cout << "Input longitude: ";
		std::cin >> usrLong;

		//Checks if coordinates are valid
		if (usrLat < 90 && usrLat > -90 && usrLong < 180 && usrLong > -180) {
			std::string nestUsrCoords = std::to_string(usrLat) + "," + std::to_string(usrLong);
			t = false;
			return nestUsrCoords;
		}
		else {
			std::cout << "Invalid\n";
			t = true;
		}
	} while (t == true);
}

//Function to write fullLink to a file
std::string writeLink(std::string fullLink) {
	std::string pchPath = "../alertMe/debugOld/pchDep.txt";

	std::ofstream pchDep(pchPath);

	pchDep << fullLink << std::endl;

	pchDep.close();

	return fullLink;
}

void delLink() {
	system("pause");
	std::remove("debugOld/pchDep.txt");
}