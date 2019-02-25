#include "pch.h"
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "libs/rapidjson/document.h"
#include "libs/rapidjson/stringbuffer.h"
#include "libs/rapidjson/writer.h"
#include "libs/rapidjson/filereadstream.h"
#include "../alertMe/curl/curl.h"
#include <unordered_map>

#pragma warning(disable:4996)

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
			//std::cout << line_ << "\n";
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

			//********************cURL********************
			//********************************************


			//********************************************
			//****************JSONCPP*********************
			
			//std::ifstream jFile("../alertMe/debugOld/repoOld.json");
			FILE* fp = fopen("../alertMe/debugOld/repoOld.json", "rb");

			char readBuffer[65536];//in bytes

			rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

			rapidjson::Document d;
			d.ParseStream(is);
			
			const rapidjson::Value& prog = d["data"];

			//Loops through each element of data array
			for (rapidjson::Value::ConstValueIterator p = prog.Begin(); p != prog.End(); p++) {
				std::time_t unixTime = (*p)["time"].GetInt();

				//Output formatted forecast
				std::cout << std::put_time(std::gmtime(&unixTime), "%c:") << " " 
						  << (*p)["summary"].GetString() << " " << (*p)["temperature"].GetFloat()
						  << "C " << std::endl;
				
			}

			fclose(fp);
			//****************JSONCPP*********************
			//********************************************

	//Waits for input and deletes file with link
	//delLink();

}//Main function

//Function to get user coordinates and verify the validity of them
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

	//Future: get current working directory or host files on server
	std::string pchPath = "../alertMe/debugOld/pchDep.txt";

	std::ofstream pchDep(pchPath);

	pchDep << fullLink << std::endl;

	pchDep.close();

	return fullLink;
}

//Function to delete created file
void delLink() {
	system("pause");
	std::remove("debugOld/pchDep.txt");
}